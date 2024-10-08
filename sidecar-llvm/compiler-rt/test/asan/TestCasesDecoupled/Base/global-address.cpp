// RUN: %clangxx_asan -o %t %s
// RUN: SIDECAR_BASE/sidecar-monitors/sideasan/x86-64/monitor & not %run taskset -c 0 %t 2>&1 | FileCheck %s
#include <sanitizer/allocator_interface.h>

int g_i = 42;
int main() {
  // CHECK: AddressSanitizer: attempting to call __sanitizer_get_allocated_size() for pointer which is not owned
  // CHECK-NOT: AddressSanitizer:DEADLYSIGNAL
  // CHECK: SUMMARY: AddressSanitizer: bad-__sanitizer_get_allocated_size
  // CHECK-NOT: AddressSanitizer:DEADLYSIGNAL
  return (int)__sanitizer_get_allocated_size(&g_i);
}
