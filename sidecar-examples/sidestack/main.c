#include <stdio.h>
#include <stdlib.h>

// Function we will jump to by tampering the return address
__attribute__((noinline))
void other_function() {
    printf("Inside other_function, but we tampered the return address to get here!\n");
}

// Function that will be tampered with
__attribute__((noinline))
void target_function(int tamper) {
    printf("Inside target_function, should return normally.\n");

    if (tamper) {
        // Overwrite the return address on the stack
        void **return_address = (void **)__builtin_frame_address(0) + 1;

        // Set the return address to point to 'other_function'
        printf("Tampering return address to point to other_function at %p\n", (void*)other_function);
        *return_address = (void*)other_function; // Change the return address
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <1 or 2>\n", argv[0]);
        return 1;
    }

    if (atoi(argv[1]) == 1) {
        printf("Triggering shadow call stack violation by tampering return address...\n");
        target_function(1); // Call target_function with tampering
    } else {
        printf("Running normally, no violation.\n");
        target_function(0); // Call target_function without tampering
    }

    return 0;
}
