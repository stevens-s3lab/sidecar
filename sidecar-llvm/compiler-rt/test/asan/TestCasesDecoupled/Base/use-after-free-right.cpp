// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK-%os --check-prefix=CHECK
// RUN: %clangxx_asan -O1 %s -o %t && not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK-%os --check-prefix=CHECK
// RUN: %clangxx_asan -O2 %s -o %t && not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK-%os --check-prefix=CHECK
// RUN: %clangxx_asan -O3 %s -o %t && not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK-%os --check-prefix=CHECK
// REQUIRES: stable-runtime

// Test use-after-free report in the case when access is at the right border of
// the allocation.

#include <stdlib.h>
int main() {
  volatile char *x = (char*)malloc(sizeof(char));
  free((void*)x);
  *x = 42;
  // CHECK: {{.*ERROR: AddressSanitizer: heap-use-after-free }}
}
