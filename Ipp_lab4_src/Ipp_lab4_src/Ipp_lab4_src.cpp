#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>
#include <functional>
#include <vector>

using namespace std;
using namespace chrono;

using test_func = void(double** matrix,int size);

milliseconds test(function<test_func> func, double** matrix, const int size)
{
	auto start_time = steady_clock::now();
	func(matrix,size);
	auto end_time = steady_clock::now();

	return duration_cast<milliseconds>(end_time - start_time);
}

double** generate_random_matrix(int size)
{
	double** matrix = new double* [size];
	srand(time(0));
	for (size_t i = 0; i < size; i++)
	{
		matrix[i] = new double[size];
		for (size_t j = 0; j < size; j++)
		{
			matrix[i][j] = rand() % 50;
		}
	}

	return matrix;
}

void func_over_matrix(double** matrix, const int size) 
{
    vector<double> row_means(size);
	for (int i = 0; i < size; i++)
	{
		double sum_of_row = 0;
		for (int j = 0; j < size; j++)
		{
			sum_of_row += matrix[i][j];
		}
		row_means[i] = sum_of_row / size;
	}

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			matrix[i][j] += row_means[i];
		}
	}
}

void func_over_matrix_mp(double** matrix, const int size)
{
    vector<double> row_means(size);

#pragma omp parallel for
	for (int i = 0; i < size; i++)
	{
		double sum_of_row = 0;
		for (int j = 0; j < size; j++)
		{
			sum_of_row += matrix[i][j];
		}
		row_means[i] = sum_of_row / size;
	}

#pragma omp parallel for
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			matrix[i][j] += row_means[i];
		}
	}
}

int main()
{
	const int size = 10000;
	double** matrix = generate_random_matrix(size);

	milliseconds default_time, mp_time;

	default_time = test(func_over_matrix, matrix, size);
	mp_time = test(func_over_matrix_mp, matrix, size);

	cout << "default time: " << default_time.count() << "ms\n";
	cout << "mp time: " << mp_time.count() << "ms\n";
	cout << "default - mp: " << default_time.count() - mp_time.count() << "ms\n";
	cout << "default / mp: " << (double)default_time.count() / mp_time.count();
}
