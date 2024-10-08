// RUN: %clangxx_asan %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %clangxx_asan -O %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// Check that we can find huge buffer overflows to the left.
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv) {
  char *x = (char*)malloc(1 << 20);
  memset(x, 0, 10);
  int res = x[-argc * 4000];  // BOOOM
  // CHECK: is located 4000 bytes to the left of
  free(x);
  return res;
}
