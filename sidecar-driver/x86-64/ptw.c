/* Slowdown methods */
#define SLOWDOWN_NONE 0
#define SLOWDOWN_BLOCK 1
#define SLOWDOWN_PRIO 1
/* Used slowdown method */
#define SLOWDOWN_METHOD SLOWDOWN_PRIO

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/cpu.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/kallsyms.h>
#include <linux/kprobes.h>
#include <linux/dcache.h>
#include <linux/ctype.h>
#include <linux/syscore_ops.h>
#include <trace/events/sched.h>
#include <asm/msr.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include <linux/mutex.h>
#include <asm/msr-index.h>
#include <asm/apic.h>
#include <linux/slab.h>
#include <linux/workqueue.h>


/* Kernel page table isolation feature for various kernels */
#ifdef X86_FEATURE_KAISER
# define PTI_FEATURE X86_FEATURE_KAISER
#else
# define PTI_FEATURE X86_FEATURE_PTI
#endif

/* Minumum number of buffers that need to be available, otherwise the process
 * should be stopped */
#define MIN_AVAIL_BUFFERS 4
/* Any less avail buffers constitutes and overflow */
#define OFLOW_BUFFERS_THRES 2

#include <linux/syscalls.h> 
#include <asm/nmi.h>
#include <asm/siginfo.h>
#include <linux/pid_namespace.h>
#include <linux/pid.h>

#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/ioctl.h>


/* mapping for base addresses */
struct dso_entry {
	void *handle;
	unsigned long base_address;
	char filename[256];
	struct list_head list;
};

static LIST_HEAD(dso_list);
static DEFINE_MUTEX(dso_list_mutex);

/* asan */
static DECLARE_WAIT_QUEUE_HEAD(wq);

/* pmi handler */
void handler_bh(struct work_struct *work);
DECLARE_WORK(workqueue, handler_bh);

/* Module parameters */

static bool debug = false;
module_param(debug, bool, 0644);
MODULE_PARM_DESC(debug, "Enable run-time debug messages.");

static int pause = 1;
module_param(pause, int, 0);

static int timestamps = 0;
module_param(timestamps, int, 0);
MODULE_PARM_DESC(debug, "Enable trace timestamp packets.");

static bool fake_ptw = false;

#define debugk(fmt, ...) while (debug == true) {\
	pr_info(fmt, ##__VA_ARGS__);\
	break;}
#ifdef DEBUG
# define DEBUGK(fmt, ...)  do { pr_info(fmt, ##__VA_ARGS__);\
	while (0)
#endif


#include <asm/processor.h>
#include "ptw.h"

#define MSR_IA32_RTIT_OUTPUT_BASE	0x00000560
#define MSR_IA32_RTIT_OUTPUT_MASK_PTRS	0x00000561
#define MSR_IA32_RTIT_CTL		0x00000570
//#define MSR_IA32_PERF_GLOBAL_STATUS	0x0000038E
//#define MSR_IA32_PERF_GLOBAL_STATUS_RESET 0x00000390
// aka MSR_CORE_PERF_GLOBAL_OVF_CTRL
#define MSR_IA32_PERF_GLOBAL_STATUS_SET	0x00000391
#define MSR_IA32_ADDR0_START		0x00000580
#define MSR_IA32_ADDR0_END		0x00000581
#define MSR_IA32_ADDR1_START		0x00000582
#define MSR_IA32_ADDR1_END		0x00000583
#define TRACE_EN	BIT_ULL(0)
#define CYC_EN		BIT_ULL(1)
#define CTL_OS		BIT_ULL(2)
#define CTL_USER	BIT_ULL(3)
#define PT_ERROR	BIT_ULL(4)
#define CR3_FILTER	BIT_ULL(7)
#define TO_PA		BIT_ULL(8)
#define MTC_EN		BIT_ULL(9)
#define TSC_EN		BIT_ULL(10)
#define DIS_RETC	BIT_ULL(11)
#define PTW_EN		BIT_ULL(12)
#define BRANCH_EN	BIT_ULL(13)
#define TRACE_TOPA_PMI_BIT	55
#define TRACE_TOPA_PMI	BIT_ULL(TRACE_TOPA_PMI_BIT)
#define MTC_MASK	(0xf << 14)
#define CYC_MASK	(0xf << 19)
#define PSB_MASK	(0xf << 24)
#define ADDR0_SHIFT	32
#define ADDR1_SHIFT	36
#define ADDR0_MASK	(0xfULL << ADDR0_SHIFT)
#define ADDR1_MASK	(0xfULL << ADDR1_SHIFT)
#define MSR_IA32_RTIT_STATUS		0x00000571
#define MSR_IA32_CR3_MATCH		0x00000572
#define TOPA_STOP	BIT_ULL(4)
#define TOPA_INT	BIT_ULL(2)
#define TOPA_END	BIT_ULL(0)
#define TOPA_SIZE_MAX	0xf
#define TOPA_SIZE_SHIFT 6

typedef enum { DEV_CLOSED, DEV_OPENED, DEV_CONFIGURED, DEV_TRACING } dev_status_t;

/* Device data (for traced processors) */
struct ptw_dev {
	struct ptw_conf conf;		/* Configuration */
	struct mutex lock;		/* Mutual exclusion lock */
	dev_status_t status;		/* Status of current device */
	unsigned int cpu;		/* #CPU tracing for device */
	u64 *topa;			/* Pointer to the topa being used */
	int process_stopped;		/* Process stopped? */
	pid_t pid;			/* Process ID */
	pid_t mid;			/* Monitor ID */
	unsigned int *rpage;		/* Reader page */
	unsigned int *wpage;		/* Writer page */
	unsigned int woff;		/* Writer offset within page */
};

static DEFINE_PER_CPU(struct ptw_dev *, cpu_dev);

int ptw_asan_cpu = -1;


/* Intel PT supported configuration */
static bool has_cr3_match;	/* Support for CR3-based filtering */
static unsigned psb_freq_mask;	/* Mask for PSF frequency configuration */
static unsigned cyc_thresh_mask;/* Mask for cycle threshold configuration */
static unsigned mtc_freq_mask;	/* Mask for MTC frequency configuration */
static unsigned addr_cfg_max;	/* Max mode for address range configuration */
static unsigned addr_range_num;	/* No address range configurations */

static int ptw_hotplug_state = -1;

static inline int pt_wrmsrl_safe(unsigned msr, u64 val)
{
	int ret = wrmsrl_safe(msr, val);
	return ret;
}


static inline int pt_rdmsrl_safe(unsigned msr, u64 *val)
{
	int ret = rdmsrl_safe(msr, val);
	return ret;
}


static inline void topa_writer_get(struct ptw_dev *dev)
{
	u64 offset;

	rdmsrl(MSR_IA32_RTIT_OUTPUT_MASK_PTRS, offset);

	*dev->wpage = (offset & 0xffffffff) >> 7;
	dev->woff = offset >> 32;
}


static inline u64 get_writer_offset(struct ptw_dev *dev)
{
	return (*dev->wpage << (dev->conf.order + PTW_TOPA_PAGE_SHIFT)) + dev->woff;
}


static inline unsigned short get_reader_page(struct ptw_dev *dev,
		unsigned long offset)
{
	unsigned long page_mask, page;

	page_mask = ~((1UL << (dev->conf.order + PTW_TOPA_PAGE_SHIFT)) - 1);
	page = (offset & page_mask) >> (dev->conf.order + PTW_TOPA_PAGE_SHIFT);
	return page;
}

static int stop_pt(void *arg)
{
	u64 ctl, status;
	struct ptw_dev *dev =  per_cpu(cpu_dev, get_cpu());

	debugk("stopping tracing on CPU %d\n", dev->cpu);

	if (!__this_cpu_read(cpu_dev)) {
		pr_warn("CPU %d (dev cpu %d) was expected to be tracing\n", 
				raw_smp_processor_id(), dev->cpu);
	}

	/* Get status */
	pt_rdmsrl_safe(MSR_IA32_RTIT_CTL, &ctl);
	pt_rdmsrl_safe(MSR_IA32_RTIT_STATUS, &status);
	if (!(ctl & TRACE_EN))
		debugk("cpu %d, trace was not enabled on stop, ctl %llx,"
				" status %llx\n", raw_smp_processor_id(), 
				ctl, status);
	if (status & PT_ERROR) {
		debugk("cpu %d, error happened: status %llx\n",
				raw_smp_processor_id(), status);
		pt_wrmsrl_safe(MSR_IA32_RTIT_STATUS, 0);
	}

	/* Stop */
	pt_wrmsrl_safe(MSR_IA32_RTIT_CTL, 0LL);

	debugk("tracing stopped on CPU %d\n", dev->cpu);

	topa_writer_get(dev);
	debugk("after: reader %hu, writer %hu offset: %hu\n",
			*dev->rpage, *dev->wpage, dev->woff);

	put_cpu();
	return 0;
}

/*
 * TOPA (Table of physical addresses)
 * topa ->	+-----------------+
 * 		+ Output area ptr +
 * 		+-----------------+
 * 		+ Output area ptr +
 *		+-----------------+
 * 		+      ....       +
 *		+-----------------+
 * 		+ topa ptr        +
 *		+-----------------+
 *
 * Output area ptr
 * |       63 - MAXPA-1    |12-9| 9-6  | 5 |   4  | 3 |  2  | 1 |  0  |
 * +-----------------------+----+------+---+------+---+-----+---+-----+
 * | Output region Base PA |  X | Size | X | STOP | X | INT | X | END |
 * +-----------------------+----+------+---+------+---+-----+---+-----+
 */
static int topa_alloc(struct ptw_dev *dev)
{
	u64 *topa, order;
	int n;
	unsigned int topasz, nmi;

	topasz = dev->conf.topasz;
	order = dev->conf.order;
	if (topasz > ((PAGE_SIZE >> 3) - 1) || order > TOPA_SIZE_MAX) {
		pr_err("Bad TOPA configuration (order=%llu, topasz=%u)\n",
				order, topasz);
		return -EINVAL;
	}

	/* Page to hold TOPA */
	topa = (u64 *)__get_free_page(GFP_KERNEL|__GFP_ZERO);
	if (!topa) {
		pr_err("Cannot allocate topa page\n");
		goto topa_error;
	}

	/* create circular topa table */
	for (n = 0; n < topasz; n++) {
		void *buf = (void *)__get_free_pages(
				GFP_KERNEL|__GFP_NOWARN|__GFP_ZERO, order);
		if (!buf) {
			pr_err("Cannot allocate %d'th PT buffer\n", n);
			goto area_error;
		}
		topa[n] = __pa(buf) | (order << TOPA_SIZE_SHIFT);
	}

	/* circular buffer, point last entry to the beginning */
	topa[topasz] = (u64)__pa(topa) | TOPA_END;

	/* Set NMI bit based on frequency requested */
	nmi = dev->conf.nmi_freq;
	if (nmi > 0) {
		for (n = nmi - 1; n < topasz; n += nmi) {
			topa[n] |= TOPA_INT;
		}
	}

	dev->topa = topa;
	return 0;

area_error:
	/* Free pages that were already allocated starting from the one before
	 * the failed entry (n) to including 0. */
	while (--n >= 0) {
		free_pages((unsigned long)__va(topa[n] & PAGE_MASK), order);
	}

topa_error:
	free_page((unsigned long)topa);
	return -ENOMEM;
}

static void start_pt(struct ptw_dev *dev)
{
	u64 ctl, status;
	u8 mtcfreq;

	debugk("starting tracing on CPU %d (%d)\n",
			dev->cpu, raw_smp_processor_id());

	/* Read control MSR and check PT is not already enabled */
	if (pt_rdmsrl_safe(MSR_IA32_RTIT_CTL, &ctl) < 0) {
		pr_err("Cannot access RTIT_CTL\n");
		return;
	}
	if (ctl & TRACE_EN) {
		pr_err("PT already active: %llx\n", ctl);
		return;
	}

	/* Set topa */
	if (pt_wrmsrl_safe(MSR_IA32_RTIT_OUTPUT_BASE, __pa(dev->topa))) {
		pr_err("output base MSR write error");
		return;
	}

	/* Clear registers */
	pt_wrmsrl_safe(MSR_IA32_RTIT_OUTPUT_MASK_PTRS, 0ULL);
	pt_wrmsrl_safe(MSR_IA32_RTIT_STATUS, 0ULL);

	/* reset pointer */
	topa_writer_get(dev);
	*dev->rpage = *dev->wpage;
	debugk("before: reader %hu, writer %hu  offset: %hu\n",
			*dev->rpage, *dev->wpage, dev->woff);

	/* Prepare control MSR */
	ctl &= ~(TSC_EN | CTL_OS | CTL_USER | CR3_FILTER | DIS_RETC | TO_PA |
		 CYC_EN | TRACE_EN | BRANCH_EN | CYC_EN | MTC_EN |
		 MTC_EN | MTC_MASK | CYC_MASK | PSB_MASK | ADDR0_MASK | ADDR1_MASK);
	
	/* if timestamps enabled */
	if (timestamps) {
		ctl |= TSC_EN; /* Enable timestamps */
		ctl |= MTC_EN; /* Enable mtc */

		/* mtcfreq from ctl */
		mtcfreq = ctl & MTC_MASK;

		/* print mtcfreq */
		debugk("MTC enabled MTCfreq 0x%x ATMfreq 0x%x\n",
				mtcfreq,
				mtc_freq_mask);
	}

	ctl |= TRACE_EN; /* Enable tracing */
	ctl |= TO_PA; /* TOPA is always on */
	ctl |= PTW_EN; /* Enable PT_WRITE */
	ctl |= CTL_USER; /* Enable user context */

	/* Write control MSR */
	if (pt_wrmsrl_safe(MSR_IA32_RTIT_CTL, ctl) < 0) {
		pr_err("cannot write PT CTL MSR\n");
		return;
	}

	/* debug start */
	pt_rdmsrl_safe(MSR_IA32_RTIT_STATUS, &status);
	if (pt_rdmsrl_safe(MSR_IA32_RTIT_CTL, &ctl) < 0) {
		pr_err("Cannot access RTIT_CTL\n");
		return;
	}
	debugk("CPU %d RTIT_CTL: 0x%llx RTIT_STATUS: 0x%llx\n", dev->cpu, ctl, status);

	/* debug end */

	dev->status = DEV_TRACING;
	__this_cpu_write(cpu_dev, dev);
	debugk("tracing started on CPU %d\n", dev->cpu);
}

static void topa_free(u64 *topa, unsigned int topasz, unsigned long order)
{
	int j;

	for (j = 0; j < topasz; j++) {
		/*
		if (topa[j] & TOPA_END)
			break;
		*/
		free_pages((unsigned long)__va(topa[j] & PAGE_MASK), order);
	}
	free_page((unsigned long)topa);
}

static void ptw_dev_init(struct ptw_dev *dev, unsigned int cpu)
{
	/* init struct */
	dev->cpu = cpu;
	dev->status = DEV_CLOSED;
	dev->pid = -1;
	dev->mid = -1;
	mutex_init(&dev->lock);
}

static int ptw_dev_configure(struct ptw_dev *dev)
{
	int err = 0;

	dev->rpage = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!dev->rpage)
		return -ENOMEM;
	dev->wpage = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!dev->wpage) {
		err = -ENOMEM;
		goto free_rpage;
	}

	dev->status = DEV_CONFIGURED;
	dev->conf.topasz = 80;
	if (pause)
		dev->conf.nmi_freq = 2;
	else
		dev->conf.nmi_freq = 0;
	dev->conf.order = 9;
	dev->process_stopped = 0;

	err = topa_alloc(dev);
	if (err)
		goto free_wpage;

	return 0;

free_wpage:
	kfree(dev->wpage);
free_rpage:
	kfree(dev->rpage);
	return err;
}


static int ptw_cpu_startup(unsigned int cpu)
{
	struct ptw_dev *dev;
	int err;

	dev = per_cpu(cpu_dev, cpu);
	if (dev) { /* This should always be zero */
		printk("dev is not null at startup!\n");
		return -EIO;
	}

	/* allocate dev */
	dev = kmalloc(sizeof(struct ptw_dev), GFP_KERNEL);
	if (!dev) {
		printk("Cannot allocate dev memory\n");
		return -ENOMEM;
	}

	ptw_dev_init(dev, cpu);

	if (!fake_ptw) {
		err = ptw_dev_configure(dev);
		if (err != 0) {
			kfree(dev);
			return err;
		}
	}

	per_cpu(cpu_dev, cpu) = dev;

	return 0;
}

static int ptw_cpu_teardown(unsigned int cpu)
{
	struct ptw_dev *dev;

	dev = per_cpu(cpu_dev, cpu);
	if (!dev)
		return 0;

	per_cpu(cpu_dev, cpu) = NULL;

	switch (dev->status) {
	case DEV_CLOSED:
	case DEV_OPENED:
		break;
	case DEV_CONFIGURED:
	default:
		topa_free(dev->topa, dev->conf.topasz, dev->conf.order);
		kfree(dev->rpage);
		kfree(dev->wpage);
		break;
	}

	kfree(dev);
	return 0;
}

static int rdwr_pagedist(struct ptw_dev *dev)
{
	int avail_pages;

	/* It is *important* that this is just ">" and not ">=", as it handles
	 * correctly the case where wpage and rpage are equal, when the reader
	 * is all caught up with the writer. The opposite case of an overflow is
	 * prevented by allowing at least for MIN_AVAIL_BUFFERS to be free
	 * between writer and reader */
	avail_pages = (*dev->rpage > *dev->wpage)?
		*dev->rpage - *dev->wpage :
		dev->conf.topasz - *dev->wpage - 1 + *dev->rpage;
	return (avail_pages - 1);
}

static long ptw_ioctl(struct file *filp, unsigned int cmd,
			    unsigned long arg)
{
	struct ptw_dev *dev;

	switch (cmd) {
	case PTW_ENABLE:
		/* set global for traced cpu */
		ptw_asan_cpu = get_cpu();

		/* set pid */
		dev = per_cpu(cpu_dev, ptw_asan_cpu);
		dev->pid = current->pid;

		wake_up_interruptible(&wq);

		/* start pt */
		start_pt(dev);
		put_cpu();

		debugk("producer %d pinned to cpu %d\n", 				
				dev->pid,
				ptw_asan_cpu);

    wait_event_interruptible(wq, dev->mid != -1);

		return 0;
	case PTW_DISABLE:
		/* set pid */
		dev = per_cpu(cpu_dev, ptw_asan_cpu);

		/* stop pt */
		ptw_asan_cpu = -1;
		dev->pid = -1;
		stop_pt(NULL);

		return 0;
	case PTW_SET_MID:
		if (ptw_asan_cpu == -1) {
			debugk("waiting for producer\n");

			/* you can block here until there is a producer */
			wait_event_interruptible(wq, ptw_asan_cpu > -1);
			if (ptw_asan_cpu == -1) return 0;
		} 

		/* set monitor id */
		dev = per_cpu(cpu_dev, ptw_asan_cpu);
		dev->mid = current->pid;
		debugk("monitor %d set\n", 				
				dev->mid);

    wake_up_interruptible(&wq);

		return 0;
	case PTW_GET_TOPA_SZ:
		dev = per_cpu(cpu_dev, ptw_asan_cpu);

		return put_user(dev->conf.topasz * (PAGE_SIZE << dev->conf.order),
				(unsigned int *)arg);
	case PTW_GET_BUF_SZ:
		dev = per_cpu(cpu_dev, ptw_asan_cpu);

		return put_user((PAGE_SIZE << dev->conf.order),
				(int *)arg);
	case PTW_GET_BUF_OFFSET:
		dev = per_cpu(cpu_dev, ptw_asan_cpu);

		return put_user(dev->woff,
				(unsigned int *)arg);
	case PTW_SET_BASE: {
	  	  struct dso_info info;
		  struct dso_entry *entry;
		  
		  if (copy_from_user(&info, (struct dso_info *)arg, 
					  sizeof(struct dso_info))) {
			  return -EFAULT;
		  }

		  entry = kmalloc(sizeof(struct dso_entry), 
				  GFP_KERNEL);
		  if (!entry) {
			  return -ENOMEM;
		  }

		  entry->handle = info.handle;
		  entry->base_address = info.base_address;
		  strncpy(entry->filename, info.filename, sizeof(entry->filename) - 1);
		  entry->filename[sizeof(entry->filename) - 1] = '\0';

		  mutex_lock(&dso_list_mutex);
		  list_add(&entry->list, &dso_list);
		  mutex_unlock(&dso_list_mutex);

		  debugk("dso %s with handle %lx base set to 0x%lx\n", 
				info.filename,
				(unsigned long)info.handle,
				info.base_address);

		  return 0;
		}
	case PTW_GET_BASE: {
		 struct dso_info user_info;
		 void *handle; 
		 int found;
		 struct dso_entry *entry;
  		 /* Initialize the info structure to be returned to the user */
		 struct dso_info info = {0}; 

		 /* Copy only the handle from the user space */
		 if (copy_from_user(&user_info, (struct dso_info *)arg, sizeof(void*))) {
			 return -EFAULT;
		 }

		 handle = user_info.handle;
		 found = 0;

		 mutex_lock(&dso_list_mutex);
		 list_for_each_entry(entry, &dso_list, list) {
			 if (entry->handle == handle) {
				 info.handle = entry->handle;
				 info.base_address = entry->base_address;
				 strncpy(info.filename, entry->filename, sizeof(info.filename) - 1);
				 info.filename[sizeof(info.filename) - 1] = '\0';
				 found = 1;
				 break;
			 }
		 }
		 mutex_unlock(&dso_list_mutex);

		 if (!found) {
			 return -ENOENT;
		 }

		 /* Copy the dso_info structure with base_address and filename back to user space */
		 if (copy_to_user((struct dso_info *)arg, &info, sizeof(struct dso_info))) {
			 return -EFAULT;
		 }

		 debugk("dso %s with handle %p base retrieved: 0x%lx\n",
				 info.filename,
				 info.handle, 
				 info.base_address);

		 return 0;
	   }
	default:
		return -ENOTTY;
	}
	return 0;
}


/* Device release handler */
static int ptw_release(struct inode *inode, struct file *filp)
{
	struct ptw_dev *dev;

	if (ptw_asan_cpu < 0)
		return 0;

	dev = per_cpu(cpu_dev, ptw_asan_cpu);

	mutex_lock(&dev->lock);

	if (dev->pid == current->pid) {
    debugk("device closed by producer %d\n", dev->pid);
		dev->pid = -1;
		stop_pt(NULL);

		/* notify monitor if one exists */
		if (dev->mid != -1) {
			debugk("wait for monitor %d\n", dev->mid)

			/* wait for monitor to consumer all available data */
			topa_writer_get(dev);
			while (*dev->rpage != *dev->wpage) {
				usleep_range(25000, 25001);
				topa_writer_get(dev);
			}

			/* send SIGUSR1 that will stop the monitor */
			kill_pid(find_vpid(dev->mid), SIGUSR1, 1);
		} else {
			ptw_asan_cpu = -1;
		}
	} else if (dev->mid == current->pid) {
    debugk("device closed by monitor %d\n", dev->mid);
		if (dev->pid == -1) {
			ptw_asan_cpu = -1;
		}
		dev->mid = -1;
	}
	mutex_unlock(&dev->lock);

	return 0;
}

/* Device open handler */
static int ptw_open(struct inode *inode, struct file *filp)
{
	debugk("device opened\n");

	return 0;
}

/* Device mmap handler */
static int ptw_mmap(struct file *filp, struct vm_area_struct *vma)
{
	struct ptw_dev *dev;
	unsigned long buffer_size, len = vma->vm_end - vma->vm_start;
	int i, err = 0;
	unsigned short num;
	phys_addr_t vma_target;
	u64 *topa;

  dev = per_cpu(cpu_dev, ptw_asan_cpu);
  if (!dev) {
    pr_err("Invalid dev pointer\n");
    return -EINVAL;
  }

	mutex_lock(&dev->lock);

	if (len == PAGE_SIZE) {
		/* mmap pointers */
		/* check flags for RW or RO */
		if (vma->vm_flags == 0xfb) {/* RW */
			debugk("read pointer mmap\n");

			/* set to rpage */
			vma_target = (u64) virt_to_phys(dev->rpage);
		} else if (vma->vm_flags == 0xf9) { /* RO */
			debugk("write pointer mmap\n");

			/* set to wpage */
			vma_target = (u64) virt_to_phys(dev->wpage);
		}

		/* set up vma */
		vma->vm_flags |= VM_IO | VM_DONTEXPAND | VM_DONTDUMP;
		vm_iomap_memory(vma, vma_target, len);

	} else {
		debugk("topa mmap\n");

		/* mmap topa */
		buffer_size = PAGE_SIZE << dev->conf.order;
		num = dev->conf.topasz;

		vma->vm_flags &= ~VM_MAYWRITE;

		if (len % PAGE_SIZE || len != num * buffer_size || vma->vm_pgoff) {
			err = -EINVAL;
			goto ret;
		}

		if (vma->vm_flags & VM_WRITE) {
			err = -EPERM;
			goto ret;
		}

		topa = dev->topa;
		for (i = 0; i < num; i++) {
      if (topa[i] == 0) {  // Avoid mapping a NULL page.
        pr_err("topa[%d] is NULL\n", i);
        err = -EINVAL;
        break;
      }
			err = remap_pfn_range(vma,
					vma->vm_start + i * buffer_size,
					topa[i] >> PTW_TOPA_PAGE_SHIFT,
					buffer_size,
					vma->vm_page_prot);
			if (err) {/* No cleanup */
				pr_err("remap error in %d\n", i);
				break;
			}
		}
	}

ret:
	mutex_unlock(&dev->lock);
	return err;
}



/* Device file operations */
static const struct file_operations ptw_fops = {
	.owner = THIS_MODULE,
	.mmap =	ptw_mmap,
	.unlocked_ioctl = ptw_ioctl,
	.open = ptw_open,
	.release = ptw_release
};

/* Device information */
static struct miscdevice ptw_miscdev = {
	MISC_DYNAMIC_MINOR,
	PTW_DEV_NAME,
	&ptw_fops
};


static int ptw_cpuid(void)
{
	unsigned a, b, c, d;
	unsigned a1, b1, c1, d1;

	cpuid(0, &a, &b, &c, &d);
	if (a < 0x14) {
		pr_info("Not enough CPUID support for PT\n");
		fake_ptw = true;
	}
	cpuid_count(0x07, 0, &a, &b, &c, &d);
	if ((b & BIT(25)) == 0) {
		pr_info("No PT support\n");
		fake_ptw = true;
	}
	cpuid_count(0x14, 0, &a, &b, &c, &d);
	if (!(c & BIT(0))) {
		pr_info("No ToPA support\n");
		fake_ptw = true;
	}
	has_cr3_match = !!(b & BIT(0));
	if (b & BIT(2))
		addr_cfg_max = 2;
	if (!(c & BIT(1))) {
		pr_info("Only single buffer output supported\n");
		fake_ptw = true;
	}
	if (a >= 1)
		cpuid_count(0x14, 1, &a1, &b1, &c1, &d1);
	if (b & BIT(1)) {
		mtc_freq_mask = (a1 >> 16) & 0xffff;
		cyc_thresh_mask = b1 & 0xffff;
		psb_freq_mask = (b1 >> 16) & 0xffff;
		addr_range_num = a1 & 0x3;
	}
	return 0;
}


	void 
handler_bh(struct work_struct *work)
{
	int dist;
	struct ptw_dev *dev;

	dev = per_cpu(cpu_dev, ptw_asan_cpu);

	/* sleep until everything has been read */
	/* change this to a percentage later on */
	topa_writer_get(dev);
	dist = rdwr_pagedist(dev);
	while (dist <= MIN_AVAIL_BUFFERS) {
		usleep_range(25000, 25001);
		topa_writer_get(dev);
		dist = rdwr_pagedist(dev);
	}

	/* resume asan process */
	debugk("START PROCESS %d\n", dev->pid);
	kill_pid(find_vpid(dev->pid), SIGCONT, 1);

	dev->process_stopped = 0;
}

static int ptw_nmi(unsigned int cmd, struct pt_regs *regs)
{
	u64 perf_status;
	struct ptw_dev *dev;
	int dist;

	/* Ignore NMIs that cannot be ours:
	 * NMI is not NMI_LOCAL
	 * Current CPU is not running PT
	 */
	dev = __this_cpu_read(cpu_dev);
	if (!dev)
		return NMI_DONE;

	/* Check the MSRs to make sure this is the correct NMI */
	rdmsrl(MSR_CORE_PERF_GLOBAL_STATUS, perf_status);
	if (!test_bit(TRACE_TOPA_PMI_BIT, (unsigned long *)&perf_status))
		return NMI_DONE;

	/* Set bit in the reset register */
	wrmsrl(MSR_CORE_PERF_GLOBAL_OVF_CTRL, TRACE_TOPA_PMI);

	topa_writer_get(dev);
	dist = rdwr_pagedist(dev);
	debugk("NMI: reader %hu, writer %hu, dist %d\n",
			*dev->rpage, *dev->wpage, dist);

	/* Check reader and stop process if needed */
	if (dist <= OFLOW_BUFFERS_THRES) {
		debugk("Ooops overflow\n");
	} else if (dev->conf.nmi_freq > 0) {
		if (dist <= MIN_AVAIL_BUFFERS) {
			/* STOP PROCESS */
			debugk("STOP PROCESS %d\n", dev->pid);
			kill_pid(find_vpid(dev->pid), SIGSTOP, 1);
			dev->process_stopped++;

			/* schedule bottom half */
			schedule_work(&workqueue);
		}
	}

	/* the reader has no segment */
	return NMI_HANDLED;
}


static int ptw_init(void)
{
	int err;
	u64 x2apic;

	/* Retrieve CPU support for Intel PT */
	err = ptw_cpuid();
	if (err < 0)
		return err;

	err = misc_register(&ptw_miscdev);
	if (err < 0) {
		pr_err("Cannot register ptw device\n");
		return err;
	}

	/* must be 0x400 (APIC_DM_NMI) -> NMI */
	/* TODO: IA32_X2APIC_LVT_PMI ?? */
	pt_rdmsrl_safe(0x834, &x2apic);
	pr_info("APIC_LVTPC: %x IA32_X2APIC_LVT_PMI %llx\n", 
			apic_read(APIC_LVTPC), x2apic);
	
	err = register_nmi_handler(NMI_LOCAL, ptw_nmi, NMI_FLAG_FIRST,
			PTW_DEV_NAME);
	if (err) {
		pr_err("NMI_LOCAL handler registration failed!");
		return err;
	}

	err = cpuhp_setup_state(CPUHP_AP_ONLINE_DYN, PTW_DEV_NAME,
				       ptw_cpu_startup,
				       ptw_cpu_teardown);
	if (err < 0)
		goto out_buffers;
	ptw_hotplug_state = err;

	pr_info("loaded\n");

	return 0;

out_buffers:
	misc_deregister(&ptw_miscdev);
	return err;
}

static void ptw_exit(void)
{
  int cpu;
  for_each_online_cpu(cpu) {
    ptw_cpu_teardown(cpu);
  }

	misc_deregister(&ptw_miscdev);
	unregister_nmi_handler(NMI_LOCAL, PTW_DEV_NAME);
	if (ptw_hotplug_state >= 0)
		cpuhp_remove_state(ptw_hotplug_state);

	pr_info("%s: exited\n", PTW_DEV_NAME);
}

module_init(ptw_init);
module_exit(ptw_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Konstantinos Kleftogiorgos");
