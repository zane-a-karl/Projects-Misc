#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

void* create_shared_mem (size_t size) {
  // Our memory buffer will be readable and writable
  int protection =  PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third party processes can't obtain an address for it),
  // so only this process and its children will be able to use it.
  int visibility =  MAP_SHARED | MAP_ANONYMOUS;

  // The first and last two param for mmap represent the called proc addr spc,
  // the fd, and the offset respectively
  return mmap(NULL, size, protection, visibility, 0, 0);
}

unsigned int sizeof_dm (int rows, int cols, size_t sizeElem) {
  size_t size = rows * (sizeof(void*) + (cols * sizeElem));
  return size;
}

void create_index (void** m, int rows, int cols, size_t sizeElem) {
  int i;
  size_t sizeRow = cols * sizeElem;
  m[0] = m + rows;
  for (i=1; i<rows; ++i) {
    m[i] = (m[i-1]+sizeRow);
  }
}

int main() {
  int A[4][4] = {{1,2,3,4},{6,7,8,9},{11,12,13,14},{16,17,18,19}};
  int B[4][4] = {{1,2,3,4},{6,7,8,9},{11,12,13,14},{16,17,18,19}};
  //C = mmap(NULL, sizeof(**C), protection, visibility, -1, 0);
  int rows = 4, cols = 4, shmid;
  int** C;
  /*int** C = (int**)malloc(sizeof(int)*4);
  for (int z = 0; z<4; z++) {
    C[z] = (int*)malloc(sizeof(int)*4);
  }
  */

  size_t sizeMat = sizeof_dm(rows, cols, sizeof(int));
  shmid = shmget(IPC_PRIVATE, sizeMat, IPC_CREAT | 0600);
  C = shmat(shmid, NULL, 0);
  create_index((void**)C, rows, cols,sizeof(int)); 
  
  pid_t cp;
  printf("Before fork\n");
  if ((cp = fork()) == 0) {
    for(int i = 1; i < 4; i+=2) {
      for(int j = 0; j < 4; j++) {
	printf("child process\n");
	printf("We are adding cell %d,%d\n",i,j);
	C[i][j] = A[i][j] + B[i][j];
	sleep(1);
      }
    }
  } else {
    wait(NULL);
    for (int k = 0; k < 4; k+=2) {
      for(int l = 0; l < 4; l++) {
	printf("parent process\n");
	printf("We are adding cell %d,%d\n",k,l);
	C[k][l] = A[k][l] + B[k][l];
	sleep(1);
      }
    }
    printf("We are printing C here\n");
       for (int m = 0; m < 4; m++) {
	 for(int n = 0; n < 4; n++) {
	   printf("%d ",C[m][n]);
	 }
	 printf("\n");
       }
  }
}
