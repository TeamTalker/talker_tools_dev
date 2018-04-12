//
//  lpc_stream.cpp
//

/////////////////////
/////////////////////
//// LPC indices ////
/////////////////////
/////////////////////

// Constructor
TktlLpcIndices::TktlLpcIndices() {
	is_reading_        = false;
	word_end_trigger_  = false;
	frame_trigger_     = false;
	is_unvoiced_frame_ = false;
	is_rest_frame_     = false;
	is_repeat_frame_   = false;
	SetSilent();
};

// Zero all LPC values
void TktlLpcIndices::SetSilent() {
    for(int i = 0; i < 12; i++) {
        indices_[i] = 0;
    };
};

// Zero K values for unvoiced frame
void TktlLpcIndices::SetUnvoiced() {
    for(int i = 6; i < 12; i++) {
        indices_[i] = 0;
    };
};

/////////////////////////////////
/////////////////////////////////
//// Point in LPC datastream ////
/////////////////////////////////
/////////////////////////////////

TktlLpcStreamPosition::TktlLpcStreamPosition() {
	word_start_byte_ = NULL;
    byte_            = NULL;
    bit_             = 0;
    fragment_        = 0;    
    saved_byte_      = NULL;
    saved_bit_       = 0;
    saved_fragment_  = 0;
};

// Set new word-start position
void TktlLpcStreamPosition::SetWordStart(uint8_t *byte) {
    word_start_byte_ = byte;
};

// Jump playhead to word-start
void TktlLpcStreamPosition::SetToWordStart() {
	byte_     = word_start_byte_;
    bit_      = 0;
    fragment_ = 0;
};

// Set playhead to arbitrary point
void TktlLpcStreamPosition::SetToPosition(uint8_t *byte, uint8_t bit, uint8_t fragment) {
    byte_     = byte;
    bit_      = bit;
    fragment_ = fragment;
};

// Set saved position
void TktlLpcStreamPosition::SetSavedPosition() {
    saved_byte_ = byte_;
    saved_bit_ = bit_;
    saved_fragment_ = fragment_;
};

// (Re)set to saved position
void TktlLpcStreamPosition::SetToSavedPosition() {
	byte_ = saved_byte_;
	bit_ = saved_bit_;
	fragment_ = saved_fragment_;
};
    
////////////////////////////////////////
////////////////////////////////////////
//// Get frame from LPC data stream ////
////////////////////////////////////////
////////////////////////////////////////

////////////////////
// Public Methods //
////////////////////

// Constructor
TktlGetLPCFrame::TktlGetLPCFrame() {
    read_word_           = true;
    gate_                = false;
    ptr_word_start_      = NULL;
    word_changed_        = true;
    loop_                = false;
    freeze_              = false;
    glitch_              = false;
    pitch_bits_		     = 6;	// Default VSM2
    frame_repeat_ 	     = 0;
    frame_energy_ 	     = 1;	// Needs to be more than 0, or frames will not play
	word_end_            = false;
	counter_		     = 0;
	frame_length_        = 75;	// 25ms default
	step_frame_          = false;
	// Zero indices in lpc_indices instances
	lpc_indices_.SetSilent();
	//lpc_indices_silent_.SetSilent();
};

// Set pitch bit count
void TktlGetLPCFrame::SetPitchBits(uint8_t bits = 6) {
	pitch_bits_ = bits;
};

// Set word start pointer to start of new word LPC data
// NOTE: doesn't actually set playhead to start of word
// This allows word to play to end before new word starts
void TktlGetLPCFrame::SetWordStart(uint8_t *ptr) {
	// Update word-start pointer if changed
	if(ptr_word_start_ != ptr) {
		head_.SetWordStart(ptr);
		word_changed_ = true;
		ptr_word_start_ = ptr;
	};
};

// Set gate on/off
void TktlGetLPCFrame::SetGate(bool gate, bool oneshot = false) {
	if(gate) {
		if(!gate_) {
			// RISING EDGE
			StartReading();
		};
	} else {
		if(gate_) {
			// FALLING EDGE
			// Stop playback only if in gated playback mode
			if(!oneshot)
				StopReading();
		};
	};
	gate_ = gate;
};

// Begin playback from start of selected word
void TktlGetLPCFrame::StartReading() {
	//LogTextMessage("Word changed: %d", word_changed_);
    // Do not set playhead to word start if freeze enabled
    if(freeze_) {
		if(word_changed_) {
			head_.SetToWordStart();
			ReadFrame();
			word_changed_ = false;
		};
	} else {
		head_.SetToWordStart();
	};
	// Reset counter and flags
	counter_                 = 0;
	read_word_               = true;
	lpc_indices_.is_reading_ = true;
};

// Stop reading frame data
void TktlGetLPCFrame::StopReading() {
	read_word_ = false;
};

// Set loop
void TktlGetLPCFrame::SetLoop(bool loop) {
    loop_ = loop;
};

// Set glitch
void TktlGetLPCFrame::SetGlitch(bool glitch) {
	glitch_ = glitch;
};

// Set freeze
void TktlGetLPCFrame::SetFreeze(bool freeze) {
	freeze_ = freeze;
};

// Step to next frame when in freeze mode
void TktlGetLPCFrame::StepFrozenFrame() {
	if(freeze_) {
		ReadFrame();
		counter_ = 0;
	};
};

// Set jump-point
void TktlGetLPCFrame::SetJumpPoint() {
	head_.SetSavedPosition();
};
    
// Set play-head to jump-point
void TktlGetLPCFrame::SetToJumpPoint() {
	head_.SetToSavedPosition();
};

// Set frame length
void TktlGetLPCFrame::SetFrameLength(uint32_t length) {
	frame_length_ = length;
};

// Tick object (call every K-Rate cycle)
TktlLpcIndices TktlGetLPCFrame::Tick() {		
	// Reset word-end trigger, frame trigger
	lpc_indices_.word_end_trigger_ = false;	
	lpc_indices_.frame_trigger_    = false;
	// Read frame if counter = 0 and read_word_ true
	if(counter_ == 0) {			
		if(read_word_) {
			// Only update frame if freeze not enabled
			if(!freeze_) {
				ReadFrame();
			};
			// Send frame-update trigger pulse
			lpc_indices_.frame_trigger_ = true;
		} else {
			lpc_indices_.is_reading_ = false;
		};
	};		
	// Update counter (if freeze true, counter will keep incrementing forever)
	counter_ = (counter_ < frame_length_) ? counter_ + 1 : 0;
	// Bit-shift pitch indices up by one, if there are only 5 bits for pitch
	if(pitch_bits_ == 5)
    	lpc_indices_.indices_[1] <<= 1;
    // Return object
	return lpc_indices_;
};

/////////////////////
// Private Methods //
/////////////////////

// Read word
void TktlGetLPCFrame::ReadFrame() {
    // Reset Rest, Unvoiced frame flags
    lpc_indices_.is_rest_frame_     = false;
    lpc_indices_.is_unvoiced_frame_ = false;
    lpc_indices_.is_repeat_frame_   = false;
      
	frame_energy_ = GetBits(4);
	if (frame_energy_ == 0) {
		// Energy = 0: rest frame
		lpc_indices_.SetSilent();
		 // Set Rest flag in lpc_indices_ instance
		lpc_indices_.is_rest_frame_ = true;
	} else if (frame_energy_ == 15) {
		// Energy = 15: stop frame            
		// If loop true, reset playhead to beginning of word
		// Also reset if freeze set
		if((loop_) || (freeze_)) {
			head_.SetToWordStart();
		} else {
			read_word_ = false;
			lpc_indices_.SetSilent();
		};
		// Trigger word-end pulse
		lpc_indices_.word_end_trigger_ = true;
	} else {
		// Get Energy index
		lpc_indices_.indices_[0] = frame_energy_;
		// Get Repeat bit
		frame_repeat_ = GetBits(1);
		// Get Pitch index
		lpc_indices_.indices_[1] = GetBits(6);
		// Get K indices
		// (Repeat frame uses the last coefficients)
		if(frame_repeat_) {
			lpc_indices_.is_repeat_frame_ = true;
		} else {
			// All frames use the first 4 coefficients
			lpc_indices_.indices_[2] = GetBits(5);
			lpc_indices_.indices_[3] = GetBits(5);
			lpc_indices_.indices_[4] = GetBits(4);
			lpc_indices_.indices_[5] = GetBits(4);
			if (lpc_indices_.indices_[1]) {
				// Voiced frames use 6 extra coefficients.
				lpc_indices_.indices_[6]  = GetBits(4);
				lpc_indices_.indices_[7]  = GetBits(4);
				lpc_indices_.indices_[8]  = GetBits(4);
				lpc_indices_.indices_[9]  = GetBits(3);
				lpc_indices_.indices_[10] = GetBits(3);
				lpc_indices_.indices_[11] = GetBits(3);
			} else {
				// Zero last 6 coefficients for unvoiced frames
				lpc_indices_.SetUnvoiced();
				// Set Unvoiced flag in lpc_indices_ instance
				lpc_indices_.is_unvoiced_frame_ = true;
			};
		};
	};
};

// Read bits from ROM
uint8_t TktlGetLPCFrame::GetBits(uint8_t bits) {
    uint8_t value;
    uint16_t data;
    data = ReverseBits(*(head_.byte_)) << 8;
    if (head_.bit_ + bits > 8) {
        data |= ReverseBits(*(head_.byte_ + 1));
    };
    data <<= head_.bit_;
    value = data >> (16-bits);
    head_.bit_ += bits;
    if (head_.bit_ >= 8) {
        head_.bit_ -= 8;
        head_.byte_++;
    };
    return value;
};

// Reverse bits
uint8_t TktlGetLPCFrame::ReverseBits(uint8_t byte) {
    // Don't reverse bits if glitch flag set
    // Maybe there's a better way to do a temporary glitch?
    if(glitch_) {
    	return byte;
    } else {
    	uint32_t r = byte;
     	__ASM volatile ("rbit %0, %1" : "=r" (r) : "r" (byte));
     	return r >> 24;
     }
};




