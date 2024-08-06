#export CROSS_COMPILE=""
#export ARCH=x86
KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m += ptw.o

all:
	make -C ${KERNELDIR} M=${PWD}

clean:
	make -C ${KERNELDIR} M=${PWD} clean
