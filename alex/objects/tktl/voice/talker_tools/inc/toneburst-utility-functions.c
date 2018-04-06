//// 8-bit max
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Max8(int8_t val, int8_t max) {
//	return (val <= max) ? val : max;
//};
//
//// 16-bit max
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Max16(int16_t val, int16_t max) {
	return (val <= max) ? val : max;
};
//
//// 32-bit max
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Max32(int32_t val, int32_t max) {
//	return (val <= max) ? val : max;
//};
//	
//// 8-bit min
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Min8(int8_t val, int8_t min) {
	return (val >= min) ? val : min;
};
//	
//// 16-bit min
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Min16(int16_t val, int16_t min) {
	return (val >= min) ? val : min;
};
//
// 32-bit min
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Min32(int32_t val, int32_t min) {
	return (val >= min) ? val : min;
};
//
//// 8-Bit clamp val to range min > max
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int8_t Clamp8(int8_t val, int8_t min, int8_t max) {
//	return (val <= min) ? min : (val >= max) ? max : val;
//};
//
//// 16-Bit clamp val to range min > max
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int16_t Clamp16(int16_t val, int16_t min, int16_t max) {
//	return (val <= min) ? min : (val >= max) ? max : val;
//};
//
//// 32-Bit clamp val to range min > max
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Clamp32(int32_t val, int32_t min, int32_t max) {
//	return (val <= min) ? min : (val >= max) ? max : val;
//};
//
//// Scale positive input to range out_min > out_max (doesn't work with signed ints?)
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t MapU32(uint32_t val, uint32_t out_min, uint32_t out_max) {
//	// Convert input vals to float
//	float x = (float)val;
//	float outMin = (float)out_min;
//	float outMax = (float)out_max;
//	// Return val in correct 27-bit integer range
//	return (int32_t)(x * (outMax - outMin) / 0x07FFFFFF + outMin);
//};

// Scale input to 0 > out_max range
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t ScaleU32(int32_t val, int32_t out_max) {
	int64_t a = (int64_t)out_max * val;
    return a >> 27;
};

// Map value between min and max. Taken from TSG/math/map object by Johannes Elliesen
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t MapU32(int32_t val, int32_t out_min, int32_t out_max) {
      int64_t a = (int64_t)out_max * val;
      a += (int64_t)out_min * ((128 << 20) - val);
      return a >> 27;
};

// Logarithmic interpolation for 27-bit param/inlet values
__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t CurveLog32(int32_t val) {
	// Transform inlet val to float in 0 - 1 range
	float inf = val * (float)(1.0f / (1 << 27));
	float curve = 1 - (1 - inf) * (1 - inf);
	// Return val in correct 27-bit integer range
	return (int32_t)(curve * (float)(1 << 27));
};

//// Exponential interpolation for 27-bit param/inlet values
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t CurveExp32(int32_t val) {
//	// Transform inlet val to float in 0 - 1 range
//	float inf = val * (float)(1.0f / (1 << 27));
//	float curve = inf*inf;
//	// Return val in correct 27-bit integer range
//	return (int32_t)(curve * (float)(1 << 27));
//};
//
//// 16-bit xfade
//// (adapted from Factory xfade object)
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int16_t Xfade16(int16_t a, int16_t b, uint16_t x) {
//	int16_t ccompl = (1 << 15) - x;
//	int32_t result = (int32_t)b * x;
//	result += (int32_t)a * ccompl;
//	return result >> 16;
//};
//
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
//
//// 32-bit square root
//// (Adapted from Factory sqrt object)
//__attribute__ ( ( always_inline ) ) __STATIC_INLINE int32_t Sqrt32(int32_t val) {
//	int32_t ai = (val > 0) ? val : -val;
//	float aif = ai;
//	aif = _VSQRTF(aif);
//	return (int)aif;
//};
//
//// Check size of binary file
//// http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
//int fsize(FILE *fp){
//	int prev=ftell(fp);
//	fseek(fp, 0L, SEEK_END);
//	int sz=ftell(fp);
//	fseek(fp,prev,SEEK_SET); //go back to where we were
//	return sz;
//};
