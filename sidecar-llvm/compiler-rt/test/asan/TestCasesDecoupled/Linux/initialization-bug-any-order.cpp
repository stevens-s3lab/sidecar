// Test to make sure basic initialization order errors are caught.
// Check that on Linux initialization order bugs are caught
// independently on order in which we list source files (if we specify
// strict init-order checking).

// RUN: %clangxx_asan -O0 %s %p/../Helpers/initialization-bug-extra.cpp -o %t
// RUN: %env_asan_opts=strict_init_order=true %run taskset -c 1 %t & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: %clangxx_asan -O0 %p/../Helpers/initialization-bug-extra.cpp %s -o %t
// RUN: %env_asan_opts=strict_init_order=true %run taskset -c 0 %t & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s

// Do not test with optimization -- the error may be optimized away.

#include <cstdio>

// 'y' is a dynamically initialized global residing in a different TU.  This
// dynamic initializer will read the value of 'y' before main starts.  The
// result is undefined behavior, which should be caught by initialization order
// checking.
extern int y;
int __attribute__((noinline)) initX() {
  return y + 1;
  // CHECK: {{AddressSanitizer: initialization-order-fiasco}}
  // CHECK: {{READ of size .* at 0x.* thread T0}}
}

// This initializer begins our initialization order problems.
static int x = initX();

int main() {
  // ASan should have caused an exit before main runs.
  printf("PASS\n");
  return 0;
}
