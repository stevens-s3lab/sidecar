CFLAGS+=-fsanitize=address
LDFLAGS+=-fsanitize=address
CFLAGS+=-mllvm -asan-decouple

CC=../../install/bin/clang

CFLAGS+=-O1 -Wall

.PHONY: all clean

PROGS=so

all: $(PROGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(PROGS): %: %.o
	$(CC) $(LDFLAGS) -o $@ $< 

clean:
	rm -f *.o $(PROGS) 
