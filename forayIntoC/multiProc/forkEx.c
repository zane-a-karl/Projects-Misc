#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (void) {

  int childProcId, c1 = 0, c2 = 0;

  printf("Before fork\n");

  if ((childProcId = fork()) == 0) {
    printf("child proc here with id = %d\n", childProcId);
    while (c1 < 5) {
      printf("child process counter is %d\n", c1);
      sleep(1);
      c1++;
    }
  } else {
    printf("parent proc here\n");
    while (c2 < 5) {
      printf("parent process will print 100 = %d\n", 100);
      sleep(1); c2++;
    }
  }

  return 0;
}
