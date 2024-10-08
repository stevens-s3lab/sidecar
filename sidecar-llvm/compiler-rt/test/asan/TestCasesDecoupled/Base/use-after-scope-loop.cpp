// RUN: %clangxx_asan -O1 -fsanitize-address-use-after-scope %s -o %t && \
// RUN:     not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

int *p[3];

int main() {
  for (int i = 0; i < 3; i++) {
    int x;
    p[i] = &x;
  }
  return **p;  // BOOM
  // CHECK: ERROR: AddressSanitizer: stack-use-after-scope
}
