// Test strict_string_checks option in strncmp function
// RUN: %clang_asan %s -o %t

// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t a & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=true %run taskset -c 0 %t a & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: not %run taskset -c 0 %t b & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t c & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t d & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t e & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t f & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t g & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t h & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t h & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: %env_asan_opts=strict_string_checks=false %run taskset -c 0 %t i & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1
// RUN: %env_asan_opts=strict_string_checks=true not %run taskset -c 0 %t i & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv) {
  assert(argc >= 2);
  enum { size = 100 };
  char fill = 'o';
  char s1[size];
  char s2[size];
  memset(s1, fill, size);
  memset(s2, fill, size);

  switch (argv[1][0]) {
    case 'a':
      s1[size - 1] = 'z';
      s2[size - 1] = 'x';
      for (int i = 0; i <= size; ++i)
        assert((strncmp(s1, s2, i) == 0) == (i < size));
      s1[size - 1] = '\0';
      s2[size - 1] = '\0';
      assert(strncmp(s1, s2, 2*size) == 0);
      break;
    case 'b':
      return strncmp(s1-1, s2, 1);
    case 'c':
      return strncmp(s1, s2-1, 1);
    case 'd':
      return strncmp(s1+size, s2, 1);
    case 'e':
      return strncmp(s1, s2+size, 1);
    case 'f':
      return strncmp(s1+1, s2, size);
    case 'g':
      return strncmp(s1, s2+1, size);
    case 'h':
      s1[size - 1] = '\0';
      assert(strncmp(s1, s2, 2*size) != 0);
      break;
    case 'i':
      s2[size - 1] = '\0';
      assert(strncmp(s1, s2, 2*size) != 0);
      break;
    // CHECK: {{.*}}ERROR: AddressSanitizer: stack-buffer-{{ov|und}}erflow 
  }
  return 0;
}
