// RUN: %clangxx_asan -O0 %s -o %t && SIDECAR_BASE/sidecar/sidecar-monitors/sideasan/x86-64/monitor & not %run taskset -c 0 %t 2>&1 | FileCheck %s

#include <sanitizer/asan_interface.h>
#include <stdio.h>
#include <stdlib.h>

// FIXME: Doesn't work with DLLs
// XFAIL: win32-dynamic-asan

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

char *heap_ptr;

int main() {
  // Disable stderr buffering. Needed on Windows.
  setvbuf(stderr, NULL, _IONBF, 0);

  heap_ptr = (char *)malloc(10);
  fprintf(stderr, "heap_ptr: " PTR_FMT "\n", heap_ptr);

  free(heap_ptr);
  free(heap_ptr);  // BOOM
  return 0;
}

void __asan_on_error() {
  int present = __asan_report_present();
  void *addr = __asan_get_report_address();
  const char *description = __asan_get_report_description();

  fprintf(stderr, "%s\n", (present == 1) ? "report present" : "");
  fprintf(stderr, "addr: " PTR_FMT "\n", addr);
  fprintf(stderr, "description: %s\n", description);
}

// CHECK: AddressSanitizer: attempting double-free
