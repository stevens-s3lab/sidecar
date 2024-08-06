// S3LAB
// source file for STM configuration
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "decouple_device.h"
// TODO: This code does not need to be ASan specific, so we should make it so in
// the future
#include "asan_internal.h"
#include "asan_descriptions.h"

#define FatalError(msg) \
	do {\
		__asan::Decorator d;\
		Printf("%s%s%s", d.Error(), msg, d.Default());\
		Die();\
	} while (0)

using namespace __sanitizer;

#ifdef USE_PTWRITE

#include <sys/ioctl.h>
#include "../../../../sidecar-driver/x86-64/ptw.h"

struct devstate {
	int devfd;
};

static struct devstate dstate;

#define DEVICE_NAME             "/dev/" PTW_DEV_NAME


void DecoupleDeviceInit(bool fake) {
	if (fake) {
		dstate.devfd = -1;
	} else{
		/* open device driver */
		dstate.devfd = open(DEVICE_NAME, O_RDWR);
		if (dstate.devfd < 0) { 
			FatalError("Error opening pt device!\n");
		} 
	}
}

void DecoupleDeviceEnable()
{
	if (dstate.devfd < 0)
		return;
	/* enable pt */ 
	if (ioctl(dstate.devfd, PTW_ENABLE, (void*)NULL) < 0) { 
		FatalError("PTW_ENABLE ioctl failed\n");
	}

}

void DecoupleDeviceDisable() { }

#else // !USE_PTWRITE


#include "../../../../sidecar-driver/aarch64/s3stm.h"

struct devstate {
	int devfd;
	char *stim_base;
};

static struct devstate dstate;

#define STIM_SIZE		0x1000
#define DEVICE_NAME		"/dev/" S3STM_DEVNAME


void DecoupleDeviceInit(bool fake)
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
	}
}

void DecoupleDeviceEnable()
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

void DecoupleDeviceDisable(void)
{
	if (dstate.devfd < 0)
		return;
	/* call ioctl to disable all coresight devices needed */
	if (ioctl(dstate.devfd, CS_DISABLE, (void*)NULL) < 0) {
		FatalError("SET_ASANID ioctl failed\n");
	}
}
#endif
