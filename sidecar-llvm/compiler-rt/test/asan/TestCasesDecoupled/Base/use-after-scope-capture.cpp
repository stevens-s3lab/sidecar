// RUN: %clangxx_asan %stdcxx11 -O1 -fsanitize-address-use-after-scope %s -o %t && \
// RUN:     not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <functional>

int main() {
  std::function<int()> f;
  {
    int x = 0;
    f = [&x]() __attribute__((noinline)) {
      return x;  // BOOM
      // CHECK: ERROR: AddressSanitizer: stack-use-after-scope
    };
  }
  return f();  // BOOM
}
