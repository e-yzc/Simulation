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


typedef uint16_t coef;
typedef struct cmatrix
{
    size_t rows;
    size_t cols;

    coef* data;

} cmatrix;

void cm_init(cmatrix* p, size_t rows, size_t cols, bool init_to_zero);
void cm_destroy(cmatrix* p);
void cm_print(cmatrix* p);

// Multiply a coefficient matrix with a scalar.
void cm_scale(cmatrix* p, coef scale);
// Multiply two coefficient matrixes and store the result in the left hand side matrix.
void cm_imult(cmatrix* lhs, cmatrix* rhs);
// Multiply two coefficient matrixes and return the resulting matrix.
cmatrix cm_mult(cmatrix* lhs, cmatrix* rhs);

#endif