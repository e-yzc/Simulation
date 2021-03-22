/*******************************************
 * File :dbmatrix.h
 * Author : F. Emre Yazici
 * Creation Date : 22.03.2021
 ********************************************/


#ifndef DBMATRIX_H_
#define DBMATRIX_H_

#include "util.h"


typedef struct db_matrix
{
    size_t rows;
    size_t cols;

    double* data;

} db_matrix;

/** Allocate the memory initialize (to 0) a double.
*/
void dbm_init(db_matrix* p, size_t rows, size_t cols);

/** Allocate the memory and initialize (to 1) a double.
*/
void dbm_init_ones(db_matrix* p, size_t rows, size_t cols);

/** Free the resources held up by a double.
*/
void dbm_destroy(db_matrix* p);


void dbm_print(db_matrix* p);


/** Return a deep copy of the coefficient matrix.
*/
db_matrix dbm_copy(db_matrix* p);

/** Return transpose of a coefficient matrix
*/
db_matrix dbm_transposed(db_matrix* p);

/** Deep copy the contents of the original matrix to the target.
*   Target matrix must be properly initialized to the same size as the original.
*/
void dbm_copy_to(db_matrix* original, db_matrix* target);

// Multiply a coefficient matrix with a scalar.
void dbm_scale(db_matrix* p, double scale);

// Add a scalar to each element of a double.
void dbm_iadd_scalar(db_matrix* p, double scalar);

// Add the contents of two double and store the result in the first one.
void dbm_iadd(db_matrix* lhs, db_matrix* rhs);

// Multiply two coefficient matrixes and store the result in the left hand side matrix.
void dbm_imult(db_matrix* lhs, db_matrix* rhs);

// Multiply two coefficient matrixes and return the resulting matrix.
db_matrix dbm_mult(db_matrix* lhs, db_matrix* rhs);

// Fill a double with random values uniformly-distributed in the interval [lower,upper].
// Seed is generated from system clock
void dbm_fillrand(db_matrix* p, double lower_lim, double upper_lim);

// Fill a double with normally-distributed random values.
// Seed is generated from system clock
void dbm_fillrandn(db_matrix* p, double mean, double stdev);

// Fill sparsely a double with random values uniformly-distributed in the interval [lower,upper].
// Number of non-zero elements is approximately equal to density * p->rows * p->cols
// Seed is generated from system clock
void dbm_fillsprand(db_matrix* p, double lower_lim, double upper_lim, double density);

// Fill sparsely a double with normally distributed random values
// Number of non-zero elements is approximately equal to density * p->rows * p->cols
// Seed is generated from system clock
void dbm_fillsprandn(db_matrix* p, double mean, double stdev, double density);


#endif