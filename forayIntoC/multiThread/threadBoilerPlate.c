#include <stdio.h>
#include <pthread.h>

void* fcn(void* var) {
  char* msg = (char*)var;
  for (int i = 0; i<10; i++) {
    printf("%s and %d", msg, i);
    sleep(1);
  }
  return NULL;
}

int main() {
  pthread_t t1,t2;
  char* m1 = "First";
  char* m2 = "Second";
  int r1,r2;

  r1 = pthread_create(&t1, NULL, fcn, (void*)m1);
  r2 = pthread_create(&t2, NULL, fcn, (void*)m2);

  printf("main() after pthread create");

  pthread_join(t1,NULL);
  pthread_join(t2,NULL);

  printf("first thread return value 1 = %d", r1);
  printf("second thread return value 2 = %d", r2);
}
