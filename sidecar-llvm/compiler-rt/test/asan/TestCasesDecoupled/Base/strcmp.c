// RUN: %clang_asan %s -o %t
// RUN: %env_asan_opts=intercept_strcmp=false %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %env_asan_opts=intercept_strcmp=true not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN:                                      not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  char s1[] = "abcd";
  char s2[] = "1234";
  assert(strcmp(s1, s2) > 0);
  assert(strcmp(s1 - 1, s2));

  // CHECK: {{.*ERROR: AddressSanitizer: stack-buffer-underflow }}
  // CHECK: READ of size 1
  return 0;
}
