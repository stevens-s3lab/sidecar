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

#include "../../sidecar-driver/x86-64/ptw.h"
#include "pt_opcodes.h"

/* driver defines */
#define DEVICE_NAME         "/dev/"PTW_DEV_NAME
#define TRACE_OUT			      "trace.bin"
#define PKT_OUT				      "ptw_packet.log"
#define PAGE_SIZE			      sysconf(_SC_PAGE_SIZE)
#define READ_BUFFER_SIZE	  PAGE_SIZE

/* controls */
#define MDEBUG 1

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

void
read_topa(void)
{
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

void 
load_dso() 
{
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

      /* here you need to see if it's a vtable or not and add offset */
      // sym_offset = 0x0;

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

void
unload_dso (void)
{
  /* free mapping */
  freeTypeMap();
}

void 
pt_init(void)
{
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

void
pt_destroy(void)
{
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

unsigned long long
process_trace_data(char* buf, unsigned long buf_ofst, unsigned long read_tgt, bool overflow, bool wrap, bool last_read)
{
  register unsigned long long target_addr;
  register unsigned typeID;
  unsigned long long ip_bytes;

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

  /* If overflow or last read, no need to enter a slow path to account for fragmented packets */
  if(!overflow && !last_read){
    read_until -= 10;
  }

  /* Variables for TNT bytes and current addresses */
  unsigned long long tnt_bytes = 0;
  static uint64_t current_typeID = 0;
  static uint64_t current_target_addr = 0;
  static uint64_t current_callsite_addr = 0;

  while (i < read_until) {
    /* Load the current packet opcode byte */
    packet_opcode = local_ptr[i];

    /* First treat the packet opcode like a regular one */
    switch (packet_opcode) {

      /* PT padding (0x00) and TNT-8 (0x00) */
      case pt_opc_pad:
        /* Both pt_opc_pad and pt_opc_tnt_8 have value 0x00 */
        if (local_ptr[i] == pt_opc_pad) {
          /* PAD packet */
          i++;
        } else {
          /* TNT-8 packet */
          i++;
#if MDEBUG
          printf("[TNT-8]\n");
#endif
        }
        break;

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
              /* Load 8 bytes of PTWRITE data (64-bit values only) */
              unsigned long long ptw_value = *(unsigned long long*)(local_ptr + i + 1);

              /* Decode value and run SideCFI logic */
              uint64_t op = ptw_value >> 62;
              switch (op){
                case 0x1:
                  /* Extracting the next 13 bits for typeID */
                  typeID = (ptw_value >> 48) & 0x3FFF; 

                  /* Extracting the lower 48 bits for target_addr */
                  target_addr = ptw_value & 0xFFFFFFFFFFFF; 

#if MDEBUG
                /* Printing the extracted details */
                printf("\n[ptw] => opcode: 0x%01lx typeID: %d target_addr: 0x%012lx\n", 
                       op,
                       typeID, 
                       (uint64_t)target_addr);
#endif

#if 0
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
            } else {
              i++;
              break;
            }

          case pt_ext_psb:
            /* PSB packet */
            i += 1;
            break;

          case pt_ext_cbr:
            /* CBR packet */
            i += 2;
            break;

          case pt_ext_psbend:
            /* PSBEnd packet */
            i += 1;
            break;

          case pt_ext_ovf:
            /* Overflow packet */
            i += 1;
            break;

          case pt_ext_tnt_64:
            /* TNT-64 packet */
            {
              tnt_bytes = 0;
              do {
                i++;
                tnt_bytes++;
              } while (local_ptr[i] & 0x80);
              /* Include the last byte */
              i++;
            }
#if MDEBUG
          printf("[TNT-64] tnt_bytes: %llu\n", tnt_bytes);
#endif
          break;
        }
        break;

      case pt_opc_tsc:
        /* TSC packet */
        i += 1 + 7; /* opcode + 7-byte payload */
        break;

      case pt_opc_tip:
      case pt_opc_tip_pge:
      case pt_opc_tip_pgd:
        {
          /* TIP packet */
          ip_bytes = 0;
          if (local_ptr[i] & 0x10) {
            /* Has IP payload */
            ip_bytes = ((local_ptr[i] >> 5) & 0x3) + 1;
            target_addr = 0;
            for (int byte = 0; byte < ip_bytes; ++byte) {
              target_addr |= ((uint64_t)local_ptr[i + 1 + byte]) << (8 * byte);
            }
            current_target_addr = target_addr;

#if MDEBUG
            printf("[TIP] target_addr: 0x%llx\n", current_target_addr);
#endif

            /* Advance i by opcode byte + IP bytes */
            i += 1 + ip_bytes;
          } else {
            /* No IP payload */
            i += 1;
          }
        }
        break;

      case pt_opc_fup:
        {
          /* FUP packet */
          ip_bytes = 0;
          if (local_ptr[i] & 0x10) {
            /* Has IP payload */
            ip_bytes = ((local_ptr[i] >> 5) & 0x3) + 1;
            target_addr = 0;
            for (int byte = 0; byte < ip_bytes; ++byte) {
              target_addr |= ((uint64_t)local_ptr[i + 1 + byte]) << (8 * byte);
            }
            current_callsite_addr = target_addr;

#if MDEBUG
            printf("[FUP] callsite_addr: 0x%llx\n", current_callsite_addr);
#endif

            /* Advance i by opcode byte + IP bytes */
            i += 1 + ip_bytes;
          } else {
            /* No IP payload */
            i += 1;
          }
        }
        break;

      case pt_opc_mode:
        /* MODE packet */
        i += 2;
        break;

      default:
        /* Unknown or unhandled opcode */
        i++;
        break;
    }

    /* Perform CFI check if all components are available */
    if (current_typeID != 0 && current_callsite_addr != 0 && current_target_addr != 0) {
      bool valid = searchTypeMap(current_typeID, current_target_addr);
      if (!valid) {
        printf("CFI CHECK ERROR at callsite 0x%llx: target 0x%llx not valid for typeID %u\n",
               current_callsite_addr, current_target_addr, (unsigned)current_typeID);
        exit(1);
      } else {
#if MDEBUG
        printf("CFI CHECK OK at callsite 0x%llx: target 0x%llx valid for typeID %u\n",
               current_callsite_addr, current_target_addr, (unsigned)current_typeID);
#endif
      }

      /* Reset the variables */
      current_typeID = 0;
      current_callsite_addr = 0;
      current_target_addr = 0;
    }
  }

  /* Handle potential packet cutoff */
  if(!overflow){
    while(i < read_amt){
      packet_opcode = local_ptr[i];
      switch (packet_opcode) {
        /* We're only interested in a packet with an extended opcode + PTWRITE ext opcode */
        case pt_opc_ext:
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

int
main(int argc, char *argv[])
{
  struct sigaction sig;
  sig.sa_sigaction = signal_handler;
  sig.sa_flags = SA_SIGINFO;

  /* set up signal handler */
  sigaction(SIGUSR1, &sig, NULL);

  /* initialize for trace capturing */
  pt_init();

  /* extract trace data from etr */
  read_topa();

  /* cleanup trace capturing */
  pt_destroy();

  return 0;
}
