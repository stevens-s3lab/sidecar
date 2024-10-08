// RUN: %clangxx_asan %stdcxx11 -O1 -fsanitize-address-use-after-scope %s -o %t && \
// RUN:     not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s


struct IntHolder {
  int val;
};

const IntHolder *saved;

__attribute__((noinline)) void save(const IntHolder &holder) {
  saved = &holder;
}

int main(int argc, char *argv[]) {
  save({argc});
  int x = saved->val;  // BOOM
  // CHECK: ERROR: AddressSanitizer: stack-use-after-scope
  return x;
}
