// Test strict_string_checks option in strchr function
// RUN: %clang_asan %s -o %t && %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  size_t size = 100;
  char fill = 'o';
  char *s = (char*)malloc(size);
  memset(s, fill, size);
  char c = 'o';
  char* r = strchr(s, c);
  // CHECK: {{.*ERROR: AddressSanitizer: heap-buffer-overflow }}
  // CHECK: READ of size 101
  assert(r == s);
  free(s);
  return 0;
}
