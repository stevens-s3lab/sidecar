// RUN: %clangxx_asan -O1 -fsanitize-address-use-after-scope %s -o %t && \
// RUN:     not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

int *p;
bool b = true;

int main() {
  if (b) {
    int x[5];
    p = x+1;
  }
  return *p;  // BOOM
  // CHECK: ERROR: AddressSanitizer: stack-use-after-scope
}
