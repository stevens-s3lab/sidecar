// RUN: %clang_asan -O2 %s -o %t
// RUN: %env_asan_opts=strip_path_prefix='"%S/"' not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <stdlib.h>
int main() {
  char *x = (char*)malloc(10 * sizeof(char));
  free(x);
  return x[5];
  // Check that paths in error report don't start with slash.
  // CHECK: heap-use-after-free
  // CHECK: #0 0x{{.*}} in main {{.*}}strip_path_prefix.c:[[@LINE-3]]
}
