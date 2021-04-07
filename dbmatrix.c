#include "dbmatrix.h"
#include <assert.h>

void dbm_init(db_matrix* p, size_t rows, size_t cols) {
	p->rows = rows;
	p->cols = cols;
	p->data = malloc(sizeof(double) * rows * cols);

	if (p->data == NULL) return;	// in case allocation fails

	unsigned i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			(p->data)[i * cols + j] = 0;

}

void dbm_init_ones(db_matrix* p, size_t rows, size_t cols) {
	p->rows = rows;
	p->cols = cols;
	p->data = malloc(sizeof(double) * rows * cols);

	if (p->data == NULL) return;	// in case allocation fails

	unsigned i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			(p->data)[i * cols + j] = 1.;
}

void dbm_destroy(db_matrix* p) {
	if (p->data == NULL) return;
	free(p->data);
}

void dbm_print(db_matrix* p) {
	int i, j;
	for (i = 0; i < p->rows; i++) {
		printf("\n");
		for (j = 0; j < p->cols; j++) {
			printf("%.4f\t", p->data[i * (p->cols) + j]);
		}
	}
	printf("\n");
}

db_matrix dbm_copy(db_matrix* p) {
	db_matrix result;
	dbm_init(&result, p->rows, p->cols);

	unsigned i, j;
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			result.data[i * result.cols + j] = p->data[i * p->cols + j];

	return result;
}


db_matrix dbm_transposed(db_matrix* p) {
	db_matrix result;
	dbm_init(&result, p->cols, p->rows);

	unsigned i, j;
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			result.data[j * result.cols + i] = p->data[i * p->cols + j];

	return result;
}


void dbm_copy_to(db_matrix* original, db_matrix* target) {
	assert(original->rows == target->rows && original->cols == target->cols);

	unsigned i, j;
	for (i = 0; i < original->rows; i++)
		for (j = 0; j < original->cols; j++)
			target->data[i * target->cols + j] = original->data[i * original->cols + j];
}


// Multiply a fixed point matrix with a scalar.
void dbm_scale(db_matrix* p, double scale) {
	unsigned i, j;
	for (i = 0; i < p->rows; i++) {
		for (j = 0; j < p->cols; j++)
			p->data[i * p->cols + j] *= scale;
	}
}

void dbm_iadd_scalar(db_matrix* p, double scalar) {
	unsigned i, j;
	for (i = 0; i < p->rows; i++) {
		for (j = 0; j < p->cols; j++)
			p->data[i * p->cols + j] += scalar;
	}
}

void dbm_iadd(db_matrix* lhs, db_matrix* rhs) {
	assert(lhs->cols == rhs->cols && lhs->rows == rhs->rows);

	unsigned i, j;
	for (i = 0; i < lhs->rows; i++)
		for (j = 0; j < lhs->cols; j++)
			lhs->data[i * lhs->cols + j] += rhs->data[i * rhs->cols + j];
}

// Multiply two fixed point matrixes and store the result in the left hand side matrix.
void dbm_imult(db_matrix* lhs, db_matrix* rhs) {
	assert(lhs->cols == rhs->rows);

	unsigned i, j, k;
	double a, b;

	// initialize a temporary matrix to store the intermediate results
	db_matrix temp;
	dbm_init(&temp, lhs->rows, rhs->cols);

	for (i = 0; i < lhs->rows; i++) {
		for (j = 0; j < rhs->cols; j++) {
			for (k = 0; k < lhs->cols; k++) {
				a = lhs->data[i * lhs->cols + k];
				b = rhs->data[k * rhs->cols + j];
				temp.data[i * temp.cols + j] += a * b;
			}
		}
	}

	// swap the temp matrix with the lhs
	double* p = lhs->data;
	lhs->data = temp.data;
	temp.data = p;
	lhs->cols = temp.cols;
	lhs->rows = temp.rows;

	// free the old matrix data
	dbm_destroy(&temp);
}

// Multiply two fixed point matrixes and return the resulting matrix.
db_matrix dbm_mult(db_matrix* lhs, db_matrix* rhs) {
	int i, j, k;
	double a, b;
	db_matrix temp;
	dbm_init(&temp, lhs->rows, rhs->cols);

	for (i = 0; i < lhs->rows; i++) {
		for (j = 0; j < rhs->cols; j++) {
			for (k = 0; k < lhs->cols; k++) {
				a = lhs->data[i * lhs->cols + k];
				b = rhs->data[k * rhs->cols + j];
				temp.data[i * temp.cols + j] += a * b;
			}
		}
	}
	return temp;
}



void dbm_fillrand(db_matrix* p, double lower_lim, double upper_lim) {
	assert(lower_lim < upper_lim);

	unsigned i, j;


	// populate with random number
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++) {
			double rand_nb = rand_db(lower_lim, upper_lim);
			(p->data)[i * p->cols + j] = rand_nb;
		}

}



void dbm_fillrandn(db_matrix* p, double mean, double stdev) {

	unsigned i, j;
	unsigned nb_vals;

	double* normal_set;

	// generate normal set
	nb_vals = p->rows * p->cols;
	normal_set = generate_normal(nb_vals, mean, stdev);

	// populate the matrix
	for (i = 0; i < p->rows; i++)
		for (j = 0; j < p->cols; j++)
			(p->data)[i * p->cols + j] = normal_set[i * p->cols + j];


	free(normal_set);
}

void dbm_fillsprand(db_matrix* p, double lower_lim, double upper_lim, double density) {

	assert(lower_lim < upper_lim);

	// number of non-zero elements
	size_t nb_nonzeros;
	nb_nonzeros = (size_t)(density * p->rows * p->cols * 2 + 1) / 2;

	// fill the matrix
	size_t i, j;
	for (; nb_nonzeros > 0; nb_nonzeros--) {
		// choose a random element that's not already non-zero
		do {
			i = rand_number16(p->rows - 1);
			j = rand_number16(p->cols - 1);
		} while (p->data[i * p->cols + j] != 0);

		// to ensure the element is always non-zero regardless of upper and lower lim
		while (p->data[i * p->cols + j] == 0) {
			p->data[i * p->cols + j] = rand_db(lower_lim, upper_lim);
		}
	}
}

void dbm_fillsprandn(db_matrix* p, double mean, double stdev, double density) {

	size_t nb_nonzeros;
	size_t nb_vals;
	size_t i, j;

	double* normal_set;

	// number of non-zero elements
	nb_nonzeros = (size_t)(density * p->rows * p->cols * 2 + 1) / 2;

	normal_set = generate_normal(nb_nonzeros, mean, stdev);

	// fill the matrix
	for (; nb_nonzeros > 0; nb_nonzeros--) {
		// choose a random element that's not already non-zero
		do {
			i = rand_number16(p->rows - 1);
			j = rand_number16(p->cols - 1);
		} while (p->data[i * p->cols + j] != 0);

		p->data[i * p->cols + j] = normal_set[nb_nonzeros - 1];
	}
	free(normal_set);
}

