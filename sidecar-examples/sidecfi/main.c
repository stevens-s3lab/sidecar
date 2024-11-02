#include <stdio.h>

// Function declarations
void foo(void);
void bar(void);
void different_signature(int);

// Type definition for a function pointer
typedef void (*func_ptr)(void);

// Main function
int main(int argc, char **argv) {
    // Valid function pointers
    func_ptr ptr1 = foo;
    func_ptr ptr2 = bar;

    // Invalid function pointer (should trigger CFI violation)
    func_ptr invalid_ptr = (func_ptr)different_signature;

    // Call first legal func
    ptr1();

    if (argc != 2) {
	    printf("Usage: %s <1 or 2>\n", argv[0]);
	    return 1;
    }

    if (atoi(argv[1]) == 1) {
    	// This line should trigger a CFI violation
    	invalid_ptr();
    } else {
	// This shoud not trigger a CFI violation
    	ptr2();
    }

    return 0;
}

// Function definitions
void foo(void) {
    printf("This is foo.\n");
}

void bar(void) {
    printf("This is bar.\n");
}

void different_signature(int x) {
    printf("This is different_signature with argument %d.\n", x);
}
