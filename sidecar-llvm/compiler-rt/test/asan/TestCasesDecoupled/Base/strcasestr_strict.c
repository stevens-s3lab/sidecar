// Test strict_string_checks option in strcasestr function
// RUN: %clang_asan %s -o %t && %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

// There's no interceptor for strcasestr on Windows
// XFAIL: windows-msvc

#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  size_t size = 100;
  char *s1 = (char*)malloc(size);
  char *s2 = (char*)malloc(size);
  memset(s1, 'o', size);
  memset(s2, 'O', size);
  s2[size - 1]='\0';
  char* r = strcasestr(s1, s2);
  // CHECK: {{.*ERROR: AddressSanitizer: heap-buffer-overflow }}
  // CHECK: READ of size 101
  assert(r == s1);
  free(s1);
  free(s2);
  return 0;
}
