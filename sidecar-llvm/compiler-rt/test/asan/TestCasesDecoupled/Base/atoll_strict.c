// Test strict_string_checks option in atoll function
// RUN: %clang_asan %s -o %t
// RUN: %run taskset -c 0 %t test1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t test1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t test1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s --check-prefix=CHECK1
// RUN: %run taskset -c 0 %t test2 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t test2 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t test2 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s --check-prefix=CHECK2
// RUN: %run taskset -c 0 %t test3 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t test3 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t test3 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s --check-prefix=CHECK3

// FIXME: Needs Windows interceptor.
// XFAIL: windows-msvc

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void test1(char *array) {
  // Last symbol is non-digit
  memset(array, '1', 10);
  array[9] = 'a';
  long long r = atoll(array);
  assert(r == 111111111);
}

void test2(char *array) {
  // Single non-digit symbol
  array[9] = 'a';
  long long r = atoll(array + 9);
  assert(r == 0);
}

void test3(char *array) {
  // Incorrect number format
  memset(array, ' ', 10);
  array[9] = '-';
  array[8] = '-';
  long long r = atoll(array);
  assert(r == 0);
}

int main(int argc, char **argv) {
  char *array = (char*)malloc(10);
  if (argc != 2) return 1;
  if (!strcmp(argv[1], "test1")) test1(array);
  // CHECK1: {{.*ERROR: AddressSanitizer: heap-buffer-overflow }}
  // CHECK1: READ of size 11
  if (!strcmp(argv[1], "test2")) test2(array);
  // CHECK2: {{.*ERROR: AddressSanitizer: heap-buffer-overflow }}
  // CHECK2: READ of size 2
  if (!strcmp(argv[1], "test3")) test3(array);
  // CHECK3: {{.*ERROR: AddressSanitizer: heap-buffer-overflow }}
  // CHECK3: READ of size 11
  free(array);
  return 0;
}
