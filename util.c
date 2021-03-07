/*******************************************
 * File :util.c
 * Author : F. Emre Yazici
 * Creation Date : 03.03.2021
 ********************************************/


#include "util.h"
#include <assert.h>

void cm_init(cmatrix* p, size_t rows, size_t cols, bool init_to_zero) {
  p->rows = rows;
  p->cols = cols;
  p->data = malloc(sizeof(coef) * rows * cols);

  if (init_to_zero) {
    unsigned i,j;
    for (i=0; i < rows; i++)
      for(j=0; j < cols; j++)
        (p->data)[i*cols + j] = 0;
  }
}

void cm_destroy(cmatrix *p) {
  free(p->data);
}

void cm_print(cmatrix *p) {
  int i,j;
  for (i = 0; i < p->rows; i++) {
    printf("\n");
    for(j = 0; j < p->cols; j++)
      printf("%d\t", p->data[i*(p->cols) + j]);
  }
  printf("\n");
}


// Multiply a coefficient matrix with a scalar.
void cm_scale(cmatrix* p, coef scale) {
    int i, j;
    for (i = 0; i < p->rows; i++) {
        for (j = 0; j < p->cols; j++)
            p->data[i * p->cols + j] *= scale;
    }
}

// Multiply two coefficient matrixes and store the result in the left hand side matrix.
void cm_imult(cmatrix* lhs, cmatrix* rhs) {
    assert(lhs->cols == rhs->rows);

    int i, j, k;
    int a, b;

    // initialize a temporary matrix to store the intermediate results
    cmatrix temp;
    cm_init(&temp, lhs->rows, rhs->cols, true);

    for (i = 0; i < lhs->rows; i++) {
        for (j = 0; j < rhs->cols; j++) {
            for (k = 0; k < lhs->cols; k++) {
                a = lhs->data[i * lhs->cols + k];
                b = rhs->data[k * rhs->cols + j];
                temp.data[i * temp.cols + j] = CADD(temp.data[i * temp.cols + j], CMULT(a, b));
            }
        }
    }
    
    // swap the temp matrix with the lhs
    coef* p = lhs->data;
    lhs->data = temp.data;
    temp.data = p;
    lhs->cols = temp.cols;
    lhs->rows = temp.rows;

    // free the old matrix data
    cm_destroy(&temp);
}

// Multiply two coefficient matrixes and return the resulting matrix.
cmatrix cm_mult(cmatrix* lhs, cmatrix* rhs) {
    int i, j, k;
    coef a, b;
    cmatrix temp;
    cm_init(&temp, lhs->rows, rhs->cols, true);
    
    for (i = 0; i < lhs->rows; i++) {
        for (j = 0; j < rhs->cols; j++) {
            for (k = 0; k < lhs->cols; k++) {
                a = lhs->data[i * lhs->cols + k];
                b = rhs->data[k * rhs->cols + j];
                temp.data[i * temp.cols + j] = CADD(temp.data[i * temp.cols + j], CMULT(a, b));
            }
        }
    }    
    return temp;
}
