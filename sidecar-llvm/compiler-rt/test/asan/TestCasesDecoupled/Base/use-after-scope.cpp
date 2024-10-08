// RUN: %clangxx_asan -O1 -fsanitize-address-use-after-scope %s -o %t && \
// RUN:     not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

// -fsanitize-address-use-after-scope is now on by default:
// RUN: %clangxx_asan -O1 %s -o %t && \
// RUN:     not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

volatile int *p = 0;

int main() {
  {
    int x = 0;
    p = &x;
  }
  *p = 5;  // BOOM
  // CHECK: ERROR: AddressSanitizer: stack-use-after-scope
  return 0;
}
