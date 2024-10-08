// Check that user may include ASan interface header.
// RUN: %clang_asan %s -o %t && %run taskset -c 0 %t & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %clang_asan -x c %s -o %t && %run taskset -c 0 %t & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %clang %s -pie -o %t && %run taskset -c 0 %t & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %clang -x c %s -pie -o %t && %run taskset -c 0 %t & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
#include <sanitizer/asan_interface.h>

int main() {
  return 0;
}
