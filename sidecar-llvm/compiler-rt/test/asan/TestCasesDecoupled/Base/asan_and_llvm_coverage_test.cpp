// RUN: %clangxx_asan -coverage -O0 %s -o %t
// RUN: /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor & %env_asan_opts=check_initialization_order=1 %run taskset -c 0 %t 2>&1 | FileCheck %s

#include <stdio.h>
int foo() { return 1; }
int XXX = foo();
int main() {
  printf("PASS\n");
// CHECK: PASS
}
