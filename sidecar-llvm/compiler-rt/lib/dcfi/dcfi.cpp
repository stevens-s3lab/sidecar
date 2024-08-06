//===-------- dcfi.cpp ----------------------------------------------------===//
//
//===----------------------------------------------------------------------===//
//
// This file implements the runtime support for DECOUPLED CFI.
//
//===----------------------------------------------------------------------===//

extern "C" {
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
}

#include "sanitizer_common/sanitizer_common.h"
#include "interception/interception.h"
#include "sidecar/sidecar_dev.h"


using namespace __sanitizer;

namespace __dcfi {

// Upper 3 bits are used to designate operation

// Reserved for sidestack
//#define SIDESTACK_FN_ENTER 0x0000000000000000
//#define SIDESTACK_FN_RET   (0x0000000000000001 << 61)

#define DCFI_OP_CHECK   (0x0000000000000001ULL << 62)
#define DCFI_OP_DLOPEN  (0x0000000000000004ULL << 61) // opcode 0x2: b100
#define DCFI_OP_DLCLOSE (0x0000000000000005ULL << 61) // opcode 0x2: b101
#define DCFI_OP_BITS	2

static inline void DCfiWrite(uint64_t data)
{
#if defined(__x86_64__)
  asm volatile ("ptwrite %0\n" : : "r" (data));
#else
  asm volatile ("nop\n" : : );
#endif
}

static void DCfiWritestring(const char *str)
{
  int bytes = strlen(str) + 1;
  int aligned_bytes = bytes & ~0x7;
  int unaligned_bytes = bytes & 0x7;
  union {
    uint64_t qword;
    uint8_t bytes[8];
  } data;


  for (const char *str_end = str + aligned_bytes; str < str_end; str += 8) {
    data.qword = *(const uint64_t *)str;
    //Printf("write string: %16llx\n", data.qword);
    DCfiWrite(data.qword);
  }

  // Write the remaining bytes using the same endianess
  data.qword = 0;
  for (int i = 0; i < unaligned_bytes; i++) {
    data.bytes[i] = str[i];
  }

  //Printf("write string: %016llx\n", data.qword);
  DCfiWrite(data.qword);
}

} // namespace __dcfi

using namespace __dcfi;

static void EnsureInterceptorsInitialized();

// Setup shadow for dlopen()ed libraries.
// The actual shadow setup happens after dlopen() returns, which means that
// a library can not be a target of any CFI checks while its constructors are
// running. It's unclear how to fix this without some extra help from libc.
// In glibc, mmap inside dlopen is not interceptable.
// Maybe a seccomp-bpf filter?
// We could insert a high-priority constructor into the library, but that would
// not help with the uninstrumented libraries.

INTERCEPTOR(void*, dlopen, const char *filename, int flag) {
  EnsureInterceptorsInitialized();
  void *handle = REAL(dlopen)(filename, flag);
  if (handle) {
    /* Get the base address of the DSO */
    pid_t pid = getpid();
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);

    FILE *maps = fopen(path, "r");
    if (!maps) {
      perror("fopen");
      return handle;
    }

    char line[256];
    uintptr_t dso_base_address = 0;
    const char *base_filename = filename;
    char pathf[256] = {0};

    /* Remove "./" prefix if it exists */
    if (strncmp(base_filename, "./", 2) == 0) {
      base_filename += 2;
    }

    /* Find the base address of the DSO */
    while (fgets(line, sizeof(line), maps)) {
      if (strstr(line, " r--p ") != NULL && strstr(line, base_filename) != NULL) {
	sscanf(line, "%lx-%*lx %*s %*s %*s %*s %255[^\n]", &dso_base_address, pathf);
        break;
      }
    }

    fclose(maps);

    if (dso_base_address) {
      struct dso_info info;
      info.handle = handle;
      info.base_address = dso_base_address;
      strncpy(info.filename, pathf, sizeof(info.filename) - 1);
      info.filename[sizeof(info.filename) - 1] = '\0';

      /* Call SidecarSetBase */
      __sidecar::SidecarSetBase(&info);
    } else {
      Printf("Failed to find the base address of the DSO %s\n", base_filename);
    }

    DCfiWrite(DCFI_OP_DLOPEN | (uint64_t)handle);
  }
  return handle;
}


INTERCEPTOR(int, dlclose, void *handle) {
  EnsureInterceptorsInitialized();
  int res = REAL(dlclose)(handle);
  if (res == 0)
    DCfiWrite(DCFI_OP_DLCLOSE | (uint64_t)handle);
  return res;
}

static BlockingMutex interceptor_init_lock(LINKER_INITIALIZED);
static bool interceptors_inited = false;

static void EnsureInterceptorsInitialized() {
  BlockingMutexLock lock(&interceptor_init_lock);
  if (interceptors_inited)
    return;

  INTERCEPT_FUNCTION(dlopen);
  INTERCEPT_FUNCTION(dlclose);

  interceptors_inited = true;
}

void get_map_base() {
  pid_t pid = getpid();
  char path[256];
  snprintf(path, sizeof(path), "/proc/%d/maps", pid);

  FILE *maps = fopen(path, "r");
  if (!maps) {
    perror("fopen");
    return;
  }

  char line[256];
  uintptr_t txt_base_address = 0;
  char pathf[256] = {0};

  while (fgets(line, sizeof(line), maps)) {
    if (strstr(line, " r--p ") != NULL) {
      sscanf(line, "%lx-%*lx %*s %*s %*s %*s %255[^\n]", &txt_base_address, pathf);
      break;
    }
  }

  fclose(maps);

  if (!txt_base_address) {
    Printf("Failed to find the base address of the .text segment.\n");
    return;
  }

  struct dso_info info;
  info.handle = (void*)0;
  info.base_address = txt_base_address;
  strncpy(info.filename, pathf, sizeof(info.filename) - 1);
  info.filename[sizeof(info.filename) - 1] = '\0';

  __sidecar::SidecarSetBase(&info);
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE
#if !SANITIZER_CAN_USE_PREINIT_ARRAY
// On ELF platforms, the constructor is invoked using .preinit_array (see below)
__attribute__((constructor(0)))
#endif
void __dcfi_init() {
  SanitizerToolName = "DCFI";
  Printf("Tool %s init!!!\n", SanitizerToolName);
  if (!__sidecar::sidecar_opened) {
    /* call sidecar rt to open driver */
    __sidecar::SidecarDeviceInit(false);
   
    /* get base and share with driver */
    get_map_base();

    // XXX: Run always and tell them which defense we are?
    __sidecar::SidecarDeviceEnable(); 
  }

}

#if SANITIZER_CAN_USE_PREINIT_ARRAY
// On ELF platforms, run cfi initialization before any other constructors.
// On other platforms we use the constructor attribute to arrange to run our
// initialization early.
extern "C" {
__attribute__((section(".preinit_array"),
               used)) void (*__dcfi_preinit)(void) = __dcfi_init;
}
#endif
