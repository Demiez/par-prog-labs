// Lab04.cpp : ПОРІВНЯННЯ МЕТОДІВ СТВОРЕННЯ ПОТОКІВ В WINDOWS І OPEN MP

#include <iostream>
#include <stdio.h>
#include <omp.h>
#include <Windows.h>
// 1. Включити режим Open MP і перевірити успішність його включення.

void CheckOpenMP()
{
	#ifdef _OPENMP
		printf("_OPENMP Defined\n");
	#else
		printf("_OPENMP UnDefined\n");
	#endif
}

// 2. Визначити кількість потоків різними способами. Порівняти отримані значення.

void GetThreads()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	std::cout << "Windows threads: " << si.dwNumberOfProcessors << std::endl;

	int cores = omp_get_max_threads();
	std::cout << "Omp threads: " << cores << std::endl;
}

/*  3. Реалізувати послідовні функції, які обчислюють  значення π (3 різні функції) з заданою точністю.
	4. Визначити найкращу функцію з боку обчислювальної складності.
	5. Визначити кількість ітерацій для отримання максимально можливої точності в разі використання даних типу double.
*/

double BaileyBorweinPlouffe(size_t N)
{
	// Bailey - Borwein - Plouffe formula
	double pi = 0;
	for (size_t i = 0; i < N; i++)
	{
		pi += (1.0 / pow(16, i)) * ((4.0 / (8.0 * i + 1)) - (2.0 / (8.0 * i + 4)) - (1.0 / (8.0 * i + 5)) - (1.0 / (8.0 * i + 6)));
	}
	return pi;
}

double Leybnitz(size_t N)
{
	// Leybnitz formula
	double pi = 0;
	for (size_t i = 1; i <= N; i++)
	{
		pi += 1.0 / ((4.0 * i - 1) * (4.0 * i + 1));
	}
	pi = 4.0 - 8.0 * pi;
	return pi;
}

double Bellard(size_t N)
{
	// Bellard's formula
	double pi = 0;
	for (size_t i = 0; i < N; i++)
	{
		pi += (pow(-1.0, i) / pow(2.0, i * 10)) * (-(32.0 / (4.0 * i + 1)) - (1.0 / (4.0 * i + 3)) + (256.0 / (10.0 * i + 1)) - (64.0 / (10.0 * i + 3)) - (4.0 / (10.0 * i + 5)) - (4.0 / (10.0 * i + 7)) + (1.0 / (10.0 * i + 9)));
	}
	pi = pi * 1.0 / pow(2.0, 6);
	return pi;
}


void GetPINumber(size_t N)
{

	double start, finish, result[3], pis[3];

	start = omp_get_wtime();
	pis[0] = Bellard(N);
	finish = omp_get_wtime();
	result[0] = finish - start;

	start = omp_get_wtime();
	pis[1] = BaileyBorweinPlouffe(N);
	finish = omp_get_wtime();
	result[1] = finish - start;

	start = omp_get_wtime();
	pis[2] = Leybnitz(N);
	finish = omp_get_wtime();
	result[2] = finish - start;

	printf("Etalon	 PI: 3.141592653589793238");
	printf("\nBellards  PI: %.18f", pis[0]);
	printf("\nBailey - Borwein - Plouffe	 PI: %.18f", pis[1]);
	printf("\nLeybnitz PI: %.18f", pis[2]);

	printf("\nBellards  time: %.8f s", result[0]);
	printf("\nBailey - Borwein - Plouffe	 time: %.8f s", result[1]);
	printf("\nLeybnitz time: %.8f s", result[2]);

}

// 6. Реалізувати паралельно найкращу з обраних функцій. Створити за допомогою функції Windows потоки. Обрати кількість ітерацій як для послідовного варанту.
// 7. Зробити висновки по ефективності використання Windows потоків по часу та точності.

typedef struct Params
{
	double pi;
	int i;
	int threads;
	int N;
} MyParams, * PMyParams;

DWORD WINAPI BBP_thread(LPVOID lpParam)
{
	PMyParams mp = (PMyParams)lpParam;
	int k = mp->i, N = mp->N, threads = mp->threads;

	for (size_t i = k * threads; i < i + threads; i++)
	{
		if (i < N)
		{
			mp->pi += (1.0 / pow(16, i)) * ((4.0 / (8.0 * i + 1)) - (2.0 / (8.0 * i + 4)) - (1.0 / (8.0 * i + 5)) - (1.0 / (8.0 * i + 6)));
		}
		else
		{
			break;
		}
	}
	return 0;
}

void ParallelPI(size_t N)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	const int MAX_THREADS = si.dwNumberOfProcessors;

	PMyParams* mp = new PMyParams[MAX_THREADS]{};
	DWORD* dwThreadIdArray = new DWORD[MAX_THREADS];
	HANDLE* hThreadArray = new HANDLE[MAX_THREADS];

	double start, finish, result, * pi = new double(0);

	for (size_t m = 0; m < MAX_THREADS; m++)
	{
		if (m < N) {

			mp[m] = (PMyParams)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MyParams));

			mp[m]->i = m;
			mp[m]->pi = 0;
			mp[m]->threads = MAX_THREADS;
			mp[m]->N = N;

			hThreadArray[m] = CreateThread(
				NULL,
				0,
				BBP_thread,
				mp[m],
				0,
				&dwThreadIdArray[m]
			);
		}
		else
		{
			break;
		}
	}
	start = omp_get_wtime();
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
	finish = omp_get_wtime();
	result = finish - start;

	for (size_t l = 0; l < MAX_THREADS; l++)
	{
		if (l < N)
		{
			*pi += mp[l]->pi;
			CloseHandle(hThreadArray[l]);
		}
		else
		{
			break;
		}

	}
	start = omp_get_wtime();
	double pi2 = BaileyBorweinPlouffe(N);
	finish = omp_get_wtime();
	double result2 = finish - start;

	printf("Calculation of thread PI %.14f", *pi);
	printf("\nCalculation of single PI %.14f", pi2);
	printf("\nThread time: %.14f s", result);
	printf("\nSingle time: %.14f s", result2);

}

// 8. Забезпечити паралельне виконання за допомогою директив Open MP. Порівняти результати з попередніми. Зробити висновки по ефективності використання Open MP потоків.

double OMPBBP(size_t N)
{
	double pi = 0;
#pragma omp parallel for reduction(+:pi)
	for (int i = 0; i < N; i++)
	{
		pi += (1.0 / pow(16, i)) * ((4.0 / (8.0 * i + 1)) - (2.0 / (8.0 * i + 4)) - (1.0 / (8.0 * i + 5)) - (1.0 / (8.0 * i + 6)));
	}

	return pi;
}

void UseOMPforBBP(size_t N)
{
	double start, finish, result[2], pi[2];

	start = omp_get_wtime();
	pi[0] = BaileyBorweinPlouffe(N);
	finish = omp_get_wtime();
	result[0] = finish - start;

	start = omp_get_wtime();
	pi[1] = OMPBBP(N);
	finish = omp_get_wtime();
	result[1] = finish - start;

	printf("\nSingle PI: %.18f", pi[0]);
	printf("\nOMP PI: %.18f", pi[1]);
	printf("\nSingle time: %.18f s", result[0]);
	printf("\nOMP time: %.18f s", result[1]);
}

// 9. Визначте значення прискорення для паралельних виконань програми. Порівняйте ці значення для обох варіантів паралельного виконання.

int main()
{
	std::cout << "Checking OpenMP =>\n";
	CheckOpenMP();
    /* Окрім /openmp також було додано ключ /Zc:twoPhase- з метою уникнення помилки компілятора
		Результат 1:
		Checking OpenMP =>
		_OPENMP Defined
	*/
	std::cout << "\nChecking Threads =>\n";
	GetThreads();
	/* Результат 2:
	Checking Threads =>
	Windows threads: 4
	Omp threads: 4
	*/
	std::cout << "\nCalculating PI =>\n";
	size_t N = 18;
	GetPINumber(N);
	/* Разом 3,4,5 Результат 3:
	Calculating PI =>
	Etalon   PI: 3.141592653589793238
	Bellards  PI: 3.141592653589792228
	Bailey - Borwein - Plouffe       PI: 3.141592653589793116
	Leybnitz PI: 3.168614749571518452
	Bellards  time: 0.00002151 s
	Bailey - Borwein - Plouffe       time: 0.00000342 s
	Leybnitz time: 0.00000147 s
	Висновок 4. Формула Лейбніца є найкращою з боку обчислювальної складності, але результат видає з явною похибкою, відносно інших методів. Найближчою буде формула  Bailey - Borwein - Plouffe.
	Висновок 5. Відмічається зниження точності розрахунків після 14 знаку => кількість ітерацію дорівнює 14
	*/
	std::cout << "\nCalculating Parallel PI =>\n";
	size_t I = 14;
	ParallelPI(I);
	/* Разом 6,7 Візьмемо кількість ітерацій 14 (size_t I = 14;) з огляду на те, що після точність знижується
	   Результат 6:
	   Calculating Parallel PI =>
		Calculation of thread PI 3.14159284961297
		Calculation of single PI 3.14159265358979
		Thread time: 0.00032120004062 s
		Single time: 0.00000342222256 s
		Висновок 7:
		Відмічається збільшення часу виконання за рахунок збільшення складності в потоках. Як наслідок, точність обчислень зменшилась.
	*/
	std::cout << "\nCalculating PI with use of OMP and low N for iterations =>\n";
	UseOMPforBBP(I);
	std::cout << "\nCalculating PI with use of OMP and large N for iterations =>\n";
	UseOMPforBBP(1000000);
	/* 	Результат 8:
		Calculating PI with use of OMP and low N for iterations =>

		Single PI: 3.141592653589793116
		OMP PI: 3.141592653589793116
		Single time: 0.000002933333234978 s
		OMP time: 0.002554444768975372 s
		Calculating PI with use of OMP and large N for iterations =>

		Single PI: 3.141592653589793116
		OMP PI: 3.141592653589793116
		Single time: 0.250258831778410240 s
		OMP time: 0.081048988069596817 s
		Висновок: Точність лишилась без змін.
		Коли використали малу кількість ітерацій (14), послідовне виконання мало перевагу у швидкості виконання.
		Коли використали значно більшу кількість ітерацій (1000000), і послідовне виконання мало перевагу.
	*/
	/*
	Висновок 9:
	Для Swindows(14) = 0.00000342222256 / 0.00032120004062 = 0.010654489810755369
	Для ОMP(14) = 0.000002933333234978 / 0.002554444768975372 = 0.0011483251744584034

	Але при ОMP(1000000) = 0.250258831778410240 / 0.081048988069596817 = 3.087747765135733

	Тобто при використанні малої кількості операцій прискорення не відбувається, але! при зростанні (в нашому випадку 1000000), прискорення очевидне.
	*/
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
