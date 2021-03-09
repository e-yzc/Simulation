/*******************************************
 * File :util.c
 * Author : F. Emre Yazici
 * Creation Date : 03.03.2021
 ********************************************/


#include "util.h"
#include <assert.h>
#include <time.h>
#include <math.h>


double fixed_to_float(fixed_point input)
{
#if SIGN_BIT == 1
	int sign = (1 << (INTEGER_BITS + FRACTIONAL_BITS) & input);
	if (sign) input = input ^ (1 << (INTEGER_BITS + FRACTIONAL_BITS));
	return (sign ? -1 : 1) * ((double)input / (double)(1 << FRACTIONAL_BITS));
#else
	return ((double)input / (double)(1 << FRACTIONAL_BITS));
#endif
}


fixed_point float_to_fixed(double input)
{
#if SIGN_BIT == 1
	int sign = input < 0;
	if (sign) input = -input;
	return (sign ? 1 << (INTEGER_BITS + FRACTIONAL_BITS) : 0) | (fixed_point)(round(input * (1 << FRACTIONAL_BITS)));
#else
	return (fixed_point)(round(input * (1 << FRACTIONAL_BITS)));
#endif
}


fixed_point fp_add(fixed_point lhs, fixed_point rhs) {
	return float_to_fixed(fixed_to_float(lhs) + fixed_to_float(rhs));
}

fixed_point fp_mult(fixed_point lhs, fixed_point rhs) {
	return float_to_fixed(fixed_to_float(lhs) * fixed_to_float(rhs));
}

// Returns a random number between lower_lim and upper_lim ( both inclusive)
// Seed must be set by srand() beforehand.
int rand_number(lower_lim, upper_lim) {
	assert(lower_lim < upper_lim);
	return lower_lim + rand() % (upper_lim + 1 - lower_lim);
}

// Returns a random fixed point between lower_lim and upper_lim ( both inclusive)
// Seed must be set by srand() beforehand.
fixed_point rand_coef(fixed_point lower_lim, fixed_point upper_lim) {
	assert(lower_lim < upper_lim);
	return rand_number(lower_lim, upper_lim);
}




/*
 * Normal random numbers generator - Marsaglia algorithm.
 * 
 * Taken from https://rosettacode.org/wiki/Statistics/Normal_distribution
 */
double* generate_normal(int n)
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
	return values;
}