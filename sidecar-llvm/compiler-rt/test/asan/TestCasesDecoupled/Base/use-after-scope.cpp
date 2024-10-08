// RUN: %clangxx_asan -O1 -fsanitize-address-use-after-scope %s -o %t && \
// RUN:     not %run %t 2>&1 | FileCheck %s

// -fsanitize-address-use-after-scope is now on by default:
// RUN: %clangxx_asan -O1 %s -o %t && \
// RUN:     not %run %t 2>&1 | FileCheck %s

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
