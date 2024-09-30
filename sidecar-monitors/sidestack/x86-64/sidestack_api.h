#ifndef SIDESTACK_API_H
#define SIDESTACK_API_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define SIDESTACK_API_DEBUG 0
#define SIDESTACK_STATS 1
#define SIDESTACK_STACK_SIZE 16384

uint32_t *shstk_base = NULL;
uint32_t *shstk_curr = NULL;

unsigned long long int shstk_pushes, shstk_pops = 0ULL;

void sidestack_initialize(){
    // Initialize a sidestack of size defined above
    shstk_base = (uint32_t*)malloc(sizeof(uint32_t) * SIDESTACK_STACK_SIZE);
    if(shstk_base == NULL){
        fprintf(stderr, "Failed to allocate shadow stack.");
        exit(1);
    }
    shstk_curr = shstk_base;
#if SIDESTACK_API_DEBUG
    printf("SideStack initialized at %p\n", (void *)shstk_base);
#endif
}

void sidestack_cleanup(){
    if (shstk_base != NULL) {
	    free(shstk_base);
	    shstk_base = NULL;
	    shstk_curr = NULL;
    }
#if SIDESTACK_STATS
    printf("Pushes: %llu\n", shstk_pushes);
    printf("Pops  : %llu\n", shstk_pops);
#endif
}

void sidestack_push(uint32_t addr) {
#if SIDESTACK_API_DEBUG
    printf("Pushing addr %x at %p\n", addr, (void *)shstk_curr);
#endif
    // Push address
    *shstk_curr = addr;
#if SIDESTACK_API_DEBUG
    printf("Addr %p now contains %x\n", (void *)shstk_curr, *shstk_curr);
#endif
    // Increment pointer
    (shstk_curr)++;
#if SIDESTACK_API_DEBUG
    printf("Pointer now at %p\n", (void *)shstk_curr);
#endif
#if SIDESTACK_STATS
    shstk_pushes++;
#endif
}

void sidestack_pop(uint32_t addr) {
    // Decrement current pointer to access last used element
    (shstk_curr)--;
#if SIDESTACK_API_DEBUG
    printf("Popping addr %x at %p\n", addr, (void *)shstk_curr);
#endif

    // Read current address
    uint32_t compare_addr = *shstk_curr;    
#if SIDESTACK_API_DEBUG
    printf("Addr at top is %x\n", *shstk_curr);
#endif

    // Compare
    if (compare_addr != addr){
        // If mismatch, go down the stack to see if we need to unwind
        uint32_t *temp_pos = shstk_curr;
        int unwind = 0;
        //printf("-----------Violation-------------\n");
        //printf("-SideStack --------- Packet Addr-\n");
        //printf("%x != %x\n", compare_addr, addr);
        //printf("---------------------------------\n");
        temp_pos = shstk_curr;
#if 1
        while(temp_pos > shstk_base){
            if(*temp_pos == addr){
                printf("------------Mismatch-------------\n");
                printf("SideStack Addr       : %x\n", compare_addr);
                printf("Incoming Packet Addr : %x\n", addr);
                printf("--Match Found -- Unwinding SideStack--\n");
                printf("Current SdStk Pointer : %p\n", (void *)shstk_curr);
                printf("New SdStk Pointer     : %p\n", (void *)temp_pos);
                printf("---------------------------------\n\n");
                shstk_curr = temp_pos;
                unwind = 1;
                break;
            }
            (temp_pos)--;
        }
        if(unwind != 1){
            printf("-----------Violation-------------\n");
            printf("SideStack Addr       : %x\n", compare_addr);
            printf("Incoming Packet Addr : %x\n", addr);
            printf("---------No Match Found-------------\n\n");
        }
#endif
    }

#if SIDESTACK_STATS
    shstk_pops++;
#endif
}

#endif

