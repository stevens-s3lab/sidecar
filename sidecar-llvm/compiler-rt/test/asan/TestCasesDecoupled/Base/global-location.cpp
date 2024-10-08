// RUN: %clangxx_asan -O2 %s -o %t
// RUN: not %run taskset -c 0 %t g & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s --check-prefix=CHECK 
// RUN: not %run taskset -c 0 %t c & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s --check-prefix=CHECK
// RUN: not %run taskset -c 0 %t f & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s --check-prefix=CHECK
// RUN: not %run taskset -c 0 %t l & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s --check-prefix=CHECK 

// CHECK: AddressSanitizer: global-buffer-overflow

#include <string.h>

struct C {
  static int array[10];
};

int global[10];
int C::array[10];

int main(int argc, char **argv) {
  int one = argc - 1;
  switch (argv[1][0]) {
  case 'g': return global[one * 11];
  case 'c': return C::array[one * 11];
  case 'f':
    static int array[10];
    memset(array, 0, 10);
    return array[one * 11];
  case 'l':
    const char *str = "0123456789";
    return str[one * 11];
  }
  return 0;
}
