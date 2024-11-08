// RUN: %clangxx_asan -O %s -o %t
// RUN: not %run taskset -c 0 %t crash & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-CRASH %s
// RUN: not %run taskset -c 0 %t bad-bounds & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-BAD-BOUNDS %s
// RUN: not %run taskset -c 0 %t bad-alignment & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck --check-prefix=CHECK-BAD-ALIGNMENT %s
// RUN: %env_asan_opts=detect_container_overflow=0 %run taskset -c 0 %t crash & SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor
//
// Test crash due to __sanitizer_annotate_contiguous_container.

#include <assert.h>
#include <string.h>

extern "C" {
void __sanitizer_annotate_contiguous_container(const void *beg, const void *end,
                                               const void *old_mid,
                                               const void *new_mid);
}  // extern "C"

static volatile int one = 1;

int TestCrash() {
  long t[100];
  t[60] = 0;
  __sanitizer_annotate_contiguous_container(&t[0], &t[0] + 100, &t[0] + 100,
                                            &t[0] + 50);
// CHECK-CRASH: AddressSanitizer: container-overflow
  return (int)t[60 * one];  // Touches the poisoned memory.
}

void BadBounds() {
  long t[100];
// CHECK-BAD-BOUNDS: ERROR: AddressSanitizer: bad parameters to __sanitizer_annotate_contiguous_container
  __sanitizer_annotate_contiguous_container(&t[0], &t[0] + 100, &t[0] + 101,
                                            &t[0] + 50);
}

void BadAlignment() {
  int t[100];
// CHECK-BAD-ALIGNMENT: ERROR: AddressSanitizer: bad parameters to __sanitizer_annotate_contiguous_container
  __sanitizer_annotate_contiguous_container(&t[1], &t[0] + 100, &t[1] + 10,
                                            &t[0] + 50);
}

int main(int argc, char **argv) {
  assert(argc == 2);
  if (!strcmp(argv[1], "crash"))
    return TestCrash();
  else if (!strcmp(argv[1], "bad-bounds"))
    BadBounds();
  else if (!strcmp(argv[1], "bad-alignment"))
    BadAlignment();
}
