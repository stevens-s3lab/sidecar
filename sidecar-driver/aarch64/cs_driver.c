#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/slab.h>
#include <linux/stm.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/coresight.h>
#include <linux/coresight-stm.h>
#include <linux/init.h>
#include <linux/types.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/pm_runtime.h>
#include <linux/iopoll.h>
#include <linux/time.h>
#include <linux/dmapool.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/of_irq.h>
#include <linux/workqueue.h> 
#include <linux/delay.h>
#include <linux/interrupt.h>

#include "s3stm.h"

/* Device macros */
#define DEVNAME	S3STM_DEVNAME
#define SYSFS_CLASSNAME "s3"
#define DEVMODE 0666


/* MMIO ports based vaddr for CoreSight components */
struct cs_mmio_struct {
	void  __iomem *cti_sys0;
	void  __iomem *cti_cpu0;
	void  __iomem *cti_cpu1;
	void  __iomem *cti_cpu2;
	void  __iomem *cti_cpu3;
	void  __iomem *stm_base;
	void  __iomem *etr_base;
	void  __iomem *etf_base;
	void  __iomem *tpiu_base;
	void  __iomem *fun0_base;
	void  __iomem *rep0_base;
	void  __iomem *stim_base;
};

struct cs_mmio_struct csmmio;


/* CoreSight stuff */
#define DB401C_BASE_CTI_SYS0	0x00810000
#define DB401C_BASE_CTI_CPU0	0x00858000
#define DB401C_BASE_CTI_CPU1	(DB401C_BASE_CTI_CPU0 + 0x1000)
#define DB401C_BASE_CTI_CPU2	(DB401C_BASE_CTI_CPU0 + 0x2000)
#define DB401C_BASE_CTI_CPU3	(DB401C_BASE_CTI_CPU0 + 0x3000)
#define MMIO_PAGE_SIZE		0x1000




/* CTI programming registers */
#define CTICONTROL		0x000
#define CTIINTACK		0x010
#define CTIAPPSET		0x014
#define CTIAPPCLEAR		0x018
#define CTIAPPPULSE		0x01C
#define CTIINEN(n)		(0x020 + (4 * n))
#define CTIOUTEN(n)		(0x0A0 + (4 * n))
#define CTITRIGINSTATUS		0x130
#define CTITRIGOUTSTATUS	0x134
#define CTICHINSTATUS		0x138
#define CTICHOUTSTATUS		0x13C
#define CTIGATE			0x140
#define ASICCTL			0x144

/* TMC Registers */
#define TMC_RSZ			0x004
#define TMC_STS			0x00c
#define TMC_RRD			0x010
#define TMC_RRP			0x014
#define TMC_RWP			0x018
#define TMC_TRG			0x01c
#define TMC_CTL			0x020
#define TMC_RWD			0x024
#define TMC_MODE		0x028
#define TMC_LBUFLEVEL		0x02c
#define TMC_CBUFLEVEL		0x030
#define TMC_BUFWM		0x034
#define TMC_RRPHI		0x038
#define TMC_RWPHI		0x03c
#define TMC_AXICTL		0x110
#define TMC_DBALO		0x118
#define TMC_DBAHI		0x11c
#define TMC_FFSR		0x300
#define TMC_FFCR		0x304
#define TMC_PSCR		0x308
#define TMC_ITMISCOP0		0xee0
#define TMC_ITTRFLIN		0xee8
#define TMC_ITATBDATA0		0xeec
#define TMC_ITATBCTR2		0xef0
#define TMC_ITATBCTR1		0xef4
#define TMC_ITATBCTR0		0xef8
#define TMC_AUTHSTATUS		0xfb8

/* register description */
/* TMC_CTL - 0x020 */
#define TMC_CTL_CAPT_EN		BIT(0)
/* TMC_STS - 0x00C */
#define TMC_STS_TMCREADY_BIT	2
#define TMC_STS_FULL		BIT(0)
#define TMC_STS_FTEMPTY		BIT(3)
#define TMC_STS_TRIGGERED	BIT(1)
#define TMC_STS_MEMERR		BIT(5)
#define TMC_AXICTL_CLEAR_MASK 0xfbf
#define TMC_AXICTL_ARCACHE_MASK (0xf << 16)

#define TMC_AXICTL_PROT_CTL_B0	BIT(0)
#define TMC_AXICTL_PROT_CTL_B1	BIT(1)
#define TMC_AXICTL_SCT_GAT_MODE	BIT(7)
#define TMC_AXICTL_WR_BURST_16	0xF00
/* Write-back Read and Write-allocate */
#define TMC_AXICTL_AXCACHE_OS	(0xf << 2)
#define TMC_AXICTL_ARCACHE_OS	(0xf << 16)

/* TMC_FFCR - 0x304 */
#define TMC_FFCR_FLUSHMAN_BIT	6
#define TMC_FFCR_EN_FMT		BIT(0)
#define TMC_FFCR_EN_TI		BIT(1)
#define TMC_FFCR_FON_FLIN	BIT(4)
#define TMC_FFCR_FON_TRIG_EVT	BIT(5)
#define TMC_FFCR_TRIGON_TRIGIN	BIT(8)
#define TMC_FFCR_STOP_ON_FLUSH	BIT(12)


#define TMC_DEVID_NOSCAT	BIT(24)

#define TMC_DEVID_AXIAW_VALID	BIT(16)
#define TMC_DEVID_AXIAW_SHIFT	17
#define TMC_DEVID_AXIAW_MASK	0x7f

#define TMC_AUTH_NSID_MASK	GENMASK(1, 0)

/* STM Registers */
#define STMDMASTARTR			0xc04
#define STMDMASTOPR			0xc08
#define STMDMASTATR			0xc0c
#define STMDMACTLR			0xc10
#define STMDMAIDR			0xcfc
#define STMHEER				0xd00
#define STMHETER			0xd20
#define STMHEBSR			0xd60
#define STMHEMCR			0xd64
#define STMHEMASTR			0xdf4
#define STMHEFEAT1R			0xdf8
#define STMHEIDR			0xdfc
#define STMSPER				0xe00
#define STMSPTER			0xe20
#define STMPRIVMASKR			0xe40
#define STMSPSCR			0xe60
#define STMSPMSCR			0xe64
#define STMSPOVERRIDER			0xe68
#define STMSPMOVERRIDER			0xe6c
#define STMSPTRIGCSR			0xe70
#define STMTCSR				0xe80
#define STMTSSTIMR			0xe84
#define STMTSFREQR			0xe8c
#define STMSYNCR			0xe90
#define STMAUXCR			0xe94
#define STMSPFEAT1R			0xea0
#define STMSPFEAT2R			0xea4
#define STMSPFEAT3R			0xea8
#define STMITTRIGGER			0xee8
#define STMITATBDATA0			0xeec
#define STMITATBCTR2			0xef0
#define STMITATBID			0xef4
#define STMITATBCTR0			0xef8


/* STM IT REGISTERS */
#define STMITTRIGGER			0xee8
#define STMITATBDATA0			0xeec
#define STMITATBCTR2			0xef0
#define STMITATBID			0xef4
#define STMITATBCTR0			0xef8
/* TMC IT REGISTERS */
#define TMC_ITMISCOP0           	0xee0
#define TMC_ITTRFLIN            	0xee8
#define TMC_ITATBDATA0          	0xeec
#define TMC_ITATBCTR2           	0xef0
#define TMC_ITATBCTR1           	0xef4
#define TMC_ITATBCTR0           	0xef8
/* CTI IT REGISTERS */
#define ITCHINACK			0xEDC /* WO CTI CSSoc 400 only*/
#define ITTRIGINACK			0xEE0 /* WO CTI CSSoc 400 only*/
#define ITCHOUT				0xEE4 /* WO RW-600 */
#define ITTRIGOUT			0xEE8 /* WO RW-600 */
#define ITCHOUTACK			0xEEC /* RO CTI CSSoc 400 only*/
#define ITTRIGOUTACK			0xEF0 /* RO CTI CSSoc 400 only*/
#define ITCHIN				0xEF4 /* RO */
#define ITTRIGIN			0xEF8 /* RO */
/* management registers */
#define CTIDEVAFF0			0xFA8
#define CTIDEVAFF1			0xFAC
/*
 ** Coresight management registers (0xf00-0xfcc)
 ** 0xfa0 - 0xfa4: Management	registers in PFTv1.0
 **		  Trace		registers in PFTv1.1
 **/
#define CORESIGHT_ITCTRL		0xf00
#define CORESIGHT_CLAIMSET		0xfa0
#define CORESIGHT_CLAIMCLR		0xfa4
#define CORESIGHT_LAR			0xfb0
#define CORESIGHT_LSR			0xfb4
#define CORESIGHT_DEVARCH		0xfbc
#define CORESIGHT_AUTHSTATUS		0xfb8
#define CORESIGHT_DEVID			0xfc8
#define CORESIGHT_DEVTYPE		0xfcc

/* Replicator registers */
#define REPLICATOR_IDFILTER0		0x000
#define REPLICATOR_IDFILTER1		0x004

/* Funnel registers */
#define FUNNEL_FUNCTL			0x000
#define FUNNEL_PRICTL			0x004

#define FUNNEL_HOLDTIME_MASK		0xf00
#define FUNNEL_HOLDTIME_SHFT		0x8
#define FUNNEL_HOLDTIME			(0x7 << FUNNEL_HOLDTIME_SHFT)
#define FUNNEL_ENSx_MASK		0xff

#define ETR_PAGES_DEFAULT		2560 /* 10 MB */

static int etr_pages = ETR_PAGES_DEFAULT;
module_param(etr_pages, int, 0);

static int pause = 1;
module_param(pause, int, 0);

static size_t etr_size = ETR_PAGES_DEFAULT * PAGE_SIZE;

#define TIMEOUT_US			100

/* QGIC2 IRQs  */
#define	SF_MODE				0 /* set to 1 for SFIFO */
#define GIC_SPI 			0
#define GIC_PPI 			1
#define SPI_IRQ				166
#define PPI_IRQ				30
#define	HWIRQ_SPI 			SPI_IRQ - 32
#define	HWIRQ_PPI 			PPI_IRQ - 16

/* set mode to PPI or SPI */
#define IRQ_MODE			GIC_PPI
#if(IRQ_MODE == GIC_SPI) 
#define HWIRQ				HWIRQ_SPI
#define IRQ_TYPE			IRQ_TYPE_LEVEL_HIGH
#else
#define HWIRQ				HWIRQ_PPI
#define IRQ_TYPE			IRQ_TYPE_LEVEL_HIGH
#endif

struct dma_buf {
	void *vaddr; /* Virtual address */
	dma_addr_t daddr; /* DMA handler */

};

struct dma_buf etr_buf;

/* physical bases */
phys_addr_t 			stim_phys;

/* etr pointers */
u32				full;
u64				rrp, rwp;
u64 				*s_rrp;

/* irq number */
/* asan state */
struct asan_state {
	u64	etr_start;
	u64	etr_end;
	u32 	asanid;
	u32 	monitorid;
	u32 	irq;
	etr_mode_t etr_mode;
	u8 	stm_running;
	u8	asan_paused;
};

/* change to list */
static struct asan_state *asansts;

/* handler bottom half */
void handler_bh(struct work_struct *work); 
DECLARE_WORK(workqueue, handler_bh);

/* define operation callback routines */
static int cs_char_mmap(struct file *file, struct vm_area_struct *vma);
static int cs_char_open(struct inode *inode, struct file *file);
static int cs_char_release(struct inode *inode, struct file *file);
static ssize_t cs_char_read(struct file *file, char __user *data, size_t len,
		loff_t *ppos);
static long cs_char_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


static const struct vm_operations_struct cs_mmap_vmops = {
	.open			= NULL,
	.close			= NULL,
};

static const struct file_operations mychardev_fops = {
	.owner      		= THIS_MODULE,
	.open			= cs_char_open,
	.release    		= cs_char_release,
	.mmap			= cs_char_mmap,
	.unlocked_ioctl 	= cs_char_ioctl,
	.read       		= cs_char_read,
	.write       		= NULL
};

struct mychar_device_data {
	struct cdev cdev;
	dev_t devnum;
	struct class *sysfs_class;
	struct device *dev;
};

static struct mychar_device_data mychardev_data;

enum tmc_mode {
	TMC_MODE_CIRCULAR_BUFFER,
	TMC_MODE_SOFTWARE_FIFO,
	TMC_MODE_HARDWARE_FIFO,
};


/* functions */

static inline void CS_LOCK(void __iomem *addr)
{
	do {
		/* Wait for things to settle */
		mb();
		writel_relaxed(0x0, addr + CORESIGHT_LAR);
	} while (0);
}

static inline void CS_UNLOCK(void __iomem *addr)
{
	do {
		writel_relaxed(CORESIGHT_UNLOCK, addr + CORESIGHT_LAR);
		/* Make sure everyone has seen this */
		mb();
	} while (0);
}

static int coresight_tmout(void __iomem *addr, u32 offset, int position, int value)
{
	int i;
	u32 val;

	for (i = TIMEOUT_US; i > 0; i--) {
		val = __raw_readl(addr + offset);
		/* waiting on the bit to go from 0 to 1 */
		if (value) {
			if (val & BIT(position))
				return 0;
			/* waiting on the bit to go from 1 to 0 */
		} else {
			if (!(val & BIT(position)))
				return 0;
		}

		/*
		 * Delay is arbitrary - the specification doesn't say how long
		 * we are expected to wait.  Extra check required to make sure
		 * we don't wait needlessly on the last iteration.
		 */
		if (i - 1)
			udelay(1);
	}

	return -EAGAIN;
}

static inline u64
coresight_read_reg_pair(void __iomem *addr, s32 lo_offset, s32 hi_offset)
{
	u64 val;

	val = readl_relaxed(addr + lo_offset);
	val |= (hi_offset < 0) ? 0 :
		(u64)readl_relaxed(addr + hi_offset) << 32;
	return val;
}

static inline void coresight_write_reg_pair(void __iomem *addr, u64 val,
		s32 lo_offset, s32 hi_offset)
{
	writel_relaxed((u32)val, addr + lo_offset);
	if (hi_offset >= 0)
		writel_relaxed((u32)(val >> 32), addr + hi_offset);
}

void
tmc_wait_for_tmcready(void __iomem *base)
{
	/* Ensure formatter, unformatter and hardware fifo are empty */
	if (coresight_tmout(base,
				TMC_STS, TMC_STS_TMCREADY_BIT, 1)) {
		printk(KERN_WARNING "s3_stm: [warning] timeout while waiting for TMC to be Ready\n");
	}
}

void tmc_flush_and_stop(void __iomem *base)
{
	u32 ffcr;

	ffcr = readl_relaxed(base + TMC_FFCR);
	ffcr |= TMC_FFCR_STOP_ON_FLUSH;
	writel_relaxed(ffcr, base + TMC_FFCR);
	ffcr |= BIT(TMC_FFCR_FLUSHMAN_BIT);
	writel_relaxed(ffcr, base + TMC_FFCR);
	/* Ensure flush completes */
	if (coresight_tmout(base,
				TMC_FFCR, TMC_FFCR_FLUSHMAN_BIT, 0)) {
		printk(KERN_WARNING "s3_stm: [warning] timeout while waiting for completion of Manual Flush\n");
	}

	tmc_wait_for_tmcready(base);
}

void tmc_flush(void __iomem *base)
{
	u32 ffcr;

	CS_UNLOCK(base);
	ffcr = readl_relaxed(base + TMC_FFCR);
	ffcr |= BIT(TMC_FFCR_FLUSHMAN_BIT);
	writel_relaxed(ffcr, base + TMC_FFCR);
	/* Ensure flush completes */
	if (coresight_tmout(base,
				TMC_FFCR, TMC_FFCR_FLUSHMAN_BIT, 0)) {
		printk(KERN_WARNING "s3_stm: [warning] timeout while waiting for completion of Manual Flush\n");
	}
	CS_LOCK(base);
}

static void etr_enable_hw(etr_mode_t etr_mode)
{
	u32 axictl, sts, mode, wm;
	
	CS_UNLOCK(csmmio.etr_base);

	/* Wait for TMCSReady bit to be set */
	tmc_wait_for_tmcready(csmmio.etr_base);

	/* set etr size in 32-bit words */
	writel_relaxed(etr_size / 4, csmmio.etr_base + TMC_RSZ);

	/* enable etr on either CBUF or SWFIFO */
	if (etr_mode) {
		/* set mode to software fifo */
		writel_relaxed(TMC_MODE_SOFTWARE_FIFO, csmmio.etr_base + TMC_MODE);
		asansts->etr_mode = 1;
	} else {
		/* set mode to software fifo */
		writel_relaxed(TMC_MODE_CIRCULAR_BUFFER, csmmio.etr_base + TMC_MODE);
		asansts->etr_mode = 0;
	}

	/* set axictl register */
	axictl = readl_relaxed(csmmio.etr_base + TMC_AXICTL);
	axictl &= ~TMC_AXICTL_CLEAR_MASK;
	axictl |= (TMC_AXICTL_PROT_CTL_B1 | TMC_AXICTL_WR_BURST_16);
	axictl |= TMC_AXICTL_AXCACHE_OS;
	writel_relaxed(axictl, csmmio.etr_base + TMC_AXICTL);
	
	/* set dma buffer on dba register */
	coresight_write_reg_pair(csmmio.etr_base, etr_buf.daddr, TMC_DBALO, TMC_DBAHI);

	/*
	 * I might need to set TMC registers properly
	 * (i.e, RRP/RWP to base address and STS to "not full").
	 */
	coresight_write_reg_pair(csmmio.etr_base, etr_buf.daddr,
			TMC_RWP, TMC_RWPHI);
	coresight_write_reg_pair(csmmio.etr_base, etr_buf.daddr,
			TMC_RRP, TMC_RRPHI);
	sts = readl_relaxed(csmmio.etr_base + TMC_STS) & ~TMC_STS_FULL;
	writel_relaxed(sts, csmmio.etr_base + TMC_STS);

	/* flush and trigger controls */
	writel_relaxed(TMC_FFCR_EN_FMT | TMC_FFCR_EN_TI |
			TMC_FFCR_FON_FLIN | TMC_FFCR_FON_TRIG_EVT |
			TMC_FFCR_TRIGON_TRIGIN,
			csmmio.etr_base + TMC_FFCR);

	/* set watermark in 32-bit words to 25% of total capacity */
	writel_relaxed((etr_size / 0x4) / 0x4, csmmio.etr_base + TMC_BUFWM);

	/* enable tmc */
	writel_relaxed(TMC_CTL_CAPT_EN, csmmio.etr_base + TMC_CTL);

	CS_LOCK(csmmio.etr_base);

	/* get etr mode */
	mode = readl_relaxed(csmmio.etr_base + TMC_MODE);
	wm = readl_relaxed(csmmio.etr_base + TMC_BUFWM);
	printk("s3_stm: ETR enabled in %s mode with watermark set to 0x%x\n", 
		mode == 0x0? "cbuf" : "sfifo", wm);
}

void etr_restart_hw(void)
{
	CS_UNLOCK(csmmio.etr_base);
	writel_relaxed(TMC_CTL_CAPT_EN, csmmio.etr_base + TMC_CTL);
	CS_LOCK(csmmio.etr_base);
}

void
etr_reset_pointers(void)
{
	CS_UNLOCK(csmmio.etr_base);
	coresight_write_reg_pair(csmmio.etr_base, etr_buf.daddr,
			TMC_RWP, TMC_RWPHI);
	coresight_write_reg_pair(csmmio.etr_base, etr_buf.daddr,
			TMC_RRP, TMC_RRPHI);
	CS_LOCK(csmmio.etr_base);

	/* reset shared software read pointer */
	*s_rrp = etr_buf.daddr;
	rrp = *s_rrp;
}

void etr_disable_hw(void)
{
	CS_UNLOCK(csmmio.etr_base);
	writel_relaxed(0x0, csmmio.etr_base + TMC_CTL);
	CS_LOCK(csmmio.etr_base);
}

static void etr_stop(void)
{
	CS_UNLOCK(csmmio.etr_base);
	
	/* flush and stop etr */
	tmc_flush_and_stop(csmmio.etr_base);

	CS_LOCK(csmmio.etr_base);

	/* disable etr */
	etr_disable_hw();
}

void cti_write_single_reg(int offset, u32 value, void __iomem *cti_base)
{
	CS_UNLOCK(cti_base);
	writel_relaxed(value, cti_base + offset);
	CS_LOCK(cti_base);
}

void stm_write_single_reg(int offset, u32 value)
{
	CS_UNLOCK(csmmio.stm_base);
	writel_relaxed(value, csmmio.stm_base + offset);
	CS_LOCK(csmmio.stm_base);
}

void etr_write_single_reg(int offset, u32 value)
{
	CS_UNLOCK(csmmio.etr_base);
	writel_relaxed(value, csmmio.etr_base + offset);
	CS_LOCK(csmmio.etr_base);
}

void etf_write_single_reg(int offset, u32 value)
{
	CS_UNLOCK(csmmio.etf_base);
	writel_relaxed(value, csmmio.etf_base + offset);
	CS_LOCK(csmmio.etf_base);
}

void tpiu_write_single_reg(int offset, u32 value)
{
	CS_UNLOCK(csmmio.tpiu_base);
	writel_relaxed(value, csmmio.tpiu_base + offset);
	CS_LOCK(csmmio.tpiu_base);
}

static int discover_stimulus(struct cs_mmio_struct *io)
{
	struct device_node* dev_node;
	int index = 0;
	int found = 0;
	const char *name = NULL;
	int ret;
	struct resource res;

	/* get device_node for coresight-stm */
	dev_node = of_find_compatible_node(NULL, NULL, "arm,coresight-stm");
	if (!dev_node)
		return -1;

	/* get stimulus base property */
	while (!of_property_read_string_index(dev_node, "reg-names", index, &name)) {
		if (strcmp("stm-stimulus-base", name)) {
			index++;
			continue;
		}

		/* We have a match and @index is where it's at */
		found = 1;
		break;
	}
	if (!found)
		goto err;

	/* get resource struct */
	ret = of_address_to_resource(dev_node, index, &res);
	if (ret)
		goto err;

	/* of_node counter fix */
	of_node_put(dev_node);

	/* get virtual address */
	io->stim_base = ioremap(res.start, resource_size(&res));
	if (!io->stim_base)
		return -1;

	/* get physical address */
	stim_phys = res.start;

	return 0;
err:
	/* done with dev_node */
	of_node_put(dev_node);
	return -1;
}

/* Dynamically discover component/node base address.
 * The functions assumes the each device only exports one page of MMIO.
 */
static int discover_node(const char *node_name, void __iomem **base)
{
	struct device_node* dev_node;
	int index = 0;
	const char *name = NULL;
	int ret;
	struct resource res;

	/* get device_node for coresight-etr */
	dev_node = of_find_node_by_name(NULL, node_name);
	if (!dev_node)
		return -1;

	/* get base property */
	of_property_read_string_index(dev_node, "reg", index, &name);

	/* get resource struct */
	ret = of_address_to_resource(dev_node, index, &res);
	if (ret)
		goto err;

	/* done with dev_node */
	of_node_put(dev_node);

	if (resource_size(&res) != MMIO_PAGE_SIZE) {
		printk(KERN_WARNING DEVNAME": node %s is larger (%llu) than a"
				"page\n", node_name, resource_size(&res));
	}

	/* get virtual address */
	*base = ioremap(res.start, resource_size(&res));
	if (!*base)
		return -1;
	/*
	printk(KERN_INFO DEVNAME": discover %s, 0x%llx:%llu\n", node_name,
			res.start, resource_size(&res));
	*/

	return 0;

err:
	/* done with dev_node */
	of_node_put(dev_node);
	return -1;
}

void
get_stmitctrl(void)
{
	u32 ctrl;

	ctrl = readl_relaxed(csmmio.stm_base + CORESIGHT_ITCTRL);
	printk(KERN_INFO "s3_stm: stmitctrl == 0x%x\n", ctrl);
}

void
get_etritctrl(void)
{
	u32 ctrl;

	ctrl = readl_relaxed(csmmio.etr_base + CORESIGHT_ITCTRL);
	printk(KERN_INFO "s3_stm: etritctrl == 0x%x\n", ctrl);
}

void
get_etfitctrl(void)
{
	u32 ctrl;

	ctrl = readl_relaxed(csmmio.etf_base + CORESIGHT_ITCTRL);
	printk(KERN_INFO "s3_stm: etfitctrl == 0x%x\n", ctrl);
}

static inline void
get_rwp(void)
{
	CS_UNLOCK(csmmio.etr_base);

	/* get rwp */
	rwp = coresight_read_reg_pair(csmmio.etr_base, TMC_RWP, TMC_RWPHI);

	CS_LOCK(csmmio.etr_base);
}

static inline void get_rrp(void)
{
	if (asansts->etr_mode) {
		CS_UNLOCK(csmmio.etr_base);

		/* get rwp */
		rrp = coresight_read_reg_pair(csmmio.etr_base, TMC_RRP, TMC_RRPHI);

		CS_LOCK(csmmio.etr_base);
	} else {
		/* get shared rrp */
		rrp = *s_rrp;
	}
}

static inline u64
get_etr_distance(void)
{
	u64 dist;

	/* get rwp */
	get_rwp();

	/* get rrp */
	get_rrp();

	/* calculate free space */
	if (rwp > rrp)
		/* writer to the end of the buffer plus start to rrp */
		dist = (asansts->etr_end - rwp) + (rrp - asansts->etr_start);
	else if (rwp < rrp)
		/* writer has wrapped around so distance to rrp is their diff */
		dist = rrp - rwp;
	else
		/* assume rwp never reaches rrp */
		dist = etr_size;

	return dist;
}


static void stm_port_enable_hw(void)
{
	CS_UNLOCK(csmmio.stm_base);
	/* ATB trigger enable on direct writes to TRIG locations */
	writel_relaxed(0x10,
			csmmio.stm_base + STMSPTRIGCSR);

	/* don't use port selection */
	writel_relaxed(0x0, csmmio.stm_base + STMSPSCR);

	/*
	 * Enable all channel regardless of their number.  When port
	 * selection isn't used (see above) STMSPER applies to all
	 * 32 channel group available, hence setting all 32 bits to 1
	 */
	writel_relaxed(~0x0, csmmio.stm_base + STMSPER);

	CS_LOCK(csmmio.stm_base);
}

static void stm_port_disable_hw(void)
{
	CS_UNLOCK(csmmio.stm_base);

	writel_relaxed(0x0, csmmio.stm_base + STMSPER);
	writel_relaxed(0x0, csmmio.stm_base + STMSPTRIGCSR);

	CS_LOCK(csmmio.stm_base);
}

static void stm_enable_hw(void)
{	
	/* enable stimulus ports */
	stm_port_enable_hw();

	CS_UNLOCK(csmmio.stm_base);

	/* 2^17 bytes between synchronization packets */
	/* set to 0xFFF to reset to 4096 bytes */
	writel_relaxed(0x1880, csmmio.stm_base + STMSYNCR);

	writel_relaxed((0x1 << 16 | 			 /* trace id */
				0x02 |			 /* timestamp enable */
				0x01),			 /* global STM enable */
				csmmio.stm_base + STMTCSR);

	CS_LOCK(csmmio.stm_base);

	asansts->stm_running = 1;
}

static 
void stm_disable_hw(void)
{
	u32 val;

	CS_UNLOCK(csmmio.stm_base);

	val = readl_relaxed(csmmio.stm_base + STMTCSR);
	val &= ~0x1; /* clear global STM enable [0] */
	writel_relaxed(val, csmmio.stm_base + STMTCSR);

	CS_LOCK(csmmio.stm_base);

	/* disable stimulus ports */
	stm_port_disable_hw();

	asansts->stm_running = 0;
}

static inline void 
cti_irq_ack(void)
{
	void __iomem *base = csmmio.cti_cpu0;
	unsigned long val;

	CS_UNLOCK(base);
	val = readl_relaxed(base + CTIINTACK);
	val |= BIT(2);
	writel_relaxed(val, base + CTIINTACK);
	CS_LOCK(base);
}

void
cti_path_status(void)
{
	u32 sys0_trigin, sys0_chin, sys0_chout;
	u32 cpu0_trigout, cpu0_chin, cpu0_chout;

	/** cti_sys 0 **/
	CS_UNLOCK(csmmio.cti_sys0);
	/* trigger in status */
	sys0_trigin = readl_relaxed(csmmio.cti_sys0 + CTITRIGINSTATUS);
	/* channel in status */
	sys0_chin = readl_relaxed(csmmio.cti_sys0 + CTICHINSTATUS);
	/* channel out status */
	sys0_chout = readl_relaxed(csmmio.cti_sys0 + CTICHOUTSTATUS);
	CS_LOCK(csmmio.cti_sys0);

	printk("s3_stm: [csmmio.cti_sys0] trigin: 0x%x chin: 0x%x chout: 0x%x\n",
			sys0_trigin,
			sys0_chin,
			sys0_chout);

	/** cti_cpu 0 **/
	CS_UNLOCK(csmmio.cti_cpu0);
	/* trigger out status */
        cpu0_trigout = readl_relaxed(csmmio.cti_cpu0 + CTITRIGOUTSTATUS);
	/* channel in status */
        cpu0_chin = readl_relaxed(csmmio.cti_cpu0 + CTICHINSTATUS);
	/* channel out status */
        cpu0_chout = readl_relaxed(csmmio.cti_cpu0 + CTICHOUTSTATUS);
	CS_LOCK(csmmio.cti_cpu0);

	printk("s3_stm: [csmmio.cti_cpu0] trigout: 0x%x chin: 0x%x chout: 0x%x\n",
			cpu0_trigout,
			cpu0_chin,
			cpu0_chout);
}

void
cti_enable_cpu(void __iomem *cti_cpu)
{
	CS_UNLOCK(cti_cpu);
	/* disable CTI before writing registers */
	writel_relaxed(0, cti_cpu + CTICONTROL);

	/* attach outport 2 to channel 2 */
	writel_relaxed(0x4, cti_cpu + CTIOUTEN(2));

	/* enable all propagations */
	writel_relaxed(0xf, cti_cpu + CTIGATE);

	/* enable cti_cpu */
	writel_relaxed(0x1, cti_cpu + CTICONTROL);
	CS_LOCK(cti_cpu);
}

static void cti_enable_full(etr_mode_t etr_mode)
{	
	/** cti_sys 0 **/
	CS_UNLOCK(csmmio.cti_sys0);
	/* disable CTI before writing registers */
	writel_relaxed(0, csmmio.cti_sys0 + CTICONTROL);

	/* enable full for sf or asyncout for cb */
	if (etr_mode) {
		/* attach inport 2 to channel 2 */
		writel_relaxed(0x4, csmmio.cti_sys0 + CTIINEN(2));
		writel_relaxed(0x0, csmmio.cti_sys0 + CTIINEN(7));
	} else {
		/* attach inport 3 to channel 2 */
		writel_relaxed(0x4, csmmio.cti_sys0 + CTIINEN(7));
		writel_relaxed(0x0, csmmio.cti_sys0 + CTIINEN(2));
	}

	/* enable all propagations */
	writel_relaxed(0xf, csmmio.cti_sys0 + CTIGATE);

	/* enable csmmio.cti_sys0 */
	writel_relaxed(0x1, csmmio.cti_sys0 + CTICONTROL);
	CS_LOCK(csmmio.cti_sys0);

	/** enable cpu 0 **/
	cti_enable_cpu(csmmio.cti_cpu0);
}

void 
cti_disable_full(void)
{
	CS_UNLOCK(csmmio.cti_sys0);
	writel_relaxed(0x0, csmmio.cti_sys0 + CTICONTROL);
	CS_LOCK(csmmio.cti_sys0);

	CS_UNLOCK(csmmio.cti_cpu0);
	writel_relaxed(0x0, csmmio.cti_cpu0 + CTICONTROL);
	CS_LOCK(csmmio.cti_cpu0);
}

static int alloc_dmam(void)
{
	int rc;
	u32 dmamask, devid;
	u64 mask;

	/* get DEVID */
	devid = readl_relaxed(csmmio.etr_base + CORESIGHT_DEVID);

	/* Check if the AXI address width is available */
	if (devid & TMC_DEVID_AXIAW_VALID) {
		dmamask = ((devid >> TMC_DEVID_AXIAW_SHIFT) &
				TMC_DEVID_AXIAW_MASK);
	}

	/*
	 * Unless specified in the device configuration, ETR uses a 40-bit
	 * AXI master in place of the embedded SRAM of ETB/ETF.
	 */
	switch (dmamask) {
		case 32:
		case 40:
		case 44:
		case 48:
		case 52:
			break;
		default:
			dmamask = 40;
	}

	/* set up DMA masks */
	mask = DMA_BIT_MASK(dmamask);
	mychardev_data.dev->dma_mask = (u64 *)&mask;
	rc = dma_set_mask_and_coherent(mychardev_data.dev,
			DMA_BIT_MASK(dmamask));
	if (rc) {
		printk(KERN_ERR DEVNAME": failed to setup DMA mask (err %d)\n", rc);
		return -1;
	}

	/* allocate DMA */
	etr_buf.vaddr = dmam_alloc_coherent(mychardev_data.dev, etr_size,
			&etr_buf.daddr, GFP_KERNEL);
	if (!etr_buf.vaddr) {
		printk(KERN_ERR DEVNAME": failed to allocate ETR buffer "
				"(%lu bytes) from DMA mem\n", etr_size);
		return -ENOMEM;
	}
	printk(KERN_INFO DEVNAME": ETR buffer of size %ld KB allocated at "
			"vaddr: 0x%p, dma addr: 0x%llx\n", 
			(long int)etr_size/1024, etr_buf.vaddr, etr_buf.daddr);

	return 0;
}

void etf_enable_hw(void)
{
	CS_UNLOCK(csmmio.etf_base);

	/* Wait for TMCSReady bit to be set */
	tmc_wait_for_tmcready(csmmio.etf_base);

	/* HWFIFO */
	writel_relaxed(TMC_MODE_HARDWARE_FIFO, csmmio.etf_base + TMC_MODE);
	writel_relaxed(TMC_FFCR_EN_FMT | TMC_FFCR_EN_TI,
			csmmio.etf_base + TMC_FFCR);
	writel_relaxed(0x0, csmmio.etf_base + TMC_BUFWM);

	/* enable etf */
	writel_relaxed(TMC_CTL_CAPT_EN, csmmio.etf_base + TMC_CTL);

	CS_LOCK(csmmio.etf_base);
}

void etf_disable_hw(void)
{
	CS_UNLOCK(csmmio.etf_base);
	tmc_flush_and_stop(csmmio.etf_base);
	writel_relaxed(0x0, csmmio.etf_base + TMC_CTL);
	CS_LOCK(csmmio.etf_base);
}

static int 
replicator_enable_hw(int inport, int outport)
{
	int rc = 0;
	u32 id0val, id1val;

	CS_UNLOCK(csmmio.rep0_base);

	id0val = readl_relaxed(csmmio.rep0_base + REPLICATOR_IDFILTER0);
	id1val = readl_relaxed(csmmio.rep0_base + REPLICATOR_IDFILTER1);

	/*
	 * Some replicator designs lose context when AMBA clocks are removed,
	 * so have a check for this.
	 */
	if (id0val == 0x0 && id1val == 0x0)
		id0val = id1val = 0xff;

	switch (outport) {
		case 0:
			id0val = 0x0;
			break;
		case 1:
			id1val = 0x0;
			break;
		default:
			WARN_ON(1);
			rc = -EINVAL;
	}

	/* enable outport */
	writel_relaxed(id0val, csmmio.rep0_base + REPLICATOR_IDFILTER0);
	writel_relaxed(id1val, csmmio.rep0_base + REPLICATOR_IDFILTER1);

	CS_LOCK(csmmio.rep0_base);

	return rc;
}

static void 
replicator_disable_hw(int inport, int outport)
{
	u32 reg;

	switch (outport) {
		case 0:
			reg = REPLICATOR_IDFILTER0;
			break;
		case 1:
			reg = REPLICATOR_IDFILTER1;
			break;
		default:
			WARN_ON(1);
			return;
	}

	CS_UNLOCK(csmmio.rep0_base);

	/* disable the flow of ATB data through port */
	writel_relaxed(0xff, csmmio.rep0_base + reg);

	CS_LOCK(csmmio.rep0_base);
}

static int 
funnel_enable_hw(int port)
{
	u32 functl;
	int rc = 0;

	CS_UNLOCK(csmmio.fun0_base);

	functl = readl_relaxed(csmmio.fun0_base + FUNNEL_FUNCTL);

	/* enable port */
	functl &= ~FUNNEL_HOLDTIME_MASK;
	functl |= FUNNEL_HOLDTIME;
	functl |= (1 << port);
	writel_relaxed(functl, csmmio.fun0_base + FUNNEL_FUNCTL);

	/* set to highest priority */
	writel_relaxed(0x0, csmmio.fun0_base + FUNNEL_PRICTL);

	CS_LOCK(csmmio.fun0_base);
	return rc;
}

static void 
funnel_disable_hw(int inport)
{
	u32 functl;

	CS_UNLOCK(csmmio.fun0_base);

	functl = readl_relaxed(csmmio.fun0_base + FUNNEL_FUNCTL);
	functl &= ~(1 << inport);
	writel_relaxed(functl, csmmio.fun0_base + FUNNEL_FUNCTL);

	CS_LOCK(csmmio.fun0_base);
}

static inline void iounmap_ptr(void __iomem *p)
{
	if (p != 0)
		iounmap(p);
}

static void mmio_cleanup(void)
{
	iounmap_ptr(csmmio.cti_sys0);
	iounmap_ptr(csmmio.cti_cpu0);
	iounmap_ptr(csmmio.cti_cpu1);
	iounmap_ptr(csmmio.cti_cpu2);
	iounmap_ptr(csmmio.cti_cpu3);
	iounmap_ptr(csmmio.stm_base);
	iounmap_ptr(csmmio.etr_base);
	iounmap_ptr(csmmio.etf_base);
	iounmap_ptr(csmmio.tpiu_base);
	iounmap_ptr(csmmio.fun0_base);
	iounmap_ptr(csmmio.rep0_base);
	iounmap_ptr(csmmio.stim_base);
}

static int generic_mmio_init(struct cs_mmio_struct *io)
{
	int ret;
	const char *node_name;

	/* discover etr */
	/* csmmio.etr_base = ioremap(0x00826000, 0x1000); */
	/* find etr base address */
	node_name = "etr";
	ret = discover_node(node_name, &io->etr_base);
	if (ret != 0)
		goto err;

	node_name = "stm";
	ret = discover_node(node_name, &io->stm_base);
	if (ret != 0)
		goto err;

	/* map etf */
	/* find etf base address */
	node_name = "etf";
	ret = discover_node(node_name, &io->etf_base);
	if (ret != 0)
		goto err;

	/* map tpiu */
	/* find tpiu base address */
	node_name = "tpiu";
	ret = discover_node(node_name, &io->tpiu_base);
	if (ret != 0)
		goto err;

	/* map replicator */
	/* find replicator base address */
	node_name = "replicator";
	ret = discover_node(node_name, &io->rep0_base);
	if (ret != 0)
		goto err;

	node_name = "funnel";
	ret = discover_node(node_name, &io->fun0_base);
	if (ret != 0)
		goto err;

	node_name = "stimulus";
	ret = discover_stimulus(io);
	if (ret != 0)
		goto err;

	return 0;
err:
	printk(KERN_ERR DEVNAME": unable to discover %s's base address\n", node_name);
	mmio_cleanup();
	return ret;
}

static int db410c_mmio_init(void)
{
	memset(&csmmio, 0, sizeof(csmmio));

	/* map CTIs */
	csmmio.cti_sys0 = ioremap(DB401C_BASE_CTI_SYS0, MMIO_PAGE_SIZE); /* sys0 */
	if (!csmmio.cti_sys0)
		goto err;
	csmmio.cti_cpu0 = ioremap(DB401C_BASE_CTI_CPU0, MMIO_PAGE_SIZE); /* cpu0 */
	if (!csmmio.cti_cpu0)
		goto err;
	csmmio.cti_cpu1 = ioremap(DB401C_BASE_CTI_CPU1, MMIO_PAGE_SIZE); /* cpu1 */
	if (!csmmio.cti_cpu1)
		goto err;
	csmmio.cti_cpu2 = ioremap(DB401C_BASE_CTI_CPU2, MMIO_PAGE_SIZE); /* cpu2 */
	if (!csmmio.cti_cpu2)
		goto err;
	csmmio.cti_cpu3 = ioremap(DB401C_BASE_CTI_CPU3, MMIO_PAGE_SIZE); /* cpu3 */
	if (!csmmio.cti_cpu3)
		goto err;

	if (generic_mmio_init(&csmmio) != 0)
		return -1;
	return 0;
err:
	printk(KERN_ERR DEVNAME": unable to map a db410c CTI's based address\n");
	mmio_cleanup();
	return -1;
}

void 
handler_bh(struct work_struct *work)
{
	/* sleep until everything has been read */
	/* change this to a percentage later on */
	get_rrp();
	get_rwp();
	while (rrp != rwp) {
		usleep_range(25000, 25001);
		get_rrp();
		get_rwp();
	}

	/* resume asan process */
	kill_pid(find_vpid(asansts->asanid), SIGCONT, 1);
	kill_pid(find_vpid(asansts->asanid - 2), SIGCONT, 1);

	asansts->asan_paused = 0;
}

static irqreturn_t
irq_handler(int irq, void *dev_id)
{
	u64 etr_space;

	/* clear cti irq port */
	cti_irq_ack();

	/* protect from multiple requests while blocked */
	if (asansts->asan_paused == 1) {
		// printk("I shouldn't be here");
		return IRQ_HANDLED;
	}

	/* get distance */
	etr_space = get_etr_distance();

	/* make sure space is enough */
	if (etr_space <= (etr_size / 0x3)) {
		/* stop asan process */
		kill_pid(find_vpid(asansts->asanid), SIGSTOP, 1);

		asansts->asan_paused = 1;

		/* schedule bottom half */
		schedule_work(&workqueue);
	}

	return IRQ_HANDLED;
}

static void
init_irq_handler(void *args)
{
	int ret;
	int linux_irq;
	struct device_node* dev_node;
	struct irq_domain *dom;
	struct irq_fwspec dummy_fwspec = {
		.param_count = 3,
		.param = {IRQ_MODE, HWIRQ, IRQ_TYPE}
	};

	/* install handler only on cpu 0 */
	if (smp_processor_id() != 0)
		return;

	/* find device node for IRQ source */
	dev_node = of_find_node_by_name(NULL, "interrupt-controller");
	if (!dev_node) {
		printk("s3_stm: Device node was not discovered\n");
		return;
	}

	/* get or create irq domain */
	dom = irq_find_host(dev_node);
	if (dom == NULL) {
		printk("s3_stm: Creating domain for %s\n", dev_node->name);
		dom = irq_domain_add_linear(dev_node, -0, &irq_domain_simple_ops, NULL);
	}

	/* find linux irq from hwirq */
	/* or create new mapping */
	linux_irq = irq_find_mapping(dom, HWIRQ);
	if (linux_irq == 0) {
		dummy_fwspec.fwnode = dom->fwnode;
		linux_irq = irq_create_fwspec_mapping(&dummy_fwspec);
	}

#if(IRQ_MODE == GIC_SPI)
        /* start interrupt handler */
        ret = request_irq(linux_irq, 
                        irq_handler,
                        IRQF_SHARED,
                        "cti_irq",
                        &irq_handler);
#else
	/* set affinity to all cpus */
	irq_set_affinity_hint(linux_irq, cpu_online_mask);

	/* set irq type */
	irq_set_irq_type(linux_irq, IRQ_TYPE_LEVEL_HIGH);

	/* set irq flags */
	irq_modify_status(linux_irq, IRQ_NOAUTOEN, 
				IRQ_PER_CPU | IRQ_NOTHREAD |
				IRQ_NOPROBE | IRQ_PER_CPU_DEVID);

	/* start interrupt handler */
	ret = request_percpu_irq(linux_irq, 
			irq_handler,
			"cti_irq", 
			&irq_handler);
#endif
	if (ret) {
		printk("s3_stm: ISR installation failed with error %d\n", ret);
	} else {
		asansts->irq = linux_irq;
	}
}


void
clear_asan_pointers(void)
{
	/* init asan status struct */
	asansts->etr_start = etr_buf.daddr;
	asansts->etr_end = etr_buf.daddr + etr_size;
	asansts->stm_running = 0;
	asansts->asan_paused = 0;
}

void
clear_asan_ids(void)
{
	asansts->asanid = -1;
	asansts->monitorid = -1;
}

void
clear_asan_status(void)
{
	clear_asan_pointers();
	clear_asan_ids();

	asansts->irq = -1;
	asansts->etr_mode = 0;
}

void
init_asan_status(void)
{
	asansts = kzalloc(sizeof(struct asan_state), GFP_KERNEL);

	/* init all asan status values to default */
	clear_asan_status();
	
	/* reset etr pointers */
	etr_reset_pointers();	
}

static int cs_char_open(struct inode *inode, struct file *file)
{
	printk("s3_stm: Driver opened by process with pid %d\n", current->pid);

	return 0;
}

static int cs_char_release(struct inode *inode, struct file *file)
{
	/* if device released by asan process */
	if (current->pid == asansts->asanid) {
		/* flush and stop etr */
		etr_stop();

		/* notify monitor if one exists */
		if (asansts->monitorid != -1) {
			printk("s3_stm: asan paused before exit\n");
			/* wait for monitor to consumer all available data */
			get_rrp();
			get_rwp();
			while (rrp != rwp) {
				usleep_range(25000, 25001);
				get_rrp();
				get_rwp();
			}
		
			/* send SIGUSR1 that will stop the monitor */
			kill_pid(find_vpid(asansts->monitorid), SIGUSR1, 1);
		}

		/* replicator disable */
		replicator_disable_hw(0, 0);

		/* disable funnel0 */
		funnel_disable_hw(7);

		/* disable used ctis */
		cti_disable_full();

		/* disable stm */
		stm_disable_hw();

		/* disable etf */
		etf_disable_hw();

		/* clear asanid */
		if (!asansts->asan_paused) {
			asansts->asanid = -1;
		}

		printk("s3_stm: Driver released by asanID %d\n", current->pid);
	} else {
		/* clear monitorid */
		asansts->monitorid = -1;

		/* reset all asan status pointers */
		clear_asan_pointers();

		/* reset rrp, rwp */
		etr_reset_pointers();

		printk("s3_stm: Driver released by monitorID %d\n", current->pid);
	}

	return 0;
}

static int cs_char_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long size;
	phys_addr_t vma_target;

	if (vma->vm_pgoff)
			return -EINVAL;

	/* calc vma size */
	size = vma->vm_end - vma->vm_start;

	if (current->pid == asansts->monitorid) {
		/* need a better way to differentiate those two */
		if (size == PAGE_SIZE) {
			/* check flags for RW or RO */
			if (vma->vm_flags == 0xfb) {/* RW */
				/* set to shared software rrp */
				vma_target = (u64) virt_to_phys(s_rrp);
			} else if (vma->vm_flags == 0xf9) { /* RO */
				if (!csmmio.etr_base)
					return -EINVAL;

				/* set to hardware rwp */
				vma_target = (u64) 0x826000;
			}
		} else {
			if (!etr_buf.daddr)
			return -EINVAL;

			/* set target to etr buffer */
			vma_target = etr_buf.daddr;
		}
	} else { /* asanid */
		if (!stim_phys)
			return -EINVAL;

		/* set target to stimulus area */
		vma_target = stim_phys;
	}
	
	/* set up vma */
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_flags |= VM_IO | VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_ops = &cs_mmap_vmops;
	vm_iomap_memory(vma, vma_target, size);

	return 0;
}

static ssize_t cs_char_read(struct file *file, char __user *data, size_t len,
		loff_t *ppos)
{
	char *buffer, *bufp;
	size_t read_len;
	u32 read_data1;
	u32 read_data2;
	u32 tmc_ctl;

	/* only read when asan session is live */
	if (asansts->asanid == -1)
		return 0;

	/* make sure rwp doesn't reach rrp */
	/* need to find a better way to do this */
	get_rrp();
	get_rwp();
	tmc_ctl = readl_relaxed(csmmio.etr_base + TMC_CTL);
	if ((rrp >= rwp) && (tmc_ctl == 0)) {
		return 0;
	}

	/* allocate local buffer */
	buffer = bufp = kzalloc(len, GFP_KERNEL);

	/* get data using RRD until 0xFFFFFFFF */
	read_len = 0;
	while (read_len < len) {
		CS_UNLOCK(csmmio.etr_base);

		/* since memory is 64 bits wide */
		/* two reads are required */

		/* perform first read and copy */
		read_data1 = readl_relaxed(csmmio.etr_base + TMC_RRD);
		if (read_data1 == 0xFFFFFFFF) {
			break;
		}
		/* copy RRD data to local buffer */
		memcpy(bufp, &read_data1, 4);
		/* move local buffer pointer */
		bufp += 4;

		/* perform second read */
		read_data2 = readl_relaxed(csmmio.etr_base + TMC_RRD);
		if (read_data2 == 0xFFFFFFFF) {
			break;
		}
		/* copy RRD data to local buffer */
		memcpy(bufp, &read_data2, 4);
		/* move local buffer pointer */
		bufp += 4;
		read_len += 8;

		CS_LOCK(csmmio.etr_base);

		get_rrp();
		get_rwp();
		if (rrp == rwp) {
			break;
		}
	}

	/* copy available data to user */
	if (copy_to_user(data, buffer, read_len)) {
		printk("s3_stm: %s copy_to_user failed\n", __func__);
		return -EFAULT;
	}

	// printk("%ld copied rrp: 0x%llx rwp: 0x%llx\n", read_len, rrp, rwp);

	/* move user pointer */
	*ppos += read_len;

	/* free buffer */
	kfree(buffer);

	return read_len;
}

static long
cs_char_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	u32 status;

	switch (cmd) {
		case GET_RWP:
			get_rwp();
			
			/* copy rwp to user */
			if (copy_to_user((u64*) arg, &rwp, sizeof(rwp)))
			{
				pr_err("s3_stm: ioctl GET_RWP error!\n");
				return -EFAULT;
			}

			break;
		case GET_RRP:
			get_rrp();
			
			/* copy rwp to user */
			if (copy_to_user((u64*) arg, &rrp, sizeof(rrp)))
			{
				pr_err("s3_stm: ioctl GET_RRP error!\n");
				return -EFAULT;
			}

			break;
		case GET_FULL:
			/* get rwp */
			status = readl_relaxed(csmmio.etr_base + TMC_STS);
			full = status & TMC_STS_FULL;

			/* copy rwp to user */
			if (copy_to_user((u32*) arg, &full, sizeof(full)))
			{
				pr_err("s3_stm: ioctl GET_FULL error!\n");
				return -EFAULT;
			}

			break;
		case GET_ETR_SZ:
			/* copy rwp to user */
                        if (copy_to_user((u64*) arg, &etr_size, sizeof(etr_size)))
                        {
                                pr_err("s3_stm: ioctl GET_FULL error!\n");
				return -EFAULT;
                        }
			break;
		case INCR_RRP:
			/* maintain software rrp */
			rrp += (unsigned int)arg;

			/* wrap around */
			if (rrp >= asansts->etr_end) {
				rrp = rrp - etr_size;
			}

			break;
		case CS_ENABLE:
			/* enable etr */
			etr_enable_hw(arg);

			/* enable replicator */
			replicator_enable_hw(0, 0);

			/* enable etf */
			etf_enable_hw();

			/* enable funnel0 */
			funnel_enable_hw(7);

			/* enable cti connections for irq */
			/* etr_full | asyncout -> csmmio.cti_sys0 -> csmmio.cti_cpu0 -> ctiirrq */
			if (pause)
				cti_enable_full(arg);

			/* enable stm */
			stm_enable_hw();

			break;
		case CS_DISABLE:
			/* disable etr after flushing */
			etr_stop();

			/* replicator disable */
			replicator_disable_hw(0, 0);

			/* disable etf */
			etf_disable_hw();

			/* disable funnel0 */
			funnel_disable_hw(7);

			/* disable used ctis */
			cti_disable_full();

			/* disable stm */
			stm_disable_hw();

			break;
		case SET_ASANID:
			asansts->asanid = (unsigned int)arg;

			printk("s3_stm: asanID set to %d\n", asansts->asanid);
				
			break;
		case SET_MONITORID:
			asansts->monitorid = (unsigned int)arg;

			printk("s3_stm: monitorID set to %d\n", asansts->monitorid);
				
			break;

		default:
			break;
	}

	return 0;
}

static int chrdev_init(void)
{
	int err;

	/* allocate chardev region and assign Major number */
	err = alloc_chrdev_region(&mychardev_data.devnum, 0, 1, DEVNAME);
	if (err != 0) {
		printk(KERN_ERR DEVNAME": alloc_chrdev_region failed\n");
		return err;
	}
	
	/* init new device */
	cdev_init(&mychardev_data.cdev, &mychardev_fops);
	mychardev_data.cdev.owner = THIS_MODULE;

	/* add device to the system where "i" is a Minor number of the new device */ 
	//cdev_add(&mychardev_data.cdev, MKDEV(mychardev_data.major, BASEMINOR), MAXMINORS);
	cdev_add(&mychardev_data.cdev, mychardev_data.devnum, 1);

	/* printk(KERN_INFO DEVNAME": device major %d\n",
			MAJOR(mychardev_data.devnum)); */

	return 0;
}

static void chrdev_cleanup(void)
{
	cdev_del(&mychardev_data.cdev);
	//unregister_chrdev_region(MKDEV(mychardev_data.major, BASEMINOR), MAXMINORS);
	unregister_chrdev_region(mychardev_data.devnum, 1);
}


/* Set the permissions of the created special device file under /dev */
static char *cs_char_devnode(struct device *dev, umode_t *mode)
{
       if (!mode)
                return NULL;
	*mode = DEVMODE;
        return NULL;
}

static int chrdev_sysfs_init(void)
{
	/* sysfs class */
	mychardev_data.sysfs_class = class_create(THIS_MODULE, SYSFS_CLASSNAME);
	if (IS_ERR(mychardev_data.sysfs_class)) {
		printk(KERN_ERR DEVNAME": class_create failed\n");
		return PTR_ERR(mychardev_data.sysfs_class);
	}
	mychardev_data.sysfs_class->devnode = cs_char_devnode;

	/* create device node /dev/mychardev-x where "x" is "i", equal to the Minor number */
	//devst = device_create(mychardev_class, NULL, MKDEV(dev_major, 0), NULL, "s3_stm");
	mychardev_data.dev = device_create(mychardev_data.sysfs_class, NULL,
			mychardev_data.devnum, NULL, DEVNAME);
	if (IS_ERR(mychardev_data.dev)) {
		printk(KERN_ERR DEVNAME": device_create failed\n");
		return PTR_ERR(mychardev_data.dev);
	}
	return 0;
}

static void chrdev_sysfs_cleanup(void)
{
	device_destroy(mychardev_data.sysfs_class, mychardev_data.devnum);
	class_destroy(mychardev_data.sysfs_class);
}

static int s3_stm_init(void)
{
	int ret;

	if (etr_pages > 0)
		etr_size = PAGE_SIZE * etr_pages;

	if ((ret = chrdev_init()) != 0)
		return -1;

	if ((ret = chrdev_sysfs_init()) != 0)
		goto sysfs_err;

	/* MMIO init*/
	if ((ret = db410c_mmio_init()) != 0)
		goto mmio_err;

	/* allocate dma memory for etr */
	if ((ret = alloc_dmam()) != 0)
		goto alloc_err;

	/* allocate shared rrp */
	/* Use alloc_page ? */
	s_rrp = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (s_rrp == NULL) {
		ret = -ENOMEM;
		goto rrp_err;
	}

	/* init asan status */
	init_asan_status();

	/* init irq handler */
	if (pause)
		on_each_cpu(init_irq_handler, NULL, 1);

	printk(KERN_INFO DEVNAME": Driver installed\n");
	return 0;

rrp_err:
	dmam_free_coherent(mychardev_data.dev, etr_size,
			etr_buf.vaddr, etr_buf.daddr);
alloc_err:
	mmio_cleanup();
mmio_err:
	chrdev_sysfs_cleanup();
sysfs_err:
	chrdev_cleanup();
	return ret;
}

static void s3_stm_exit(void)
{
	/* stop irq handler */
	if (asansts) {
		if (asansts->irq != -1) {
#if(IRQ_MODE == GIC_SPI)
			free_irq(asansts->irq, &irq_handler);
#else
			free_percpu_irq(asansts->irq, &irq_handler);
#endif
		}
		
		kfree(asansts);
	}

	kfree(s_rrp);
	dmam_free_coherent(mychardev_data.dev, etr_size,
			etr_buf.vaddr, etr_buf.daddr);
	mmio_cleanup();
	chrdev_sysfs_cleanup();
	chrdev_cleanup();
	printk(KERN_INFO DEVNAME": Driver unistalled\n");
}

module_init(s3_stm_init);
module_exit(s3_stm_exit);

MODULE_INFO(intree, "Y");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("STM Integration Test driver");
MODULE_AUTHOR("Koss Kleftogiorgos <kklefto1@stevens.edu>");
MODULE_AUTHOR("Georgios Portokalidis <georgios@portokalidis.net>");
