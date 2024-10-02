#include <stdio.h>
#include <stdlib.h>
#include <asm/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>
#include <syscall.h>

#define arch_prctl(code, addr) syscall(SYS_arch_prctl, (code), (addr))

// Function that will be tampered with
__attribute__((noinline))
void test_function() {
    printf("Inside test_function\n");
}

// Function to tamper with return address
void tamper_return_address() {
    // Access the current frame pointer (x86_64 specific)
    void **frame_pointer;
    asm volatile ("movq %%rbp, %0" : "=r"(frame_pointer));

    // Overwrite the return address (which is right above the frame pointer)
    void **return_address = frame_pointer + 1;
    printf("Tampering return address from %p to NULL\n", *return_address);
    *return_address = NULL; // Set it to an invalid address to trigger a violation
}

__attribute__((no_sanitize("shadow-call-stack"))) // Disable shadow call stack for main
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <1 or 2>\n", argv[0]);
        return 1;
    }

    if (atoi(argv[1]) == 1) {
        printf("Triggering shadow call stack violation...\n");
        tamper_return_address(); // Tamper with the return address
    } else {
        printf("Running normally, no violation.\n");
    }

    test_function(); // Call the function (return address will be tampered with in case 1)

    return 0;
}
