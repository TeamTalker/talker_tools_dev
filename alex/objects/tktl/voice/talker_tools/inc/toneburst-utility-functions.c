// 8-bit max
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Max8(int8_t val, int8_t max) {
	return (val <= max) ? val : max;
};

// 16-bit max
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Max16(int16_t val, int16_t max) {
	return (val <= max) ? val : max;
};

// 32-bit max
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Max32(int32_t val, int32_t max) {
	return (val <= max) ? val : max;
};

//// 8-bit min
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Min8(int8_t val, int8_t min) {
	return (val >= min) ? val : min;
};

//// 16-bit min
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Min16(int16_t val, int16_t min) {
	return (val >= min) ? val : min;
};

// 32-bit min
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Min32(int32_t val, int32_t min) {
	return (val >= min) ? val : min;
};

// 8-Bit clamp val to range min > max
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int8_t Clamp8(int8_t val, int8_t min, int8_t max) {
	return (val <= min) ? min : (val >= max) ? max : val;
};

// 16-Bit clamp val to range min > max
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int16_t Clamp16(int16_t val, int16_t min, int16_t max) {
	return (val <= min) ? min : (val >= max) ? max : val;
};

// 32-Bit clamp val to range min > max
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Clamp32(int32_t val, int32_t min, int32_t max) {
	return (val <= min) ? min : (val >= max) ? max : val;
};

// Scale positive input to range out_min > out_max. Extracted from community:rbrt/math/scale by Robert Schirmer
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t MapU32(uint32_t val, uint32_t out_min, uint32_t out_max) {
	return (___SMMUL(((out_max - out_min) >> 1), val) << 6) + out_min;
};

// Scale input to 0 > out_max range
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t ScaleU32(int32_t val, int32_t out_max) {
	return (val * out_max / 0x07FFFFFF);
};

// Map value float (0.0f to 1.0f range) between min and max. 
__attribute__ ( ( always_inline ) ) __STATIC_INLINE float MapF(float val, float out_min, float out_max) {
	return val * (out_max - out_min) + out_min;
};


// Logarithmic interpolation for 27-bit param/inlet values
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t CurveLog32(int32_t val) {
	// Invert range
	int32_t inv = (1 << 27) - val;
	//Multiply of the MSB (arm instruction)
	int32_t curve = (1 << 27) - ___SMMUL(inv << 3, inv << 2);
	
	return curve;
};

// Exponential interpolation for 27-bit param/inlet values
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t CurveExp32(int32_t val) {
	return ___SMMUL(val << 3, val << 2);
};

// 16-bit xfade
// (adapted from Factory xfade object)
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Xfade16(int16_t a, int16_t b, uint32_t x) {
    int32_t ccompl = (1 << 16) - x;
    int32_t result = (int32_t)b * x;
    result += (int32_t)a * ccompl;
    return result >> 16;
};

// 27-bit xfade
// (adapted from Factory xfade object)
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Xfade27(int32_t a, int32_t b, uint32_t x) {
	int32_t ccompl = (1 << 31) - x;
	int64_t result = (int64_t)b * x;
	result += (int64_t)a * ccompl;
	return result >> 27;
};

// 32-bit slew function, with pointer to state variable
// (Adapted from Factory glide object)
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Slew32(int32_t target, int32_t *state, int32_t slewTime, bool enable) {
	if (enable && slewTime > 0) {
		*state = ___SMMLA(*state - target, (-1 << 26) + (slewTime >> 1), *state);
		return *state;
	} else {
		// Update slew state so we don't get a jump next time slew is enabled
		*state = target;
		return target;
	};
};

// 32-bit square root
// (Adapted from Factory sqrt object)
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Sqrt32(int32_t val) {
	int32_t ai = (val > 0) ? val : -val;
	float aif = ai;
	aif = _VSQRTF(aif);
	return (int)aif;
};

//// Check size of binary file
//// http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
//int fsize(FILE *fp){
//	int prev=ftell(fp);
//	fseek(fp, 0L, SEEK_END);
//	int sz=ftell(fp);
//	fseek(fp,prev,SEEK_SET); //go back to where we were
//	return sz;
//};
