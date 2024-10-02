#ifndef ASAN_API_H
#define ASAN_API_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define ASAN_API_DEBUG 1

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define SHADOW_SCALE 3
#define SHADOW_GRANULARITY 8
#define SHADOW_OFFSET 0x00007fff8000

#define ASAN_PAGE_SIZE 4096

#define SANITIZER_WORDSIZE 64

#if SANITIZER_WORDSIZE == 64

#define FIRST_32_SECOND_64(a, b) (b)
#else
# define FIRST_32_SECOND_64(a, b) (a)
#endif

#define MEM_TO_SHADOW(mem) (((mem) >> SHADOW_SCALE) + (SHADOW_OFFSET))

/* low mem */
#define kLowMemBeg      0
#define kLowMemEnd      (SHADOW_OFFSET ? SHADOW_OFFSET - 1 : 0)

/* low shadow */
#define kLowShadowBeg   SHADOW_OFFSET
#define kLowShadowEnd   MEM_TO_SHADOW(kLowMemEnd)

/* high mem */
#define kHighMemEnd	0x7fffffffffff
#define kHighMemBeg     (MEM_TO_SHADOW(kHighMemEnd) + 1)

/* high shadow */
#define kHighShadowBeg  MEM_TO_SHADOW(kHighMemBeg)
#define kHighShadowEnd  MEM_TO_SHADOW(kHighMemEnd)

/* shadow gap */
#define kZeroBaseShadowStart 0
#define kZeroBaseMaxShadowStart (1 << 18)
#define kShadowGapBeg   (kLowShadowEnd ? kLowShadowEnd + 1 \
		: kZeroBaseShadowStart)
#define kShadowGapEnd   (kHighShadowBeg - 1)

void __asan_check_failed(const char *file, int line, const char *cond,
		uint64_t v1, uint64_t v2) {
#if ASAN_API_DEBUG
	printf("Sanitizer CHECK failed: %s:%d %s (%ld, %ld)\n", file, line, cond,
			v1, v2);
#endif
}

#define CHECK_IMPL(c1, op, c2) \
	do { \
		uint64_t v1 = (uint64_t)(c1); \
		uint64_t v2 = (uint64_t)(c2); \
		if (unlikely(!(v1 op v2))) \
		__asan_check_failed(__FILE__, __LINE__, \
				"(" #c1 ") " #op " (" #c2 ")", v1, v2); \
	} while (false) \

#define CHECK(a)       CHECK_IMPL((a), !=, 0)
#define CHECK_EQ(a, b) CHECK_IMPL((a), ==, (b))
#define CHECK_NE(a, b) CHECK_IMPL((a), !=, (b))
#define CHECK_LT(a, b) CHECK_IMPL((a), <,  (b))
#define CHECK_LE(a, b) CHECK_IMPL((a), <=, (b))
#define CHECK_GT(a, b) CHECK_IMPL((a), >,  (b))
#define CHECK_GE(a, b) CHECK_IMPL((a), >=, (b))

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

const int kAsanHeapLeftRedzoneMagic = 0xfa;
const int kAsanHeapFreeMagic = 0xfd;
const int kAsanStackLeftRedzoneMagic = 0xf1;
const int kAsanStackMidRedzoneMagic = 0xf2;
const int kAsanStackRightRedzoneMagic = 0xf3;
const int kAsanStackAfterReturnMagic = 0xf5;
const int kAsanInitializationOrderMagic = 0xf6;
const int kAsanUserPoisonedMemoryMagic = 0xf7;
const int kAsanContiguousContainerOOBMagic = 0xfc;
const int kAsanStackUseAfterScopeMagic = 0xf8;
const int kAsanGlobalRedzoneMagic = 0xf9;
const int kAsanInternalHeapMagic = 0xfe;
const int kAsanArrayCookieMagic = 0xac;
const int kAsanIntraObjectRedzone = 0xbb;
const int kAsanAllocaLeftMagic = 0xca;
const int kAsanAllocaRightMagic = 0xcb;
const int kAsanShadowGap = 0xcc;

void asan_print_mem_layout (void)
{
#if ASAN_API_DEBUG
	printf("kHighMemEnd %lx\n", kHighMemEnd);
	printf("kHighMemBeg %lx\n", kHighMemBeg);
	printf("kHighShadowEnd %lx\n", kHighShadowEnd);
	printf("kHigShadowBeg %lx\n", kHighShadowBeg);
	printf("kShadowGapEnd %lx\n", kShadowGapEnd);
	printf("kShadowGapBeg %lx\n", kShadowGapBeg);
	printf("kLowShadowEnd %lx\n", kLowShadowEnd); 
	printf("kLowShadowBeg %lx\n", kLowShadowBeg); 
	printf("kLowMemEnd %lx\n", kLowMemEnd); 
	printf("kLowMemBeg %x\n", kLowMemBeg); 
#endif
}

void asan_mmap_shadow (void)
{
	char *mmap_region[3];
	
	/* mmap high shadow */
	mmap_region[0] = (char *)mmap((void *)kHighShadowBeg,
			kHighShadowEnd - kHighShadowBeg,
			PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE,
			-1,
			0x0);
	if (mmap_region[0] == MAP_FAILED) {
		printf("error: High Shadow mmap failed with %d\n", errno);
		exit(1);
	}

	/* mmap shadow gap */
#if 0
	mmap_region[1] = (char *)mmap((void *)kShadowGapBeg,
			kShadowGapEnd - kShadowGapBeg,
			PROT_NONE,
			MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE,
			-1,
			0x0);
	if (mmap_region[1] == MAP_FAILED) {
		printf("error: Shadow Gap mmap failed with %d\n", errno);
		goto clean_shadow_high;
	}
#endif

	/* mmap low shadow */
	mmap_region[2] = (char *)mmap((void *)kLowShadowBeg,
			kLowShadowEnd - kLowShadowBeg,
			PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED | MAP_NORESERVE,
			-1,
			0x0);
	if (mmap_region[2] == MAP_FAILED) {
		printf("error: Low Shadow mmap failed with %d\n", errno );
		goto clean_shadow_gap;
	}

	return;

clean_shadow_gap:
	munmap(mmap_region[1], kShadowGapEnd - kShadowGapBeg);
clean_shadow_high:
	munmap(mmap_region[1], kHighShadowEnd - kHighShadowBeg);

	exit(1);
}

static inline bool __asan_addr_is_in_low_mem(uint64_t a) 
{
	return a <= kLowMemEnd;
}

static inline bool __asan_addr_is_in_low_shadow(uint64_t a) 
{
	return a >= kLowShadowBeg && a <= kLowShadowEnd;
}

static inline bool __asan_addr_is_in_high_mem(uint64_t a) 
{
	return kHighMemBeg && a >= kHighMemBeg && a <= kHighMemEnd;
}

static inline bool __asan_addr_is_in_high_shadow(uint64_t a) 
{
	return kHighMemBeg && a >= kHighShadowBeg && a <= kHighShadowEnd;
}

static inline bool __asan_addr_is_in_shadow_gap(uint64_t a) 
{
	return a >= kShadowGapBeg && a <= kShadowGapEnd;
}

static inline bool __asan_addr_is_in_mem(uint64_t a) 
{
	return __asan_addr_is_in_low_mem(a) || 
		__asan_addr_is_in_high_mem(a);
}

static inline bool __asan_addr_is_aligned_by_granularity(uint64_t a) {
	return (a & (SHADOW_GRANULARITY - 1)) == 0;
}

static inline bool __asan_addr_is_in_shadow(uint64_t a)
{
	return __asan_addr_is_in_low_shadow(a) || 
		__asan_addr_is_in_high_shadow(a);
}

static inline uint64_t __asan_most_significant_set_bit_index(uint64_t x) 
{
	CHECK_NE(x, 0U);
	unsigned long up;

	up = SANITIZER_WORDSIZE - 1 - __builtin_clzl(x);
	return up;
}

static inline uint64_t __asan_least_significant_set_bit_index(uint64_t x) 
{
	CHECK_NE(x, 0U);
	unsigned long up;

	up = __builtin_ctzl(x);
	return up;
}

static inline bool __asan_is_power_of_two(uint64_t x) 
{
	return (x & (x - 1)) == 0;
}

static inline uint64_t __asan_round_up_to_power_of_two(uint64_t size) 
{
	CHECK(size);
	if (__asan_is_power_of_two(size)) return size;

	uint64_t up = __asan_most_significant_set_bit_index(size);
	CHECK_LT(size, (1ULL << (up + 1)));
	CHECK_GT(size, (1ULL << up));
	return 1ULL << (up + 1);
}

static inline uint64_t __asan_round_up_to(uint64_t size, uint64_t boundary) 
{
	return (size + boundary - 1) & ~(boundary - 1);
}

static inline uint64_t __asan_round_down_to(uint64_t x, uint64_t boundary) 
{
	return x & ~(boundary - 1);
}

static inline bool __asan_is_aligned(uint64_t a, uint64_t alignment) 
{
	return (a & (alignment - 1)) == 0;
}

static inline bool __asan_mem_is_zero(const char *beg, uint64_t size) 
{
	const char *end = beg + size;
	uint64_t *aligned_beg = (uint64_t *)__asan_round_up_to((uint64_t)beg, 
			sizeof(uint64_t));
	uint64_t *aligned_end = (uint64_t *)__asan_round_down_to((uint64_t)end, 
			sizeof(uint64_t));
	uint64_t all = 0;

	for (const char *mem = beg; mem < (char*) aligned_beg && mem < end; mem++)
		all |= *mem;
	for (; aligned_beg < aligned_end; aligned_beg++)
		all |= *aligned_beg;

	if ((char *)aligned_end >= beg) {
		for (const char *mem = (char *)aligned_end; mem < end; mem++) all |= *mem;
	}

	return all == 0;
}

static inline uint32_t __asan_rz_log2size(uint32_t rz_log) 
{
	CHECK_LT(rz_log, 8);
	return 16 << rz_log;
}

static inline uint64_t __asan_log2(uint64_t x) {
	CHECK(__asan_is_power_of_two(x));
	return __asan_least_significant_set_bit_index(x);
}

static inline uint32_t __asan_rz_size2log(uint32_t rz_size) 
{
	CHECK_GE(rz_size, 16);
	CHECK_LE(rz_size, 2048);
	CHECK(__asan_is_power_of_two(rz_size));
	uint32_t res = __asan_log2(rz_size) - 4;
	CHECK_EQ(rz_size, __asan_rz_log2size(res));
	return res;
}

static inline uint64_t __asan_compute_rz_log(uint64_t user_requested_size) 
{
	uint32_t rz_log = (user_requested_size <= 64 - 16)  	? 0
		: user_requested_size <= 128 - 32         	? 1
		: user_requested_size <= 512 - 64         	? 2
		: user_requested_size <= 4096 - 128       	? 3
		: user_requested_size <= (1 << 14) - 256  	? 4
		: user_requested_size <= (1 << 15) - 512  	? 5
		: user_requested_size <= (1 << 16) - 1024 	? 6
		: 7;

	/* allocating 16 bytes for the ChunkHeader */
	uint32_t hdr_log = __asan_rz_size2log(__asan_round_up_to_power_of_two(16));
	uint32_t min_log = __asan_rz_size2log(16);
	uint32_t max_log = __asan_rz_size2log(2048);
	return min(max(rz_log, max(min_log, hdr_log)), max(max_log, hdr_log));
}

static inline bool __asan_address_is_poisoned(uint64_t a)
{
	const uint64_t kAccessSize = 1;
	unsigned char *shadow_address = (unsigned char *) MEM_TO_SHADOW(a);
	signed char shadow_value = *shadow_address;

	if (shadow_value) {
		unsigned char last_accessed_byte = 
			(a & (SHADOW_GRANULARITY - 1))
			+ kAccessSize - 1;
		return (last_accessed_byte >= shadow_value);
	}

	return false;
}

static inline uint64_t __asan_region_is_poisoned(uint64_t beg, 
		uint64_t size)
{
	if (!size) return 0;

	uint64_t end = beg + size;

	if (!__asan_addr_is_in_mem(beg)) return beg;
	if (!__asan_addr_is_in_mem(end)) return end;

	CHECK_LT(beg, end);

	uint64_t aligned_b = __asan_round_up_to(beg, SHADOW_GRANULARITY);
	uint64_t aligned_e = __asan_round_down_to(end, SHADOW_GRANULARITY);
	uint64_t shadow_beg = MEM_TO_SHADOW(aligned_b);
	uint64_t shadow_end = MEM_TO_SHADOW(aligned_e);

	if (!__asan_address_is_poisoned(beg) &&
			!__asan_address_is_poisoned(end - 1) &&
			(shadow_end <= shadow_beg ||
			 __asan_mem_is_zero((const char *)shadow_beg,
				 shadow_end - shadow_beg)))
		return 0;

	for (; beg < end; beg++)
		if (__asan_address_is_poisoned(beg))
			return beg;

	return 0;
}

static inline bool __asan_is_invalid_pointer_pair(uint64_t a1, uint64_t a2) {
	  if (a1 == a2)
		      return false;

	  /* 256B in shadow memory can be iterated quite fast */
	  static const uint64_t kMaxOffset = 2048;

	  uint64_t left = a1 < a2 ? a1 : a2;
	  uint64_t right = a1 < a2 ? a2 : a1;
	  uint64_t offset = right - left;
	  if (offset <= kMaxOffset)
		  return __asan_region_is_poisoned(left, offset);

	  /* we need stack info to proceed */
#if 0
	  AsanThread *t = GetCurrentThread();

	  /* check whether left is a stack memory pointer */
	  if (uint64_t shadow_offset1 = t->GetStackVariableShadowStart(left)) {
		  uint64_t shadow_offset2 = t->GetStackVariableShadowStart(right);
		  return shadow_offset2 == 0 || shadow_offset1 != shadow_offset2;
	  }

	  /* check whether left is a heap memory address */
	  HeapAddressDescription hdesc1, hdesc2;
	  if (GetHeapAddressInformation(left, 0, &hdesc1) &&
			  hdesc1.chunk_access.access_type == kAccessTypeInside)
		  return !GetHeapAddressInformation(right, 0, &hdesc2) ||
			  hdesc2.chunk_access.access_type != kAccessTypeInside ||
			  hdesc1.chunk_access.chunk_begin != hdesc2.chunk_access.chunk_begin;

	  /* check whether left is an address of a global variable */
	  GlobalAddressDescription gdesc1, gdesc2;
	  if (GetGlobalAddressInformation(left, 0, &gdesc1))
		  return !GetGlobalAddressInformation(right - 1, 0, &gdesc2) ||
			  !gdesc1.PointsInsideTheSameVariable(gdesc2);

	  if (t->GetStackVariableShadowStart(right) ||
			  GetHeapAddressInformation(right, 0, &hdesc2) ||
			  GetGlobalAddressInformation(right - 1, 0, &gdesc2))
		  return true;
#endif

	  /* At this point we know nothing about both a1 and a2 addresses. */
	  return false;
}

static inline void __asan_print_legend(void)
{
	printf(
			"Shadow byte legend (one shadow byte represents %d "
			"application bytes):\n",
			(int)SHADOW_GRANULARITY);
	printf("  Addressable:           %02x\n", 0);
	printf("  Partially addressable: ");
	for (uint8_t i = 1; i < SHADOW_GRANULARITY; i++) {
		printf("%02x ", i);
	}
	printf("\n");
	printf("  Heap left redzone:       %02x\n",
			kAsanHeapLeftRedzoneMagic);
	printf("  Freed heap region:       %02x\n", kAsanHeapFreeMagic);
	printf("  Stack left redzone:      %02x\n",
			kAsanStackLeftRedzoneMagic);
	printf("  Stack mid redzone:       %02x\n",
			kAsanStackMidRedzoneMagic);
	printf("  Stack right redzone:     %02x\n",
			kAsanStackRightRedzoneMagic);
	printf("  Stack after return:      %02x\n",
			kAsanStackAfterReturnMagic);
	printf("  Stack use after scope:   %02x\n",
			kAsanStackUseAfterScopeMagic);
	printf("  Global redzone:          %02x\n", kAsanGlobalRedzoneMagic);
	printf("  Global init order:       %02x\n",
			kAsanInitializationOrderMagic);
	printf("  Poisoned by user:        %02x\n",
			kAsanUserPoisonedMemoryMagic);
	printf("  Container overflow:      %02x\n",
			kAsanContiguousContainerOOBMagic);
	printf("  Array cookie:            %02x\n",
			kAsanArrayCookieMagic);
	printf("  Intra object redzone:    %02x\n",
			kAsanIntraObjectRedzone);
	printf("  ASan internal:           %02x\n", kAsanInternalHeapMagic);
	printf("  Left alloca redzone:     %02x\n", kAsanAllocaLeftMagic);
	printf("  Right alloca redzone:    %02x\n", kAsanAllocaRightMagic);
	printf("  Shadow gap:              %02x\n", kAsanShadowGap);
}

static inline void __asan_print_shadow(uint64_t addr)
{
	uint8_t *bytes;
	uint8_t *guilty;

	if (!__asan_addr_is_in_mem(addr)) return;

	uint64_t shadow_addr = MEM_TO_SHADOW(addr);

	const uint64_t n_bytes_per_row = 16;
	uint64_t aligned_shadow = shadow_addr & ~(n_bytes_per_row - 1);

	printf("Shadow bytes around the buggy address:\n");
	for (int i = -5; i <= 5; i++) {
		uint64_t row_shadow_addr = aligned_shadow + i * n_bytes_per_row;
		/*
		 * Skip rows that would be outside the shadow range. This can happen when
		 * the user address is near the bottom, top, or shadow gap of the address
		 * space.
		 */
		if (!__asan_addr_is_in_shadow(row_shadow_addr)) continue;

		/* print space and pointer */
		printf("%s%p:", 
				(i == 0) ? "=>" : "  ", 
				(uint8_t *)row_shadow_addr);

		bytes = (uint8_t *)row_shadow_addr;
		guilty = (uint8_t *)shadow_addr;

		/* print shadow bytes */
		for (uint64_t i = 0; i < n_bytes_per_row; i++) {
			uint8_t *p = bytes + i;
			printf("%s%02x%s", (p == guilty) ? "[" : 
					(p - 1 == guilty && i != 0) ? "" : " ",
					*p,
					p == guilty ? "]" : "");
		}
		printf("\n");
	}
	__asan_print_legend();
}

static inline char* __asan_find_error(uint64_t addr)
{
	if (!__asan_addr_is_in_mem(addr)) return "unknown-crash";

	unsigned char *shadow_addr = (unsigned char *) MEM_TO_SHADOW(addr);

	/* fix the case where the shadow byte is partial */
	if (*shadow_addr < 9)
		shadow_addr = (unsigned char *) MEM_TO_SHADOW(addr + 8);

	switch (*shadow_addr) {
		case kAsanHeapLeftRedzoneMagic:
		case kAsanArrayCookieMagic:
			return "heap-buffer-overflow";
		case kAsanHeapFreeMagic:
			return "heap-use-after-free";
		case kAsanStackLeftRedzoneMagic:
			return "stack-buffer-underflow";
		case kAsanInitializationOrderMagic:
			return "initialization-order-fiasco";
		case kAsanStackMidRedzoneMagic:
		case kAsanStackRightRedzoneMagic:
			return "stack-buffer-overflow";
		case kAsanStackAfterReturnMagic:
			return "stack-use-after-return";
		case kAsanUserPoisonedMemoryMagic:
			return "use-after-poison";
		case kAsanContiguousContainerOOBMagic:
			return "container-overflow";
		case kAsanStackUseAfterScopeMagic:
			return "stack-use-after-scope";
		case kAsanGlobalRedzoneMagic:
			return "global-buffer-overflow";
		case kAsanIntraObjectRedzone:
			return "intra-object-overflow";
		case kAsanAllocaLeftMagic:
		case kAsanAllocaRightMagic:
			return "dynamic-stack-buffer-overflow";
	}

	return "unknown-crash";
}

static inline void __asan_report_error(uint64_t addr, 
		int is_write,
		uint32_t access_size, 
		uint32_t exp,
		bool fatal, 
		char * custom_error)
{
	printf("\n=================================================================\n");
	printf("ERROR: AddressSanitizer: %s on address %p\n", 
			custom_error == NULL ? __asan_find_error(addr) : custom_error,
			(void *)addr);
	printf("%s of size %d at %p thread T0\n",
			is_write? "WRITE": "READ",
			access_size,
			(void *)addr);

	__asan_print_shadow(addr);
	printf("\n=================================================================\n");

	if (fatal)
		exit(0);
}

static inline bool __asan_quick_check_for_unpoisoned_region(uint64_t beg, 
		uint64_t size) 
{
	if (size == 0) return true;

	if (size <= 32)
		return !__asan_address_is_poisoned(beg) &&
			!__asan_address_is_poisoned(beg + size - 1) &&
			!__asan_address_is_poisoned(beg + size / 2);

	if (size <= 64)
		return !__asan_address_is_poisoned(beg) &&
			!__asan_address_is_poisoned(beg + size / 4) &&
			!__asan_address_is_poisoned(beg + size - 1) &&
			!__asan_address_is_poisoned(beg + 3 * size / 4) &&
			!__asan_address_is_poisoned(beg + size / 2);

	return false;
}

static inline void __asan_fast_poison_shadow(uint64_t aligned_beg, 
		uint64_t aligned_size,
		uint8_t value) 
{
	
	uint64_t shadow_beg = MEM_TO_SHADOW(aligned_beg);
	uint64_t shadow_end = MEM_TO_SHADOW(
			aligned_beg + aligned_size - SHADOW_GRANULARITY) + 1;

	memset((void*)shadow_beg, value, shadow_end - shadow_beg);
}

static inline void __asan_poison_shadow(uint64_t addr, 
		uint64_t size, uint8_t value) {
	CHECK(__asan_addr_is_aligned_by_granularity(addr));
	CHECK(__asan_addr_is_in_mem(addr));
	CHECK(__asan_addr_is_aligned_by_granularity(addr + size));
	CHECK(__asan_addr_is_in_mem(addr + size - SHADOW_GRANULARITY));
	__asan_fast_poison_shadow(addr, size, value);
}

static inline void __asan_fast_poison_shadow_partial_right_redzone(
		    uint64_t aligned_addr, 
		    uint64_t size, 
		    uint64_t redzone_size,
		    uint8_t value) 
{
	/* TODO: pass to user as argument? */
	bool poison_partial = 1;
	uint8_t *shadow = (uint8_t*)MEM_TO_SHADOW(aligned_addr);
	for (uint64_t i = 0; i < redzone_size; i += SHADOW_GRANULARITY, shadow++) {
		if (i + SHADOW_GRANULARITY <= size) {
			*shadow = 0;  // fully addressable
		} else if (i >= size) {
			*shadow = (SHADOW_GRANULARITY == 128) ? 0xff : value;  // unaddressable
		} else {
			// first size-i bytes are addressable
			*shadow = poison_partial ? (uint8_t)(size - i) : 0;
		}
	}
}


/* ============= ASan Decoupled API ================ */

static inline void asan_api_csr(const uint64_t pkt[])
{
	uint64_t addr;
	uint64_t bad;
	uint8_t size, fatal, is_write;

	/* parse arguments */
	size = pkt[0] >> 8 & 0x3f;
	fatal = pkt[0] >> 14 & 0x1;
	is_write = pkt[0] >> 15 & 0x1;
	addr = pkt[0] >> 16;

	/* fast check */
	if (unlikely(__asan_address_is_poisoned(addr) ||
				__asan_address_is_poisoned(addr + size - 1))) {
		/* slow check */
		bad = __asan_region_is_poisoned(addr, size);
		__asan_report_error(bad, is_write, size, 0, fatal, NULL);
	}

#if ASAN_API_DEBUG
	printf("API: Check Small Region\n");
	printf("\taddr: 0x%lx isWrite: 0x%x fatal: 0x%x size: 0x%x\n", 
			addr,
			is_write,
			fatal,
			size);
#endif
}

static inline void asan_api_csre(const uint64_t pkt[])
{
	uint64_t addr;
	uint64_t bad;
	uint8_t size, fatal, is_write;
	uint32_t exp;

	/* parse arguments */
	size = pkt[0] >> 8 & 0x3f;
	fatal = pkt[0] >> 14 & 0x1;
	is_write = pkt[0] >> 15 & 0x1;
	addr = pkt[0] >> 16;
	exp = pkt[1];

	/* to be defined */
	(void)exp;

	/* fast check */
	if (unlikely(__asan_address_is_poisoned(addr) ||
				__asan_address_is_poisoned(addr + size - 1))) {
		/* slow check */
		bad = __asan_region_is_poisoned(addr, size);
		__asan_report_error(bad, is_write, size, 0, fatal, NULL);
	}

#if ASAN_API_DEBUG
	printf("API: Check Small Region Experimental\n");
	printf("\taddr: 0x%lx isWrite: 0x%x fatal: 0x%x size: 0x%x exp: 0x%x\n", 
			addr,
			is_write,
			fatal,
			size,
			exp);
#endif
}

static inline void asan_api_clr(const uint64_t pkt[])
{
	uint64_t addr;
	uint8_t fatal, is_write;
	uint32_t size;
	uint64_t bad;

	/* parse arguments */
	fatal = pkt[0] >> 14 & 0x1;
	is_write = pkt[0] >> 15 & 0x1;
	addr = pkt[0] >> 16;
	size = pkt[1];

	/* check for negative size param */
	if (addr > addr + size) {
		__asan_report_error(addr, is_write, size, 0, fatal, "negative-size-param");
	}

	/* check region */
	if ((bad = __asan_region_is_poisoned(addr, size))) {
		__asan_report_error(bad, is_write, size, 0, fatal, NULL);
	}

#if ASAN_API_DEBUG
	printf("API: Check Large Region\n");
	printf("\taddr: 0x%lx isWrite: 0x%x fatal: 0x%x size: 0x%x\n", 
			addr,
			is_write,
			fatal,
			size);
#endif
}

static inline void asan_api_clre(const uint64_t pkt[])
{
	uint64_t addr;
	uint8_t fatal, is_write;
	uint32_t size;
	uint32_t exp;
	uint64_t bad;

	/* parse arguments */
	fatal = (pkt[0] >> 14) & 0x1;
	is_write = (pkt[0] >> 15) & 0x1;
	addr = (pkt[0] >> 16) & 0xFFFFFFFFFFFF;
	size = pkt[1] & 0xFFFFFFFF;
	exp = (pkt[1] >> 32) & 0xFFFFFFFF;

	/* check for negative size param */
	if (addr > addr + size) {
		__asan_report_error(addr, is_write, size, 0, fatal, "negative-size-param");
	}

	/* to be defined */
	(void)exp;

	/* check region */
	if ((bad = __asan_region_is_poisoned(addr, size))) {
		__asan_report_error(bad, is_write, size, 0, fatal, NULL);
	}

#if ASAN_API_DEBUG
	printf("API: Check Large Region Experimental\n");
	printf("\taddr: 0x%lx isWrite: 0x%x fatal: 0x%x size: 0x%x exp: 0x%x\n", 
			addr,
			is_write,
			fatal,
			size,
			exp);
#endif
}

static inline void asan_api_co(const uint64_t pkt[])
{
	uint64_t addr;
	uint32_t size;

	/* parse arguments */
	addr = pkt[0] >> 16;
	size = pkt[1] >> 16;

	/* TODO: fix */
	/* the check alone may not be enough */
	/* we might need list of globals */
	/* look at CheckODRViolationViaPoisoning */

	/* check area */
	if (__asan_region_is_poisoned(addr, size)) {
		__asan_report_error(addr, 0, size, 0, 1, "odr-violation");
	}

#if ASAN_API_DEBUG
	printf("API: Check ODR\n");
	printf("\taddr: 0x%lx size: 0x%x\n", 
			addr, size);
#endif
}

static inline void asan_api_crc(const uint64_t pkt[])
{
	uint64_t from;
	uint64_t to, bad = 0;
	uint32_t size;
	uint8_t fatal;

	/* parse arguments */
	fatal = pkt[0] >> 15 & 0x1;
	from = pkt[0] >> 16;
	to = pkt[1] >> 16;
	size = pkt[2];

	/* check source */
	if (from > from + size) { 
		__asan_report_error(from, 0, size, 0, 0, "negative-size-param");
	}
	if (!__asan_quick_check_for_unpoisoned_region(from, size) &&
			(bad = __asan_region_is_poisoned(from, size))) {
		__asan_report_error(bad, 0, size, 0, fatal, NULL);
	}

	/* check destination */
	if (to > to + size) { 
		__asan_report_error(to, 0, size, 0, 0, "negative-size-param");
		printf("ASan string function size overflow detected\n");
	}
	if (!__asan_quick_check_for_unpoisoned_region(to, size) &&
			(bad = __asan_region_is_poisoned(to, size))) {
		__asan_report_error(bad, 1, size, 0, fatal, NULL);
	}

#if ASAN_API_DEBUG
	printf("API: Check Region Copy\n");
	printf("\tfrom: 0x%lx to: 0x%lx fatal: 0x%x size: 0x%x\n", 
			from, to, fatal, size);
#endif
}

static inline void __asan_ca_print(uint64_t addr, 
		int fatal,
		int is_write,
		uint32_t access_size, 
		uint32_t exp)
{
#if ASAN_API_DEBUG
	printf("API: Check Access %d %s %s\n", access_size, is_write? "Write" : "Read", exp? "Experimental" : "");
	printf("\taddr: 0x%lx fatal: 0x%x\n", addr, fatal);
#endif

}

#define CHECK_DEFINE(size) 						\
static inline void asan_api_ca ## size(const uint64_t pkt[])  		\
{                                                       		\
	uint8_t isWrite, fatal;						\
	uint64_t addr;							\
									\
	/* parse arguments */						\
	fatal = pkt[0] >> 14 & 0x1;					\
	isWrite = pkt[0] >> 15 & 0x1;					\
	addr = pkt[0] >> 16;						\
									\
	if (!__asan_addr_is_in_mem(addr) && 				\
			!__asan_addr_is_in_shadow(addr))		\
	return;                                         		\
									\
	uint64_t sp = MEM_TO_SHADOW(addr);                   		\
	/* TODO: look into *reinterpret_cast */				\
	uint64_t s = size <= SHADOW_GRANULARITY ?			\
			*(uint8_t *)(sp) 	         		\
			: *(uint16_t *)(sp);        			\
	if (unlikely(s)) {                                      	\
	    if (unlikely(size >= SHADOW_GRANULARITY ||      		\
		  ((signed char)((addr & (SHADOW_GRANULARITY - 1))	\
		  + size - 1)) >= (signed char)s)) {                    \
			__asan_report_error(addr, 			\
					isWrite, 			\
					size, 				\
					0,				\
					fatal,				\
					NULL);				\
	    }                                                           \
	}								\
									\
	__asan_ca_print(addr, fatal, isWrite, size, 0);			\
}                                                       		\
static inline void asan_api_cae ## size(const uint64_t pkt[]) 		\
{                                                       		\
	uint8_t isWrite, fatal;						\
	uint32_t exp;							\
	uint64_t addr;							\
									\
	/* parse arguments */						\
	fatal = pkt[0] >> 14 & 0x1;					\
	isWrite = pkt[0] >> 15 & 0x1;					\
	addr = pkt[0] >> 16;						\
	exp = pkt[1];							\
									\
	if (!__asan_addr_is_in_mem(addr) && 				\
			!__asan_addr_is_in_shadow(addr))		\
	return;                                         		\
									\
	uint64_t sp = MEM_TO_SHADOW(addr);                   		\
	/* TODO: look into *reinterpret_cast */				\
	uint64_t s = size <= SHADOW_GRANULARITY ?			\
			*(uint8_t *)(sp) 	         		\
			: *(uint16_t *)(sp);        			\
	if (unlikely(s)) {                                      	\
	    if (unlikely(size >= SHADOW_GRANULARITY ||      		\
		  ((signed char)((addr & (SHADOW_GRANULARITY - 1))	\
		  + size - 1)) >= (signed char)s)) {                    \
			__asan_report_error(addr, 			\
					isWrite, 			\
					size, 				\
					exp,				\
					fatal,				\
					NULL);				\
	    }                                                           \
	}								\
									\
	__asan_ca_print(addr, fatal, isWrite, size, 1);			\
}		                                                       	\

CHECK_DEFINE(1)
CHECK_DEFINE(2)
CHECK_DEFINE(4)
CHECK_DEFINE(8)
CHECK_DEFINE(16)

static inline void asan_api_ccac(const uint64_t pkt[])
{
	uint64_t addr;

	/* parse arguments */
	addr = pkt[0] >> 16;

	uint64_t s = MEM_TO_SHADOW(addr);
	unsigned char sval = *(unsigned char*)(s);

	/* TODO: what should we return here */
	// if (sval == kAsanArrayCookieMagic) return *p;
	
	if (sval == kAsanHeapFreeMagic) {
		__asan_report_error(addr, 0, 0, 1, 0, "double-free");
	}

#if ASAN_API_DEBUG
	printf("API: Check CXX Array Cookie\n");
	printf("\taddr: 0x%lx\n", addr);
#endif
}

static inline void asan_api_al(const uint64_t pkt[])
{
	uint64_t alignment;
	uint64_t allocated;
	uint32_t size;
	uint32_t allocated_size;

	/* parse arguments */
	alignment = pkt[0] >> 16;
	allocated = pkt[1] >> 16;
	size = pkt[2] & 0xFFFFFFFF;
	allocated_size = (pkt[2] >> 32) & 0xFFFFFFFF;

	if (*(uint8_t *)MEM_TO_SHADOW((uint64_t) allocated) == 0) {
		__asan_poison_shadow((uint64_t) allocated, 
				allocated_size, 
				kAsanHeapLeftRedzoneMagic);
	}

	uint64_t rz_log = __asan_compute_rz_log(size);
	uint64_t rz_size = __asan_rz_log2size(rz_log);

	uint64_t alloc_beg = allocated;

	uint64_t user_beg = alloc_beg + rz_size;
	if (!__asan_is_aligned(user_beg, alignment))
		user_beg = __asan_round_up_to(user_beg, alignment);

	uint64_t size_rounded_down_to_granularity =
		__asan_round_down_to(size, SHADOW_GRANULARITY);

	if (size_rounded_down_to_granularity)
		__asan_poison_shadow(user_beg, size_rounded_down_to_granularity, 0);

	if (size != size_rounded_down_to_granularity) {
		uint8_t *shadow =
			(uint8_t *)MEM_TO_SHADOW(user_beg + size_rounded_down_to_granularity);
		*shadow = size & (SHADOW_GRANULARITY - 1);
	}

#if ASAN_API_DEBUG
	printf("API: Allocate Long\n");
	printf("\talignment 0x%lx allocated: 0x%lx"
			"size: 0x%x allocated_size: 0x%x\n", 
			alignment,
			allocated, 
			size, 
			allocated_size);
#endif
}

static inline void asan_api_as(const uint64_t pkt[])
{
	uint64_t alignment = 0x8;
	uint64_t allocated;
	uint32_t size;
	uint32_t allocated_size;

	/* parse arguments */
	allocated = pkt[0] >> 16;
	size = pkt[1] & 0xFFFFFFFF;
	allocated_size = (pkt[1] >> 32) & 0xFFFFFFFF; 

	if (*(uint8_t *)MEM_TO_SHADOW((uint64_t) allocated) == 0) {
		__asan_poison_shadow((uint64_t) allocated, 
				allocated_size, 
				kAsanHeapLeftRedzoneMagic);
	}

	uint64_t rz_log = __asan_compute_rz_log(size);
	uint64_t rz_size = __asan_rz_log2size(rz_log);

	uint64_t alloc_beg = allocated;

	uint64_t user_beg = alloc_beg + rz_size;
	if (!__asan_is_aligned(user_beg, alignment))
		user_beg = __asan_round_up_to(user_beg, alignment);

	uint64_t size_rounded_down_to_granularity =
		__asan_round_down_to(size, SHADOW_GRANULARITY);

	if (size_rounded_down_to_granularity)
		__asan_poison_shadow(user_beg, size_rounded_down_to_granularity, 0);

	if (size != size_rounded_down_to_granularity) {
		uint8_t *shadow =
			(uint8_t *)MEM_TO_SHADOW(user_beg + size_rounded_down_to_granularity);
		*shadow = size & (SHADOW_GRANULARITY - 1);
	}

#if ASAN_API_DEBUG
	printf("API: Allocate Short\n");
	printf("\tallocated: 0x%lx size: 0x%x allocated_size: 0x%x\n", 
			allocated, size, allocated_size);
#endif
}

static inline void asan_api_upwv(const uint64_t pkt[])
{
	uint8_t val;
	uint64_t addr;
	uint32_t size;
	uint8_t *beg_chunk, *end_chunk;
	int8_t beg_offset, end_offset;
	int8_t beg_value, end_value;
	uint64_t beg_addr, end_addr;

	/* parse arguments */
	val = pkt[0] >> 8 & 0xff;
	addr = pkt[0] >> 16;
	size = pkt[1];

	/* this is not needed anymore */
	(void)val;

	/* get ranges */
	beg_addr = (uint64_t)addr;
	end_addr = beg_addr + size;

	/* prepare beg shadow vars */
	beg_chunk = (uint8_t*)MEM_TO_SHADOW(beg_addr);
	beg_offset = beg_addr & (SHADOW_GRANULARITY - 1);
	beg_value = *beg_chunk;

	/* prepare end shadow vars */
	end_chunk = (uint8_t*)MEM_TO_SHADOW(end_addr);
	end_offset = end_addr & (SHADOW_GRANULARITY - 1);
	end_value = *end_chunk;

	if (beg_chunk == end_chunk) {
		CHECK_LT(beg_offset, end_offset);
		int8_t value = beg_value;
		CHECK_EQ(value, end_value);
		/* We can only poison memory if the byte in end_offset is unaddressable. */
		/* No need to re-poison memory if it is poisoned already. */
		if (value > 0 && value <= end_offset) {
			if (beg_offset > 0) {
				*beg_chunk = min(value, beg_offset);
			} else {
				*beg_chunk = kAsanUserPoisonedMemoryMagic;
			}
		}
		return;
	}

	CHECK_LT(beg_chunk, end_chunk);
	if (beg_offset > 0) {
		/* Mark bytes from beg_offset as unaddressable. */
		if (beg_value == 0) {
			*beg_chunk = beg_offset;
		} else {
			*beg_chunk = min(beg_value, beg_offset);
		}
		beg_chunk++;
	}

	memset(beg_chunk, kAsanUserPoisonedMemoryMagic, end_chunk - beg_chunk);
	/* Poison if byte in end.offset is unaddressable. */
	if (end_value > 0 && end_value <= end_offset) {
		*end_chunk = kAsanUserPoisonedMemoryMagic;
	}

#if ASAN_API_DEBUG
	printf("API: User Poison With Value\n");
	printf("\tval: 0x%x addr: 0x%lx size: 0x%x\n", 
			val, addr, size);
#endif
}

static inline void asan_api_pwv(const uint64_t pkt[])
{
	uint8_t val;
	uint64_t addr;
	uint32_t size;

	/* parse arguments */
	val = pkt[0] >> 8 & 0xff;
	addr = pkt[0] >> 16;
	size = pkt[1];

	__asan_poison_shadow(addr, size, val);

	uint64_t aligned_size = size & ~(SHADOW_GRANULARITY - 1);

#if ASAN_API_DEBUG
	printf("API: Poison With Value\n");
	printf("\tval: 0x%x addr: 0x%lx size: 0x%x\n", 
			val, addr, size);
#endif

	if (size == aligned_size)
		return;

	signed char end_offset = (signed char)(size - aligned_size);
	signed char* shadow_end = (signed char*)MEM_TO_SHADOW(addr + aligned_size);
	signed char end_value = *shadow_end;

	if (val != 0x0) {
		/* 
		 * If possible, mark all the bytes mapping to last shadow byte as
		 * unaddressable.
		 */
		if (end_value > 0 && end_value <= end_offset)
			*shadow_end = val;
	} else {
		/*
		 * If necessary, mark few first bytes mapping to last shadow byte
		 * as addressable
		 */
		if (end_value != 0)
			*shadow_end = max(end_value, end_offset);
	}
}

static inline void asan_api_pprr(const uint64_t pkt[])
{
	uint8_t val;
	uint64_t addr;
	uint32_t size;
	uint16_t rz_size;

	/* parse arguments */
	val = pkt[0] >> 8 & 0xff;
	addr = pkt[0] >> 16;
	size = pkt[1] & 0xFFFFFFFF;
	rz_size = (pkt[1] >> 32) & 0xFFFFFFFF;

	CHECK(__asan_addr_is_aligned_by_granularity(addr));
	CHECK(__asan_addr_is_in_mem(addr));
	__asan_fast_poison_shadow_partial_right_redzone(addr, 
			size, 
			rz_size, 
			val);

#if ASAN_API_DEBUG
	printf("API: Poison PRR\n");
	printf("\tval: 0x%x addr: 0x%lx size: 0x%x rz_size: 0x%x\n", 
			val, addr, size, rz_size);
#endif
}

static inline void asan_api_pasm(const uint64_t pkt[])
{
	uint8_t poison;
	uint64_t addr;
	uint32_t size;

	/* parse arguments */
	poison = pkt[0] >> 15 & 0x1;
	addr = pkt[0] >> 16;
	size = pkt[1];

	if (size == 0) return;

	uint64_t aligned_size = size & ~(SHADOW_GRANULARITY - 1);

	__asan_poison_shadow(addr, aligned_size,
			poison ? kAsanStackUseAfterScopeMagic : 0);

#if ASAN_API_DEBUG
	printf("API: Poison Aligned Stack Memory\n");
	printf("\tpoison: 0x%x addr: 0x%lx size: 0x%x\n", 
			poison, addr, size);
#endif

	if (size == aligned_size)
		return;

	signed char end_offset = (signed char)(size - aligned_size);
	signed char* shadow_end = (signed char*)MEM_TO_SHADOW(addr + aligned_size);
	signed char end_value = *shadow_end;

	if (poison) {
		/* 
		 * If possible, mark all the bytes mapping to last shadow byte as
		 * unaddressable.
		 */
		if (end_value > 0 && end_value <= end_offset)
			*shadow_end = (signed char)kAsanStackUseAfterScopeMagic;
	} else {
		/*
		 * If necessary, mark few first bytes mapping to last shadow byte
		 * as addressable
		 */
		if (end_value != 0)
			*shadow_end = max(end_value, end_offset);
	}
}

static inline void asan_api_pior(const uint64_t pkt[])
{
	uint8_t poison;
	uint64_t addr;
	uint32_t size;

	/* parse arguments */
	poison = pkt[0] >> 15 & 0x1;
	addr = pkt[0] >> 16;
	size = pkt[1];

	uint64_t end = addr + size;

	CHECK(size);
	CHECK_LE(size, 4096);
	CHECK(__asan_is_aligned(end, SHADOW_GRANULARITY));
	if (!__asan_is_aligned(addr, SHADOW_GRANULARITY)) {
		*(uint8_t *)MEM_TO_SHADOW(addr) =
			poison ? (uint8_t)(addr % SHADOW_GRANULARITY) : 0;
		addr |= SHADOW_GRANULARITY - 1;
		addr++;
	}
	for (; addr < end; addr += SHADOW_GRANULARITY)
		*(uint8_t*)MEM_TO_SHADOW(addr) = poison ? kAsanIntraObjectRedzone : 0;

#if ASAN_API_DEBUG
	printf("API: Poison Inra Object Redzone\n");
	printf("\tpoison: 0x%x addr: 0x%lx size: 0x%x\n", 
			poison, addr, size);
#endif
}

static inline void asan_api_sac(const uint64_t pkt[])
{
	uint64_t beg;
	uint64_t end;
	uint64_t old_mid;
	uint64_t new_mid;
	uint64_t granularity = SHADOW_GRANULARITY;

	/* parse arguments */
	beg = pkt[0] >> 16;
	end = pkt[1] >> 16;
	old_mid = pkt[2] >> 16;
	new_mid = pkt[3] >> 16;

	if (!(beg <= old_mid && beg <= new_mid && 
				old_mid <= end && 
				new_mid <= end &&
				__asan_is_aligned(beg, granularity))) {
		/* contiguous container specialized error msg */
		printf("\n=================================================================\n");
		printf("ERROR: AddressSanitizer: bad parameters to "
				"__sanitizer_annotate_contiguous_container:\n"
				"      beg     : %p\n"
				"      end     : %p\n"
				"      old_mid : %p\n"
				"      new_mid : %p\n",
				(void *)beg, 
				(void *)end, 
				(void *)old_mid, 
				(void *)new_mid);
		printf("\n=================================================================\n");
		exit(0);
	}

	/* sanity check */
	CHECK_LE(end - beg,
			FIRST_32_SECOND_64(1UL << 30, 1ULL << 34));

	uint64_t a = __asan_round_down_to(min(old_mid, new_mid), granularity);
	uint64_t c = __asan_round_up_to(max(old_mid, new_mid), granularity);
	uint64_t d1 = __asan_round_down_to(old_mid, granularity);

	if (a + granularity <= d1)
		CHECK_EQ(*(uint8_t*)MEM_TO_SHADOW(a), 0);

	uint64_t b1 = __asan_round_down_to(new_mid, granularity);
	uint64_t b2 = __asan_round_up_to(new_mid, granularity);

	__asan_poison_shadow(a, b1 - a, 0);
	__asan_poison_shadow(b2, c - b2, kAsanContiguousContainerOOBMagic);

	if (b1 != b2) {
		CHECK_EQ(b2 - b1, granularity);
		*(uint8_t*)MEM_TO_SHADOW(b1) = (uint8_t)(new_mid - b1);
	}

#if ASAN_API_DEBUG
	printf("API: Sanitizer Annotate Contiguous\n");
	printf("\tbeg_p: 0x%lx end_p: 0x%lx old_mid_p: 0x%lx new_mid_p: 0x%lx\n",
			beg, end, old_mid, new_mid);
#endif
}

static inline void asan_api_fu(const uint64_t pkt[])
{
	uint64_t addr;
	uint64_t beg;
	uint64_t end;
	uint32_t size;
	
	/* parse arguments */
	addr = pkt[0] >> 16;
	size = pkt[1];

	/* calculate shadow ranges */
	beg = MEM_TO_SHADOW(addr);
	end = MEM_TO_SHADOW(addr + size);

	uint64_t page_size = ASAN_PAGE_SIZE;
	uint64_t beg_aligned = __asan_round_up_to(beg, page_size);
	uint64_t end_aligned = __asan_round_down_to(end, page_size);
	if (beg_aligned < end_aligned)
		madvise(&beg_aligned, end_aligned - beg_aligned, MADV_DONTNEED);

#if ASAN_API_DEBUG
	printf("API: Flush Unneeded\n");
	printf("\taddr: 0x%lx size: 0x%x\n", 
			addr, size);
#endif
}

static inline void asan_api_cfip(const uint64_t pkt[])
{
	uint64_t a1;
	uint64_t a2;

	/* parse arguments */
	a1 = pkt[0] >> 16;
	a2 = pkt[1] >> 16;

	if (__asan_is_invalid_pointer_pair(a1, a2)) {
		printf("\n=================================================================\n");
		printf("ERROR: AddressSanitizer: invalid-pointer-pair\n");
		printf("\n=================================================================\n");
		exit(0);
	}

#if ASAN_API_DEBUG
	printf("API: Check For Invalid Pointer Pair\n");
	printf("\ta: 0x%lx b: 0x%lx\n", 
			a1, a2);
#endif
}

static inline void asan_api_pse(const uint64_t pkt[], const int pkt_size)
{
	uint64_t addr;
	uint16_t sb_size;
	uint8_t vars, var_poison;
	int i, zone_size;
	int index = 0;
	uint8_t *shadow;

	/* parse arguments */
	addr = pkt[0] >> 16;
	sb_size = pkt[1] & 0xffff;
	vars = pkt[0] >> 8 & 0xff;

	/* calculate shadow address */
	shadow = (uint8_t*)MEM_TO_SHADOW(addr);

	/* create left redzone shadow bytes */
	for (i = 0; i < sb_size / SHADOW_GRANULARITY; i++, index++) {
		shadow[index] = kAsanStackLeftRedzoneMagic;
	}

	/* create mid redzones and var shadow bytes */
	for (i = 2; i < pkt_size; i++) {
		/* only if mid redzone is needed */
		zone_size = (pkt[i] & 0xFFFFFFFF) / SHADOW_GRANULARITY;
		for (; index < zone_size; index++) {
			shadow[index] = kAsanStackMidRedzoneMagic;
		}

		/* calculate rounded up var shadow byte size */
		zone_size = index + ((((pkt[i] >> 32) & 0xffff) + SHADOW_GRANULARITY - 1) / SHADOW_GRANULARITY);

		/* var shadow bytes */
		var_poison = (pkt[i] >> 48) & 0x1;
		for (; index < zone_size; index++) {
			shadow[index] = var_poison ? kAsanStackUseAfterScopeMagic : 0x0;
		}
	}
	
	/* create right redzone */
	for (; index < sb_size; index++) {
		shadow[index] = kAsanStackRightRedzoneMagic;
	}

	(void)vars;

#if ASAN_API_DEBUG
	printf("API: Poison Stack Entry\n");
	printf("\taddr: 0x%lx vars: 0x%x sb_size: 0x%x\n", 
			addr, vars, sb_size); 
	for (i = 2; i < pkt_size; i++) {
		printf("\toffset_%d: 0x%lx length_%d: 0x%lx poison: 0x%lx\n", 
				i - 2, pkt[i] & 0xFFFFFFFF, i - 2, 
				(pkt[i] >> 32) & 0xffff, 
				(pkt[i] >> 48) & 0x1);
	}
#endif
}

static inline void asan_api_pseb(const uint64_t pkt[], int index)
{
	uint64_t addr;
	uint8_t qwords;
	uint16_t sb_size;
	int pkt_index, i;
	uint8_t *shadow;

	/* parse arguments */
	qwords = pkt[0] >> 8 & 0xff;
	sb_size = pkt[2] & 0xffff;
	addr = pkt[0] >> 16;

	/* calculate shadow address */
	shadow = (uint8_t *)MEM_TO_SHADOW(addr);

	/* start at the first magic packet */
	pkt_index = 2;

	/* poison shadow bytes */
	for (i = 0; i < sb_size; i += 8, pkt_index++) {
		shadow[i] = pkt[pkt_index] & 0xff;
		shadow[i + 1] = (pkt[pkt_index] >> 8) & 0xff;
		shadow[i + 2] = (pkt[pkt_index] >> 16) & 0xff;
		shadow[i + 3] = (pkt[pkt_index] >> 24) & 0xff;
		shadow[i + 4] = (pkt[pkt_index] >> 32) & 0xff;
		shadow[i + 5] = (pkt[pkt_index] >> 40) & 0xff;
		shadow[i + 6] = (pkt[pkt_index] >> 48) & 0xff;
		shadow[i + 7] = (pkt[pkt_index] >> 56) & 0xff;
	}

	(void)qwords;

#if ASAN_API_DEBUG
	printf("API: Poison Stack Entry Bytes\n");
	printf("\taddr: 0x%lx magic_qwords: 0x%x sb_size: 0x%x\n", 
			addr, qwords, sb_size);
	for (i = 2; i < index; i++) {
		printf("\tmagic_%d: 0x%lx\n", 
				i - 1, pkt[i]);
	}
#endif
}

#endif
