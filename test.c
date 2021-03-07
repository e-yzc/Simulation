#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "util.h"

#define MADD(lhs, rhs, mask) ((lhs + rhs) & mask)     // addition with bit masking to simulate custom precision 
#define MMULT(lhs,rhs,mask) ((lhs * rhs) & mask)      // multiplication with bit masking to simulate custom precision

#define CMASK 0b111111                                // 6-bit bitmask for coefficient arithmetic
#define CADD(lhs,rhs) (MADD(lhs,rhs,CMASK))           // coefficient add with masking
#define CMULT(lhs,rhs) (MMULT(lhs,rhs,CMASK))         // coefficient multiplicate with masking


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
          a.data[a.cols * i + j] = 1+i;
  for (i = 0; i < b.rows; i++)
      for (j = 0; j < b.cols; j++)
          b.data[b.cols * i + j] = 52;

  cm_print(&a);
  cm_print(&b);


  cmatrix c;
  c = cm_mult(&a, &b);
  cm_print(&c);

  cm_imult(&a, &b);
  cm_print(&a);

  return 0;
}