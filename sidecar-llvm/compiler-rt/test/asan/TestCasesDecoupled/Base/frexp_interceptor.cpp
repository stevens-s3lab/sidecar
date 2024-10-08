// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

// Test the frexp() interceptor.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int main() {
  double x = 3.14;
  int *exp = (int*)malloc(sizeof(int));
  free(exp);
  double y = frexp(x, exp);
  // CHECK: AddressSanitizer
  return 0;
}
