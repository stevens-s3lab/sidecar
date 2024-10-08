// RUN: %clangxx_asan -O0 -mllvm -asan-instrument-dynamic-allocas %s -o %t
// RUN: not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
//
// REQUIRES: stable-runtime

#include <assert.h>
#include <stdint.h>

__attribute__((noinline)) void foo(int index, int len) {
  if (index > len) {
    char str[len]; //NOLINT
    assert(!(reinterpret_cast<uintptr_t>(str) & 31L));
    str[index] = '1'; // BOOM
// CHECK: ERROR: AddressSanitizer: dynamic-stack-buffer-overflow
// CHECK: WRITE of size 1
  }
}

int main(int argc, char **argv) {
  foo(33, 10);
  return 0;
}
