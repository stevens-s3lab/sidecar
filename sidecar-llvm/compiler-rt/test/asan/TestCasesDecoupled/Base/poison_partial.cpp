// RUN: %clangxx_asan -O0 %s -o %t
// RUN: not %run taskset -c 0 %t  & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor     2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t heap & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: %env_asan_opts=poison_partial=0 %run taskset -c 0 %t & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
// RUN: %env_asan_opts=poison_partial=0 %run taskset -c 0 %t heap & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor
#include <string.h>
char g[21];
char *x;

int main(int argc, char **argv) {
  if (argc >= 2)
    x = new char[21];
  else
    x = &g[0];
  memset(x, 0, 21);
  int *y = (int*)x;
  return y[5];
}
// CHECK: 0 bytes to the right
