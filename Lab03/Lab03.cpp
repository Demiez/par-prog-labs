// Lab03.cpp : ВИКОРИСТАННЯ SIMD КОМАНД ПРИ РОЗРОБЦІ ПРОГРАМ

#include <iostream>
#include <omp.h>
#include <intrin.h>
#include <emmintrin.h>
#include <stdio.h>
#include <stdint.h>


// 1. Скласти функції для визначення типу процесору, а також можливості використання SSE, AVX, AVX-512 команд різного типу. Усі наступні функції розробляти з використанням SIMD команд з блоком найбільшої довжини, які підтримуються Вашим процесором.

typedef enum {
	INTEL, AMD, OTHER
} ProcessorType;

ProcessorType DefineProcessorType() {
	ProcessorType Type = OTHER;
	char INTELName[13] = "GenuineIntel";
	char AMDName[13] = "AuthenticAMD";
	char ProcessorName[13];
	ProcessorName[12] = 0;
	int Regs[4];
	__cpuid(Regs, 0);
	memcpy(ProcessorName, &Regs[1], 4);
	memcpy(ProcessorName + 4, &Regs[3], 4);
	memcpy(ProcessorName + 8, &Regs[2], 4);
	if (memcmp(INTELName, ProcessorName, 12) == 0)
		Type = INTEL;
	else
	{
		if (memcmp(AMDName, ProcessorName, 12) == 0)
			Type = AMD;
	}
	return Type;
}

typedef enum {
	SSESUPPORT, SSE2SUPPORT, SSE3SUPPORT, SSSE3SUPPORT,
	SSE41SUPPORT, SSE42SUPPORT, AVXSUPPORT, AVX2SUPPORT,
	AVX512SUPPORT
};

unsigned MaxFun(unsigned* ExtFun) {
	int regs[4];
	__cpuidex(regs, 0, 0);
	int res = regs[0];
	__cpuidex(regs, 0x80000000, 0);
	if (ExtFun) *ExtFun = regs[0];
	return res;
}

bool check_properties(uint32_t fun, uint32_t index, uint32_t bit) {
	uint32_t r[4];
	uint32_t mask = 1 << bit;
	__cpuidex((int*)r, fun, 0);
	return (r[index] & mask) == mask;
}

unsigned SIMDSupport() {
	bool AVX512, AVX, SSE, b;
	unsigned  maska = 0;
	unsigned max_fun = MaxFun(0);
	// Перевірка можливості використання команд XSAVE, XRESTOR, XGETBV:
	if (max_fun >= 1) {
		b = check_properties(1, 2, 26) && check_properties(1, 2, 27);
		if (b) {
			unsigned long long res = _xgetbv(0);
			int flags1 = 7 + (7 << 5), flags2 = 7, flags3 = 3;
			AVX512 = (res & flags1) == flags1;
			AVX = (res & flags2) == flags2;
			SSE = (res & flags3) == flags3;
			if (max_fun >= 7 && AVX512) {
				// AVX512
				b = check_properties(7, 1, 16);
				if (b) maska |= (1 << AVX512SUPPORT);
			}
			if (AVX) {
				if (max_fun >= 7) {
					// AVX2
					b = check_properties(7, 1, 5);
					if (b) maska |= 1 << AVX2SUPPORT;
				}
				if (max_fun >= 1) {
					// AVX
					b = check_properties(1, 2, 28);
					if (b)	maska |= 1 << AVXSUPPORT;
				}
			}
			if (SSE) {
				b = check_properties(1, 2, 20);
				if (b) maska |= 1 << SSE42SUPPORT;
				b = check_properties(1, 2, 19);
				if (b) maska |= 1 << SSE41SUPPORT;
				b = check_properties(1, 2, 9);
				if (b) maska |= 1 << SSSE3SUPPORT;
				b = check_properties(1, 2, 0);
				if (b) maska |= 1 << SSE3SUPPORT;
				b = check_properties(1, 3, 26);
				if (b) maska |= 1 << SSE2SUPPORT;
				b = check_properties(1, 3, 25);
				if (b) maska |= 1 << SSESUPPORT;
			}
		}
	}
	return maska;
}

void CheckSIMD()
{
	ProcessorType Type;
	Type = DefineProcessorType();
	switch (Type)
	{
	case INTEL:
		printf("ProcessorType: INTEL\n"); break;
	case AMD:
		printf("ProcessorType: AMD\n"); break;
	case OTHER:
		printf("ProcessorType: OTHER\n"); break;
	}

	unsigned maska = SIMDSupport();
	printf("\nSSE: %i", (maska >> SSESUPPORT) & 1);
	printf("\nSSE2: %i", (maska >> SSE2SUPPORT) & 1);
	printf("\nSSE3: %i", (maska >> SSE3SUPPORT) & 1);
	printf("\nSSE41: %i", (maska >> SSE41SUPPORT) & 1);
	printf("\nSSE42: %i", (maska >> SSE42SUPPORT) & 1);
	printf("\nSSSE3: %i", (maska >> SSSE3SUPPORT) & 1);
	printf("\nAVX: %i", (maska >> AVXSUPPORT) & 1);
	printf("\nAVX2: %i", (maska >> AVX2SUPPORT) & 1);
	printf("\nAVX512: %i", (maska >> AVX512SUPPORT) & 1);
}

/* 2. Скласти функції для обчислення: x[i] = |y[i]| + |z[i]|  (i = 0.. n-1) для даних наступних типів без використання SIMD команд: (суми чисел за модулем)
	цілі довжиною 8 біт;
	цілі довжиною 16 біт;
	цілі довжиною 32 біт;
	цілі довжиною 64 біт;
	з плаваючою крапкою звичайної точності;
	з плаваючою крапкою подвійної точності.

	3. Порівняти обчислювальну складність для цих функцій для довжини масиву 4096 * 4096
	4. Результати виконання пунктів 2, 3 занести в таблицю.
	5. Зробити висновки по ефективності використання SIMD команд та можливостей компілятору для виконання арифметичних операцій.
*/

template  <typename T>
void sum(T* x, T* y, T* z, int N)
{
	for (int i = 0; i < N; N++)
	{
		x[i] = abs(y[i] + z[i]);

	}
}

template  <typename T>
void sumSSE(T* x, T* y, T* z, int Size, int MODE)
{
	size_t const delta = 128 / MODE;
	for (size_t p = 0; p < Size; p += delta)
	{
		__m128i a = _mm_loadu_si128((__m128i*) & y[p]);
		__m128i b = _mm_loadu_si128((__m128i*) & z[p]);
		T* c = new T[delta]{};

		if (MODE == 8)
			c = (T*)_mm_abs_epi8(_mm_add_epi8(a, b)).m128i_i8;
		if (MODE == 16) {
			c = (T*)_mm_abs_epi16(_mm_add_epi16(a, b)).m128i_i16;
		}
		if (MODE == 32)
			c = (T*)_mm_abs_epi32(_mm_add_epi32(a, b)).m128i_i32;
		if (MODE == 64)
			c = (T*)_mm_abs_epi64(_mm_add_epi64(a, b)).m128i_i64; // Illegal instruction error

		for (size_t i = 0; i < delta; i++)
		{
			x[p + i] = c[i];
		}

	}

}

void sumAVX(float* x, const float* y, const float* z, int Size)
{
	int delta = 256 / 32;
	for (size_t i = 0; i < Size; i += delta)
	{
		__m256 a = _mm256_loadu_ps(&y[i]);
		__m256 b = _mm256_loadu_ps(&z[i]);

		_mm256_storeu_ps(&x[i], _mm256_add_ps(a, b));
	}
}

void sumAVX(double* x, const double* y, const double* z, int Size)
{
	int delta = 256 / 64;
	for (size_t i = 0; i < Size; i += delta)
	{
		__m256d a = _mm256_loadu_pd(&y[i]);
		__m256d b = _mm256_loadu_pd(&z[i]);

		_mm256_storeu_pd(&x[i], _mm256_add_pd(a, b));
	}
}

void ResultFuncNoSIMD(int N)
{
	__int8* x = new __int8[N], * y = new __int8[N], * z = new __int8[N] {};
	__int16* x2 = new __int16[N], * y2 = new __int16[N], * z2 = new __int16[N];
	__int32* x3 = new __int32[N], * y3 = new __int32[N], * z3 = new __int32[N];
	__int64* x4 = new __int64[N], * y4 = new __int64[N], * z4 = new __int64[N];
	float* x5 = new float[N], * y5 = new float[N], * z5 = new float[N];
	double* x6 = new double[N], * y6 = new double[N], * z6 = new double[N];

	for (int i = 0; i < N; i++)
	{
		int k = rand() / 5;
		int m = rand() / 7;

		y[i] = k;
		y2[i] = k;
		y3[i] = k;
		y4[i] = k;

		z[i] = m;
		z2[i] = m;
		z3[i] = m;
		z4[i] = m;

		y5[i] = (float)k;
		y6[i] = k + 0.;
		z5[i] = (float)m;
		z6[i] = m + 0.;
		//std::cout << y2[i] << " - " << z2[i] << std::endl;
	}

	double start, finish, result[12];

	start = omp_get_wtime();
	sum(x, y, z, N);
	finish = omp_get_wtime();
	result[0] = finish - start;

	start = omp_get_wtime();
	sum(x2, y2, z2, N);
	finish = omp_get_wtime();
	result[1] = finish - start;

	start = omp_get_wtime();
	sum(x3, y3, z3, N);
	finish = omp_get_wtime();
	result[2] = finish - start;

	start = omp_get_wtime();
	sum(x4, y4, z4, N);
	finish = omp_get_wtime();
	result[3] = finish - start;

	start = omp_get_wtime();
	sum(x5, y5, z5, N);
	finish = omp_get_wtime();
	result[4] = finish - start;

	start = omp_get_wtime();
	sum(x6, y6, z6, N);
	finish = omp_get_wtime();
	result[5] = finish - start;



	start = omp_get_wtime();
	sumSSE(x, y, z, N, 8);
	finish = omp_get_wtime();
	result[6] = finish - start;

	start = omp_get_wtime();
	sumSSE(x2, y2, z2, N, 16);
	finish = omp_get_wtime();
	result[7] = finish - start;

	start = omp_get_wtime();
	sumSSE(x3, y3, z3, N, 32);
	finish = omp_get_wtime();
	result[8] = finish - start;

	/*
	start = omp_get_wtime();
	sumSSE(x4, y4, z4, N, 64);
	finish = omp_get_wtime();
	result[9] = finish - start;
	*/
	
	start = omp_get_wtime();
	sumAVX(x5, y5, z5, N);
	finish = omp_get_wtime();
	result[10] = finish - start;

	start = omp_get_wtime();
	sumAVX(x6, y6, z6, N);
	finish = omp_get_wtime();
	result[11] = finish - start;

	printf("\nInt8 time: %f s", result[0]);
	printf("\nInt16 time: %f s", result[1]);
	printf("\nInt32 time: %f s", result[2]);
	printf("\nInt64 time: %f s", result[3]);
	printf("\nFloat time: %f s", result[4]);
	printf("\nDouble time: %f s", result[5]);

	printf("\nInt8 SSE time: %f s", result[6]);
	printf("\nInt16 SSE time: %f s", result[7]);
	printf("\nInt32 SSE time: %f s", result[8]);
	//printf("\nInt64 SSE time: %f s", result[9]);
	printf("\nFloat AVX time: %f s", result[10]);
	printf("\nDouble AVX time: %f s", result[11]);

}

/* 6. Скласти функції для обчислення  x[i] = √ (y[i])  (i = 0.. n-1) для даних з плаваючою крапкою звичайної та подвійної точності без використання та з використанням SIMD команд.
   7. Порівняти обчислювальну складність для цих функцій для довжини масиву 4096 * 4096.
*/

template  <typename T>
void square(T* x, T* y, int N)
{
	for (int i = 0; i < N; N++)
	{
		if (typeid(T) == typeid(float))
			x[i] = sqrtf(y[i]);
		if (typeid(T) == typeid(double))
			x[i] == sqrt(y[i]);
	}
}

void squareFloatAVX(float* x, const float* y, int Size)
{
	int delta = 256 / 32;
	for (size_t i = 0; i < Size; i += delta)
	{
		__m256 a = _mm256_loadu_ps(&y[i]);

		_mm256_storeu_ps(&x[i], _mm256_sqrt_ps(a));
	}
}

void squareDoubleAVX(double* x, const double* y, int Size)
{
	int delta = 256 / 64;
	for (size_t i = 0; i < Size; i += delta)
	{
		__m256d a = _mm256_loadu_pd(&y[i]);

		_mm256_storeu_pd(&x[i], _mm256_sqrt_pd(a));
	}
}

void SquareSIMD(int N)
{
	float* x = new float[N], * y = new float[N];
	double* x2 = new double[N], * y2 = new double[N];

	for (int i = 0; i < N; i++)
	{
		int k = rand();
		y[i] = (float)k;
		y[i] = k + 0.;
	}

	double start, finish, result[4];

	start = omp_get_wtime();
	square(x, y, N);
	finish = omp_get_wtime();
	result[0] = finish - start;

	start = omp_get_wtime();
	square(x2, y2, N);
	finish = omp_get_wtime();
	result[1] = finish - start;

	start = omp_get_wtime();
	squareFloatAVX(x, y, N);
	finish = omp_get_wtime();
	result[2] = finish - start;

	start = omp_get_wtime();
	squareDoubleAVX(x2, y2, N);
	finish = omp_get_wtime();
	result[3] = finish - start;

	printf("\nFLOAT without SIMD time: %f s", result[0]);
	printf("\nDOUBLE without SIMD time: %f s", result[1]);
	printf("\nFLOAT with use of AVX time: %f s", result[2]);
	printf("\nDOUBLE with use of AVX time: %f s", result[3]);
}

// 9. (Вищий балл) Дослідити ефективність команд зрушення (SIMD) для зрушення даних вправо для студентів з парним номером.

void print_16_num(__m128i var)
{
	uint8_t* val = (uint8_t*)&var;
	printf(" %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i \n",
		val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7], val[8], val[9], val[10], val[11], val[12], val[13], val[14], val[15]);
}
void print_8_num(__m128i var)
{
	uint16_t* val = (uint16_t*)&var;
	printf(" %i %i %i %i %i %i %i %i \n",
		val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7]);
}
void print_4_num(__m128i var)
{
	uint16_t* val = (uint16_t*)&var;
	printf(" %i %i %i %i \n",
		val[0], val[1], val[2], val[3]);
}

int main()
{
    std::cout << "Checking SIMD SSE and AVX =>\n";
	CheckSIMD();
	/* Результат 1:
		SSE: 1
		SSE2: 1
		SSE3: 1
		SSE41: 1
		SSE42: 1
		SSSE3: 1
		AVX: 1
		AVX2: 0
		AVX512: 0
	*/
	std::cout << "\nFunction for different data types 4096*4096 =>";
	int N = 4096 * 4096;
	ResultFuncNoSIMD(N);
	/* Разом 2,3,4,5 - закоменчені рядки, де виникає error. Результат:
		Int8 time: 5.234549 s
		Int16 time: 5.380987 s
		Int32 time: 5.025555 s
		Int64 time: 4.920511 s
		Float time: 4.834545 s	
		Double time: 5.000456 s
		Int8 SSE time: 0.120455 s
		Int16 SSE time: 0.213117 s
		Int32 SSE time: 0.456662 s
		Float AVX time: 0.072541 s
		Double AVX time: 0.121964 s
		Висновок: Таким чином, завдяки використанню SIMD команд час виконання став значно швидшим. Крім того, у випадку int64 SSE => Illegal Instruction Error
	*/
	std::cout << "\n Square without and with SIMD 4096*4096 =>";
	SquareSIMD(N);
	/* Разом 6,7 Результат
		FLOAT time: 31.323399 s
		DOUBLE time: 22.117894 s
		FLOAT AVX time: 0.096722 s
		DOUBLE AVX time: 0.200663 s
		З використанням SIMD команд маємо значне прискорення виконання
	*/
	std::cout << "\n Shift to the right => \n";
	__m128i _16 = _mm_set_epi8(128, 64, 32, 16, 8, 4, 2, 1, 128, 64, 32, 16, 8, 4, 2, 1);
	print_16_num(_mm_srli_si128(_16, 1));

	__m128i _8 = _mm_set_epi16(128, 64, 32, 16, 8, 4, 2, 1);
	print_8_num(_mm_srli_si128(_8, 1));

	__m128i _4 = _mm_set_epi32(128, 64, 32, 16);
	print_4_num(_mm_srli_si128(_4, 1));

	_4 = _mm_set_epi32(128, 64, 32, 16);
	print_4_num(_mm_srli_epi32(_4, 1));
	/*
	Результат
	Shift to the right =>
	 2 4 8 16 32 64 128 1 2 4 8 16 32 64 128 0
	 512 1024 2048 4096 8192 16384 32768 0
	 0 8192 0 16384
	 8 0 16 0
	*/
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
