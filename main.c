using namespace std;
#include <iostream>
#include <iomanip>
#include <bitset>
#include <float.h>
#include <math.h>

extern double expDouble(double x);

extern double dummy(double x);

#define RANDTESTSIZE 0x100000

float randtest[RANDTESTSIZE];

typedef union {
	double d;
	int64_t i64;
	uint64_t ui64;
} binary64;

uint64_t get_cycle_counter() {
#ifdef __k1__
    __builtin_k1_barrier();
    unsigned k = __k1_counter_num(0);
    __builtin_k1_barrier();
    return k;

#else

		uint64_t time;
		__asm__ __volatile__(															 \
          "xorl %%eax,%%eax\n\t"                       \
          "cpuid\n\t"                                  \
          "rdtsc\n\t"                                  \
          "movl %%eax,(%0)\n\t"                        \
          "movl %%edx,4(%0)\n\t"                       \
          "xorl %%eax,%%eax\n\t"                       \
          "cpuid\n\t"                                  \
          : /* nothing */                              \
          : "S"((&time))                                \
          : "eax", "ebx", "ecx", "edx", "memory");
		return time;
#endif
}

void test1(double x) {
	binary64 b64;
	b64.d = x;
	cout << bitset<64>(b64.ui64) << endl;
}

void test2(int64_t x) {
	double d = ((union{int64_t t; double d;}){x}).d;
	cout << d << endl;
}

int64_t singleTest(double x) {
	binary64 b1,b2;
	//cout << x << endl;
	b1.d = expDouble(x);
	b2.d = exp(x);
	return b1.ui64>b2.ui64?(b1.ui64-b2.ui64):(b2.ui64-b1.ui64);
}

int64_t randTest() {
	srand (time (0));
	int64_t sum = 0;
	for (int64_t i = 0; i < 100000; i++) {
		double x = ((double)((unsigned)rand() | 1) / (double)RAND_MAX)* 100 - 50;
		int64_t diff = singleTest(x);
		sum += diff;
	}
	return sum;
}

void performanceTest() {
	srand (time (0));

	unsigned long long i;

	for (i=0; i<RANDTESTSIZE; i++) {
		float x = ((double)((unsigned)rand() | 1) / (double)RAND_MAX) ;
		//		printf("%llu   %1.34e\n", i,x);
		randtest[i]=x;
	}

	uint64_t t0,t1,t_empty;
	t0 = get_cycle_counter();
	for (i = 0; i < RANDTESTSIZE; i++) {
		double x = randtest[i];
		double y = dummy(x);
	}
	t1 = get_cycle_counter();
	t_empty = t1 - t0;
	cout << "Empty loop 1 cycles : " << (double)(t_empty)/RANDTESTSIZE << endl;


	t0 = get_cycle_counter();
	for (i = 0; i < RANDTESTSIZE; i++) {
		double x = randtest[i];
		double y = exp(x);
	}
	t1 = get_cycle_counter();
	cout << "Libc expf cycles : " << (double)(t1-t0-t_empty)/RANDTESTSIZE << endl;

	t0 = get_cycle_counter();
	for (i = 0; i < RANDTESTSIZE; i++) {
		double x = randtest[i];
		double y = expDouble(x);
	}
	t1 = get_cycle_counter();
	cout << "ExpDouble cycles : " << (double)(t1-t0-t_empty)/RANDTESTSIZE << endl;
}

void test3() {
	double x;

	cin >> x;
	binary64 b1,b2;
	b1.d = expDouble(x);
	b2.d = exp(x);
	cout << b1.d << endl;
	cout << "     : " << "seeeeeeeeeeemmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm" << endl;
	cout << "expd : " << bitset<64>(b1.i64) << endl;
	cout << "exp  : " << bitset<64>(b2.i64) << endl;
	cout << "diff : " << b1.i64 - b2.i64 << endl;
}

int main() {
	cout << setprecision(99);
	//test3();
	
	performanceTest();
	cout << randTest() << endl;
	
	return 0;
}