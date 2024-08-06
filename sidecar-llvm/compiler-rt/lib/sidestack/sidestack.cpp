//===-- sidestack.cpp -----------------------------------------------------===//
//===----------------------------------------------------------------------===//
//
// This file implements the runtime support for the side stack.
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
}

#include "sanitizer_common/sanitizer_common.h"
#include "sidecar/sidecar_dev.h"

using namespace __sanitizer;

extern "C" SANITIZER_INTERFACE_ATTRIBUTE
#if !SANITIZER_CAN_USE_PREINIT_ARRAY
// On ELF platforms, the constructor is invoked using .preinit_array (see below)
__attribute__((constructor(0)))
#endif
void __sidestack_init() {
  SanitizerToolName = "SIDESTACK";
  Printf("Tool %s init!!!\n", SanitizerToolName);
  if (!__sidecar::sidecar_opened) {
    __sidecar::SidecarDeviceInit(false);
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
               used)) void (*__sidestack_preinit)(void) = __sidestack_init;
}
#endif
