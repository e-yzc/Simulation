#include "fpmatrix.h"
#include <assert.h>

void fpm_init(fp_matrix* p, size_t rows, size_t cols) {
	p->rows = rows;
	p->cols = cols;
	p->data = malloc(sizeof(fixed_point) * rows * cols);

	if (p->data == NULL) return;	// in case allocation fails

	unsigned i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			(p->data)[i * cols + j] = 0;

}

void fpm_init_ones(fp_matrix* p, size_t rows, size_t cols) {
	p->rows = rows;
	p->cols = cols;
	p->data = malloc(sizeof(fixed_point) * rows * cols);

	if (p->data == NULL) return;	// in case allocation fails

	unsigned i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			(p->data)[i * cols + j] = 1;
}

void fpm_destroy(fp_matrix* p) {
	free(p->data);
}

void fpm_print(fp_matrix* p) {
	int i, j;
	for (i = 0; i < p->rows; i++) {
		printf("\n");
		for (j = 0; j < p->cols; j++)
			printf("%d\t", p->data[i * (p->cols) + j]);
	}
	printf("\n");
}

fp_matrix fpm_copy(fp_matrix* p) {
	fp_matrix result;
	fpm_init(&result, p->rows, p->cols);

	unsigned i, j;
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			result.data[i * result.cols + j] = p->data[i * p->cols + j];

	return result;
}


fp_matrix fpm_transposed(fp_matrix* p) {
	fp_matrix result;
	fpm_init(&result, p->cols, p->rows);

	unsigned i, j;
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			result.data[j * result.cols + i] = p->data[i * p->cols + j];

	return result;
}


void fpm_copy_to(fp_matrix* original, fp_matrix* target) {
	assert(original->rows == target->rows && original->cols == target->cols);

	unsigned i, j;
	for (i = 0; i < original->rows; i++)
		for (j = 0; j < original->cols; j++)
			target->data[i * target->cols + j] = original->data[i * original->cols + j];
}


// Multiply a fixed point matrix with a scalar.
void fpm_scale(fp_matrix* p, fixed_point scale) {
	unsigned i, j;
	for (i = 0; i < p->rows; i++) {
		for (j = 0; j < p->cols; j++)
			p->data[i * p->cols + j] = fp_mult(p->data[i * p->cols + j], scale);
	}
}

void fpm_iadd_scalar(fp_matrix* p, fixed_point scalar) {
	unsigned i, j;
	for (i = 0; i < p->rows; i++) {
		for (j = 0; j < p->cols; j++)
			p->data[i * p->cols + j] = fp_add(p->data[i * p->cols + j], scalar);
	}
}

void fpm_iadd(fp_matrix* lhs, fp_matrix* rhs) {
	assert(lhs->cols == rhs->cols && lhs->rows == rhs->rows);

	unsigned i, j;
	for (i = 0; i < lhs->rows; i++)
		for (j = 0; j < lhs->cols; j++)
			lhs->data[i * lhs->cols + j] = fp_add(lhs->data[i * lhs->cols + j], rhs->data[i * rhs->cols + j]);
}

// Multiply two fixed point matrixes and store the result in the left hand side matrix.
void fpm_imult(fp_matrix* lhs, fp_matrix* rhs) {
	assert(lhs->cols == rhs->rows);

	unsigned i, j, k;
	fixed_point a, b;

	// initialize a temporary matrix to store the intermediate results
	fp_matrix temp;
	fpm_init(&temp, lhs->rows, rhs->cols, true);

	for (i = 0; i < lhs->rows; i++) {
		for (j = 0; j < rhs->cols; j++) {
			for (k = 0; k < lhs->cols; k++) {
				a = lhs->data[i * lhs->cols + k];
				b = rhs->data[k * rhs->cols + j];
				temp.data[i * temp.cols + j] = fp_add(temp.data[i * temp.cols + j], fp_mult(a, b));
			}
		}
	}

	// swap the temp matrix with the lhs
	fixed_point* p = lhs->data;
	lhs->data = temp.data;
	temp.data = p;
	lhs->cols = temp.cols;
	lhs->rows = temp.rows;

	// free the old matrix data
	fpm_destroy(&temp);
}

// Multiply two fixed point matrixes and return the resulting matrix.
fp_matrix fpm_mult(fp_matrix* lhs, fp_matrix* rhs) {
	int i, j, k;
	fixed_point a, b;
	fp_matrix temp;
	fpm_init(&temp, lhs->rows, rhs->cols, true);

	for (i = 0; i < lhs->rows; i++) {
		for (j = 0; j < rhs->cols; j++) {
			for (k = 0; k < lhs->cols; k++) {
				a = lhs->data[i * lhs->cols + k];
				b = rhs->data[k * rhs->cols + j];
				temp.data[i * temp.cols + j] = fp_add(temp.data[i * temp.cols + j], fp_mult(a, b));
			}
		}
	}
	return temp;
}



void fpm_fillrand(fp_matrix* p, fixed_point lower_lim, fixed_point upper_lim) {
	assert(lower_lim < upper_lim);

	unsigned i, j;

	// set the seed
	srand((unsigned)time(NULL));

	// populate with random number
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			(p->data)[i * p->cols + j] = rand_coef(lower_lim, upper_lim);
}

void fpm_fillsprand(fp_matrix* p, fixed_point lower_lim, fixed_point upper_lim, double density) {

	assert(lower_lim < upper_lim);

	// set the seed
	srand((unsigned)time(NULL));

	// number of non-zero elements
	size_t nb_nonzeros;
	nb_nonzeros = (size_t)(density * p->rows * p->cols * 2 + 1) / 2;

	// fill the matrix
	size_t i, j;
	for (; nb_nonzeros > 0; nb_nonzeros--) {
		// choose a random element that's not already non-zero
		do {
			i = rand_number(0, p->rows - 1);
			j = rand_number(0, p->cols - 1);
		} while (p->data[i * p->cols + j] != 0);

		// to ensure the element is always non-zero regardless of upper and lower lim
		while (p->data[i * p->cols + j] == 0) {
			p->data[i * p->cols + j] = rand_coef(lower_lim, upper_lim);
		}
	}
}
