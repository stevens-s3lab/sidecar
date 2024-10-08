// Test for LeakSanitizer+AddressSanitizer of different sizes.
// REQUIRES: leak-detection
//
// RUN: %clangxx_asan -O0 %s -o %t
// RUN: not %run taskset -c 0 %t 0 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 1000 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 1000000 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 10000000 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s

#include <cstdlib>
#include <stdio.h>
#include <thread>
int *t;

__attribute__((noopt)) void leak(int n) {
#if defined(__ANDROID__) || defined(__BIONIC__)
  // Bionic does not acutally allocate when n==0, hence
  // there would not be a leak.
  // Re-adjust n so the test can pass.
  if (n == 0)
    n = 1;
#endif

  // Repeat few times to make sure that at least one pointer is
  // not somewhere on the stack.
  for (int i = 0; i < 10; ++i) {
    t = new int[n];
    printf("t: %p\n", t);
    t = 0;
  }
}

int main(int argc, char **argv) {
  leak(atoi(argv[1]));
}
// CHECK: LeakSanitizer: detected memory leaks
