////////////////////////////
////////////////////////////
//// Init class members ////
////////////////////////////
////////////////////////////

//bool TktlShared::inited = false;
uint8_t  *TktlShared::ptr_vsm_sdram_ = NULL;
uint8_t  *TktlShared::ptr_vsm2_sdram_ = NULL;
struct   TktlVSM2ROMHeader *TktlShared::ptr_vsm2_rom_header_ = NULL;
struct   TktlVSM2WordMeta *TktlShared::ptr_vsm2_word_meta_sdram_ = NULL;
struct   TktlVSM2FFWordMeta *TktlShared::ptr_vsm2ff_word_meta_sdram_;
uint16_t TktlShared::vsm2_word_count_ = 0;
uint8_t  *TktlShared::ptr_lpc_coef_tables_sdram_ = NULL;
uint8_t  *TktlShared::ptr_lpc_chirp_tables_sdram_ = NULL;
uint8_t  *TktlShared::ptr_chirp_pitch_table_sdram_ = NULL;
uint8_t  TktlShared::vsm2_lpc_tables_id_ = 7;
uint8_t  TktlShared::rom_chirp_id_ = 7;
uint8_t  TktlShared::rom_pitch_bits_ = 0;
uint32_t TktlShared::vsm2_rom_size_ = 0;
uint32_t TktlShared::rom_bend_increment_ = 0;
uint8_t  TktlShared::vsm2_preset_index_ = 0;
uint8_t  TktlShared::filter_pitch_offset_ = 12; // Init to center value in table

//////////////////////////
//////////////////////////
//// Shared Functions ////
//////////////////////////
//////////////////////////

/////////////////////
// Maths Functions //
/////////////////////

// Min function for work-count
uint16_t TktlMin16U(uint16_t val, uint16_t max_val) {
	return (val < max_val) ? val : max_val;
};

////////////////////////////////
// Load Binary Files to SDRAM //
////////////////////////////////

uint8_t TktlLoadFileSDRAM(const char *path, uint8_t *&ptr, bool verbose) {
	
	// File-loading vars
	FRESULT file_error;
	FIL bin_file;
	UINT bytes_read;
	uint32_t bin_size;
	static uint8_t *ptr_data_sdram = NULL;
	
	// Check file-path passed in
	if(path == "") {
		LogTextMessage("WARNING: No file selected!");
		return 1;
	}
	
	if(verbose)
		LogTextMessage("Attempting to open binary data file from '%s'", path);

	// Attempt to open binary file, and keep return status in file_error var
	file_error = f_open(&bin_file, path, FA_READ | FA_OPEN_EXISTING);

	// Return error if binary file not found
	if (file_error != FR_OK) {
		LogTextMessage("ERROR: Unable to open binary data file");
		return 1;
	};

	bin_size = f_size(&bin_file);
	
	if(verbose)
		LogTextMessage("Allocating space in SDRAM");

	// Allocate memory in SDRAM and set pointer to memory location of first table
	ptr_data_sdram = (uint8_t *)sdram_malloc(bin_size);

	// Load lpcdata into sdram
	int remaining_bytes = bin_size;
	int offset = 0;

	while (remaining_bytes > 0) {
		if (remaining_bytes > sizeof(fbuff)) {
			file_error = f_read(&bin_file, fbuff, sizeof(fbuff), & bytes_read);
			if (bytes_read == 0)
				break;
			memcpy((char *)(&ptr_data_sdram[0]) + offset, (char *) fbuff, bytes_read);
			remaining_bytes -= bytes_read;
			offset += bytes_read;
		} else {
			file_error = f_read(&bin_file, fbuff, remaining_bytes, & bytes_read);
			memcpy((char *)(&ptr_data_sdram[0]) + offset, (char *) fbuff, bytes_read);
			remaining_bytes = 0;
		};
	};
	
	// Return error if failed reading file
	if (file_error != FR_OK) {
		LogTextMessage("ERROR: Failed reading file, aborting\n");
		return 1;
	};
	
	// Return error if failed closing file
	file_error = f_close(&bin_file);
	if (file_error != FR_OK) {
		LogTextMessage("ERROR: Failed closing file, aborting\n");
		return 1;
	};
	
	// Set pointer (passed in by reference) to data in SDRAM
	ptr = ptr_data_sdram;
	
	LogTextMessage("Binary data copied to SDRAM (%d bytes)", bin_size);
	LogTextMessage("SDRAM remaining: %d", sdram_get_free());
	
	// Success! Return no error
	return 0;
};

/////////////////////////////////
// Display VSM2 word meta-data //
/////////////////////////////////

void TktlDisplayWordMeta(uint8_t debug, uint16_t word_index, struct TktlVSM2WordMeta *word_meta) {
	LogTextMessage("\n==== Talker Tools Reader message: ====");
	LogTextMessage("Word %d spelling: %s", word_index, word_meta->spelling);
	if(debug == 1) {
		LogTextMessage("Word %d length: %d", word_index, word_meta->length);
		LogTextMessage("Word %d increment: %d", word_index, word_meta->increment);
		LogTextMessage("Word %d LPC entry-point: %d\n", word_index, word_meta->lpc_entry_point);
	};
};

void TktlDisplayWordMetaFF(uint8_t debug, uint16_t word_index, struct TktlVSM2FFWordMeta *word_meta) {
	LogTextMessage("\n==== Talker Tools Reader message: ====");
	LogTextMessage("Word %d spelling: %s", word_index, word_meta->spelling);
	if(debug == 1) {
		LogTextMessage("Word %d length (frames): %d, (bytes): %d", word_index, word_meta->length, word_meta->length * 8);
		LogTextMessage("Word %d LPC entry-point: %d\n", word_index, word_meta->lpc_entry_point);
	};
};
