#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <asm/prctl.h>        /* Definition of ARCH_* constants */
#include <sys/syscall.h>      /* Definition of SYS_* constants */
#include <unistd.h>
#include <syscall.h>

__attribute__((noinline))
static int leaf(int n)
{
	return n * 2 - 127;
}

__attribute__((noinline))
static int tamper_return_address()
{
	// Get the current frame pointer (x86_64 specific code)
	void **frame_pointer;
	asm volatile ("movq %%rbp, %0" : "=r"(frame_pointer));

	// Overwrite the return address (two levels up)
	void **return_address = frame_pointer + 1;  // Move to the return address
	printf("Tampering return address from %p to NULL\n", *return_address);
	*return_address = NULL;  // Set it to NULL or an invalid address to cause a crash
	return 0;
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

	// Trigger the test based on the argument
	if (argc > 1 && atoi(argv[1]) == 1) {
		printf("Triggering shadow call stack violation by tampering with the return address.\n");
		tamper_return_address();
	} else {
		printf("Running normally, no violation.\n");
	}

	// Call the function, which might have its return address tampered with
	root(argc, fptr);
	fptr(0);

	return 0;
}
