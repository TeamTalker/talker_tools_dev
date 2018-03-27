/* Team Talker 2018
 *
 *
 *
 */
 
#ifndef TKTL_FILE_IO_H
#define TKTL_FILE_IO_H

///////////////////////////////////
///////////////////////////////////
//// Common Struct Definitions ////
///////////////////////////////////
///////////////////////////////////

/////////////////////
// VSM2 ROM Header //
/////////////////////

struct TktlVSM2ROMHeader {
    char file_type[4];
    uint32_t major_version;
    uint32_t point_version;
    uint32_t rom_size;
    char name[32];
    char author[32];
    char description[128];
    uint32_t constant_frame_size;
    uint32_t lpc_tables_id;
    uint32_t lpc_size;
    uint32_t has_tones;
    uint32_t word_count;
    char spare[128]; 
};

/////////////////////////
// VSM2 Word-Meta info //
/////////////////////////

struct TktlVSM2WordMeta {
    uint32_t length;
    uint32_t increment;
    char spelling[16];
    uint32_t lpc_entry_point;
};

///////////////////
///////////////////
//// Functions ////
///////////////////
///////////////////

////////////////////////////
// Load bin file to SDRAM //
////////////////////////////

// Arguments:
// 1. path: file-path string
// 2. *&ptr: pointer to address of SDRAM data start. To be updated to point to copied data in SDRAM
// Returns:
// 0 on success
// 1 on fail
uint8_t TktlLoadFileSDRAM(const char *path, uint8_t *&ptr);

/////////////////////////////////
// Display VSM2 word meta-data //
/////////////////////////////////

// Arguments:
// 1. debug switch. Displays extended word information
// 2. word-index: index of word
// 3. word_meta: pointer to word meta struct 
void TktlDisplayWordMeta(uint8_t debug, uint16_t word_index, struct TktlVSM2WordMeta *word_meta);

/////////////////////
// Maths Functions //
/////////////////////

// Min function for work-count
uint16_t TktlMin16U(uint16_t val, uint16_t max);

/////////////////////////////////////
/////////////////////////////////////
//// Include Implementation File ////
/////////////////////////////////////
/////////////////////////////////////

#include "./file_io.c"

#endif /* TKTL_FILE_IO_H */