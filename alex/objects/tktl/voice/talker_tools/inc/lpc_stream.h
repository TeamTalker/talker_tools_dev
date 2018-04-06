//
//  lpc_stream.cpp
//
//  Created by alx on 21/03/2018.
//  Copyright © 2018 alx. All rights reserved.
//
//  C++ implementation adapted from Talkie library for Arduino by Peter Knight
//  https://github.com/going-digital/Talkie
//  With huge thanks!
//

#ifndef TKTL_LPC_STREAM_H
#define TKTL_LPC_STREAM_H

/////////////////////
/////////////////////
//// LPC indices ////
/////////////////////
/////////////////////

class TktlLpcIndices {
public:
	/////////////////////////
	// Public data members //
	/////////////////////////
	
    uint8_t indices_[12];
    /*
         LPC Indices array:
         0  energy
         1  pitch
         2  k1
         3  k2
         4  k3
         5  k4
         6  k5
         7  k6
         8  k7
         9  k8
         10 k9
         11 k10
     */
    
    // Flags
    bool is_reading_ ;
	bool word_end_trigger_;
	bool frame_trigger_ ;
	bool is_unvoiced_frame_;
	bool is_rest_frame_;
	bool is_repeat_frame_;

    ////////////////////
    // Public methods //
    ////////////////////
	
	// Constructor
    TktlLpcIndices();
    // Zero all K values
    void SetSilent();
    // Zero K values for unvoiced frame
    void SetUnvoiced();
};

/////////////////////////////////
/////////////////////////////////
//// Point in LPC datastream ////
/////////////////////////////////
/////////////////////////////////

class TktlLpcStreamPosition {
public:
	/////////////////////////
	// Public data members //
	/////////////////////////
	
    uint8_t *word_start_byte_;
    
    uint8_t *byte_;
    uint8_t bit_;
    uint8_t fragment_;			// (Currently unused)
    
    uint8_t *saved_byte_;
    uint8_t saved_bit_;
    uint8_t saved_fragment_;	// (Currently unused)
    
    ////////////////////
    // Public methods //
    ////////////////////
    
    // Constructor
    TktlLpcStreamPosition();
    
	// Set new word-start position
	void SetWordStart(uint8_t *byte);

	// Jump playhead to word-start
	void SetToWordStart();

	// Set playhead to arbitrary point
	void SetToPosition(
		uint8_t *byte,
		uint8_t bit,
		uint8_t fragment
	);

	// Set saved position
	void SetSavedPosition();

	// (Re)set to saved position
	void SetToSavedPosition();
    
};

////////////////////////////////////////
////////////////////////////////////////
//// Get frame from LPC data stream ////
////////////////////////////////////////
////////////////////////////////////////

class TktlGetLPCFrame {
public:
	////////////////////
	// Public methods //
	////////////////////
    
    // Constructor
    TktlGetLPCFrame();
    
	// Set pitch bit count
	void SetPitchBits(uint8_t bits);

	// Set gate on/off
	// oneshot flag disables gate if set true)
	void SetGate(bool gate, bool oneshot);

	// Set word start pointer to start of word LPC data
	void SetWordStart(uint8_t *ptr);

	// Begin playback from start of selected word
	void StartReading();
	
	// Stop reading frame data
	void StopReading();

	// Set loop on/off
	void SetLoop(bool loop);

	// Set glitch on/off
	void SetGlitch(bool glitch);

	// Set freeze on/off
	void SetFreeze(bool freeze);

	// Step to next frame when in freeze mode
	void StepFrozenFrame();

	// Set new jump-point
	void SetJumpPoint();
	
	// Jump play-head to previously-set jump-point
	void SetToJumpPoint();

	// Set frame length (determines playback rate)
	void SetFrameLength(uint32_t length);

	// Tick object (call every K-Rate cycle)
	TktlLpcIndices Tick();
    
private:
	//////////////////////////
	// Private data members //
	//////////////////////////
	
	// Pointer to start of word
    uint8_t *ptr_word_start_;
    
    // Word-playback flags
    bool word_changed_;
    bool gate_;
    bool read_word_;
    bool freeze_;
    bool loop_;
	bool glitch_;
    
    // Global LPC settings
    uint8_t pitch_bits_;
    
    // Per-frame LPC settings
    uint8_t frame_repeat_;
    uint8_t frame_energy_;
    
    // Word-playback settings
    bool word_end_;   
	uint32_t counter_;
	uint8_t frame_length_ ;
	bool step_frame_;
    
    // Stream position object instance
    TktlLpcStreamPosition head_;
    
    // LPC indices object instance
    TktlLpcIndices lpc_indices_;
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    // Read bits from ROM
    uint8_t GetBits(uint8_t bits);
    
    // Reverse bits
    uint8_t ReverseBits(uint8_t byte);
    
    // Read LPC frame, updating index values in 'lpc_indices'
    void ReadFrame();
    
    // TODO
    void IncrementCounters();
};

#include "lpc_stream.cpp"

#endif /* TKTL_LPC_STREAM_H */

