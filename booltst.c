#include <stdio.h>
#include <stdlib.h>

int main () {
  bool *z = (bool *)malloc(2*3*sizeof(bool));
  size_t j;
  for (j=0; j<6; ++j) {
    z[j] = j*4;
    printf("This is z[%zu] = %zu\n", j, z[j]);
  }
  return 0;
}
