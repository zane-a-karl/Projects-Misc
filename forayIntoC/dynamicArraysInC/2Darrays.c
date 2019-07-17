#include <stdio.h>
#include <stdlib.h>

/* Use a single pointer to allocate a memory block of size r*c */
void printSinglePtr2DArray (void) {
  int r = 3, c = 4;
  int *arr = (int*)malloc(r * c * sizeof(int));

  int i, j, count = 0;
  for (i=0; i<r; ++i) {
    for (j=0; j<c; ++j) {
      *(arr + i*c + j) = ++count;
    }
  }

  for (int i=0; i<r; ++i) {
    for (int j=0; j<c; ++j) {
      if (j != c-1) {
	printf("%d ",  *(arr + i*c + j));//Notice we do this instead of a[i][j]
      } else {
	printf("%d\n",  *(arr + i*c + j));//B/c a is a single ptr!
      }
    }
  }
}

  /* Use an array of pointers, C99 allows variable sized arrays */
void printArrayOfPointers2DArray (void) {
  int r = 3, c = 4, i, j, count;
  int *arr[r];
  for (i=0; i<r; ++i) {
    arr[i] = (int *)malloc(c*sizeof(int));
  }

  // Note that arr[i][j] = *(*(arr + i) + j)
  count = 0;
  for (int i=0; i<r; ++i) {
    for (int j=0; j<c; ++j) {
      arr[i][j] = ++count; // Or *(*(arr + i) + j)
    }
  }
  
  for (int i=0; i<r; ++i) {
    for (int j=0; j<c; ++j) {
      if (j != c-1) {
	printf("%d ",  *(*(arr + i) + j));
      } else {
	printf("%d\n",  *(*(arr + i) + j));
      }
    }
  }
}

/* Use a pointer to a pointer */
void printPointerToAPointer2DArray (void) {
  int r = 3, c = 4, i, j, count;
  int **arr = (int **)malloc(r *sizeof(int));
  for (i=0; i<r; ++i) {
    arr[i] = (int *)malloc(c * sizeof(int));
  }

  // Note that arr[i][j] = *(*(arr + i) + j)
  count = 0;
  for (int i=0; i<r; ++i) {
    for (int j=0; j<c; ++j) {
      arr[i][j] = ++count; // Or *(*(arr + i) + j)
    }
  }
  
  for (int i=0; i<r; ++i) {
    for (int j=0; j<c; ++j) {
      if (j != c-1) {
	printf("%d ",  *(*(arr + i) + j));
      } else {
	printf("%d\n",  *(*(arr + i) + j));
      }
    }
  }
}

/* Using a double pointer and one malloc call for all rows */
void printDoublePointer2DArray (void) {
  int r = 3, c = 4, i, j, count = 0;
  int **arr;

  arr = (int **)malloc(r * sizeof(int*));
  arr[0] = (int *)malloc(c * r * sizeof(int));
  
  for (i=0; i<r; ++i) {
    arr[i] = (*arr + c * i);
  }

  // Note that arr[i][j] = *(*(arr + i) + j)
  for (int i=0; i<r; ++i) {
    for (int j=0; j<c; ++j) {
      arr[i][j] = ++count; // Or *(*(arr + i) + j)
    }
  }
  
  for (int i=0; i<r; ++i) {
    for (int j=0; j<c; ++j) {
      if (j != c-1) {
	printf("%d ",  *(*(arr + i) + j));
      } else {
	printf("%d\n",  *(*(arr + i) + j));
      }
    }
  }
}

int main (void) {
  printSinglePtr2DArray();

  printf("\n");

  printArrayOfPointers2DArray();

  printf("\n");

  printPointerToAPointer2DArray();

  printf("\n");

  printDoublePointer2DArray();
  return 0;
}
