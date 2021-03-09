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

typedef uint16_t coef;
typedef struct cmatrix
{
    size_t rows;
    size_t cols;

    coef* data;

} cmatrix;

/** Allocate the memory initialize (to 0) a cmatrix.
*/
void cm_init(cmatrix* p, size_t rows, size_t cols);

/** Allocate the memory and initialize (to 1) a cmatrix.
*/
void cm_init_ones(cmatrix* p, size_t rows, size_t cols);

/** Free the resources held up by a cmatrix.
*/
void cm_destroy(cmatrix* p);


void cm_print(cmatrix* p);


/** Return a deep copy of the coefficient matrix.
*/
cmatrix cm_copy(cmatrix* p);

/** Return transpose of a coefficient matrix 
*/
cmatrix cm_transposed(cmatrix* p);

/** Deep copy the contents of the original matrix to the target.
*   Target matrix must be properly initialized to the same size as the original.
*/ 
void cm_copy_to(cmatrix* original, cmatrix* target);

// Multiply a coefficient matrix with a scalar.
void cm_scale(cmatrix* p, coef scale);

// Add a scalar to each element of a cmatrix.
void cm_iadd_scalar(cmatrix* p, coef scalar);

// Add the contents of two cmatrix and store the result in the first one.
void cm_iadd(cmatrix* lhs, cmatrix* rhs);

// Multiply two coefficient matrixes and store the result in the left hand side matrix.
void cm_imult(cmatrix* lhs, cmatrix* rhs);

// Multiply two coefficient matrixes and return the resulting matrix.
cmatrix cm_mult(cmatrix* lhs, cmatrix* rhs);

// Fill a cmatrix with random values normally-distributed in the interval [lower,upper].
// Seed is generated from system clock
void cm_fillrand(cmatrix* p, coef lower_lim, coef upper_lim);

// Fill sparsely a cmatrix with random values normally-distributed in the interval [lower,upper].
// Number of non-zero elements is approximately equal to density * p->rows * p->cols
// Seed is generated from system clock
void cm_fillsprand(cmatrix* p, coef lower_lim, coef upper_lim, double density);

#endif