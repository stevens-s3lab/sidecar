// RUN: %clang_asan -O2 %s -o %t
// We need replace_intrin=0 to avoid reporting errors in memcpy.
// RUN: %env_asan_opts=replace_intrin=0:check_printf=1 not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck --check-prefix=CHECK-ON %s
// RUN: %env_asan_opts=replace_intrin=0:check_printf=0 %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck --check-prefix=CHECK-OFF %s
// RUN: %env_asan_opts=replace_intrin=0 not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck --check-prefix=CHECK-ON %s

// FIXME: printf is not intercepted on Windows yet.
// XFAIL: windows-msvc

#include <stdio.h>
#include <string.h>
int main() {
  volatile char c = '0';
  volatile int x = 12;
  volatile float f = 1.239;
  volatile char s[] = "34";
  volatile char fmt[2];
  memcpy((char *)fmt, "%c %d %f %s\n", sizeof(fmt));
  printf((char *)fmt, c, x, f, s);
  return 0;
  // Check that format string is sanitized.
  // CHECK-ON: stack-buffer-overflow
  // CHECK-ON-NOT: 0 12 1.239 34
  // CHECK-OFF: 0
}
