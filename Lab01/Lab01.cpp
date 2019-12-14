// Lab01.cpp : МЕТОДИ ВИЗНАЧЕННЯ ОБЧИСЛЮВАЛЬНОЇ СКЛАДНОСТІ ПРОГРАМ

#include <iostream>
#include <windows.h>
#include <time.h>
#include <chrono>
#include <omp.h>


// 1.	Для функції time визначте дату останнього використання 32 бітного часу. Для цього, наприклад,  перетворіть час 0x7FFFFFFF в форматі time_t в формат SystemTime.


void ConvertTimeTToSystemTime(PSYSTEMTIME st, time_t* tt) {
	tm my_tm;
	localtime_s(&my_tm, tt);
	memset(st, 0, sizeof(SYSTEMTIME));
	st->wYear = my_tm.tm_year + 1900;
	st->wMonth = my_tm.tm_mon + 1;
	st->wDay = my_tm.tm_mday;
}

// 2. Експериментально визначте точність обчислення часу при використанні функцій з табл. 1.1. Запишіть отримані результати в табл. 1.1. 

int accuracy_time()
{
	time_t start = time(0), finish = start;
	while (finish == start)
	{
		finish = time(0);
	}
	return finish - start;
}

double accuracy_clock()
{
	clock_t start = clock(), finish = start;
	while ((finish - start) == 0)
	{
		finish = clock();
	}
	return (finish - start + 0.) / CLOCKS_PER_SEC;
}

double GetAccuracySystemTimeAsFileTime()
{
	FILETIME Start, Finish;
	GetSystemTimeAsFileTime(&Start);
	do {
		GetSystemTimeAsFileTime(&Finish);
	} while (Finish.dwLowDateTime == Start.dwLowDateTime);
	return (Finish.dwLowDateTime - Start.dwLowDateTime + 0.) / 10000000;
}

double GetAccuracySystemTimePreciseAsFileTime()
{
	FILETIME Start, Finish;
	GetSystemTimePreciseAsFileTime(&Start);
	do {
		GetSystemTimePreciseAsFileTime(&Finish);
	} while (Finish.dwLowDateTime == Start.dwLowDateTime);
	return (Finish.dwLowDateTime - Start.dwLowDateTime + 0.) / 10000;
}

double GetAccuracyTickCount()
{
	DWORD Start, Finish;
	for (int i = 0; i < 10; i++)
	{
		Start = GetTickCount();
		Finish = GetTickCount();
		while (Start == Finish)
			Finish = GetTickCount();
	}
	return (0. + (Finish - Start)) / 1000;
}

typedef unsigned __int64 uint64_t;

double GetAccuracyRDTSC()
{
	uint64_t Start, Finish;
	Start = __rdtsc();
	do {
		Finish = __rdtsc();
	} while (Finish - Start == 0);
	return (Finish - Start + 0.);
}

double GetAccuracyQuary()
{
	LARGE_INTEGER liStart, liFinish;
	QueryPerformanceCounter(&liStart);
	do {
		QueryPerformanceCounter(&liFinish);
	} while ((liFinish.QuadPart - liStart.QuadPart) == 0);
	LARGE_INTEGER liFreq;
	QueryPerformanceFrequency(&liFreq);
	return (double)(liFinish.QuadPart - liStart.QuadPart) / liFreq.QuadPart;
}

double accuracy_chrono() {
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point finish = start;
	while ((finish - start).count() == 0) {
		finish = std::chrono::high_resolution_clock::now();
	};
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start);
	return time_span.count();
}

double accuracy_openmp()
{
	double start = omp_get_wtime(), finish;
	do {
		finish = omp_get_wtime();
	} while (finish - start == 0);
	return finish - start;
}

void GetAccuracy() {
	for (int i = 0; i < 10; i++)
	{
		printf("\nAccuracy time: %i s", accuracy_time());
		printf("\nAccuracy clock: %2.4f s", accuracy_clock());
		printf("\nAccuracy GetSystemTimeAsFileTime: %2.4f s", GetAccuracySystemTimeAsFileTime());
		printf("\nAccuracy GetSystemTimePreciseAsFileTime: %2.e s", GetAccuracySystemTimePreciseAsFileTime());
		printf("\nAccuracy GetTickCount: %2.4f s", GetAccuracyTickCount());
		printf("\nAccuracy RDTSC: %2.4f tacts", GetAccuracyRDTSC());
		printf("\nAccuracy QueryPerfomanceCounter: %2.e s", GetAccuracyQuary());
		printf("\nAccuracy chrono: %2.e s", accuracy_chrono());
		printf("\nAccuracy omp_get_wtime: %2.e s", accuracy_openmp());
	}
}

// 3. Перевірте використання функцій __rdtsc та  QueryPerformanceCounter для визначення часу додавання масиву чисел розміром 1000 елементів. Порівняйте результати

void summary(int arr[])
{
	int summ = 0;
	for (int i = 0; i < 1000; i++)
	{
		summ += arr[i];
	}
}

void Function1000Check() {
	std::cout << "\nFunction1000Check =>";
	int* arr = new int[1000];

	for (int i = 0; i < 1000; i++)
	{
		arr[i] = rand() % 5;
	}

	uint64_t Start, Finish;
	Start = __rdtsc();
	summary(arr);
	Finish = __rdtsc();
	double result = Finish - Start;

	LARGE_INTEGER liStart, liFinish;
	QueryPerformanceCounter(&liStart);
	summary(arr);
	QueryPerformanceCounter(&liFinish);
	LARGE_INTEGER liFreq;
	QueryPerformanceFrequency(&liFreq);
	double result2 = (double)(liFinish.QuadPart - liStart.QuadPart);

	printf("\nRDTSC : %2.4f tacts", result);
	printf("\nQuery : %2.4f tacts", result2);
}

// 4. Для задачі додавання масиву чисел розміром 100000, 200000, 300000 елементів використайте абсолютний та відносний вимір часу. Для абсолютного виміру використовувати функцію omp_get_wtime. Для відносного визначте кількість циклів додавання , які можна виконати за 2 с для кожного розміру. Вимір виконувати функцією GetTickCount. Порівняйте відношення часу T(200000)/T (100000), T(300000)/T (100000) для обох методів. Зробить висновки по можливості використання  відносного виміру часу.

void summary(int arr[], int size)
{
	int summ = 0;
	for (int i = 0; i < size; i++)
	{
		summ += arr[i];
	}

}

double omp_time(int size)
{
	int* a = new int[size] {};
	for (int i = 0; i < size; i++)
	{
		a[i] = rand() % 5;
	}
	double start = omp_get_wtime(), finish;
	summary(a, size);
	finish = omp_get_wtime();
	return finish - start;

}

void GetTickCounts() 
{
	std::cout << "\nGetTickCounts =>";
	double omp_results[3] = { omp_time(10000), omp_time(20000), omp_time(30000) };

	double clock_results[3] = {};
	for (int i = 0; i < 3; i++)
	{
		int Count = 0;
		int size = (i + 1) * 10000;
		int* a = new int[size] {};
		for (int i = 0; i < size; i++)
		{
			a[i] = rand() % 5;
		}
		DWORD start = GetTickCount(), finish;
		do {
			summary(a, (i + 1) * 10000);
			finish = GetTickCount();
			Count++;
		} while (finish - start < 2000);
		delete a;
		clock_results[i] = 2000.0 / Count;
	}

	printf("\nT(20000)/T(10000) omp: %f s", omp_results[1] / omp_results[0]);
	printf("\nT(20000)/T(10000) clock: %f s", clock_results[1] / clock_results[0]);
	printf("\nT(30000)/T(10000) omp: %f s", omp_results[2] / omp_results[0]);
	printf("\nT(30000)/T(10000) clock: %f s", clock_results[2] / clock_results[0]);
}

// 5. Складіть функції для обчислення множення для 2-х квадратних матриць. Перша функція не використовує об’єкти, інша використовує.
// 6. Визначте формулу для обчислення O(n).
// 7. Перевірте експериментально цю формулу для різних n у разі невикористання та використання об’єктів. Поясніть отримані результати для матриць розміром 512, 1024, 2048
// 8. Задайте режим Release при виконаннії додатків і виконайте попередній пункт. Поясніть отримані результати.
// 9. Дослідіть вплив типу даних (int8, int16, int32, __int64, float, double) для розміру матриць n = 1024. Отримані результати занести в таблицю. Зробіть висновки по впливу типу даних. 
// 10. Якщо є технічна можливість, виконайте пункт 9 за допомогою засобів Performance для Visual Studio.
class Matrix
{
public:
	int** pa;
	int N;
	int** multiply(Matrix* mb);
	Matrix(int n);
};

Matrix::Matrix(int n) : N(n)
{
	pa = new int* [N];
	for (int m = 0; m < N; m++)
	{
		pa[m] = new int[N] {};
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			pa[i][j] = rand() % 5;
		}
	}
}

int** Matrix::multiply(Matrix* mb)
{
	int** pc = new int* [N];
	for (int m = 0; m < N; m++)
	{
		pc[m] = new int[N] {};
	}

	int i, j, k;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
			for (k = 0; k < N; k++)
				pc[i][k] += pa[i][k] * mb->pa[k][j];
	}

	return pc;
}

void mul_ijk(int** pc, int** pa, int** pb, int N)
{
	int i, j, k;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
			for (k = 0; k < N; k++)
				pc[i][k] += pa[i][k] * pb[k][j];
	}
}

void CalculateMatrix(__int8 N) 
{
	std::cout << "\nCalculateMatrix =>";
	int** pa = new int* [N];
	int** pb = new int* [N];
	int** pc = new int* [N];

	for (int m = 0; m < N; m++)
	{
		pa[m] = new int[N] {};
		pb[m] = new int[N] {};
		pc[m] = new int[N] {};
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			pa[i][j] = rand() % 5;
			pb[i][j] = rand() % 5;
		}
	}
	double start = omp_get_wtime(), finish, result;
	mul_ijk(pc, pa, pb, N);
	finish = omp_get_wtime();
	result = finish - start;

	Matrix* pma = new Matrix(N);
	Matrix* pmb = new Matrix(N);

	double start2 = omp_get_wtime(), finish2, result2;
	int** pc2 = pma->multiply(pmb);
	finish2 = omp_get_wtime();
	result2 = finish2 - start2;
	delete pma;
	delete pmb;

	printf("\nfunction: %2.4f", result);
	printf("\nobject: %2.4f", result2);
}

int main()
{
	SYSTEMTIME st;
	time_t tt = (time_t)0x7FFFFFFF;
	ConvertTimeTToSystemTime(&st, &tt);
	printf("%4.4d:%2.2d:%2.2d\n", st.wYear, st.wMonth, st.wDay);
	// Результат 1: 2038:01:19, тобто дата останнього використання 32 бітного часу – 19 січня 2038 року.

	//GetAccuracy();
	// Результат 2: викликавши void функцію GetAccuracy із застосуванням в ній циклу for з інкрементом до 10 отримано наступні результати:
	/*
	Accuracy time: 1 s
	Accuracy clock: 0.0010 s
	Accuracy GetSystemTimeAsFileTime: 0.0010 s
	Accuracy GetSystemTimePreciseAsFileTime: 2e-03 s (відмічається девіація 1e-03 s, 2e-03 s, 3e-03 s, 9e-04 s)
	Accuracy GetTickCount: 0.0160 s
	Accuracy RDTSC: 66.0000 tacts (відмічається девіація 66.000, 68.000, 72.000, 73.000, 76.000, 81.000 tacts) 
	Accuracy QueryPerfomanceCounter: 1e-06 s
	Accuracy chrono: 2e-06 s (відмічається девіація 2е-06 s, 3e-06 s)
	Accuracy omp_get_wtime: 1e-06 s
	*/

	Function1000Check();
	/* Результат 3: 
	RDTSC : 10548.0000 tacts 
	Query: 49.0000 tacts
	Відмічається значна розбіжність результатів, що веде за собою висновок про значну затратність RDTSC 	
	*/

	//GetTickCounts();
	/* Результат 4: Отримано досить схожі результати, тому можна передбачити допустимість використання
	GetTickCounts =>
	T(20000)/T(10000) omp: 1.967213 s
	T(20000)/T(10000) clock: 2.044364 s
	T(30000)/T(10000) omp: 3.114754 s
	T(30000)/T(10000) clock: 3.082754 s
	*/

	CalculateMatrix(64);
	CalculateMatrix(512);
	CalculateMatrix(1024);
	CalculateMatrix(2048);


	/* 5,6,7,8,9,10 - Викнонано разом
	Результат 5,6,7
	для 5 завдання припустимо, що є матриця N 64 (передамо в якості аргументу 64):
	CalculateMatrix =>
	function: 0.0013
	object: 0.0014
	для завдання 7
	512: 
	CalculateMatrix =>
	function: 1.0567
	object: 1.0375
	1024
	CalculateMatrix =>
	function: 13.6877
	object: 14.6543
	2048
	CalculateMatrix =>
	function: 158.5530
	object: 195.9645
	для 6 завдання Для мультиплікації двох матриць необхідна notation O(n^3), беручи до уваги той факт, що виконується відповідно три цикли по n дій для кожного. Досягти O(n^2) неможливо, найшвидший відомий алгоритм для мультиплікації матриць - Копперсмита—Винограда із асимптотичною складністю O(n^2.3737).
	для 7 завдання як вже зазначалось, досягти асимптотичної складності O(n^2) неможливо, тому крім зростання кількості операцій для матриць 512-1024-2048 і наявності O(n^3), маэмо нелінійне зростання часу виконання обчислень, замість наприклад різниці при 512 до 1024 у 8 разів.

	для 8 завдання використано Release Configuration:
	CalculateMatrix =>
	function: 0.0004
	object: 0.0007
	CalculateMatrix =>
	function: 0.5111
	object: 0.6147
	CalculateMatrix =>
	function: 8.1989
	object: 10.1066
	CalculateMatrix =>
	function: 121.4246
	object: 143.0191
	Прискорення виконання пов'язане з тим, що відповідно офіційної документації(https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-set-debug-and-release-configurations?view=vs-2019), release конфігурація не має інформації про налагодження програми і є повністю оптимізованою.

	для 9 завдання використані різні типи даних для матриці 1024 int8, int16, int32, __int64, float, double (аргумент та команди функції змінено назад на int для демонстрації попередніх завдань)
	Результати
	__int8	12.9677
	__int16	13.1252
	__int32	13.4119
	__int64	14.2433
	float	13.9117
	double	14.9365

	для 10 завдання в папці lab01 для одного типу даних використано Performance Instrumentation Profiler(Report20191214-1315.vsps) 
	*/

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
