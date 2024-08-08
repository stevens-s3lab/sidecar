CFLAGS+=-fsanitize=shadow-call-stack -fsanitize-sidestack
LDFLAGS+=-fsanitize=shadow-call-stack -fsanitize-sidestack

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
