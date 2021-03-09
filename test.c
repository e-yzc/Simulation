/*******************************************
 * File :test.c
 * Author : F. Emre Yazici
 * Creation Date : 03.03.2021
 ********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "util.h"


int main()
{
  int i, j;
  
  /***** Test coefficient artihmetic *****/
  printf("\n***** Coefficient artihmetic test *****\n");

  coef x,y;
  x = 0b111111;
  y = 0b100000;
 
  printf("%d + %d = %d\n",x, y, CADD(x,y));
  printf("Expected result: %d\n", (x + y) & 0b111111);

  x = 0b011111;
  y = 0b000110;
  printf("%d * %d = %d\n", x, y, CMULT(x, y));
  printf("Expected result: %d\n", (x * y) & 0b111111);


  /***** Test coefficient matrix initialization *****/
  printf("\n***** Matrix initialization test *****\n");

  cmatrix a,b;

  cm_init(&a, 2, 2);
  printf("Init to zeros:\n");
  cm_print(&a);
    
  cm_init_ones(&b, 2, 2);
  printf("Init to ones:\n");
  cm_print(&b);


  /***** Test elementary matrix operations *****/
  printf("\n\n***** Elementary operations test *****\n");

  // Give initial values to A and B
  for (i=0; i < a.rows; i++)
      for(j=0; j < a.cols; j++)
        a.data[a.cols*i + j] = i + j + 1;
  for (i = 0; i < b.rows; i++)
      for (j = 0; j < b.cols; j++)
          b.data[b.cols * i + j] = i + j;

  printf("\nMatrix A:\n");
  cm_print(&a);
  printf("\nMatrix B:\n");
  cm_print(&b);
  
  printf("\nScale A by 5:\n");
  cm_scale(&a, 5);
  cm_print(&a);

  printf("\nAdd 5 to each elemnt of B:\n");
  cm_iadd_scalar(&b, 5);
  cm_print(&b);

  printf("\nA = AB\n");
  cm_imult(&a, &b);
  cm_print(&a);


  cmatrix c;
  cm_init_ones(&c, 2, 1);
  c.data[1] = 2;
  printf("\nMatrix C:\n");
  cm_print(&c);

  printf("\nC transposed:\n");
  cmatrix c_t;
  c_t = cm_transposed(&c);
  cm_print(&c_t);

  cmatrix tmp;
  tmp = cm_mult(&c_t, &c);
  printf("\nC . C = \n");
  cm_print(&tmp);
  cm_destroy(&tmp);

  // Test random fill methods

  cmatrix randcm;
  size_t rows, cols;
  coef lower, upper;
  rows = 10; cols = 10;
  cm_init(&randcm, rows, cols);
  printf("\n%d by %d matrix filled by random numbers\n", rows, cols);
  lower = 0; upper = 31;
  printf("Lower limit: %d, upper limit: %d\n", lower, upper);
  cm_fillrand(&randcm, lower, upper);
  cm_print(&randcm);
  cm_destroy(&randcm);

  cmatrix sprandcm;
  double density;
  density = 0.1;
  cm_init(&sprandcm, rows, cols);
  printf("\n%d by %d matrix filled sparsely by random numbers\n", rows, cols);
  printf("Density; %f\n", density);
  printf("Expected number of non-zero elements: %f\n", density * rows * cols);
  cm_fillsprand(&sprandcm, lower, upper, density);
  cm_print(&sprandcm);
  unsigned nb_nz;
  for (i = nb_nz = 0; i < rows * cols; i++) {
      if (sprandcm.data[i] != 0)
          nb_nz++;
  }
  printf("Number of non-zero elements: %d", nb_nz);
  cm_destroy(&sprandcm);


  // free resources
  cm_destroy(&a);
  cm_destroy(&b);
  cm_destroy(&c);
  cm_destroy(&c_t);
  
  return 0;
}