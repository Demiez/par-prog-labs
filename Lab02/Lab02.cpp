// Lab02.cpp : ВИКОРИСТАННЯ ВЛАСТИВОСТЕЙ СУЧАСНИХ ПРОЦЕСОРІВ ПРИ СТВОРЕННІ ПРОГРАМ. КОНВЕЙЄР


#include <iostream>
#include <omp.h>
// 1. Складіть функції для обчислення кількості позитивних чисел в заданому масиві

int positiveArrayCounter(int* a, int size)
{
	int amount = 0;
	for (int i = 0; i < size; i++)
	{
		if (a[i] >= 0)
			amount++;
	}

	return amount;
}

void allPositiveArray(int N)
{
	int* pa = new int[N];

	for (int i = 0; i < N; i++)
	{
		pa[i] = i + 1;
	}

	double start = omp_get_wtime();
	int amount = positiveArrayCounter(pa, N);
	double finish = omp_get_wtime();
	double result = finish - start;
	printf("\nTime when all numbers positive: %f s", result);
	printf("\nPositive quantity: %d", amount);
}

void allNegativeArray(int N)
{
	int* pa = new int[N];

	for (int i = 0; i < N; i++)
	{
		pa[i] *= -1;
	}

	double start = omp_get_wtime();
	int amount = positiveArrayCounter(pa, N);
	double finish = omp_get_wtime();
	double result = finish - start;
	printf("\nTime when all numbers negative: %f s", result);
	printf("\nPositive quantity: %d", amount);

}

void allRandomArray(int N)
{
	int* pa = new int[N];
	for (int i = 0; i < N; i++)
	{
		int k = rand() % 2 % 2;
		if (k == 1)
			pa[i] *= -1;
	}

	double start = omp_get_wtime();
	int amount = positiveArrayCounter(pa, N);
	double finish = omp_get_wtime();
	double result = finish - start;
	printf("\nTime when numbers random: %f s", result);
	printf("\nPositive quantity: %d", amount);
}

// 2. Складіть функцію для упорядкування чисел методом пухирька.
void incrSorting(int* parr, int length_array)
{
	int temp = 0;

	for (int k = 0; k < length_array; k++)
	{
		for (int i = 0; i < (length_array - 1); i++)
			if (parr[i] > parr[i + 1])
			{

				temp = parr[i];
				parr[i] = parr[i + 1];
				parr[i + 1] = temp;
			}
	}
}

void decrSorting(int* parr, int length_array)
{
	int temp = 0;

	for (int k = 0; k < length_array; k++)
	{
		for (int i = (length_array - 1); i > 0; i--)
			if (parr[i] < parr[i - 1])
			{
				temp = parr[i];
				parr[i] = parr[i - 1];
				parr[i - 1] = temp;
			}
	}
}

void BubbleSorting(int N)
{
	double start, finish, result[2];
	int* pa = new int[N];

	for (int i = 0; i < N; i++)
	{
		pa[i] = rand() / 3;
	}

	start = omp_get_wtime();
	incrSorting(pa, N);
	finish = omp_get_wtime();
	result[0] = finish - start;

	for (int i = 0; i < N; i++)
	{
		pa[i] = rand();
	}

	start = omp_get_wtime();
	decrSorting(pa, N);
	finish = omp_get_wtime();
	result[1] = finish - start;

	printf("\nTime for increment bubble sorting: %f s", result[0]);
	printf("\nTime for decrement bubble sorting: %f s", result[1]);
}

//3. Складіть функцію для множення 2-х поліномів. Першій поліном має довільні коефіцієнти цілого типу, інший, коефіцієнти, які дорівнюють -1, 0, 1. Імовірність усіх значень однакова

void mp(int* a, int* b, int* c, int N)
{
	for (int i = 0; i < N; i++)
	{
		for (int k = 0; k < N; k++)
		{
			c[i + k] += a[i] + b[k];
		}
	}
}

void MultiplyPolynom(int N)
{
	double start, finish, result;
	int* pa = new int[N];
	int* pb = new int[N];
	int* pc = new int[N * 2 - 1]{};

	for (int i = 0; i < N; i++)
	{
		pa[i] = rand() % 51 - 25;
		pb[i] = rand() % 3 - 1;
	}

	start = omp_get_wtime();
	mp(pa, pb, pc, N);
	finish = omp_get_wtime();
	result = finish - start;

	printf("\nTime of function: %f s", result);

}

// 4. Складіть фнкцію для округлення заданого масиву чисел з плаваючою крапкою (дані типу float, double)
void rnd(float* a, int N)
{
	for (int i = 0; i < N; i++)
	{
		a[i] = roundf(a[i]);
	}
}

void RoundArrayOfNum(int N)
{
	double start, finish, result;
	float* pa = new float[N];

	for (int i = 0; i < N; i++)
	{
		pa[i] = (rand() + 0.) / 37;
	}

	start = omp_get_wtime();
	rnd(pa, N);
	finish = omp_get_wtime();
	result = finish - start;

	printf("\nTime of function: %f s", result);
}

int main()
{
    std::cout << "Calculating positive numbers in array =>\n";
	allPositiveArray(50000);
	allNegativeArray(50000);
	allRandomArray(50000);
	/* Результат 1:
	Time when all numbers positive: 0.000140 s
	Positive quantity: 50000
	Time when all numbers negative: 0.000140 s
	Positive quantity: 50000
	Time when numbers random: 0.000349 s
	Positive quantity: 25124
	При всіх однакових знаках на конвеєрі похибка прогнозування менша і, як наслідок, менше витрачається часу. При різних знаках в третьому випадку часу витрачається більше
	*/
	std::cout << "\nUsing bubble sorting (increment/decrement), N=10000 =>";
	BubbleSorting(10000);
	/* Результат 2:
	Using bubble sorting (increment/decrement), N=10000 =>
	Time for increment bubble sorting: 0.401143 s
	Time for decrement bubble sorting: 0.362787 s
	*/
	std::cout << "\nMultiplying polynoms, N=15000 =>";
	MultiplyPolynom(15000);
	/* Результат 3:
	Multiplying polynoms, N=15000 =>
	Time of function: 0.852029 s
	*/
	std::cout << "\nRounding an array of numbers, N=30000 =>";
	RoundArrayOfNum(30000);
	/* Результат 4:
	Rounding an array of numbers, N=30000 =>
	Time of function: 0.001695 s
	*/
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
