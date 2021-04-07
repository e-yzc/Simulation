/*******************************************
 * File :test.c
 * Author : F. Emre Yazici
 * Creation Date : 03.03.2021
 ********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fpmatrix.h"
#include "dbmatrix.h"

int mains()
{
  int i, j;
  
  /***** Test fixed point artihmetic *****/
  printf("\n***** fixed point artihmetic test *****\n");

  fixed_point x, y;
  double dbl = -7.5;
  x = 3<<FRACTIONAL_BITS;//fp_inverse(7 << FRACTIONAL_BITS);
  printf("%x -> %f\n",(int)x,fixed_to_float(x));
  printf("%f -> %x\n", dbl, float_to_fixed(dbl));

  y = 2 << FRACTIONAL_BITS;

  printf("%x + %x = %x\n",x, y, fp_add(x,y));
  printf("Expected result: %x\n", float_to_fixed(fixed_to_float(x) + fixed_to_float(y)));

  x = x << 1;
  y = x << 2;
  printf("%x * %x = %x\n", x, y, fp_mult(x, y));
  printf("Expected result: %x\n", float_to_fixed(fixed_to_float(x)*fixed_to_float(y)));




  /***** Test fixed point matrix initialization *****/
  printf("\n***** Matrix initialization test *****\n");

  fp_matrix a,b;

  fpm_init(&a, 2, 2);
  printf("Init to zeros:\n");
  fpm_print(&a);
    
  fpm_init_ones(&b, 2, 2);
  printf("Init to ones:\n");
  fpm_print(&b);


  /***** Test elementary matrix operations *****/
  printf("\n\n***** Elementary operations test *****\n");

  // Give initial values to A and B
  for (i = 0; i < a.rows; i++)
      for (j = 0; j < a.cols; j++)
      {
          a.data[a.cols * i + j] = float_to_fixed(1. + i + j);
      }
  for (i = 0; i < b.rows; i++)
      for (j = 0; j < b.cols; j++)
          b.data[b.cols * i + j] = float_to_fixed(0. + i + j);

  printf("\nMatrix A:\n");
  fpm_print(&a);
  printf("\nMatrix B:\n");
  fpm_print(&b);
  
  printf("\nScale A by 5:\n");
  fpm_scale(&a, float_to_fixed(5));
  fpm_print(&a);

  printf("\nAdd 5 to each elemnt of B:\n");
  fpm_iadd_scalar(&b, 5);
  fpm_print(&b);

  printf("\nA = AB\n");
  fpm_imult(&a, &b);
  fpm_print(&a);


  fp_matrix c;
  fpm_init_ones(&c, 2, 1);
  c.data[1] = float_to_fixed(2);
  printf("\nMatrix C:\n");
  fpm_print(&c);

  printf("\nC transposed:\n");
  fp_matrix c_t;
  c_t = fpm_transposed(&c);
  fpm_print(&c_t);

  fp_matrix tmp;
  tmp = fpm_mult(&c_t, &c);
  printf("\nC . C = \n");
  fpm_print(&tmp);
  fpm_destroy(&tmp);

  
  /***** Test random fill methods *****/

  fp_matrix randcm;
  size_t rows, cols;
  fixed_point lower, upper;
  rows = 10; cols = 10;
  fpm_init(&randcm, rows, cols);
  printf("\n%d by %d matrix filled by random numbers\n", rows, cols);
  lower = 0; upper = 31;
  printf("Lower limit: %d, upper limit: %d\n", lower, upper);
  fpm_fillrand(&randcm, lower, upper);
  fpm_print(&randcm);
  fpm_destroy(&randcm);


  fp_matrix sprandcm;
  double density;
  density = 0.1;
  fpm_init(&sprandcm, rows, cols);
  printf("\n%d by %d matrix filled sparsely by random numbers\n", rows, cols);
  printf("Density; %f\n", density);
  printf("Expected number of non-zero elements: %f\n", density * rows * cols);
  fpm_fillsprand(&sprandcm, lower, upper, density);
  fpm_print(&sprandcm);
  unsigned nb_nz;
  for (i = nb_nz = 0; i < rows * cols; i++) {
      if (sprandcm.data[i] != 0)
          nb_nz++;
  }
  printf("Number of non-zero elements: %d\n", nb_nz);
  fpm_destroy(&sprandcm);

  fp_matrix randncm;
  double mean, stdev;
  density = 0.1;
  mean = 10;
  stdev = 2;
  fpm_init(&randncm, rows, cols);
  printf("\n%d by %d matrix filled sparsely by normally distributed random numbers\n", rows, cols);
  printf("Density: %f\n", density);
  printf("Expected mean: %f\n", mean);
  printf("Expected stddev: %f\n", stdev);
  fpm_fillrandn(&randncm, mean, stdev);
  fpm_print(&randncm);

  printf("Mean: %f\n", fixed_to_float(fp_mean(randncm.data, rows*cols)));
  printf("Stddev: %f\n", fixed_to_float(fp_stddev(randncm.data, rows*cols)));

  fpm_destroy(&randncm);


  fp_matrix sprandncm; 
  density = 0.1;
  mean = 10;
  stdev = 2;
  fpm_init(&sprandncm, rows, cols);
  printf("\n%d by %d matrix filled sparsely by normally distributed random numbers\n", rows, cols);
  printf("Density: %f\n", density);
  printf("Mean: %f\n", mean);
  printf("Stdev: %f\n", stdev);
  fpm_fillsprandn(&sprandncm, mean, stdev, density);
  fpm_print(&sprandncm);
  fpm_destroy(&sprandncm);


  /**************Double Matrix ************/
  db_matrix dba, dbb;
  
  dbm_init(&dba, 3, 3);
  dbm_init_ones(&dbb, 3, 3);

  dbm_print(&dba);
  dbm_print(&dbb);

  dbm_iadd_scalar(&dba, 2);
  dbm_print(&dba);

  dbm_fillrand(&dba, 0, 1);
  dbm_print(&dba);

  dbm_imult(&dba, &dbb);
  dbm_print(&dba);


  // free resources
  fpm_destroy(&a);
  fpm_destroy(&b);
  fpm_destroy(&c);
  fpm_destroy(&c_t);
  
  dbm_destroy(&dba);
  dbm_destroy(&dbb);

  return 0;
}