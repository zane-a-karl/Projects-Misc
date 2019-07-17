#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 4
#define T 2

int A[N][N] = {{1,2,3,4},{2,3,4,5},{3,4,5,6},{4,5,6,7}};
int B[N][N] = {{1,2,3,4},{2,3,4,5},{3,4,5,6},{4,5,6,7}};
int C[N][N] = {{0}};
/*int** C = (int**)malloc(sizeof(int)*N);
for (int z=0; z<N; z++) {
  C[z] = (int*)malloc(sizeof(int)*N);
 }
*/
void* add_mats(void* var) {
  int id = *(int*)var;
  int i,j;
  for (i = id; i < N; i++) {
    for ( j = id; j < N; j++) {
      C[i][j] = A[i][j] + B[i][j];
    }
  }
      pthread_exit(NULL);
}

int main() {
  pthread_t thread[T];

  int tid[T];
  int i,j;
  for (i=0; i<T; i++) {
    tid[i] = i;
    pthread_create(&thread[i], NULL, add_mats, &tid[i]);
  }
  for (i=0; i<T; i++) {
    pthread_join(thread[i], NULL);
  }
  for (i=0; i<N; i++) {
    for(j=0; j<N; j++) {
      if (C[i][j]/10 < 1) {
	printf((j < N-1) ? "%d  " : "%d\n", C[i][j]);
      } else {
	printf((j < N-1) ? "%d " : "%d\n", C[i][j]);
      }
    }
  }
  return 0;
}
