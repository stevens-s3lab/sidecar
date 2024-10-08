// RUN: %clangxx_asan %stdcxx11 -O0 -fsanitize-address-use-after-scope %s -o %t
// RUN: not %run taskset -c 0 %t 0 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 2 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 3 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 4 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 5 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 6 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 7 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 8 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 9 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s
// RUN: not %run taskset -c 0 %t 10 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor 2>&1 | FileCheck %s

#include <stdlib.h>
#include <string>
#include <vector>

template <class T> struct Ptr {
  void Store(T *ptr) { t = ptr; }

  void Access() { *t = {}; }

  T *t;
};

template <class T, size_t N> struct Ptr<T[N]> {
  using Type = T[N];
  void Store(Type *ptr) { t = *ptr; }

  void Access() { *t = {}; }

  T *t;
};

template <class T> __attribute__((noinline)) void test() {
  Ptr<T> ptr;
  {
    T x;
    ptr.Store(&x);
  }

  ptr.Access();
  // CHECK: ERROR: AddressSanitizer: stack-use-after-scope
}

int main(int argc, char **argv) {
  using Tests = void (*)();
  Tests tests[] = {
    &test<bool>,
    &test<char>,
    &test<int>,
    &test<double>,
    &test<float>,
    &test<void*>,
    &test<std::vector<std::string>>,
    &test<int[3]>,
    &test<int[1000]>,
    &test<char[3]>,
    &test<char[1000]>,
  };

  int n = atoi(argv[1]);
  if (n == sizeof(tests) / sizeof(tests[0])) {
    for (auto te : tests)
      te();
  } else {
    tests[n]();
  }

  return 0;
}
