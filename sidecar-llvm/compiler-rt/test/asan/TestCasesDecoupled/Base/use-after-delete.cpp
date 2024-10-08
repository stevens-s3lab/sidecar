// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK-%os --check-prefix=CHECK
// RUN: %clangxx_asan -O1 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK-%os --check-prefix=CHECK
// RUN: %clangxx_asan -O2 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK-%os --check-prefix=CHECK
// RUN: %clangxx_asan -O3 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s --check-prefix=CHECK-%os --check-prefix=CHECK
// REQUIRES: stable-runtime

#include <stdlib.h>
int main() {
  char * volatile x = new char[10];
  delete[] x;
  return x[5];
  // CHECK: {{.*ERROR: AddressSanitizer: heap-use-after-free }}
}
