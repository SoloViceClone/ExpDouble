using namespace std;
#include <iostream>
#include <iomanip>
#include <bitset>
#include <float.h>
#include <math.h>

extern double expDouble(double x);

typedef union {
	double d;
	int64_t i64;
	uint64_t ui64;
} binary64;

void test1(double x) {
	binary64 b64;
	b64.d = x;
	cout << bitset<64>(b64.ui64) << endl;
}

void test2(int64_t x) {
	double d = ((union{int64_t t; double d;}){x}).d;
	cout << d << endl;
}

int64_t toFixedPoint(double x) {
	int64_t xbits  = ((union{double d; int64_t t; }){x}).t;
	int64_t sign = xbits >> 63;
	int64_t e = ((((uint64_t)xbits) >> 52) & 0b011111111111) - 1023;
	int64_t result = (xbits << 11) | ((int64_t)1 << 63);
	result = (uint64_t)result >> (10-e);
	result = (result + sign) ^ sign;

	return result;
}

int16_t roundToNearest(int64_t x) {
	x = x + ((int64_t)1 << 52);
	return (int16_t)(x >> 53);
}

int16_t roundToNearest16(int16_t x) {
	x = x + (int16_t)0b0100;
	return (x >> 3);
}

int32_t roundToNearest32(int32_t x) {
	x = x + (int32_t)0b0100000000000000000000;
	return (x >> 21);
}

int main() {
	cout << setprecision(99);
	double x;
	cin >> x;
	int64_t xf = toFixedPoint(x);
	const int64_t log2 = 6243314768165359;
	int16_t e = ((int16_t)(xf >> 48) * (int32_t)47274) >> 17;
	e = roundToNearest16(e);
	int64_t y = xf - e * log2;
	int16_t a = (y >> 40) & 0b01111111111111;
	int16_t sign_a = a >> 12;
	int64_t z = (uint64_t)(y << 24) >> 24;
	cout << "0000000000011111111111111111111111111111111111111111111111111111" << endl;

	binary64 b1,b2;
	b1.d = expDouble(x);
	b2.d = exp(x);
	cout << "expd : " << bitset<64>(b1.i64) << endl;
	cout << "exp  : " << bitset<64>(b2.i64) << endl;
	cout << "diff : " << b1.i64 - b2.i64 << endl;
	return 0;
}