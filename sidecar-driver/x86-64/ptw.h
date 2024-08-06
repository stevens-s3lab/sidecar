#ifndef PTW_H
#define PTW_H

/* PTW device */
#define PTW_DEV_NAME "ptw"
#define PTW_IOC_MAGIC 't'
#define PTW_IOC_SEQ(n) (n + 0xa0)

/* Returned by PTW_IOC_GETDATA when the device is no longer tracing and no
 * more data are expected */
#define PTW_DONE 0 
/* Returned by PTW_IOC_GETDATA when the device is still tracing, so more data
 * are expected */
#define PTW_TRACING 1

/* How many address ranges are supported for filtering */
#define PTW_FILTER_RANGE_MAX 2U
/* Address range filter type */
enum PTW_FILTER_TYPE { PTW_FILTER_OFF = 0, PTW_FILTER_ON,
	PTW_FILTER_STOP };

/* Add this to order and shift left to get topa page size */
#define PTW_TOPA_PAGE_SHIFT 12 

struct ptw_filter_range {
	unsigned long long start, /* Start virtual address */
		 end;	/* End virtual address of range */
	/* Mode of address range 0: 0 = off, 1 = filter, 2 = trace-stop (if
	 * supported) */
	unsigned char mode;	
};

struct ptw_conf {
	struct ptw_filter_range addr_range[PTW_FILTER_RANGE_MAX];
	unsigned branch:1;	/* Enable branch tracing */
	unsigned user:1;	/* Enable user code tracing */
	unsigned kern:1; 	/* Enable kernel-code tracing */
	unsigned tsc:1;		/* Enable trace timing */
	unsigned cr3:1;		/* Enable CR3 filter */
	unsigned retc:1;	/* Enable return compression */
	unsigned pause:1; 	/* Pause when about to overwrite data */
	/* Send cycle packets every 2^(n-1) cycles, 0 disables.
	 * 0: 0, 1: 1, 2: 2, 3: 4, 4: 8, 5: 16, 6: 32, 7: 64, 
	 * 8: 128, 9: 256, 10: 512, 11: 1024, 12: 2048, 13: 4096, 
	 * 14: 8192, 15: 16384 */
	unsigned short cyc;
	/* Send MTC packets at feq. 2^(n-1) 
	 * MTC will be sent each time the selected ART bit toggles. The
	 * following Encodings are defined:
	 * 0: ART(0), 1: ART(1), 2: ART(2), 3: ART(3), 4: ART(4), 5: ART(5),
	 * 6: ART(6), 7: ART(7), 8: ART(8), 9: ART(9), 10: ART(10), 11: ART(11),
	 * 12: ART(12), 13: ART(13), 14: ART(14), 15: ART(15) */
	unsigned short mtc;
	/* Send PSB packets every 2K^n bytes
	 * Note that PSB insertion is not precise, but the average output bytes
	 * per PSB should approximate the SW selected period. The following
	 * Encodings are defined:
	 * 0: 2K, 1: 4K, 2: 8K, 3: 16K, 4: 32K, 5: 64K, 6: 128K, 7: 256K, 8:
	 * 512K, 9: 1M, 10: 2M, 11: 4M, 12: 8M, 13: 16M, 14: 32M, 15: 64M */
	unsigned short psb;	
	unsigned short topasz;	/* Number of output areas in TOPA */
	/* How frequently should TOPA output areas generate an interrupt:
	 * 0 -> never, 1 -> every entry, 2-> every two entries, etc.
	 * Should divide topasz */
	unsigned short nmi_freq; 
	/* Size of each TOPA output area in 2^n pages. Current kernel
	 * implementations wont probably work for anything > 9 */
	/* 0: 4K, 1: 8K, 2: 16K, 3: 32K, 4: 64K, 5: 128K, 6: 256K, 7: 512K, 8:
	 * 1M, 9: 2M, 10: 4M, 11: 8M, 12: 16M, 13: 32M, 14: 64M, 15: 128M*/
	unsigned char order;
};


/* Configure tracing. Argument is structure pointer (read) */
#define PTW_ENABLE _IO(PTW_IOC_MAGIC, PTW_IOC_SEQ(0))
#define PTW_DISABLE _IO(PTW_IOC_MAGIC, PTW_IOC_SEQ(1))
#define PTW_SET_MID _IO(PTW_IOC_MAGIC, PTW_IOC_SEQ(2))
#define PTW_GET_TOPA_SZ _IOR(PTW_IOC_MAGIC, PTW_IOC_SEQ(3), unsigned int)
#define PTW_GET_BUF_SZ _IOR(PTW_IOC_MAGIC, PTW_IOC_SEQ(4), int)
#define PTW_GET_BUF_OFFSET _IOR(PTW_IOC_MAGIC, PTW_IOC_SEQ(5), unsigned int)



#endif
