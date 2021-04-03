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

	fixed_point fp_one = float_to_fixed(1.0);

	unsigned i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			(p->data)[i * cols + j] = fp_one;
}

void fpm_destroy(fp_matrix* p) {
	if (p->data == NULL) return;
	free(p->data);
}

void fpm_print(fp_matrix* p) {
	int i, j;
	for (i = 0; i < p->rows; i++) {
		printf("\n");
		for (j = 0; j < p->cols; j++) {
			printf("%.4f\t", fixed_to_float(p->data[i * (p->cols) + j]));
		}
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
	fpm_init(&temp, lhs->rows, rhs->cols);

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


	// populate with random number
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++) {
			fixed_point rand_nb = rand_fp(lower_lim, upper_lim);
			(p->data)[i * p->cols + j] = rand_nb;
		}
	
}



void fpm_fillrandn(fp_matrix* p, double mean, double stdev) {

	unsigned i, j;
	unsigned nb_vals;

	fixed_point* normal_set;

	// generate normal set
	nb_vals = p->rows * p->cols;
	normal_set = fp_generate_normal(nb_vals, mean, stdev);

	// populate the matrix
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			(p->data)[i * p->cols + j] = normal_set[i * p->cols + j];


	free(normal_set);
}

void fpm_fillsprand(fp_matrix* p, fixed_point lower_lim, fixed_point upper_lim, double density) {

	assert(lower_lim < upper_lim);

	// number of non-zero elements
	size_t nb_nonzeros;
	nb_nonzeros = (size_t)(density * p->rows * p->cols * 2 + 1) / 2;
	
	size_t i, j;
	if (nb_nonzeros >= p->cols * p->rows) {
		for (i = 0; i < p->rows*p->cols; i++)
			p->data[i] = rand_fp(lower_lim, upper_lim);
		return;
	}

	// fill the matrix
	for (; nb_nonzeros > 0;) {
		nb_nonzeros--;

		// choose a random element that's not already non-zero
		do {
			i = p->rows > 1 ? rand_number16(p->rows - 1) : 0;
			j = p->cols > 1 ? rand_number16(p->cols - 1) : 0;
		} while (p->data[i * p->cols + j] != 0);
			
		p->data[i * p->cols + j] = rand_fp(lower_lim, upper_lim);
	}
}

void fpm_fillsprandn(fp_matrix* p, double mean, double stdev, double density) {

	size_t nb_nonzeros;
	size_t nb_vals;
	size_t i, j;

	fixed_point* normal_set;

	// number of non-zero elements
	nb_nonzeros = (size_t)(density * p->rows * p->cols * 2 + 1) / 2;

	if (nb_nonzeros > p->cols * p->rows) nb_nonzeros = p->cols * p->rows;

	normal_set = fp_generate_normal(nb_nonzeros, mean, stdev);

	// fill the matrix
	for (; nb_nonzeros > 0; nb_nonzeros--) {
		// choose a random element that's not already non-zero
		do {
			i = p->rows > 1 ? rand_number16(p->rows - 1) : 0;
			j = p->cols > 1 ? rand_number16(p->cols - 1) : 0;
		} while(p->data[i * p->cols + j] != 0);

		p->data[i * p->cols + j] = normal_set[nb_nonzeros - 1];
	}
	free(normal_set);
}


void fpm_make_sparse(fp_matrix* p, double density) {
	unsigned nb_zeros, i, j;
	unsigned* indexes;
	unsigned tmp;

	nb_zeros = floor((1 - density) * p->rows * p->cols);
	
	// this will hold the indexes which will be set to zero in the matrix
	indexes = (unsigned*)malloc(sizeof(unsigned) * nb_zeros);

	for (i = 0; i < nb_zeros; i++) {
		indexes[i] = i;
	}

	// shuffle indexes to create a random sequence of unique numbers
	for (i = 0; i < nb_zeros; i++) {
		j = i > 0 ? rand_number16(i) : 0;
		tmp = indexes[i];
		indexes[i] = indexes[j];
		indexes[j] = tmp;
	}


	for (i = 0; i < nb_zeros; i++) {
		p->data[indexes[i]] = 0;
	}

	free(indexes);
}
