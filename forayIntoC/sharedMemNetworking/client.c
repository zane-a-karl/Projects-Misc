#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define SHSIZE 100

int main (int argc, char* argv[]) {
  int shmid;
  key_t key;
  char* shm;
  char* s;

  key = 9876;
  if ((shmid = shmget(key, SHSIZE, IPC_CREAT | 0666)) < 0) {
    printf("shmget failed");
    exit(1);
  }

  if ((shm = shmat(shmid, NULL, 0)) == (char*)(-1)) {
    printf("shmat failed");
    exit(1);
  }

  for (s = shm; *s != 0; ++s) {
    printf("%c", *s);
  }
  printf("\n");

  *shm = '*';
  
  return 0;	    
}
