// Check that LSan annotations work fine.
// RUN: %clangxx_asan -O0 %s -o %t && %run taskset -c 0 %t & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %clangxx_asan -O3 %s -o %t && %run taskset -c 0 %t & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor

#include <sanitizer/lsan_interface.h>
#include <stdlib.h>

int *x, *y;

int main() {
  x = new int;
  __lsan_ignore_object(x);

  {
    __lsan::ScopedDisabler disabler;
    y = new int;
  }

  x = y = nullptr;
  return 0;
}
