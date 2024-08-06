// S3LAB
// vim: ts=2:sw=2:expandtab

extern "C" {
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
}
#include "sanitizer_common/sanitizer_common.h"
#include "sidecar_dev.h"


// For some reason we have two separate drivers and interfaces
#include "../../../../sidecar-driver/aarch64/s3stm.h"

#define FatalError(msg) \
  do {\
    Printf("Sidecar Error: %s\n", msg);\
    Die();\
  } while (0)

using namespace __sanitizer;



namespace __sidecar {

struct devstate {
  int devfd;
  char *stim_base;
};

static struct devstate dstate;
bool sidecar_opened = false;


#if defined(__x86_64__) || defined(__i386__)

// XXX: This should not be hardware specific
#define DEVICE_NAME             "/dev/" PTW_DEV_NAME


void SidecarDeviceInit(bool fake) {
  if (fake) {
    dstate.devfd = -1;
  } else{
    /* open device driver */
    dstate.devfd = open(DEVICE_NAME, O_RDWR);
    if (dstate.devfd < 0) { 
      FatalError("Error opening sidecar device!\n");
    }
    sidecar_opened = true;
  }
}

void SidecarDeviceEnable()
{
  if (dstate.devfd < 0)
    return;
  /* enable pt */ 
  if (ioctl(dstate.devfd, PTW_ENABLE, (void*)NULL) < 0) { 
    FatalError("PTW_ENABLE ioctl failed\n");
  }
}

void SidecarDeviceDisable() { }

void SidecarSetBase(struct dso_info *info) {
  if (dstate.devfd < 0)
    return;

  if (ioctl(dstate.devfd, PTW_SET_BASE, info) < 0) {
    FatalError("PTW_SET_BASE ioctl failed");
  }

  //Printf("Sent ioctl PTW_SET_BASE: handle=%llx, base_address=0x%llx, filename=%s\n",
  //	(unsigned long long)info->handle, info->base_address, info->filename);
}

#elif defined(__aarch64__)

#define STIM_SIZE		0x1000
#define DEVICE_NAME		"/dev/" S3STM_DEVNAME


void SidecarDeviceInit(bool fake)
{
	if (fake) {
		/* mmap stm stimulus ports */ 
		dstate.stim_base = (char *)mmap((void *)STIM_OFFSET,
					 STIM_SIZE, 
#ifdef NOMESSAGES
					 PROT_NONE,
#else
					 PROT_WRITE,
#endif
					 MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, 
					 -1, 
					 0x0); 
		if (dstate.stim_base == MAP_FAILED) { 
			FatalError("asan_stm: error mapping memory!\n");
		}
		dstate.devfd = -1;
	} else {
		/* open device driver */
		dstate.devfd = open(DEVICE_NAME, O_RDWR);
		if (dstate.devfd == -1) {
			FatalError("asan_stm: error opening coresight device!\n");
		}

		/* mmap stm stimulus ports */ 
		dstate.stim_base = (char *)mmap((void *)STIM_OFFSET,
					 STIM_SIZE, 
					 PROT_WRITE, 
					 MAP_SHARED | MAP_FIXED, 
					 dstate.devfd, 
					 0x0); 
		if (dstate.stim_base == MAP_FAILED) { 
			FatalError("asan_stm: error mapping STM stimulus ports!\n");
		}
    sidecar_opened = true;
	}
}

void SidecarDeviceEnable()
{
	if (dstate.devfd < 0)
		return;
	/* enable stm to circular buffer path */
	if (ioctl(dstate.devfd, CS_ENABLE, ETR_CBUFFER) < 0) {
		FatalError("CS_ENABLE ioctl failed\n");
	}

	if (ioctl(dstate.devfd, SET_ASANID, getpid()) < 0) {
		FatalError("SET_ASANID ioctl failed\n");
	}
}

void SidecarSetBase(struct dso_info *info) {}

void SidecarDeviceDisable(void)
{
	if (dstate.devfd < 0)
		return;
	/* call ioctl to disable all coresight devices needed */
	if (ioctl(dstate.devfd, CS_DISABLE, (void*)NULL) < 0) {
		FatalError("SET_ASANID ioctl failed\n");
	}
}
#endif 

} // namespace __sidecar
