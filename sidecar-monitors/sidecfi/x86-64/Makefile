CC=$(CROSS_COMPILE)gcc
CFLAGS+=-O3 -Wall -I../../../sidecar-driver/x86-64/ 
LIBS+=-lcrypto

BINS=monitor
src = $(wildcard *.c)
OBJS= $(src:.c=.o)

TGT=-DTGT_X86_64
CFLAGS+=$(TGT)

all: $(BINS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

monitor: ${OBJS}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *.log $(BINS) *.out


