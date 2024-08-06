
#ifeq ($(CROSS_COMPILE),"")
#	CROSS=""
#else
#	CROSS="CROSS_COMPILE=${CROSS_COMPILE}"
#endif

CROSS=
ifdef CROSS_COMPILE
	CROSS=CROSS_COMPILE=${CROSS_COMPILE}
endif

# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq (${KERNELRELEASE},)
	obj-m += cs_driver.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
endif

all:
	make ARCH=arm64 ${CROSS} -C ${KERNELDIR} \
		M=${PWD} modules

clean:
	make -C ${KERNELDIR} M=${PWD} clean

