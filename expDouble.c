using namespace std;

#include <iostream>
#include <bitset>
#include "./ea.data"
#include "./eb.data"

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
	Fixed-point: 64bits.64bits
*/

static inline int128_t toFixedPoint2(double x) {
	binary64 b;
	b.d = x;
	int128_t sign = (int128_t)b.i64 >> 64;
	int32_t e = (((b.ui64) >> 52) & 0b011111111111) - 1023;
	e = ((-1 - ((e + 75) >> 31)) & (e + 75)) - 75;
	uint128_t val = (uint128_t)(b.ui64 << 12) + (uint128_t)18446744073709551615ull + 1;
	val = (val << 53) >> (53 - e);
	val = (val + sign) ^ sign;
	return val;
}

/*
	Round a real number to the nearest integer
	Algo: roundToNearest(x) = trunc(x+0.5)
*/

static inline int16_t roundToNearest16(int16_t x) {
	x = x + (int16_t)0b0100;
	return (x >> 3);
}

/*
	Multiplication 2 fixed-point real number of format 1bit.63bits
*/
static inline uint64_t mult(uint64_t a, uint64_t b) {
	return (((uint128_t)a * (uint128_t)b) >> 63);
}

double expDouble(double x) {
	const double inf_threshold = 0x1.62e42fefa39efp9;
	const double sup_threshold = -0x1.74385446d71c3p9;
	if (__builtin_expect((x >= inf_threshold || x <= sup_threshold), 0)) {
		if (x >= inf_threshold) return 1.0/0.0;
		else return 0.0;
	}

	//int128_t xf = toFixedPoint2(x);
	int128_t xf = toFixedPoint2(x);
	// e = x * inv_log(2)
	int16_t e = ((int16_t)(xf >> 59) * (int32_t)47274) >> 17;
	e = roundToNearest16(e);


	const uint64_t one = 9223372036854775808UL; // 1.0 in fixed-point format
	const uint64_t log2 = 12786308645202655660ul;	// ln(2) in fixed-point format
	int64_t y = xf - e * log2;
	int16_t a = (y >> 51) & 0b01111111111111; // 13 first digits of y
	int16_t b = (y >> 38) & 0b01111111111111; // 13 next digits of y
	int16_t sign_y = y >> 63;

	int64_t z = (uint64_t)(y << 26) >> 27;
	
	__builtin_prefetch(&ea[a],0,0);
	__builtin_prefetch(&eb[b],0,0);
	

	uint64_t eab = mult(ea[a],eb[b]);
	
	uint64_t ey = eab + mult(eab,z) + (1 << 10);

	uint64_t result = ((ey << (sign_y & 1)) - one) >> 11;
	e = e + sign_y;
	result = ((uint64_t)(e + 1023) << 52) | result;

	#ifdef DEBUG
	cout << "-----------" << endl;
	cout << x << endl;
	cout << "xf   : " << bitset<64>((uint128_t)xf >> 64) << bitset<64>(xf) << endl;
	cout << "y    : " << bitset<64>(y) << endl;
	cout << "z    : " << bitset<64>(z) << endl;
	cout << "ea   : " << bitset<64>(ea[a]) << endl;
	cout << "eb   : " << bitset<64>(eb[b]) << endl;
	cout << "eab  : " << bitset<64>(eab) << endl;
	cout << "ea+eb: " << bitset<64>(ea[a]+eb[b]) << endl;
	cout << ey << endl;
	cout << "ey   : " <<  bitset<64>(ey) << endl;
	cout << bitset<16>(sign_y & 1) << endl;
	cout << bitset<64>(result) << endl;
	cout << "-----------" << endl;
	#endif

	return ((union{uint64_t i; double d;}){result}).d;

}