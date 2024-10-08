// Test string s1 overflow in strpbrk function
// RUN: %clang_asan %s -o %t && %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

// Test intercept_strpbrk asan option
// RUN: %env_asan_opts=intercept_strpbrk=false %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor

#include <assert.h>
#include <string.h>
#include <sanitizer/asan_interface.h>

int main(int argc, char **argv) {
  char *r;
  char s2[] = "ab";
  char s1[4] = "cab";
  __asan_poison_memory_region ((char *)&s1[2], 2);
  r = strpbrk(s1, s2);
  // CHECK:'s1'{{.*}} <== Memory access at offset {{[0-9]+}} partially overflows this variable
  assert(r == s1 + 1);
  return 0;
}
