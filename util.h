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

#define MADD(lhs, rhs, mask) ((lhs + rhs) & mask)     // addition with bit masking to simulate custom precision 
#define MMULT(lhs,rhs,mask) ((lhs * rhs) & mask)      // multiplication with bit masking to simulate custom precision

#define CMASK 0b111111                                // bitmask for coefficient arithmetic
#define CADD(lhs,rhs) (MADD(lhs,rhs,CMASK))           // coefficient add with masking
#define CMULT(lhs,rhs) (MMULT(lhs,rhs,CMASK))         // coefficient multiplicate with masking

#define MAX_COEF CMASK
#define MIN_COEF 0


#define FRACTIONAL_BITS 5
#define INTEGER_BITS 6
#define SIGN_BIT 1

typedef uint16_t fixed_point;

 /// Converts fp -> double
double fixed_to_float(fixed_point input);

/// Converts double -> fp 
fixed_point float_to_fixed(double input);

// Add two fixed point numbers
fixed_point fp_add(fixed_point lhs, fixed_point rhs);

// Multiply two fixed point numbers
fixed_point fp_mult(fixed_point lhs, fixed_point rhs);

fixed_point rand_fp(fixed_point, fixed_point);

// Generates a normal distribution set with n elements with mean = 0 stdev = 1
double* generate_normal(int n);

// Generates a normal distribution set with n elements in fixed point format 
fixed_point* fp_generate_normal(int n, double mean, double stdev);

#endif