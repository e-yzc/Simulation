/*******************************************
 * File :util.h
 * Author : F. Emre Yazici
 * Creation Date : 03.03.2021
 ********************************************/

#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define FRACTIONAL_BITS 18
#define INTEGER_BITS 6
#define SIGN_BIT 1
#define TOTAL_BITS (FRACTIONAL_BITS + INTEGER_BITS + SIGN_BIT)

// bitmasks for fixed point truncation: set non-relevant bits to 0
#define FPMASK ((1 << TOTAL_BITS) - 1)
// bitmask for fixed point multiplication: choose only relevant bits.
#define FPMULTMASK	(((1 <<TOTAL_BITS) - (fixed_point)1) << (FRACTIONAL_BITS))

typedef uint32_t fixed_point; // must have at least 2*(FRACTIONAL + INTEGER + SIGN) number of bits for multiplication to be valid

 /// Converts fp -> double
inline double fixed_to_float(fixed_point input)
{
#if SIGN_BIT == 1
	int sign = ((1 << (TOTAL_BITS - 1)) & input);
	if (sign) input = ((input ^ FPMASK) + 1) & FPMASK;
	return (sign ? -1 : 1) * ((double)input / (double)(1 << FRACTIONAL_BITS));
#else
	return ((double)input / (double)(1 << FRACTIONAL_BITS));
#endif
}

/// Converts double -> fp 
inline fixed_point float_to_fixed(double input)
{
#if SIGN_BIT == 1
	int sign = input < 0;
	if (sign) input = -input;
	fixed_point res = (fixed_point)(round(input * (double)(1 << FRACTIONAL_BITS)));
	if (sign) res = ((res^FPMASK) + 1) & FPMASK;
	return res;
#else
	return (fixed_point)(round(input * (1 << FRACTIONAL_BITS)));
#endif
}

inline fixed_point fp_inverse(fixed_point x) {
	return (x ^ FPMASK) + 1;
}


// Add two fixed point numbers
inline fixed_point fp_add(fixed_point lhs, fixed_point rhs) {
	//return (lhs + rhs) & FPMASK;
	return float_to_fixed(fixed_to_float(lhs) + fixed_to_float(rhs));
}

// Multiply two fixed point numbers
inline fixed_point fp_mult(fixed_point lhs, fixed_point rhs) {

	// TODO: debug twos complement multiplication
	//// sign extend the operands
	//int lhs_sign = ((1 << (TOTAL_BITS - 1)) & lhs);
	//if (lhs_sign) lhs = lhs | (((1 << TOTAL_BITS) - 1) << TOTAL_BITS);
	//int rhs_sign = ((1 << (TOTAL_BITS - 1)) & rhs);
	//if (rhs_sign) rhs = rhs | (((1 << TOTAL_BITS) - 1) << TOTAL_BITS);

	//return ((lhs * rhs) & FPMULTMASK) >> FRACTIONAL_BITS;
	return float_to_fixed(fixed_to_float(lhs) * fixed_to_float(rhs));
}

int rand_number16(int n);
int rand_number32(int n);

fixed_point rand_fp(fixed_point, fixed_point);

// Generates a normal distribution set with n elements with mean = 0 stdev = 1
double* generate_normal(int n);

// Generates a normal distribution set with n elements in fixed point format 
fixed_point* fp_generate_normal(int n, double mean, double stdev);

double mean(double* values, int n);

double stddev(double* values, int n);

fixed_point fp_mean(fixed_point* values, int n);

fixed_point fp_stddev(fixed_point* values, int n);

#endif