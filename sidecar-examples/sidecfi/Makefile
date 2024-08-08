CFLAGS+=-fvisibility=hidden -flto -fsanitize=cfi-icall -fsanitize-cfi-cross-dso -fsanitize-cfi-decouple
#LDFLAGS+=-ldl

CC=../../install/bin/clang

CFLAGS+=-O1 -Wall

.PHONY: all clean

PROGS=main

all: $(PROGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(PROGS): %: %.o
	$(CC) $(LDFLAGS) -o $@ $< 

clean:
	rm -f *.o $(PROGS) 
