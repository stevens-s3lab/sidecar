// Checks that the ASan debugging API for getting report information
// returns correct values.
// RUN: %clangxx_asan -O0 %s -o %t && not %run taskset -c 0 %t 2>&1 & /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor | FileCheck %s

#include <sanitizer/asan_interface.h>
#include <stdio.h>
#include <stdlib.h>

// FIXME: Doesn't work with DLLs
// XFAIL: win32-dynamic-asan

int main() {
  // Disable stderr buffering. Needed on Windows.
  setvbuf(stderr, NULL, _IONBF, 0);

  char *heap_ptr = (char *)malloc(10);
  free(heap_ptr);
  int present = __asan_report_present();
  fprintf(stderr, "%s\n", (present == 0) ? "no report" : "");
  heap_ptr[0] = 'A'; // BOOM
  return 0;
}

// If we use %p with MSVC, it comes out all upper case. Use %08x to get
// lowercase hex.
#ifdef _MSC_VER
# ifdef _WIN64
#  define PTR_FMT "0x%08llx"
# else
#  define PTR_FMT "0x%08x"
# endif
// Solaris libc omits the leading 0x.
#elif defined(__sun__) && defined(__svr4__)
# define PTR_FMT "0x%p"
#else
# define PTR_FMT "%p"
#endif

void __asan_on_error() {
  int present = __asan_report_present();
  void *pc = __asan_get_report_pc();
  void *bp = __asan_get_report_bp();
  void *sp = __asan_get_report_sp();
  void *addr = __asan_get_report_address();
  int is_write = __asan_get_report_access_type();
  size_t access_size = __asan_get_report_access_size();
  const char *description = __asan_get_report_description();

  fprintf(stderr, "%s\n", (present == 1) ? "report" : "");
  fprintf(stderr, "pc: " PTR_FMT "\n", pc);
  fprintf(stderr, "bp: " PTR_FMT "\n", bp);
  fprintf(stderr, "sp: " PTR_FMT "\n", sp);
  fprintf(stderr, "addr: " PTR_FMT "\n", addr);
  fprintf(stderr, "type: %s\n", (is_write ? "write" : "read"));
  fprintf(stderr, "access_size: %ld\n", access_size);
  fprintf(stderr, "description: %s\n", description);
}

// CHECK: AddressSanitizer: heap-use-after-free
// CHECK: WRITE of size 1
