#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "../../../sidecar-driver/x86-64/ptw.h"
#include "pt_opcodes.h"
#include "sidestack_decode.h"

#define CPU_USAGE 0

/* driver defines */
#define DEVICE_NAME         "/dev/"PTW_DEV_NAME
#define TRACE_OUT           "trace.bin"
#define PKT_OUT             "ptw_packet.log"
#define PAGE_SIZE           sysconf(_SC_PAGE_SIZE)
#define READ_BUFFER_SIZE    PAGE_SIZE

/* controls */
#define MDEBUG 0

/* output trace file */
FILE *trace_out;

/* memory buffer for overflow processing */
char *overflow_buf = NULL;

/* sidetypemap */
uint64_t txt_base_addr = -1;
uint64_t ro_base_addr = -1;

/* file descriptor device */
int fd;

/* topa buffer */
char *topa;
uint64_t topa_size; 
uint64_t *rpage, *wpage;
uint32_t topa_end;
int buf_sz;
unsigned int leftover = 0;

/* status */
int running = 1;
unsigned long long int bytes_written = 0;

/* mapping struct */
#define MAX_TYPEMAP 99999

typedef struct TargetAddress {
    unsigned long long address;
    struct TargetAddress *next;
    void *dso_handle;
} TargetAddress;

TargetAddress *typeIDArray[MAX_TYPEMAP] = {NULL};

typedef struct TypeMap {
    char symbol[256];
    unsigned typeID;
    uint64_t address;
} TypeMap;

struct dso_info idso;

void printTypeMap() {
#if MDEBUG
    printf("\n| CEID\t|\ttrg_addr1\t|\ttrg_addr2\t|\t.....\n");
    printf("+-------+-----------------------+-----------------------+\n");
    for (int i = 0; i < MAX_TYPEMAP; ++i) {
        TargetAddress *current = typeIDArray[i];
        if (current != NULL) {
            printf("| %d\t|", i);
            while (current != NULL) {
                printf("\t 0x%llx\t|", current->address);
                current = current->next;
            }
            printf("\n");
        }
    }
#endif
}

void insertTypeMap(int tID, unsigned long long address, void *dso_handle) {
    TargetAddress *newAddr = (TargetAddress *)malloc(sizeof(TargetAddress));
    if (newAddr == NULL) {
        perror("Failed to allocate memory for new target address");
        exit(EXIT_FAILURE);
    }

    newAddr->address = address;
    newAddr->dso_handle = dso_handle;
    newAddr->next = typeIDArray[tID];
    typeIDArray[tID] = newAddr;
}

void removeEntriesByDSOHandle(void *dso_handle) {
    for (int i = 0; i < MAX_TYPEMAP; ++i) {
        TargetAddress *current = typeIDArray[i];
        TargetAddress *prev = NULL;
        while (current != NULL) {
            if (current->dso_handle == dso_handle) {
                if (prev == NULL) {
                    typeIDArray[i] = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                current = (prev == NULL) ? typeIDArray[i] : prev->next;
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
    printTypeMap();
}

inline int searchTypeMap(int tID, unsigned long long address) {
    TargetAddress *current = typeIDArray[tID];
    while (current != NULL) {
        if (current->address == address) {
            return 1; // Address found
        }
        current = current->next;
    }
    return 0; // Address not found
}

unsigned long long process_trace_data(char* buf, 
                                      unsigned long buf_ofst, 
                                      unsigned long read_tgt, 
                                      bool overflow, 
                                      bool wrap, 
                                      bool last_read);

void read_topa(void) {
    size_t read_size;
    int read_pgs;
    char *local_base = topa;
    unsigned int buf_offset;

    bool wrap = false;
    unsigned long long i = 0;

    /* allocate overflow decoder */
    overflow_buf = (char*)malloc(16);

    /* main read and decode loop */
    while (running) {
        read_pgs = 0;
        if (*wpage > *rpage) {
            /* calculate available data size */
            read_pgs = *wpage - *rpage;
        } else if (*wpage < *rpage) {
            /* calculate available data size */
            read_pgs = topa_end - *rpage;
            wrap = true;
        }

        if (read_pgs > 0) {
            read_size = read_pgs * buf_sz;
            /* process leftover from wraparound */
            if(leftover > 0){
                leftover = 16 - process_trace_data(overflow_buf, 0, 16, true, false, false);

                for(int j = 0; j < 16; j++){
                    overflow_buf[j] = '\0';
                }
            }

            /* wraparound - sync to position after any wraparound has been handled */
            if (leftover > 0){
                i = leftover;
                leftover = 0;
            }

            i = process_trace_data(topa, i, (unsigned long)local_base + read_size, false, wrap, false);

            /* move local base */
            local_base += read_size;
            if (local_base >= topa + topa_size) {
                local_base -= topa_size;
            }

            /* count total trace data written */
            bytes_written += read_size;

            /* move shared rrp */
            *rpage += read_pgs;

            /* wrap around */
            if (*rpage >= topa_end) {
                *rpage -= topa_end;
                wrap = false;
                i = 0;
            }

        }  else {
            usleep(100);
        }
    }

    free(overflow_buf);

    /* get topa page offset */
    if (ioctl(fd, PTW_GET_BUF_OFFSET, (unsigned int*) &buf_offset) < 0) {
        printf("PTW_GET_BUF_OFFSET ioctl failed with errno %s \n",
                strerror(errno));
        exit(EXIT_FAILURE); 
    }

    /* read remaining */
    i = process_trace_data(local_base, 0, buf_offset, false, false, true);
}

void signal_handler(int n, siginfo_t *info, void *unused) {
    running = 0;
}

void load_dso() {
    char filename[260];

    /* add extension for typemap */
    snprintf(filename, sizeof(filename), "%s.tp", idso.filename);

    /* open file sidecfi typemap */
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open typemap %s", filename);
        return;
    }

    char line[512];
    TypeMap typeinfo;

#if MDEBUG
    printf("\nTYPEMAP UPDATE: data read from file %s\n", filename);
#endif
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%255s %u %lx", typeinfo.symbol, &typeinfo.typeID, &typeinfo.address ) == 3) {
            typeinfo.address += idso.base_address;

            insertTypeMap(typeinfo.typeID, typeinfo.address, idso.handle);
        }
    }
    fclose(file);

    printTypeMap();
}

void freeTypeMap() {
    for (int i = 0; i < MAX_TYPEMAP; ++i) {
        TargetAddress *current = typeIDArray[i];
        while (current != NULL) {
            TargetAddress *temp = current;
            current = current->next;
            free(temp);
        }
    }
}

void unload_dso (void) {
    /* free mapping */
    freeTypeMap();
}

void pt_init(void) {
    /* open device driver */
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        printf("Failed to open %s: %s\n", DEVICE_NAME,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* ioctl set monitor */
    if (ioctl(fd, PTW_SET_MID, (void*)NULL) < 0) {
        perror("PTW_SET_MID");
        exit(EXIT_FAILURE);
    }

    /* mmap read pointer */
    rpage = (uint64_t *)mmap(0, PAGE_SIZE, PROT_WRITE | PROT_READ,
            MAP_SHARED, fd, 0x0);
    if (rpage == MAP_FAILED) {
        printf("Failed to mmap read pointer\n");
        exit(EXIT_FAILURE);
    }

    /* mmap write pointer */ 
    wpage = (uint64_t *)mmap(0, PAGE_SIZE, PROT_READ, 
            MAP_SHARED, fd, 0x0); 
    if (wpage == MAP_FAILED) { 
        printf("Failed to mmap write pointer\n"); 
        exit(EXIT_FAILURE); 
    }

    /* get topa size */
    if (ioctl(fd, PTW_GET_TOPA_SZ, (uint64_t*) &topa_size) < 0) {
        printf("PTW_GET_TOPA_SZ ioctl failed with errno %s \n",
                strerror(errno));
        exit(EXIT_FAILURE); 
    }

    /* get single buffer size */
    if (ioctl(fd, PTW_GET_BUF_SZ, (int*) &buf_sz) < 0) {
        printf("PTW_GET_BUF_SZ ioctl failed with errno %s \n",
                strerror(errno));
        exit(EXIT_FAILURE); 
    }

    /* mmap topa buffer */
    topa = (char *)mmap(0, (size_t)topa_size, PROT_READ,
            MAP_SHARED, fd, 0x0);
    if (topa == MAP_FAILED) {
        printf("Failed to map topa\n");
        exit(EXIT_FAILURE);
    }

    /* get the bin base and init typemap */
    idso.handle = 0;
    if (ioctl(fd, PTW_GET_BASE, &idso) < 0) {
        perror("PTW_GET_BASE");
        exit(EXIT_FAILURE);
    }
    load_dso();

    /* get rrp and calc topa_end */
    topa_end = topa_size / buf_sz;

#if 0
    printf("topa_sz: %ld topa_end: %d buf_sz: %d rpage: %ld wpage: %ld\n", 
            topa_size, 
            topa_end,
            buf_sz,
            *rpage,
            *wpage);
#endif
}

void pt_destroy(void) {
    /* unmapp rrp */
    munmap(rpage, PAGE_SIZE);

    /* unmap rwp */
    munmap(wpage, PAGE_SIZE);

    /* unmap etr */
    munmap(topa, topa_size);

    /* cleanup typemap */
    unload_dso();

    /* close device driver */
    close(fd);
}

void sidestack_init (void) {
    //Initialize side stack
    sidestack_initialize();
}

void sidestack_deinit (void) {
    //Cleanup the side stack
    sidestack_cleanup();
}

void print_ptw_details(unsigned long long ptw_value) {
    // Extracting the upper 2 bits for opcode
    unsigned opcode = (ptw_value >> 62) & 0x03; // 0x03 is 00000011 in binary

    // Extracting the next 14 bits for typeID
    unsigned typeID = (ptw_value >> 48) & 0x3FFF; // 0x3FFF is 0011111111111111 in binary

    // Extracting the lower 48 bits for target_addr
    unsigned long long target_addr = ptw_value & 0xFFFFFFFFFFFF; // 0xFFFFFFFFFFFF is 48 bits of 1

    // Printing the extracted details
    printf("opcode: 0x%02x typeID: %d target_addr: 0x%012" PRIx64 "\n", opcode, typeID, (uint64_t)target_addr);
}

unsigned long long process_trace_data(char* buf, unsigned long buf_ofst, unsigned long read_tgt, bool overflow, bool wrap, bool last_read) {
    register unsigned long long target_addr;
    register unsigned typeID;

    /* Current packet opcode byte */
    unsigned char packet_opcode = 0x0;

    /* Local read-until counter; 64-bits */
    unsigned long long read_until;
    if(!overflow && !last_read)
        read_until = read_tgt - buf_ofst - (unsigned long long) buf;
    else
        read_until = read_tgt;

    unsigned long long read_amt = read_until;

    /* Local position counter; 64-bits */
    unsigned long long i = 0;

    /* Compute starting address given arguments */
    char* local_ptr;
    if(!last_read)
        local_ptr = buf + buf_ofst;
    
    else
        local_ptr = buf;

    /* If overflow or last read no need to enter a slow path to account for fragmented packets */
    if(!overflow && !last_read){
        read_until -= 10;
    }

    while (i < read_until) {
        /* Load the current packet opcode byte */
        packet_opcode = local_ptr[i];

        /* First treat the packet opcode like a regular one*/
        switch (packet_opcode) {

            /* PT padding (0x00)*/
            case pt_opc_pad:
                i++;
                break;

            /* PT extension opcode (0x02) */
            /* aka the interesting case */
            case pt_opc_ext:
                /* Increment counter and reload opcode */
                i++;
                packet_opcode = local_ptr[i];

                /* Now spin the wheel on extended opcodes */
                switch(packet_opcode){                
                    default:
                        /* Ext PTWRITE (0x12) */
                        /* NOTE: PTWRITE opcode REQUIRES masking (use 0x1F) */
                        if ((packet_opcode & pt_opm_ptw) == pt_ext_ptw){
                            /* Decode value and run SideGuard logic */
                            uint64_t op = *(uint64_t*)(local_ptr + i + 1) >> 62;
                            if (op == 0x1 || op == 0x2) {
                                // SideCFI Logic
                                unsigned long long ptw_value = *(unsigned long long*)(local_ptr + i + 1);
                                switch (op){
                                    case 0x1:
                                        /* Extracting the next 13 bits for typeID */
                                        typeID = (ptw_value >> 48) & 0x3FFF; 

                                        /* Extracting the lower 48 bits for target_addr */
                                        target_addr = ptw_value & 0xFFFFFFFFFFFF; 

                                        /* Printing the extracted details */
#if MDEBUG
                                        printf("\n[ptw] => opcode: 0x%01lx typeID: %d target_addr: 0x%012lx\n", 
                                                op,
                                                typeID, 
                                                (uint64_t)target_addr);
#endif

                                        bool found = searchTypeMap(typeID, target_addr);

                                        if (!found) { 
                                            printf("CFI CHECK ERROR: no matching address found in the typemap!\n");
                                            printf("MSG{0x%llx, %d}\n",
                                                    target_addr, typeID);
                                            exit(1);
                                        }
#if MDEBUG
                                        else 
                                            printf("CFI CHECK: MSG{0x%llx, %d}\n", target_addr, typeID);
#endif

                                        break;
                                    case 0x2: 
#if MDEBUG
                                        printf("\n[ptw] => opcode: 0x%01lx dlopcode: 0x%01llx handle: 0x%016llx\n",
                                                op,
                                                (ptw_value >> 61) & 0x1,
                                                ptw_value & 0x1FFFFFFFFFFFFFFF);
#endif
                                        if (((ptw_value >> 61) & 0x1) == 0x0) {
                                            /* get the bin base and init typemap */
                                            idso.handle = (void*)(ptw_value & 0x1FFFFFFFFFFFFFFF);
                                            if (ioctl(fd, PTW_GET_BASE, &idso) < 0) {
                                                perror("PTW_GET_BASE");
                                                exit(EXIT_FAILURE);
                                            }
                                            load_dso(); 
                                        } else if (((ptw_value >> 61) & 0x1) == 0x1){
                                            /* unload dso */
                                            void *dso_handle = (void*)(ptw_value & 0x1FFFFFFFFFFFFFFF);
                                            removeEntriesByDSOHandle(dso_handle);
                                        }

                                        break; 
                                    default:
                                        printf("Encountered invalid SideCFI opcode %lx!\n", op);
                                }

                                i+=8;

                                /* Handled overflow buffer - no need to go further */
                                if (overflow)
                                    return i;

                                break;
                            } else if (op == 0x0 || op == 0x3) {
                                // SideStack Logic
                                uint32_t ptw_value = *(uint32_t*)(local_ptr + i + 1);
                                uint32_t addr;
                                switch (op){
                                    case 0x3:
                                        addr = (ptw_value << 2) >> 2;
                                        sidestack_pop(addr);
                                        break;
                                    case 0x0:
                                        sidestack_push(ptw_value);
                                        break;
                                    default:
                                        printf("Encountered invalid SideStack opcode %lx!\n", op);
                                }

                                i+=4;

                                /* Handled overflow buffer - no need to go further */
                                if(overflow)
                                    return i;

                                break;
                            }
                        }

                    /* Not a PTWRITE? Continue processing packets. */
                    /* Ext PSB (0x82) */
                    case pt_ext_psb:
                        i++;
                        break;

                    /* Ext CBR (0x03) */
                    case pt_ext_cbr:
                        i += 2;
                        break;

                    /* Ext PSBEnd (0x23) */
                    case pt_ext_psbend:
                        i++;
                        break;
                    
                    /* No matches: Decode Error */
                    i++;
                    break;

                }

                break;

            /* Decode Error */
            default:
                i++;
                break;
        }
    }
    
    /* Take the slow path to check if a PTWRITE has been cut off */
    if(!overflow){
        while(i < read_amt){
            packet_opcode = local_ptr[i];
            switch (packet_opcode) {
                /* We're only interested in a packet with an extended opcode + PTWRITE ext opcode */
                case 0x02:
                    i++;
                    packet_opcode = local_ptr[i];
                    if ((packet_opcode & pt_opm_ptw) == pt_ext_ptw){
                        if(wrap){
                            i--;
                            leftover = read_amt - i;
                            memcpy(overflow_buf, local_ptr+i, leftover);
                            memcpy(overflow_buf + leftover, topa, 10 - leftover);
                        }
                        else{
                            i--;
                        }

                        return buf_ofst + i;
                    }
                    break;
                
                default:
                    break;
            }
            i++;
        }
    }
    

    return buf_ofst + i;
}

int fileExists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int main(int argc, char *argv[]) {
    struct sigaction sig;
    sig.sa_sigaction = signal_handler;
    sig.sa_flags = SA_SIGINFO;
  #if CPU_USAGE 
    struct timeval start, stop, diff;
    struct rusage myusage_start, myusage_end;
  #endif

    /* set up signal handler */
    sigaction(SIGUSR1, &sig, NULL);

    printf("This is the SideGuard monitor for application %s\n", argv[1]);
    printf("Processing  : Concurrent\n");
    printf("PT Decoder  : Custom\n");

    /* initialize sidestack */
    sidestack_init();

    /* initialize for trace capturing */
    pt_init();

  #if CPU_USAGE 
    getrusage(RUSAGE_SELF, &myusage_start); 
    gettimeofday(&start, NULL);
  #endif

    /* extract trace data from etr */
    read_topa();

  #if CPU_USAGE 
    gettimeofday(&stop, NULL);
    getrusage(RUSAGE_SELF, &myusage_end);

    timersub(&stop, &start, &diff);

    struct timeval utime_diff, stime_diff;
    timersub(&myusage_end.ru_utime, &myusage_start.ru_utime, &utime_diff);
    timersub(&myusage_end.ru_stime, &myusage_start.ru_stime, &stime_diff);

    printf("time=%lu.%06lu utime=%lu.%06lu stime=%lu.%06lu\n",
            diff.tv_sec, (unsigned long)diff.tv_usec, 
            utime_diff.tv_sec, (unsigned long)utime_diff.tv_usec, 
            stime_diff.tv_sec, (unsigned long)stime_diff.tv_usec); 

    double wall_time = diff.tv_sec + diff.tv_usec / 1000000.0; 
    double cpu_time = utime_diff.tv_sec + utime_diff.tv_usec / 1000000.0 + stime_diff.tv_sec + stime_diff.tv_usec / 1000000.0; 
    double cpu_usage = (cpu_time / wall_time) * 100.0; 

    printf("CPU usage = %.2f%%\n", cpu_usage);
  #endif

    /* cleanup trace capturing */
    pt_destroy();

    /* cleanup sidestack */
    sidestack_deinit();

    return 0;
}

