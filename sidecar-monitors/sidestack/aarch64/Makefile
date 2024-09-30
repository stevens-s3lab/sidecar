CC=$(CROSS_COMPILE)gcc
CFLAGS+=-O2 -Wall -I../../../sidecar-driver/aarch64/ -I../../sideasan/aarch64/opencsd/decoder/include 
LIBS+=-lcrypto

BINS=monitor
HEADERS=asan_decode.h asan_api.h ../../../sidecar-driver/aarch64/s3stm.h
MLIBS+=-Lopencsd/decoder/lib/builddir -lopencsd -lopencsd_c_api
src = $(wildcard *.c)
OBJS= $(src:.c=.o)

TGT=-DTGT_AARCH64
CFLAGS+=$(TGT)

ifneq ($(SYSROOT),)
	LDFLAGS+=--sysroot=$(SYSROOT) -Wl,-rpath $(SYSROOT)/lib/aarch64-linux-gnu/
	CFLAGS+=--sysroot=$(SYSROOT)
endif

all: $(BINS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

monitor: ${OBJS}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(MLIBS)

clean:
	rm -f *.o *.log $(BINS) *.out


