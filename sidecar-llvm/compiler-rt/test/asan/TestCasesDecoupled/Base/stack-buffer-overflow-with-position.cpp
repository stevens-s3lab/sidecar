// RUN: %clangxx_asan -O2 %s -o %t
// RUN: not %run taskset -c 0 %t -2 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-m2 %s
// RUN: not %run taskset -c 0 %t -1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-m1 %s
// RUN: %run taskset -c 0 %t 0 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %run taskset -c 0 %t 8 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: not %run taskset -c 0 %t 9 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor  2>&1 | FileCheck --check-prefix=CHECK-9  %s
// RUN: not %run taskset -c 0 %t 10 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-10 %s
// RUN: not %run taskset -c 0 %t 30 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-30 %s
// RUN: not %run taskset -c 0 %t 31 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-31 %s
// RUN: not %run taskset -c 0 %t 41 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-41 %s
// RUN: not %run taskset -c 0 %t 42 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-42 %s
// RUN: not %run taskset -c 0 %t 62 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-62 %s
// RUN: not %run taskset -c 0 %t 63 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-63 %s
// RUN: not %run taskset -c 0 %t 73 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-73 %s
// RUN: not %run taskset -c 0 %t 74 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-74 %s
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
int main(int argc, char **argv) {
  assert(argc >= 2);
  int idx = atoi(argv[1]);
  char AAA[10], BBB[10], CCC[10];
  memset(AAA, 0, sizeof(AAA));
  memset(BBB, 0, sizeof(BBB));
  memset(CCC, 0, sizeof(CCC));
  int res = 0;
  char *p = AAA + idx;
  printf("AAA: %p\ny: %p\nz: %p\np: %p\n", AAA, BBB, CCC, p);
  // make sure BBB and CCC are not removed;
  return *(short*)(p) + BBB[argc % 2] + CCC[argc % 2];
}
// CHECK-m2: 'AAA'{{.*}} <== {{.*}}underflows this variable
// CHECK-m1: 'AAA'{{.*}} <== {{.*}}partially underflows this variable
// CHECK-9:  'AAA'{{.*}} <== {{.*}}partially overflows this variable
// CHECK-10: 'AAA'{{.*}} <== {{.*}}overflows this variable
// CHECK-30: 'BBB'{{.*}} <== {{.*}}underflows this variable
// CHECK-31: 'BBB'{{.*}} <== {{.*}}partially underflows this variable
// CHECK-41: 'BBB'{{.*}} <== {{.*}}partially overflows this variable
// CHECK-42: 'BBB'{{.*}} <== {{.*}}overflows this variable
// CHECK-62: 'CCC'{{.*}} <== {{.*}}underflows this variable
// CHECK-63: 'CCC'{{.*}} <== {{.*}}partially underflows this variable
// CHECK-73: 'CCC'{{.*}} <== {{.*}}partially overflows this variable
// CHECK-74: 'CCC'{{.*}} <== {{.*}}overflows this variable
// REQUIRES: shadow-scale-3
