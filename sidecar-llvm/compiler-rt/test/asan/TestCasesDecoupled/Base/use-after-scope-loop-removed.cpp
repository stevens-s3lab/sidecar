// RUN: %clangxx_asan -O1 -fsanitize-address-use-after-scope %s -o %t && \
// RUN:     not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <stdlib.h>

int *p;

int main() {
  for (int i = 0; i < 3; i++) {
    int x;
    p = &x;
  }
  return *p;  // BOOM
  // CHECK: ERROR: AddressSanitizer: stack-use-after-scope
}
