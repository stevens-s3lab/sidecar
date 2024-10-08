// RUN: %clang_asan -O2 %s -o %t
// RUN: %env_asan_opts=sleep_before_dying=1 not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <stdlib.h>
int main() {
  char *x = (char*)malloc(10 * sizeof(char));
  free(x);
  return x[5];
  // CHECK: Sleeping for 1 second
}
