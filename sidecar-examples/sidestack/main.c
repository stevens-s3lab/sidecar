#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <asm/prctl.h>        /* Definition of ARCH_* constants */
#include <sys/syscall.h>      /* Definition of SYS_* constants */
#include <unistd.h>
#include <syscall.h>


__attribute__((noinline))
static int
leaf(int n)
{
	return n * 2 - 127;
}

__attribute__((noinline))
static int bar(int n)
{
	return leaf(n);
}


__attribute__((noinline))
static int foobar(int n)
{
	n = leaf(n);
	return bar(n);
}

__attribute__((noinline))
static int barfoo(int n)
{
	printf("What barfood %d is this\n", n);
	return foobar(n);
}

static int foo(int n)
{
	n = leaf(n);
	return printf("What foo %d is this\n", bar(n));
}

static int root(int n, int (*fptr)(int))
{
	if (n != 1) {
		printf("yowza %d\n", n);
	}
	return fptr(n);
}

#define arch_prctl(code, addr) syscall(SYS_arch_prctl, (code), (addr))


__attribute__((no_sanitize("shadow-call-stack")))
int main(int argc, char **argv) 
{
	long gs;
	int (*fptr)(int);

#if 0
	void *scs = mmap(NULL, 8192, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANON, -1, 0);
	if (scs == MAP_FAILED) {
		perror("mmap");
		return -1;
	}
	printf("Mapped SCS at %8p\n", scs);

	if (arch_prctl(ARCH_SET_GS, scs) != 0) {
		perror("arch_prctl");
		return -1;
	}
#endif

	if (arch_prctl(ARCH_GET_GS, &gs) != 0) {
		perror("arch_prctl");
		return -1;
	}
	printf("Hello world gs=%lx!\n", gs);

	if (argc == 1) {
		fptr = foo; 
	} else if (argc == 2) {
		fptr = bar;
	} else if (argc == 3) {
		fptr = barfoo;
	} else {
		fptr = foobar;
	} 
	root(argc, fptr);
	fptr(0);
#if 0
	if (arch_prctl(ARCH_SET_GS, 0) != 0) {
		perror("arch_prctl");
		return -1;
	}
#endif
	return 0;
}
