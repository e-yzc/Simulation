#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "util.h"


int main()
{
  int i, j;
  coef x,y;
  x = 0b111111;
  y = 0b100000;
    
  printf("%d + %d = %d\n",x, y, CADD(x,y));

  cmatrix a,b;

  cm_init(&a, 2, 2, true);
  cm_init(&b, 2, 2, true);

  for (i=0; i < a.rows; i++)
      for(j=0; j < a.cols; j++)
        a.data[a.cols*i + j] = i;
  for (i = 0; i < b.rows; i++)
      for (j = 0; j < b.cols; j++)
          b.data[b.cols * i + j] = j;

  cm_print(&a);
  cm_print(&b);
  
  cm_scale(&a, 5);
  cm_print(&a);

  for (i = 0; i < a.rows; i++)
      for (j = 0; j < a.cols; j++)
          a.data[a.cols * i + j] = i+j;
  for (i = 0; i < b.rows; i++)
      for (j = 0; j < b.cols; j++)
          b.data[b.cols * i + j] = 24 * (i + j);

  cm_print(&a);
  cm_print(&b);


  cmatrix c;
  c = cm_mult(&a, &b);
  cm_print(&c);

  cm_imult(&a, &b);
  cm_print(&a);

  cmatrix d,e;
  cm_init(&d, 1, 5, true);
  cm_init(&e, 5, 1, true);

  for (i = 0; i < d.rows; i++)
      for (j = 0; j < d.cols; j++)
          d.data[i * d.cols + j] = j+1;
  for (i = 0; i < e.rows; i++)
      for (j = 0; j < e.cols; j++)
          e.data[i * e.cols + j] = i + 1;


  cm_print(&d);
  cm_print(&e);

  cm_imult(&d, &e);

  cm_print(&d);




  cm_destroy(&a);
  cm_destroy(&b);
  cm_destroy(&c);
  cm_destroy(&d);
  cm_destroy(&e);
  
  return 0;
}