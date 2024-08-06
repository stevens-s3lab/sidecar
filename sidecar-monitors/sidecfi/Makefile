CC=$(CROSS_COMPILE)gcc
CFLAGS+=-O3 -Wall -I../../sidecar-driver/x86-64/ 
LIBS+=-lcrypto

BINS=monitor
src = $(wildcard *.c)
OBJS= $(src:.c=.o)

ARCH=$(shell $(CC) -v 2>&1 | grep Target:)
ifeq ($(ARCH), Target: x86_64-linux-gnu)
	TGT=-DTGT_X86_64
else
	TGT=-DTGT_AARCH64
endif
CFLAGS+=$(TGT)

ifneq ($(SYSROOT),)
	LDFLAGS+=--sysroot=$(SYSROOT) -Wl,-rpath $(SYSROOT)/lib/aarch64-linux-gnu/
	CFLAGS+=--sysroot=$(SYSROOT)
endif

all: $(BINS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

monitor: ${OBJS}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *.log $(BINS) *.out


