// RUN: %clangxx_asan -O0 %s -o %t
// RUN: not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK --check-prefix=MALLOC-CTX

// Also works if no malloc context is available.
// RUN: %env_asan_opts=malloc_context_size=0:fast_unwind_on_malloc=0 not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %env_asan_opts=malloc_context_size=0:fast_unwind_on_malloc=1 not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// REQUIRES: stable-runtime

#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv) {
  char *x = (char*)malloc(10 * sizeof(char));
  memset(x, 0, 10);
  int res = x[argc];
  free(x + 5);  // BOOM
  // CHECK: AddressSanitizer: attempting free {{.*}}in thread T0
  // CHECK: invalid-free.cpp:[[@LINE-2]]
  // CHECK: is located 5 bytes inside of 10-byte region
  // CHECK: allocated by thread T0 here:
  // MALLOC-CTX: invalid-free.cpp:[[@LINE-8]]
  return res;
}
