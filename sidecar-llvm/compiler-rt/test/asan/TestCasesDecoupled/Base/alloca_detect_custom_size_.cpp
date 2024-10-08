// RUN: %clangxx_asan -O0 -mllvm -asan-instrument-dynamic-allocas %s -o %t
// RUN: not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
//

#include <assert.h>

struct A {
  char a[3];
  int b[3];
};

__attribute__((noinline)) void foo(int index, int len) {
  volatile struct A str[len] __attribute__((aligned(32)));
  assert(!(reinterpret_cast<long>(str) & 31L));
  str[index].a[0] = '1'; // BOOM
// CHECK: ERROR: AddressSanitizer: dynamic-stack-buffer-overflow
// CHECK: WRITE of size 1
}

int main(int argc, char **argv) {
  foo(10, 10);
  return 0;
}
