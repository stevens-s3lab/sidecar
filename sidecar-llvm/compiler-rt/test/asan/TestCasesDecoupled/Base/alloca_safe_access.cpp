// RUN: %clangxx_asan -O0 -mllvm -asan-instrument-dynamic-allocas %s -o %t
// RUN: %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor
//

#include <assert.h>

__attribute__((noinline)) void foo(int index, int len) {
  volatile char str[len] __attribute__((aligned(32)));
  assert(!(reinterpret_cast<long>(str) & 31L));
  str[index] = '1';
}

int main(int argc, char **argv) {
  foo(4, 5);
  foo(39, 40);
  return 0;
}
