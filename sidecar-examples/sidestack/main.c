#include <stdio.h>
#include <stdlib.h>

// Function that will be called and where we will tamper the return address
__attribute__((noinline))
void target_function() {
    printf("Inside target_function, should return normally.\n");
}

// Function to tamper with the return address of target_function
void tamper_return_address() {
    // Get the current frame pointer (x86_64 specific)
    void **frame_pointer;
    asm volatile ("movq %%rbp, %0" : "=r"(frame_pointer));

    // Overwrite the return address (the value right above the frame pointer)
    void **return_address = frame_pointer + 1;

    // Set the return address to some invalid location (NULL in this case)
    printf("Tampering return address from %p to NULL\n", *return_address);
    *return_address = (void*)0xdeadbeef; // Jump to an invalid address
}

__attribute__((no_sanitize("shadow-call-stack"))) // Disable shadow call stack for main
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <1 or 2>\n", argv[0]);
        return 1;
    }

    if (atoi(argv[1]) == 1) {
        printf("Triggering shadow call stack violation by tampering return address.\n");
        tamper_return_address(); // Modify return address to invalid location
    } else {
        printf("Running normally, no violation.\n");
    }

    target_function(); // This should trigger the violation if return address is tampered

    return 0;
}
