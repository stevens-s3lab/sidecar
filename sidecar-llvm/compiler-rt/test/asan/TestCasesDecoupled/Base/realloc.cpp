// RUN: %clangxx_asan -O0 %s -o %t
// Default is true (free on realloc to 0 size)
// RUN: %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %env_asan_opts=allocator_frees_and_returns_null_on_realloc_zero=true %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %env_asan_opts=allocator_frees_and_returns_null_on_realloc_zero=false %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=NO-FREE

#include <stdio.h>
#include <stdlib.h>

int main() {
  void *p = malloc(42);
  p = realloc(p, 0);
  if (p) {
    // NO-FREE: Allocated something on realloc(p, 0)
    fprintf(stderr, "Allocated something on realloc(p, 0)\n");
  } else {
    // CHECK: realloc(p, 0) returned nullptr
    fprintf(stderr, "realloc(p, 0) returned nullptr\n");
  }
  free(p);
}
