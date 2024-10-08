// Minimal test for LeakSanitizer+AddressSanitizer.
// REQUIRES: leak-detection
//
// RUN: %clangxx_asan  %s -o %t
// RUN: /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor & %env_asan_opts=detect_leaks=1 not %run taskset -c 0 %t  2>&1 | FileCheck %s
// RUN: /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor & not %run taskset -c 0 %t  2>&1 | FileCheck %s
// RUN: /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor & %env_asan_opts=detect_leaks=0     %run taskset -c 0 %t
#include <stdio.h>
int *t;

int main(int argc, char **argv) {
  t = new int[argc - 1] - 100000;
  printf("t: %p\n", t);
}
// CHECK: LeakSanitizer: detected memory leaks
