using namespace std;
#include <iostream>
#include <iomanip>
#include <bitset>
#include <float.h>
#include <math.h>

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
	double x = 799;
	int64_t xf = toFixedPoint(x);
	/*
	int32_t e = ((int64_t)(xf >> 32) * 3025550) >> 21;
	cout << bitset<32>(xf >> 32) << endl;
	cout << bitset<32>(3025550) << endl;
	e = roundToNearest32(e);
	cout << e << endl;
	*/
	int16_t e = ((int16_t)(xf >> 48) * (int32_t)47274) >> 17;
	cout << bitset<32>(e) << endl;
	e = roundToNearest16(e);
	cout << e << endl;
	return 0;
}