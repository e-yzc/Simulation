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
#include <math.h>
#include "fixed_point.h"

int rand_number16(int n);
int rand_number32(int n);

fixed_point rand_fp(fixed_point, fixed_point);
double rand_db(double, double);

// Generates a normal distribution set with n elements
double* generate_normal(int n, double mean, double stdev);

// Generates a normal distribution set with n elements in fixed point format 
fixed_point* fp_generate_normal(int n, double mean, double stdev);

double mean(double* values, int n);

double stddev(double* values, int n);

fixed_point fp_mean(fixed_point* values, int n);

fixed_point fp_stddev(fixed_point* values, int n);



#endif