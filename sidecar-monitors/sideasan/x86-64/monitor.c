#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
#include "asan_decode.h"

/* driver defines */
#define DEVICE_NAME         "/dev/"PTW_DEV_NAME
#define TRACE_OUT			      "trace.bin"
#define PKT_OUT				      "ptw_packet.log"
#define PAGE_SIZE			      sysconf(_SC_PAGE_SIZE)
#define READ_BUFFER_SIZE	  PAGE_SIZE

/* controls */
#define MDEBUG 0

/* output trace file */
FILE *trace_out;

/* memory buffer for overflow processing */
char *overflow_buf = NULL;

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

	/* get topa page offset */
	if (ioctl(fd, PTW_GET_BUF_OFFSET, (unsigned int*) &buf_offset) < 0) {
		printf("PTW_GET_BUF_OFFSET ioctl failed with errno %s \n",
				strerror(errno));
		exit(EXIT_FAILURE); 
	}

	i = process_trace_data(local_base, 0, buf_offset, false, false, true);
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

	/* close device driver */
	close(fd);
}

void
asan_opcode_parser(uint64_t pkt)
{
	/* reset pkt index */
	asdec.pkt_index = 0;


	/* set L1 parser based on opcode */
	asdec.parser_index = pkt & 0xff;

	printf("OPCODE: %d\n", asdec.parser_index);

	/* case that only a single file is used */
	switch (asdec.parser_index) {
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 16:
			/* call api through L1 */
			parser[asdec.parser_index](pkt);
			break;
		default:
			/* store first pkt */
			asdec.pkt[asdec.pkt_index++] = pkt;
			break;
	}
}

unsigned long long
process_trace_data(char* buf, unsigned long buf_ofst, unsigned long read_tgt, bool overflow, bool wrap, bool last_read)
{
	uint64_t ptw_value;
	uint32_t ptw_value_32;

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

	//printf("Reading from buffer %llu, offset by %llu, and a target of %llu.\n", buf, buf_ofst, read_tgt);

	/* If overflow or last read no need to enter a slow path to account for fragmented packets */
	if(!overflow && !last_read){
		read_until -= 10;
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
							unsigned char payload_size = (packet_opcode >> 5) & 0x03;
							printf("Payload size field: %d\n", payload_size);

							if (payload_size) {
								ptw_value = *(uint64_t*)(local_ptr + i + 1);
								i+=9;
							} else {
								ptw_value_32 = *(uint32_t*)(local_ptr + i + 1);
								ptw_value = (uint64_t)ptw_value_32; 
								i+=5;
							}

							parser[asdec.parser_index](ptw_value);


							/* Handled overflow buffer - no need to go further */
							if (overflow)
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
							memcpy(overflow_buf + leftover, topa, 10 - leftover);
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
	
	//printf("%ld bytes processed.\n", i);

	return buf_ofst + i;
}

int fileExists(const char *filename) {
	struct stat buffer;
	return (stat(filename, &buffer) == 0);
}

void 
asan_init (void) 
{
	/* print layout */
	asan_print_mem_layout();

	/* map shadown memory */
	asan_mmap_shadow();

	/* init parser */
	asdec.parser_index = OPCODE_PARSER;
	asdec.pkt_index = 0;
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

	/* initialize ASAN */
  	asan_init();

	/* extract trace data from topa */
	read_topa();

	/* cleanup trace capturing */
	pt_destroy();

	return 0;
}
