//===-- scs.cpp -----------------------------------------------------===//
//===----------------------------------------------------------------------===//
//
// This file implements the runtime support for shadow call stack.
//
//===----------------------------------------------------------------------===//

// S3LAB
// source file for STM configuration
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <asm/prctl.h>        /* Definition of ARCH_* constants */
#include <sys/syscall.h>      /* Definition of SYS_* constants */
}

#include "sanitizer_common/sanitizer_common.h"

using namespace __sanitizer;

#define arch_prctl(code, addr) syscall(SYS_arch_prctl, (code), (addr))

#define SCS_SIZE (10*1024*1024) // 10MB

#define FatalError(msg) \
  do {\
    Printf("SCS Error: %s\n", msg);\
    Die();\
  } while (0)

static void setup_shadow_stack(void)
{
	void *scs;

	if (arch_prctl(ARCH_GET_GS, &scs) != 0) {
		FatalError("error when issuing arch_prctl()");
	}

	if (scs != NULL) {
		FatalError("GS already has a value");
	}

	scs = mmap(NULL, SCS_SIZE, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANON, -1, 0);
	if (scs == MAP_FAILED) {
		FatalError("mmap of stack failed");
	}
	//printf("Mapped SCS at %8p\n", scs);

	if (arch_prctl(ARCH_SET_GS, scs) != 0) {
		FatalError("error when issuing arch_prctl()");
	}
}

static void destroy_shadow_stack(void)
{
	if (arch_prctl(ARCH_SET_GS, 0) != 0) {
		FatalError("SCS: error when issuing arch_prctl()");
	}
}


extern "C" SANITIZER_INTERFACE_ATTRIBUTE
#if !SANITIZER_CAN_USE_PREINIT_ARRAY
// On ELF platforms, the constructor is invoked using .preinit_array (see below)
__attribute__((constructor(0)))
#endif
void __scs_init() {
  SanitizerToolName = "SHADOW CALL STACK";
  Printf("Tool %s init!!!\n", SanitizerToolName);
  setup_shadow_stack();
}

extern "C" SANITIZER_INTERFACE_ATTRIBUTE
#if !SANITIZER_CAN_USE_PREINIT_ARRAY
// On ELF platforms, the destructor is invoked using .fini_array (see below)
__attribute__((destructor(0)))
#endif
void __scs_cleanup () {
  Printf("Tool %s cleaning up!!!\n", SanitizerToolName);
  destroy_shadow_stack();
}

#if SANITIZER_CAN_USE_PREINIT_ARRAY
// On ELF platforms, run cfi initialization before any other constructors.
// On other platforms we use the constructor attribute to arrange to run our
// initialization early.
extern "C" {
__attribute__((section(".preinit_array"),
               used)) void (*__scs_preinit)(void) = __scs_init;
__attribute__((section(".fini_array"),
               used)) void (*__scs_fini)(void) = __scs_cleanup;
}
#endif
