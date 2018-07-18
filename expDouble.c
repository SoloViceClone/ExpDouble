typedef __int128_t int128_t;
typedef __uint128_t uint128_t;

typedef union {
	int64_t i64;
	uint64_t ui64;
	double d;
} binary64;

/* 
	Transform a real number x from floatting-point format to fixed-point format 
	Floatting-point: 1 bit sign + 11 bits power + 52 bits significand
	Fixed-point: 11bits.53bits
*/
static inline int64_t toFixedPoint(double x) {
	int64_t xbits  = ((union{double d; int64_t t; }){x}).t;
	int64_t sign = xbits >> 63;
	int64_t e = ((((uint64_t)xbits) >> 52) & 0b011111111111) - 1023;
	int64_t result = (xbits << 11) | ((int64_t)1 << 63);
	result = (uint64_t)result >> (10-e);
	result = (result + sign) ^ sign;
	return result;
}

/*
	Round a real number to the nearest integer
	Algo: roundToNearest(x) = trunc(x+0.5)
*/

static inline int16_t roundToNearest16(int16_t x) {
	x = x + (int16_t)0b0100;
	return (x >> 3);
}

double expDouble(double x) {
	const double inf_threshold = 0x1.62e42fefa39efp9;
	if (__builtin_expect(x >= inf_threshold,0)) {
		return 1.0/0.0;
	} else {
		int64_t xf = toFixedPoint(x);
		int16_t e = ((int16_t)(xf >> 48) * (int32_t)47274) >> 17;
		e = roundToNearest16(e);
	}

}