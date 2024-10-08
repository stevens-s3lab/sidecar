// RUN: %clangxx_asan -O0 -mllvm -asan-instrument-dynamic-allocas %s -o %t
// RUN: not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
//

// This is reduced testcase based on Chromium code.
// See http://reviews.llvm.org/D6055?vs=on&id=15616&whitespace=ignore-all#toc.

#include <stdint.h>
#include <assert.h>

int a = 7;
int b;
int c;
int *p;

__attribute__((noinline)) void fn3(int *first, int second) {
}

int main() {
  int d = b && c;
  int e[a];
  assert(!(reinterpret_cast<uintptr_t>(e) & 31L));
  int f;
  if (d)
    fn3(&f, sizeof 0 * (&c - e));
  e[a] = 0;
// CHECK: ERROR: AddressSanitizer: dynamic-stack-buffer-overflow
// CHECK: WRITE of size 4
  return 0;
}
