#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

struct zane {
  int x;
};
//typedef zane karl;

int main ()
{

  /* The following tests bit masks *

  size_t mask;
  size_t startbit = 0;//7;
  //mask = ((1<<1) - 1) << startbit;
  mask = ((1<<3) - 1) << startbit;
  printf("This is mask = %zu\n", mask);
  int isolatedbits;
  karl var;
  //var.x = 95;
  var.x = 6;
  isolatedbits = var.x&mask;
  //printf("This is 95&1 =  %d\n", var.x&1);
  printf("This is isolatedbits = %d\n", isolatedbits);

  *****************************************************/

  /* The following tests endian-ness of j *

  int x = 0;
  int y = 6;
  printf("This is x>>y  = %d\n", (x>>y));

  int j,k;
  for (j=0; j<8; ++j) {
    printf("j = %d\n", j);
    for (k=0; k<3; ++k) {
      printf("This is (%d>>%d) & 1 = %d\n",j, k, (j>>k)&1);
    }
    printf("\n");
  }
  *****************************************************/

  /* The following tests size_t *

     float x = 0.5;
     printf("2 * x = 2 * %f = %f", x, 2*x);
  *****************************************************/

  /* The following tests dynamic resizing *

     int *buffer, i, j=0, k;
     bool tf = true;

     while (tf) {
       for (i=0; i<10; ++i) {
         if (i==3 && j==0) {
           buffer = (int *)malloc(sizeof(int)*i);
           j++;
           break;
         }
         if (i==6 && j==1) {
           buffer = (int *)malloc(sizeof(int)*i);
           j++;
           break;
         }
       }
       for (k=0; k<i; ++k) {
         buffer[k] = i;
         printf ("buffer[%d] = %d", k, buffer[k]);
       }
       free(buffer);
       printf ("\n");
       if (j==2)
         tf = false;
     }
  *****************************************************/

     /* The following tests assignment to pointers to arrays
        note we use sizeof(*arr) THIS IS IMPORTANT!!! *

        size_t (*arr)[3] = (size_t (*)[3])malloc(3 * sizeof(*arr));
        for (size_t i=0; i<3; ++i) {
          arr[i][0] = 1111+i; printf("arr[%zu][0]=%zu, ", i, arr[i][0]);
          arr[i][1] = 2222+i; printf("arr[%zu][1]=%zu, ", i, arr[i][1]);
          arr[i][2] = 3333+i; printf("arr[%zu][2]=%zu\n", i, arr[i][2]);
        }

  *****************************************************/

        /* The following tests a bit twiddling method of printing 0,...,m-1
           compile with gcc test.c -lm *
        
        int m = 8;
        int lgm = (int)log2((double)m);
        for (int j = 0; j < m; ++j) {
                for (int k=0; k<lgm; ++k) {
                        if ((j >> k) & 1) {
                                printf("1 ");
                        }
                        else {
                                printf("0 ");
                        }
                }
                printf("\n");
        }

        This line proves that pr-values are rendered in big endian, as we had to move right to get the more significant digits. 
        printf("%d", (8>>3) & 1); 

        *****************************************************/
        
        return 0;
}
