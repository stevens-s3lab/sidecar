// RUN: %clangxx_asan -O0 %s -o %t && %env_asan_opts=strict_memcmp=0 %run taskset -c 0 %t & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %clangxx_asan -O0 %s -o %t && %env_asan_opts=strict_memcmp=1 not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// Default to strict_memcmp=1.
// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <stdio.h>
#include <string.h>
int main() {
  char kFoo[] = "foo";
  char kFubar[] = "fubar";
  int res = memcmp(kFoo, kFubar, strlen(kFubar));
  printf("res: %d\n", res);
  // CHECK: AddressSanitizer: stack-buffer-overflow
  return 0;
}
