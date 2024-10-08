// RUN: %clangxx_asan -O0 -mllvm -asan-instrument-dynamic-allocas %s -o %t
// RUN: not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
//

#include <assert.h>

__attribute__((noinline)) void foo(int index, int len) {
  volatile char str[len] __attribute__((aligned(32)));
  assert(!(reinterpret_cast<long>(str) & 31L));
  str[index] = '1'; // BOOM
// CHECK: ERROR: AddressSanitizer: dynamic-stack-buffer-overflow
// CHECK: WRITE of size 1
}

int main(int argc, char **argv) {
  foo(33, 10);
  return 0;
}
