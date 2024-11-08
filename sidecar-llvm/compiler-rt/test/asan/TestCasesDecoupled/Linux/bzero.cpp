// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s

// REQUIRES: !android

#include <assert.h>
#include <strings.h>

int main(int argc, char *argv[]) {
  char buf[100];
  // *& to suppress bzero-to-memset optimization.
  (*&bzero)(buf, sizeof(buf) + 1);
  // CHECK:      AddressSanitizer: stack-buffer-overflow
  // CHECK-NEXT: WRITE of size 101 at
  return 0;
}
