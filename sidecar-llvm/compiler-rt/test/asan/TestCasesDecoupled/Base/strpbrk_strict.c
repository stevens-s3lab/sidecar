// Test strict_string_checks option in strpbrk function
// RUN: %clang_asan %s -o %t && %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  size_t size = 100;
  char fill = 'o';
  char *s1 = (char*)malloc(size);
  char *s2 = (char*)malloc(2);
  memset(s1, fill, size);
  s2[0] = fill;
  s2[1]='\0';
  char* r = strpbrk(s1, s2);
  // CHECK: {{.*ERROR: AddressSanitizer: heap-buffer-overflow }}
  // CHECK: READ of size 101
  assert(r == s1);
  free(s1);
  free(s2);
  return 0;
}
