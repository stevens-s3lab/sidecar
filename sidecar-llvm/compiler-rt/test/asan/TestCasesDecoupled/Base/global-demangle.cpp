// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

namespace XXX {
class YYY {
 public:
  static char ZZZ[];
};
char YYY::ZZZ[] = "abc";
}

int main(int argc, char **argv) {
  return (int)XXX::YYY::ZZZ[argc + 5];  // BOOM
  // CHECK: {{READ of size 1 at 0x.*}}
}
