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
//#include <intel-pt.h>
#include "pt_opcodes.h"

#include <sys/mman.h>
#include <sys/ioctl.h>

#include <sys/resource.h>
#include <sys/time.h>

#include "../../../sidecar-driver/x86-64/ptw.h"

#include "sidestack_decode.h"

#define CPU_USAGE 0

/* driver defines */
#define DEVICE_NAME         "/dev/"PTW_DEV_NAME
#define TRACE_OUT			"trace.bin"
#define PKT_OUT				"ptw_packet.log"
#define PAGE_SIZE			sysconf(_SC_PAGE_SIZE)
#define READ_BUFFER_SIZE	PAGE_SIZE

/* output trace file */
FILE *trace_out;

/* memory buffer for overflow processing */
char *overflow_buf = NULL;

/* decoder global structs */
struct ptw_decoder sd;

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

unsigned long long process_trace_data(char* buf, unsigned long buf_ofst, unsigned long read_tgt, bool overflow, bool wrap, bool last_read);

void
read_topa(void)
{
	size_t read_size;
	int read_pgs;
	char *local_base = topa;

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
			//printf("Wrapped. Consuming end.\n");
			read_pgs = topa_end - *rpage;
			wrap = true;
		}

		if (read_pgs > 0) {
			read_size = read_pgs * buf_sz;
			//printf("Read size is %d\n", read_size);

			/* process leftover from wraparound */
			if(leftover > 0){
				//printf("Handling leftover of %d\n", leftover);
				leftover = 16 - process_trace_data(overflow_buf, 0, 16, true, false, false);

				for(int j = 0; j < 16; j++){
					overflow_buf[j] = '\0';
				}
			}

			/* wraparound - sync to position after any wraparound has been handled */
			if (leftover > 0){
				i = leftover;
				//i = local_base - topa + leftover;
				leftover = 0;
			}

			i = process_trace_data(topa, i, (unsigned long)local_base + read_size, false, wrap, false);

			//printf("i moved to %d\n", i);

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

	process_trace_data(topa, i, (unsigned long)local_base + buf_sz, false, false, false);
}

void signal_handler(int n, siginfo_t *info, void *unused) {
	running = 0;
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

	/* get rrp and calc topa_end */
	topa_end = topa_size / buf_sz;
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

	/* close device driver */
	close(fd);
}

void 
sidestack_init (void) 
{
	//Initialize side stack
	sidestack_initialize();
}

void
sidestack_deinit (void)
{
	//Cleanup the side stack
	sidestack_cleanup();
}

unsigned long long
process_trace_data(char* buf, unsigned long buf_ofst, unsigned long read_tgt, bool overflow, bool wrap, bool last_read)
{
	/* Current packet opcode byte */
	unsigned char packet_opcode = 0x0;

	/* Local read-until counter; 64-bits */
	unsigned long long read_until;
	if(!overflow && !last_read)
		read_until = read_tgt - buf_ofst - (unsigned long long) buf;
	
	else if (last_read)
		read_until = topa_end - *buf;
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

	//printf("Reading from buffer %llu, offset by %llu, and a target of %llu.\n", buf, buf_offset, read_tgt);

	/* If overflow or last read no need to enter a slow path to account for fragmented packets */
	if(!overflow && !last_read){
		read_until -= 6;
	}

	//printf("Computed read size as %llu and reading until %llu.\n", read_amt, read_until);

	while (i < read_until) {
		//if(last_read && (i > topa_end - *buf))
		//	break;

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
							/* Load 4 bytes of PTWRITE data (32-bit values only) */
							uint32_t ptw_value = *(uint32_t*)(local_ptr + i + 1);

							/* Decode value and run SideStack logic */
							uint32_t op = ptw_value >> 30;
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
									printf("Encountered invalid SideStack opcode %x!\n", op);
							}

							i+=5;

							/* Handled overflow buffer - no need to go further */
							if(overflow)
								return i;

							break;
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
					//printf("	ExtERR %x, %d\n", packet_opcode, i);
					i++;
					break;

				}

				break;

			/* Decode Error */
			default:
				i++;
				break;
		}

		//printf("------------------\n");
	}
	
	/* Take the slow path to check if a PTWRITE has been cut off */
	if(!overflow){
		//printf("Have delta of %d. Entering slow path.\n", read_amt - read_until);
		//printf("i at %d and read_amt at %d.\n", i, read_amt);
		while(i < read_amt){
			packet_opcode = local_ptr[i];
			switch (packet_opcode) {
				/* We're only interested in a packet with an extended opcode + PTWRITE ext opcode */
				case 0x02:
					i++;
					packet_opcode = local_ptr[i];
					if ((packet_opcode & pt_opm_ptw) == pt_ext_ptw){
						//printf("Found cutoff PTWRITE!\n");
						if(wrap){
							i--;
							//printf("Copying into overflow buffer.\n");
							leftover = read_amt - i;
							//printf("leftover is %d\n", leftover);
							memcpy(overflow_buf, local_ptr+i, leftover);
							memcpy(overflow_buf + leftover, topa, 6 - leftover);
							//for(int k = 0; k < 10; k++){
							//	printf("%hhx", overflow_buf[k]);
							//}
							//printf("\n");
						}
						else{
							i--;
							//printf("Keeping i at %d.\n", i);
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
	
	// printf("%ld bytes processed.\n", i);

	return buf_ofst + i;
}
int
main(int argc, char *argv[])
{
	struct sigaction sig;
	sig.sa_sigaction = signal_handler;
	sig.sa_flags = SA_SIGINFO;
  #if CPU_USAGE 
	struct timeval start, stop, diff;
	struct rusage myusage_start, myusage_end;
  #endif

	/* set up signal handler */
	sigaction(SIGUSR1, &sig, NULL);

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
