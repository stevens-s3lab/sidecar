#include <stdio.h>
#include <stdlib.h>

// Function that will be called normally
__attribute__((noinline))
void target_function() {
    printf("Inside target_function, should return normally.\n");
}

// Function we will jump to by tampering the return address
__attribute__((noinline))
void other_function() {
    printf("Inside other_function, but we tampered the return address to get here!\n");
}

// Function to tamper with the return address of target_function
void tamper_return_address() {
    // Get the current frame pointer (x86_64 specific)
    void **frame_pointer;
    asm volatile ("movq %%rbp, %0" : "=r"(frame_pointer));

    // Overwrite the return address (right above the frame pointer)
    void **return_address = frame_pointer + 1;

    // Set the return address to the address of 'other_function'
    printf("Tampering return address to point to other_function at %p\n", (void*)other_function);
    *return_address = (void*)other_function; // Make the return go to a legit function
}

__attribute__((no_sanitize("shadow-call-stack"))) // Disable shadow call stack for main
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <1 or 2>\n", argv[0]);
        return 1;
    }

    if (atoi(argv[1]) == 1) {
        printf("Triggering shadow call stack violation by tampering return address.\n");
        tamper_return_address(); // Modify return address to a legit function (other_function)
    } else {
        printf("Running normally, no violation.\n");
    }

    target_function(); // This should return to 'other_function' if tampered

    return 0;
}
