// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %clangxx_asan -O1 %s -o %t && not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %clangxx_asan -O2 %s -o %t && not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %clangxx_asan -O3 %s -o %t && not %run taskset -c 0 %t 2>&1 & SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

// CHECK: AddressSanitizer: global-buffer-overflow
int global[10];
void __attribute__((noinline)) call4(int i) { global[i+10]++; }
void __attribute__((noinline)) call3(int i) { call4(i); }
void __attribute__((noinline)) call2(int i) { call3(i); }
void __attribute__((noinline)) call1(int i) { call2(i); }
int main(int argc, char **argv) {
  call1(argc);
  return global[0];
}
