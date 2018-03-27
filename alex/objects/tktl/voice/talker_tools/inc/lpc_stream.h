//
//  lpc_stream.hpp
//  C++ Tests
//
//  Created by alx on 21/03/2018.
//  Copyright © 2018 alx. All rights reserved.
//

#ifndef TKTL_LPC_STREAM_H
#define TKTL_LPC_STREAM_H

/////////////////
// LPC indices //
/////////////////

class TktlLpcIndices {
public:
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
    // Constructor
    TktlLpcIndices();
    // Zero all K values
    void Silence();
    // Zero K values for unvoiced frame
    void Unvoiced();
};

/////////////////////////////
// Point in LPC datastream //
/////////////////////////////

class TktlLpcStreamPosition {
public:
    uint8_t *word_start_byte_;
    
    uint8_t *byte_;
    uint8_t bit_;
    uint8_t fragment_;			// (Currently unused)
    
    uint8_t *saved_byte_;
    uint8_t saved_bit_;
    uint8_t saved_fragment_;	// (Currently unused)
    
    // Constructor
    TktlLpcStreamPosition();
    
    // Set to playhead to word-start
    void SetWordStart(uint8_t *byte);
    
    // Reset to start of word (ie for loop)
    void ResetToWordStart();
    
    // Set playhead to point
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

////////////////////////////////////
// Get frame from LPC data stream //
////////////////////////////////////

class TktlGetLPCFrame {
public:
    // Constructor
    TktlGetLPCFrame();
    
    // Set pitch bit count
	void SetPitchBits(uint8_t bits);
    
     // Get frame
    TktlLpcIndices GetFrame();
    
    // Set word-start pointer
    void StartWord(uint8_t *ptr);
    
    // Set loop
    void SetLoop(bool loop);
    
    // Set glitch
    void SetGlitch(bool glitch);
    
    // Stop reading frame data
    void StopRead();
    
    // Check for word-end
    bool CheckForWordEnd();
    
    // Set jump-point
    void SetJumpPoint();
    
    // Set play-head to jump-point
    void SetToJumpPoint();
    
private:
    uint8_t *ptr_word_start_;
    bool read_word_;
    bool frame_loop_;
    bool frame_glitch_;
    uint8_t frame_repeat_;
    uint8_t frame_energy_;
    bool word_end_;
    
    TktlLpcStreamPosition head_;
    TktlLpcIndices lpc_indices_;
    
    // Read LPC frame, updating index values in 'lpc_indices'
    void ReadFrame();
    
    // Read bits from ROM
    uint8_t GetBits(uint8_t bits);
    
    // Reverse bits
    uint8_t ReverseBits(uint8_t byte);
};

#include "lpc_stream.cpp"

#endif /* TKTL_LPC_STREAM_H */

