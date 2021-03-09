/*******************************************
 * File :util.c
 * Author : F. Emre Yazici
 * Creation Date : 03.03.2021
 ********************************************/


#include "util.h"
#include <assert.h>
#include <time.h>

void cm_init(cmatrix* p, size_t rows, size_t cols) {
	p->rows = rows;
	p->cols = cols;
	p->data = malloc(sizeof(coef) * rows * cols);

	if (p->data == NULL) return;	// in case allocation fails

	unsigned i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			(p->data)[i * cols + j] = 0;

}

void cm_init_ones(cmatrix* p, size_t rows, size_t cols) {
	p->rows = rows;
	p->cols = cols;
	p->data = malloc(sizeof(coef) * rows * cols);

	if (p->data == NULL) return;	// in case allocation fails

	unsigned i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			(p->data)[i * cols + j] = 1;
}

void cm_destroy(cmatrix* p) {
	free(p->data);
}

void cm_print(cmatrix* p) {
	int i, j;
	for (i = 0; i < p->rows; i++) {
		printf("\n");
		for (j = 0; j < p->cols; j++)
			printf("%d\t", p->data[i * (p->cols) + j]);
	}
	printf("\n");
}

cmatrix cm_copy(cmatrix* p) {
	cmatrix result;
	cm_init(&result, p->rows, p->cols);

	unsigned i, j;
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			result.data[i * result.cols + j] = p->data[i * p->cols + j];

	return result;
}


cmatrix cm_transposed(cmatrix* p) {
	cmatrix result;
	cm_init(&result, p->cols, p->rows);

	unsigned i, j;
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			result.data[j * result.cols + i] = p->data[i * p->cols + j];

	return result;
}


void cm_copy_to(cmatrix* original, cmatrix* target) {
	assert(original->rows == target->rows && original->cols == target->cols);

	unsigned i, j;
	for (i = 0; i < original->rows; i++)
		for (j = 0; j < original->cols; j++)
			target->data[i * target->cols + j] = original->data[i * original->cols + j];
}


// Multiply a coefficient matrix with a scalar.
void cm_scale(cmatrix* p, coef scale) {
	unsigned i, j;
	for (i = 0; i < p->rows; i++) {
		for (j = 0; j < p->cols; j++)
			p->data[i * p->cols + j] = CMULT(p->data[i * p->cols + j], scale);
	}
}

void cm_iadd_scalar(cmatrix* p, coef scalar) {
	unsigned i, j;
	for (i = 0; i < p->rows; i++) {
		for (j = 0; j < p->cols; j++)
			p->data[i * p->cols + j] = CADD(p->data[i * p->cols + j], scalar);
	}
}

void cm_iadd(cmatrix* lhs, cmatrix* rhs) {
	assert(lhs->cols == rhs->cols && lhs->rows == rhs->rows);

	unsigned i, j;
	for (i = 0; i < lhs->rows; i++)
		for (j = 0; j < lhs->cols; j++)
			lhs->data[i * lhs->cols + j] = CADD(lhs->data[i * lhs->cols + j], rhs->data[i * rhs->cols + j]);
}

// Multiply two coefficient matrixes and store the result in the left hand side matrix.
void cm_imult(cmatrix* lhs, cmatrix* rhs) {
	assert(lhs->cols == rhs->rows);

	unsigned i, j, k;
	coef a, b;

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

// Returns a random number between lower_lim and upper_lim ( both inclusive)
// Seed must be set by srand() beforehand.
int rand_number(lower_lim, upper_lim) {
	assert(lower_lim < upper_lim);
	return lower_lim + rand() % (upper_lim + 1 - lower_lim);
}

// Returns a random coefficient between lower_lim and upper_lim ( both inclusive)
// Seed must be set by srand() beforehand.
coef rand_coef(coef lower_lim, coef upper_lim) {
	assert(lower_lim < upper_lim);
	assert(lower_lim >= MIN_COEF && upper_lim <= MAX_COEF);
	return rand_number(lower_lim, upper_lim);
}


void cm_fillrand(cmatrix* p, coef lower_lim, coef upper_lim) {
	assert(lower_lim < upper_lim);

	unsigned i, j;

	// set the seed
	srand((unsigned)time(NULL));

	// populate with random number
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			(p->data)[i * p->cols + j] = rand_coef(lower_lim, upper_lim);
}

void cm_fillsprand(cmatrix* p, coef lower_lim, coef upper_lim, double density) {

	assert(lower_lim < upper_lim);

	// set the seed
	srand((unsigned)time(NULL));

	// number of non-zero elements
	size_t nb_nonzeros;
	nb_nonzeros = (size_t)( density * p->rows * p->cols * 2 + 1) / 2;

	// fill the matrix
	size_t i, j;
	for (; nb_nonzeros > 0; nb_nonzeros--) {
		// choose a random element that's not already non-zero
		do {
			i = rand_number(0, p->rows - 1);
			j = rand_number(0, p->cols - 1);
		} while (p->data[i*p->cols + j] != 0);

		// to ensure the element is always non-zero regardless of upper and lower lim
		while (p->data[i * p->cols + j] == 0) {
			p->data[i * p->cols + j] = rand_coef(lower_lim, upper_lim);
		}
	}
}
