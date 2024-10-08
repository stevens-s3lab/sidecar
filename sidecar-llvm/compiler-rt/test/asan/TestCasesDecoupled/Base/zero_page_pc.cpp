// Check that ASan correctly detects SEGV on the zero page.
// RUN: %clangxx_asan %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

typedef void void_f();
int main() {
  void_f *func = (void_f *)0x4;
  func();
  // x86 reports the SEGV with both address=4 and pc=4.
  // On PowerPC64 ELFv1, the pointer is taken to be a function-descriptor
  // pointer out of which three 64-bit quantities are read. This will SEGV, but
  // the compiler is free to choose the order. As a result, the address is
  // either 0x4, 0xc or 0x14. The pc is still in main() because it has not
  // actually made the call when the faulting access occurs.
  // CHECK: {{AddressSanitizer: (SEGV|access-violation).*(address|pc) 0x0*[4c]}}
  return 0;
}
