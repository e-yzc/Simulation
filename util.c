/*******************************************
 * File :util.c
 * Author : F. Emre Yazici
 * Creation Date : 03.03.2021
 ********************************************/


#include "util.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>

/* return a uniform random value in the range 0..n-1 inclusive 
* taken from https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)Randomization.html
*/
int rand_number16(int n) {
	assert(n > 2);

	int limit;
	int r;

	limit = RAND_MAX - (RAND_MAX % n);

	while ((r = rand()) >= limit);

	return r % n;
}

int rand_number32(int n) {
	return (rand_number16(RAND_MAX) | (rand_number16(RAND_MAX) << 16)) % n;
}

// generate quasi-uniform random double between lower_lim and upper_lim
double rand_db(double lower_lim, double upper_lim) {
	double range = (upper_lim - lower_lim);
	double step = RAND_MAX / range;
	return lower_lim + (rand() / step);
}

// Returns a random fixed point between lower_lim and upper_lim ( both inclusive)
fixed_point rand_fp(fixed_point lower_lim, fixed_point upper_lim) {
	assert(lower_lim < upper_lim);
	static fixed_point randfp;

	do {
		randfp = rand_number32(upper_lim);
	} while (randfp < lower_lim);

	return randfp;
}


/*
 * Normal random numbers generator - Marsaglia algorithm.
 *
 * Taken from https://rosettacode.org/wiki/Statistics/Normal_distribution
 */
double* generate_normal(int n, double mean, double stdev)
{
	int i;
	int m = n + n % 2;
	double* values = (double*)calloc(m, sizeof(double));
	//double average, deviation;

	if (values)
	{
		for (i = 0; i < m; i += 2)
		{
			double x, y, rsq, f;
			do {
				x = 2.0 * rand() / (double)RAND_MAX - 1.0;
				y = 2.0 * rand() / (double)RAND_MAX - 1.0;
				rsq = x * x + y * y;
			} while (rsq >= 1. || rsq == 0.);
			f = sqrt(-2.0 * log(rsq) / rsq);
			values[i] = x * f;
			values[i + 1] = y * f;
		}
	}

	for (i = 0; i < n; i++)
		values[i] = stdev * values[i] + mean;

	return values;
}

fixed_point* fp_generate_normal(int n, double mean, double stdev) {
	fixed_point* fp_values = (fixed_point*)malloc(n * sizeof(fixed_point));
	double* dbl_values = generate_normal(n, mean, stdev);
	int i;
	for (i = 0; i < n; i++)
	{
		fp_values[i] = float_to_fixed(dbl_values[i]);
	}
	free(dbl_values);

	return fp_values;
}


// Taken from https://rosettacode.org/wiki/Statistics/Normal_distribution
double mean(double* values, int n)
{
	int i;
	double s;
	s = 0;

	for (i = 0; i < n; i++)
	{
		s += values[i];
	}
	
	return s / n;
}

// Taken from https://rosettacode.org/wiki/Statistics/Normal_distribution
double stddev(double* values, int n)
{
	int i;
	double average = mean(values, n);
	double s = 0;

	for (i = 0; i < n; i++)
		s += (values[i] - average) * (values[i] - average);
	return sqrt(s / (n - 1));
}


fixed_point fp_mean(fixed_point* values, int n) {
	int i;
	double s = 0;

	for (i = 0; i < n; i++) {
		s += fixed_to_float(values[i]);
	}
	s = s / n;
	return float_to_fixed(s);
}

fixed_point fp_stddev(fixed_point* values, int n) {
	int i;
	double average = fixed_to_float(fp_mean(values, n));
	double s = 0;

	for (i = 0; i < n; i++)
		s += (fixed_to_float(values[i]) - average) * (fixed_to_float(values[i]) - average);
	return float_to_fixed(sqrt(s / (n - 1)));

}

/**
* Basic Lorenz attractor calculation. Returns the distance from the origin.
* source: https://www.algosome.com/articles/lorenz-attractor-programming-code.html
*/
double* lorenz_attractor(unsigned n, double t) {

	double x = 0.1;
	double y = 0;
	double z = 0;

	double a = 10.0;
	double b = 28.0;
	double c = 8.0 / 3.0;

	unsigned i;

	double* result = malloc(sizeof(double) * n);

	//Iterate and update x,y and z locations

	//based upon the Lorenz equations

	for (i = 0; i < n; i++) {

		double xt = x + t * a * (y - x);
		double yt = y + t * (x * (b - z) - y);
		double zt = z + t * (x * y - c * z);

		x = xt;
		y = yt;
		z = zt;

		result[i] = sqrt(x * x + y * y + z * z);
	}

	return result;
}