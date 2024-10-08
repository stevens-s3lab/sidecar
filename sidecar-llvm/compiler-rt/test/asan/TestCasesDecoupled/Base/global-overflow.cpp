// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %clangxx_asan -O1 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %clangxx_asan -O2 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s
// RUN: %clangxx_asan -O3 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <string.h>
int main(int argc, char **argv) {
  static char XXX[10];
  static char YYY[10];
  static char ZZZ[10];
  memset(XXX, 0, 10);
  memset(YYY, 0, 10);
  memset(ZZZ, 0, 10);
  int res = YYY[argc * 10];  // BOOOM
  // CHECK: {{READ of size 1 at 0x.* thread T0}}
  res += XXX[argc] + ZZZ[argc];
  return res;
}
