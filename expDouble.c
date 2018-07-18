using namespace std;

#include <iostream>
#include <bitset>
#include "./ea_p.data"

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
	result = result & ((-e-54) >> 63);
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

static inline int64_t mult(int64_t a, int64_t b) {
	return (((int128_t)a * (int128_t)b) >> 53);
}

double expDouble(double x) {
	const double inf_threshold = 0x1.62e42fefa39efp9;
	if (__builtin_expect(x >= inf_threshold,0)) {
		return 1.0/0.0;
	} else {
		int64_t xf = toFixedPoint(x);
		// e = x * inv_log(2)
		int16_t e = ((int16_t)(xf >> 48) * (int32_t)47274) >> 17;
		e = roundToNearest16(e);
		const int64_t log2 = 6243314768165359;
		int64_t y = xf - e * log2;
		int16_t a = (y >> 40) & 0b01111111111111;
		int16_t sign_y = y >> 63;
		int64_t z = (uint64_t)(y << 24) >> 24;
		// e^y = e^a * e^z = e^a + e^a * P(z)
		// p(x) = x * (1 + x/2 * (1 + x/3 * (1 + x/4)))
		// 1/3 = 3002399751580330
		// 1 = 9007199254740992
		int64_t p_z = 9007199254740992 + (z >> 2);
		p_z = 9007199254740992 + mult(3002399751580330,z);
		p_z = 9007199254740992 + (mult(p_z,z) >> 1);
		p_z = mult(p_z,z);
		//__builtin_expect(&ea[a],0,0);
		int64_t ey = ea[a] + mult(ea[a],p_z);
		cout << "-----------" << endl;
		cout << "xf   : " << bitset<64>(xf) << endl;
		cout << "z    : " << bitset<64>(z) << endl;
		cout << sign_y << endl;
		cout << p_z << endl;
		cout << "pz   : " << bitset<64>(p_z) << endl;
		cout << ey << endl;
		cout << "ey   : " <<  bitset<64>(ey) << endl;
		cout << bitset<16>(sign_y & 1) << endl;
		
		int64_t result = ((ey << (sign_y & 1)) - 9007199254740992) >> 1;
		e = e + sign_y;
		result = ((uint64_t)(e + 1023) << 52) | result;
		cout << bitset<64>(result) << endl;
		cout << "-----------" << endl;

		return ((union{int64_t i; double d;}){result}).d;
	}

}