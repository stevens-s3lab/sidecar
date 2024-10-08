// Sanity checking a test in pure C.
// RUN: %clang_asan -O2 %s -o %t
// RUN: not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

// Sanity checking a test in pure C with -pie.
// RUN: %clang_asan -O2 %s %pie %fPIE -o %t
// RUN: not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// REQUIRES: stable-runtime

#include <stdlib.h>
int main() {
  char *x = (char*)malloc(10 * sizeof(char));
  free(x);
  return x[5];
  // CHECK: heap-use-after-free
  // CHECK: free
  // CHECK: main{{.*}}sanity_check_pure_c.c:[[@LINE-4]]
  // CHECK: malloc
  // CHECK: main{{.*}}sanity_check_pure_c.c:[[@LINE-7]]
}
