/*******************************************
 * File :fpmatrix.h
 * Author : F. Emre Yazici
 * Creation Date : 09.03.2021
 ********************************************/


#ifndef FPMATRIX_H_
#define FPMATRIX_H_

#include "util.h"


typedef struct fp_matrix
{
    size_t rows;
    size_t cols;

    fixed_point* data;

} fp_matrix;

/** Allocate the memory initialize (to 0) a fixed point.
*/
void fpm_init(fp_matrix* p, size_t rows, size_t cols);

/** Allocate the memory and initialize (to 1) a fixed point.
*/
void fpm_init_ones(fp_matrix* p, size_t rows, size_t cols);

/** Free the resources held up by a fixed point.
*/
void fpm_destroy(fp_matrix* p);


void fpm_print(fp_matrix* p);


/** Return a deep copy of the coefficient matrix.
*/
fp_matrix fpm_copy(fp_matrix* p);

/** Return transpose of a coefficient matrix
*/
fp_matrix fpm_transposed(fp_matrix* p);

/** Deep copy the contents of the original matrix to the target.
*   Target matrix must be properly initialized to the same size as the original.
*/
void fpm_copy_to(fp_matrix* original, fp_matrix* target);

// Multiply a coefficient matrix with a scalar.
void fpm_scale(fp_matrix* p, fixed_point scale);

// Add a scalar to each element of a fixed point.
void fpm_iadd_scalar(fp_matrix* p, fixed_point scalar);

// Add the contents of two fixed point and store the result in the first one.
void fpm_iadd(fp_matrix* lhs, fp_matrix* rhs);

// Multiply two coefficient matrixes and store the result in the left hand side matrix.
void fpm_imult(fp_matrix* lhs, fp_matrix* rhs);

// Multiply two coefficient matrixes and return the resulting matrix.
fp_matrix fpm_mult(fp_matrix* lhs, fp_matrix* rhs);

// Fill a fixed point with random values uniformly-distributed in the interval [lower,upper].
// Seed is generated from system clock
void fpm_fillrand(fp_matrix* p, fixed_point lower_lim, fixed_point upper_lim);

// Fill a fixed point with normally-distributed random values.
// Seed is generated from system clock
void fpm_fillrandn(fp_matrix* p, double mean, double stdev);

// Fill sparsely a fixed point with random values uniformly-distributed in the interval [lower,upper].
// Number of non-zero elements is approximately equal to density * p->rows * p->cols
// Seed is generated from system clock
void fpm_fillsprand(fp_matrix* p, fixed_point lower_lim, fixed_point upper_lim, double density);

// Fill sparsely a fixed point with normally distributed random values
// Number of non-zero elements is approximately equal to density * p->rows * p->cols
// Seed is generated from system clock
void fpm_fillsprandn(fp_matrix* p, double mean, double stdev, double density);


#endif