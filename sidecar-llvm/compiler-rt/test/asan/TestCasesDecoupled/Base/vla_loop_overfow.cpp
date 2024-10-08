// RUN: %clangxx_asan -O0 -mllvm -asan-instrument-dynamic-allocas %s -o %t
// RUN: not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
//
// REQUIRES: stable-runtime

#include <assert.h>
#include <stdint.h>

void foo(int index, int len) {
  for (int i = 1; i < len; ++i) {
    char array[len];
    assert(!(reinterpret_cast<uintptr_t>(array) & 31L));
    array[index + i] = 0;
// CHECK: ERROR: AddressSanitizer: dynamic-stack-buffer-overflow
// CHECK: WRITE of size 1
  }
}

int main(int argc, char **argv) {
  foo(9, 21);
  return 0;
}
