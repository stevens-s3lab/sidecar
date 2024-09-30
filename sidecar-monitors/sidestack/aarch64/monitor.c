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

#include <sys/mman.h>
#include <sys/ioctl.h>

#include <s3stm.h>

#include "opencsd/c_api/opencsd_c_api.h"
#include "sidestack_decode.h"

/* s3_stm driver defines */
#define DEVICE_NAME             	"/dev/"S3STM_DEVNAME
#define TRACE_OUT			"trace.bin"
#define PKT_OUT				"stm_packet.log"
#define PAGE_SIZE			sysconf(_SC_PAGE_SIZE)
#define READ_BUFFER_SIZE		PAGE_SIZE
#define TMC_RRP                 	0x014
#define TMC_RWP                 	0x018
#define TMC_RRPHI               	0x038
#define TMC_RWPHI               	0x03c

/* opencsd defines */
#define INPUT_BLOCK_SIZE		1024
#define PACKET_STR_LEN 			1024
#define STMTCSR_TRC_ID_MASK		0x007F0000
#define STMTCSR_TRC_ID_SHIFT		16

/* output trace file */
FILE *trace_out;

/* input trace for decoder */
struct stm_decoder {
	uint64_t packets_decoded;
	FILE *trace_in;
	char *data_buffer;
	size_t data_size;
};

/* decoder global structs */
struct stm_decoder sd;

/* file descriptor device */
int fd;

/* etr buffer */
char *etr_buffer;
uint64_t etr_size; 
uint64_t etr_end; 
uint64_t *s_rrp;
char *s_rwp;

/* status */
int running = 1;
unsigned long long int bytes_written = 0;

static char packet_str[PACKET_STR_LEN];

static inline uint64_t
read_reg_pair(char *addr, int lo_offset, int hi_offset)
{
	uint64_t val, vhi, vlo;
	char *vallo;
	char *valhi;

	vallo = addr + lo_offset;
	vlo = (uint64_t)(unsigned char)vallo[3] << 24 
		| vallo[2] << 16 
		| vallo[1] << 8 
		| vallo[0];

	valhi = addr + hi_offset;
	vhi = (uint64_t)(unsigned char)valhi[3] << 24 
		| valhi[2] << 16 
		| valhi[1] << 8 
		| valhi[0];

	val = (vhi << 32) | vlo;
	return val;
}

void 
get_pointers(int i) 
{ 
	uint64_t rwp; 

	/* get rwp */ 
	rwp = read_reg_pair(s_rwp, TMC_RWP, TMC_RWPHI);

	printf("%03d\trwp: %lx rrp: %lx\n",  
			i, rwp, *s_rrp); 
}

void
read_etr_cb(void)
{
	uint64_t rwp;
	size_t read_size;
	char *local_rrp = etr_buffer;

	while (running) {
		/* read mmapped rw: */
		rwp = read_reg_pair(s_rwp, TMC_RWP, TMC_RWPHI);

		read_size = 0;
		if (rwp > *s_rrp) {
			/* calculate available data size */
			read_size = rwp - *s_rrp;
		} else if (rwp < *s_rrp) {
			/* calculate available data size */
			read_size = etr_end - *s_rrp;
		}

		if (read_size > 0) {
			/* write data to file */
			fwrite(local_rrp, 1, read_size, trace_out);

			/* move local rrp */
			local_rrp += read_size;
			if (local_rrp >= etr_buffer + etr_size) {
				local_rrp -= etr_size;
			}

			/* count total trace data written */
			bytes_written += read_size;
			
			/* move shared rrp */
			*s_rrp += read_size;
			/* wrap around */
			if (*s_rrp >= etr_end) {
				*s_rrp -= etr_size;
			}
		} else {
			usleep(5000);
		}
	}

	/* log monitor stats */
	// printf("%lld bytes copied\n", bytes_written);
}

void signal_handler(int n, siginfo_t *info, void *unused) {
	running = 0;
}

void 
cs_init(void)
{
	/* open device driver */
	fd = open(DEVICE_NAME, O_RDWR);
	if (fd < 0) {
		printf("Failed to open %s: %s\n", DEVICE_NAME,
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* open trace file */
	trace_out = fopen(TRACE_OUT, "wb");
	if (trace_out == NULL) {
		printf("Failed to open %s: %s\n", TRACE_OUT,
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* ioctl set monitor */
	if (ioctl(fd, SET_MONITORID, getpid()) < 0) {
		printf("SET_MONITORID ioctl failed with errno %s \n",
				strerror(errno));
	}

	/* mmap shared rrp */
	s_rrp = (uint64_t *)mmap(0, PAGE_SIZE, PROT_WRITE | PROT_READ,
			MAP_SHARED, fd, 0x0);
	if (s_rrp == MAP_FAILED) {
		printf("Failed to map shared rrp\n");
		exit(EXIT_FAILURE);
	}

	/* mmap shared rwp */ 
	s_rwp = (char *)mmap(0, PAGE_SIZE, PROT_READ, 
			MAP_SHARED, fd, 0x0); 
	if (s_rwp == MAP_FAILED) { 
		printf("Failed to map shared rwp\n"); 
		exit(EXIT_FAILURE); 
	}

	/* get etr buffer size */
	if (ioctl(fd, GET_ETR_SZ, (uint64_t*) &etr_size) < 0) {
		printf("GET_ETR_SZ ioctl failed with errno %s \n",
				strerror(errno));
	}

	/* mmap etr buffer */
	etr_buffer = (char *)mmap(0, etr_size, PROT_READ,
			MAP_SHARED, fd, 0x0);
	if (etr_buffer == MAP_FAILED) {
		printf("Failed to map etr buffer\n");
		exit(EXIT_FAILURE);
	}

	/* get rrp and calc etr_end */
	etr_end = *s_rrp + etr_size;
}

void
cs_destroy(void)
{
	/* close trace file */
	fclose(trace_out);

	/* unmapp rrp */
	munmap(s_rrp, PAGE_SIZE);

	/* unmap rwp */
	munmap(s_rwp, PAGE_SIZE);

	/* unmap etr */
	munmap(etr_buffer, etr_size);
	
	/* close device driver */
	close(fd);
}

void
decoder_init(void)
{
	int ret = 0;

	/* open trace file */
	sd.trace_in = fopen(TRACE_OUT, "rb");
	if (sd.trace_in == NULL) {
		printf("Failed to open %s: %s\n", TRACE_OUT,
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* get trace size */
	fseek(sd.trace_in, 0L, SEEK_END);
	sd.data_size = ftell(sd.trace_in);
	rewind(sd.trace_in);

	/* load file in memory */
	sd.data_buffer = malloc(sd.data_size + 1);
	fread(sd.data_buffer, sd.data_size, 1, sd.trace_in);
	fclose(sd.trace_in);

	/* set up logging in the library */
	ret = ocsd_def_errlog_init(OCSD_ERR_SEV_INFO,1);

	/* set up the output - to file and stdout, set custom logfile name */
	if(ret == 0) {
		/* delete existing packet log */
		remove(PKT_OUT);
		ret = ocsd_def_errlog_config_output(C_API_MSGLOGOUT_FLG_FILE, 
				PKT_OUT);
	}
}

void
decoder_destroy(void)
{
	free(sd.data_buffer);
}

ocsd_datapath_resp_t 
packet_handler(void *context, 
		const ocsd_datapath_op_t op, 
		const ocsd_trc_index_t index_sop, 
		const void *p_packet_in)
{
	ocsd_datapath_resp_t resp = OCSD_RESP_CONT;
	ocsd_stm_pkt *pkt = (ocsd_stm_pkt *) p_packet_in;

	switch(op)
	{
		case OCSD_OP_DATA:
			if (pkt->type != STM_PKT_D64)
				break;

      uint32_t ptw_value = pkt->payload.D32;
      uint32_t op = ptw_value >> 30;
      uint32_t addr;

      switch (op){
        case 0x3:
          addr = (ptw_value << 2) >> 2;
          sidestack_pop(addr);
          break;
        case 0x0:
          sidestack_push(ptw_value);
        default:
          printf("Encountered invalid SideStack opcode %lx!\n", op);
          break;
      }

			break;
		case OCSD_OP_EOT:
			sprintf(packet_str, "**** END OF TRACE ****\n");
			ocsd_def_errlog_msgout(packet_str);
			break;

		default: break;
	}

	return resp;
}

/*** generic ***/
static ocsd_err_t 
create_generic_decoder(dcd_tree_handle_t handle, 
		const char *p_name, 
		const void *p_cfg, 
		const void *p_context)
{
	ocsd_err_t ret = OCSD_OK;
	uint8_t CSID = 0;

	ret = ocsd_dt_create_decoder(handle,
			p_name,
			OCSD_CREATE_FLG_PACKET_PROC,
			p_cfg,
			&CSID);

	if(ret == OCSD_OK)
	{
		/* Attach the packet handler to the output of the packet processor */
		ret = ocsd_dt_attach_packet_callback(handle,
				CSID, 
				OCSD_C_API_CB_PKT_SINK,
				&packet_handler,
				p_context);

		if(ret != OCSD_OK)
			/* if the attach failed then destroy the decoder. */
			ocsd_dt_remove_decoder(handle,CSID); 
	}
	
	return ret;
}

/*** STM specific settings ***/
static ocsd_err_t create_decoder_stm(dcd_tree_handle_t dcd_tree_h)
{
	static uint8_t test_trc_id_override = 0x01;
	ocsd_stm_cfg trace_config_stm;

	trace_config_stm.reg_tcsr = 0x00A00005;
	if (test_trc_id_override != 0) {
		trace_config_stm.reg_tcsr &= ~STMTCSR_TRC_ID_MASK;
		trace_config_stm.reg_tcsr |= ((((uint32_t) test_trc_id_override) 
					<< STMTCSR_TRC_ID_SHIFT) & STMTCSR_TRC_ID_MASK);
	}
	trace_config_stm.reg_feat3r = 0x10000;  /* channel default */
	trace_config_stm.reg_devid = 0xFF;      /* master default */

	/* not using hw event trace decode */
	trace_config_stm.reg_hwev_mast = 0;
	trace_config_stm.reg_feat1r = 0;
	trace_config_stm.hw_event = HwEvent_Unknown_Disabled;

	/* create STM decoder */
	return create_generic_decoder(dcd_tree_h, 
			OCSD_BUILTIN_DCD_STM, 
			(void *) &trace_config_stm, 
			0);
}

ocsd_datapath_resp_t 
gen_trace_elem_print(const void *p_context, 
		const ocsd_trc_index_t index_sop, 
		const uint8_t trc_chan_id, 
		const ocsd_generic_trace_elem *elem) {
	ocsd_datapath_resp_t resp = OCSD_RESP_CONT;
	int offset = 0;

	sprintf(packet_str,
			"Idx:%" OCSD_TRC_IDX_STR "; TrcID:0x%02X; ",
		       	index_sop, 
			trc_chan_id);
	offset = strlen(packet_str);

	if (ocsd_gen_elem_str(elem, 
				packet_str + offset, 
				PACKET_STR_LEN - offset) == OCSD_OK) {
		/* add in <CR> */
		if (strlen(packet_str) == PACKET_STR_LEN - 1) 
			/* maximum length */
			packet_str[PACKET_STR_LEN - 2] = '\n';
		else
			strcat(packet_str, "\n");
	} else {
		strcat(packet_str, "Unable to create element string\n");
	}

	/* print it using the library output logger. */
	ocsd_def_errlog_msgout(packet_str);

	return resp;
}

ocsd_err_t 
process_data_block(dcd_tree_handle_t dcd_tree_h, 
		int block_index, 
		char *p_block, 
		const int block_size)
{
	ocsd_err_t ret = OCSD_OK;
	uint32_t bytes_done = 0;
	ocsd_datapath_resp_t dp_ret = OCSD_RESP_CONT;
	uint32_t bytes_this_time = 0;

	while ((bytes_done < (uint32_t) block_size) && (ret == OCSD_OK)) {
		if (OCSD_DATA_RESP_IS_CONT(dp_ret)) {
			dp_ret = ocsd_dt_process_data(dcd_tree_h, 
					OCSD_OP_DATA,
					block_index + bytes_done,
					block_size - bytes_done,
					((uint8_t *) p_block) + bytes_done,
					&bytes_this_time);
			bytes_done += bytes_this_time;
		} else if (OCSD_DATA_RESP_IS_WAIT(dp_ret)) {
			dp_ret = ocsd_dt_process_data(dcd_tree_h, 
					OCSD_OP_FLUSH,
					0,
					0,
					NULL,
					NULL);
		} else {
			/* data path responded with an error - stop processing */
			ret = OCSD_ERR_DATA_DECODE_FATAL;
		}
	}
	return ret;
}

int 
process_trace_data(void)
{
	ocsd_err_t ret = OCSD_OK;
	char message[512];
	dcd_tree_handle_t dcdtree_handle = C_API_INVALID_TREE_HANDLE;
	ocsd_trc_index_t index = 0;
	size_t block_size = INPUT_BLOCK_SIZE;
	struct timespec t1, t2;
	double duration;

	/* print sign-on message in log */
	sprintf(message, "Monitor packet printer\nLibrary Version %s\n\n",
			ocsd_get_version_str());
	ocsd_def_errlog_msgout(message);

	/* create a decode tree for etr data */
	dcdtree_handle = ocsd_create_dcd_tree(OCSD_TRC_SRC_FRAME_FORMATTED, 
			OCSD_DFRMTR_FRAME_MEM_ALIGN);

	/* if tree exists */
	if (dcdtree_handle != C_API_INVALID_TREE_HANDLE) {
		/* create stm specific decoder */
		ret = create_decoder_stm(dcdtree_handle);

		/* set size of desired block */
		block_size = sd.data_size;

		/* get starting time */
		clock_gettime(CLOCK_MONOTONIC, &t1);

		/* feed block to decoder */
		ret = process_data_block(dcdtree_handle, 
				index,
				sd.data_buffer,
				block_size);

		/* get end time */
		clock_gettime(CLOCK_MONOTONIC, &t2);

		/* iterate indexes */
		index += block_size;
		*sd.data_buffer += block_size;

		/* print out execution time */
		duration = ((double)t2.tv_sec + 1.0e-9*t2.tv_nsec) - 
				((double)t1.tv_sec + 1.0e-9*t1.tv_nsec);
		printf("Trace of size %lf MB was decoded in %.5f sec\n", (double) sd.data_size / (1024 * 1024), duration);

		/* no errors - let the data path know we are at end of trace */
		if(ret == OCSD_OK)
			ocsd_dt_process_data(dcdtree_handle, 
					OCSD_OP_EOT, 
					0,
					0,
					NULL,
					NULL);

		/* dispose of the decode tree */
		ocsd_destroy_dcd_tree(dcdtree_handle);
	} else {
		printf("Failed to create trace decode tree\n");
		ret = OCSD_ERR_NOT_INIT;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
#if 1
	struct sigaction sig;
	sig.sa_sigaction = signal_handler;
	sig.sa_flags = SA_SIGINFO;

	/* set up signal handler */
	sigaction(SIGUSR1, &sig, NULL);

	/* initialize for trace capturing */
	cs_init();
		
	/* extract trace data from etr */
	read_etr_cb();

	/* cleanup trace capturing */
	cs_destroy();
#endif

	/* perform all decoder initializations */
	decoder_init();

	/* decode extracted trace */
	process_trace_data();

	/* cleanup decoder */
	decoder_destroy();
			
	return 0;
}

