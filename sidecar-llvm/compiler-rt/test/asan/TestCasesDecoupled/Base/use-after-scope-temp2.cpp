// RUN: %clangxx_asan %stdcxx11 -O1 -fsanitize-address-use-after-scope %s -o %t && \
// RUN:     not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s


struct IntHolder {
  __attribute__((noinline)) const IntHolder &Self() const {
    return *this;
  }
  int val = 3;
};

const IntHolder *saved;

int main(int argc, char *argv[]) {
  saved = &IntHolder().Self();
  int x = saved->val;  // BOOM
  // CHECK: ERROR: AddressSanitizer: stack-use-after-scope
  return x;
}
