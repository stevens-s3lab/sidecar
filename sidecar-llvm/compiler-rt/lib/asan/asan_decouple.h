#ifndef ASAN_DECOUPLE_H
#define ASAN_DECOUPLE_H

// S3LAB file
// vim: ts=2:sw=2:expandtab

#include "decouple_device.h"
#include "../../../../coresight_utils/monitor/asan_decouple.h"

namespace __asan {



#define ENABLE_MISC_API
#define ENABLE_CHECKING_API
#define ENABLE_POISONING_API

typedef enum {
  VIOLATION_READ,
  VIOLATION_WRITE,
  VIOLATION_ODR,
  VIOLATION_COPY,
  VIOLATION_CXX_ARRAY_COOKIE,
  VIOLATION_MAX
} vio_t;


#define VIOLATION_NAME_MAX 10
#define VIOLATION_NAME_INITIALIZER "READ", "WRITE", "ODR", "Copy", "CXX"

#ifdef ASAN_PERF_ANALYZE
enum {
  // Intercepted poisoning functions
  FastPoisonShadow_COUNTER = 0,
  PoisonShadow_COUNTER,
  FastPoisonShadowPartialRightRedzone_COUNTER,
  PoisonShadowPartialRightRedzone_COUNTER,
  SetShadow_COUNTER,
  PoisonAlignedStackMemory_COUNTER,
  __asan_poison_memory_region_COUNTER,
  __asan_unpoison_memory_region_COUNTER,
  __sanitizer_annotate_contiguous_container_COUNTER,
  AsanPoisonOrUnpoisonIntraObjectRedzone_COUNTER,
  Allocate_COUNTER,
  // Non-tracked poisoning functions (they use one of the above)
  __asan_alloca_poison_COUNTER,
  __asan_allocas_unpoison_COUNTER,
  __asan_poison_stack_entry_COUNTER,
  __asan_poison_stack_entry_bytes_COUNTER,
  RePoisonChunk_COUNTER,
  RegisterGlobal_COUNTER,
  UnregisterGlobal_COUNTER,
  __asan_register_globals_COUNTER,
  __asan_unregister_globals_COUNTER,
  Deallocate_COUNTER,
  ClearShadowForThreadStackAndTLS_COUNTER,
  ClearShadowMemoryForContextStack_COUNTER,
  Recycle_COUNTER,
  OnMap_COUNTER,
  OnUnmap_COUNTER,
  QuarantineChunk_COUNTER,
  FakeStack__Destroy_COUNTER,
  OnMalloc_COUNTER,
  OnFree_COUNTER,
  // Intercepted checking functions
  __sanitizer_contiguous_container_find_bad_address_COUNTER,
  __asan_address_is_poisoned_COUNTER,
  __sanitizer_verify_contiguous_container_COUNTER,
  CheckODRViolationViaPoisoning_COUNTER,
  ACCESS_MEMORY_RANGE_COUNTER,
  CHECK_SMALL_REGION_COUNTER,
  __sanitizer_ptr_sub_COUNTER,
  __sanitizer_ptr_cmp_COUNTER,
  // Interceptions of checking functions depends on other functions
  AddressIsPoisoned_COUNTER,
  __asan_region_is_poisoned_COUNTER,
  GetStackFrameAccessByAddr_COUNTER,
  __asan_loadN_COUNTER,
  __asan_exp_loadN_COUNTER,
  __asan_storeN_COUNTER,
  __asan_exp_storeN_COUNTER,
  __asan_storeN_noabort_COUNTER,
  MAX_COUNTER,
};

#define FC_COUNTER_NAME_MAXSZ 80
#define FC_COUNTER_NAME_INITIALIZER \
  xstr(FastPoisonShadow),\
  xstr(PoisonShadow),\
  xstr(FastPoisonShadowPartialRightRedzone),\
  xstr(PoisonShadowPartialRightRedzone),\
  xstr(SetShadow),\
  xstr(PoisonAlignedStackMemory),\
  xstr(__asan_poison_memory_region),\
  xstr(__asan_unpoison_memory_region),\
  xstr(__sanitizer_annotate_contiguous_container),\
  xstr(AsanPoisonOrUnpoisonIntraObjectRedzone),\
  xstr(Allocate),\
  \
  xstr(__asan_alloca_poison),\
  xstr(__asan_allocas_unpoison),\
  xstr(__asan_poison_stack_entry),\
  xstr(__asan_poison_stack_entry_bytes),\
  xstr(RePoisonChunk),\
  xstr(RegisterGlobal),\
  xstr(UnregisterGlobal),\
  xstr(__asan_register_globals),\
  xstr(__asan_unregister_globals),\
  xstr(Deallocate),\
  xstr(ClearShadowForThreadStackAndTLS),\
  xstr(ClearShadowMemoryForContextStack),\
  xstr(Recycle),\
  xstr(OnMap),\
  xstr(OnUnmap),\
  xstr(QuarantineChunk),\
  xstr(FakeStack__Destroy),\
  xstr(OnMalloc),\
  xstr(OnFree),\
  \
  xstr(__sanitizer_contiguous_container_find_bad_address),\
  xstr(__asan_address_is_poisoned),\
  xstr(__sanitizer_verify_contiguous_container),\
  xstr(CheckODRViolationViaPoisoning),\
  xstr(ACCESS_MEMORY_RANGE),\
  xstr(CHECK_SMALL_REGION),\
  xstr(__sanitizer_ptr_sub),\
  xstr(__sanitizer_ptr_cmp),\
  \
  xstr(AddressIsPoisoned),\
  xstr(__asan_region_is_poisoned),\
  xstr(GetStackFrameAccessByAddr),\
  xstr(__asan_loadN),\
  xstr(__asan_exp_loadN_),\
  xstr(__asan_storeN),\
  xstr(__asan_exp_storeN),\
  xstr(__asan_storeN_noabort)

#define FUNC_COUNTER_NAME(name) (name ## _COUNTER)
#define FUNC_COUNTER(name) (ffreqc.counter[(FUNC_COUNTER_NAME(name))])
#define FUNC_CALLED(name) do { FUNC_COUNTER(name)++; } while(0)

struct FuncCallCounters {
  uptr counter[MAX_COUNTER];

  void Print();
};
extern struct FuncCallCounters ffreqc;

struct InstCounters {
  uptr inst_op_counter;

  void Print();
};
extern struct InstCounters instc;

// enum VAL_BUCKETS { VAL_ZERO = 0, VAL_OTHER, VAL_MAX };
// #define VAL_BUCKET_NAME_INITIALIZER "0", "other"

//edited by shan
enum VAL_BUCKETS { VAL_ZERO = 0, VAL_F1, VAL_F2, VAL_F3, VAL_F5, VAL_F6, VAL_F7, VAL_F8,\
  VAL_F9, VAL_FF, VAL_FA, VAL_FC, VAL_FD, VAL_FE, VAL_AC, VAL_BB, VAL_CA, VAL_CB, VAL_OTHER, VAL_MAX };
#define VAL_BUCKET_NAME_MAXSZ 10
#define VAL_BUCKET_NAME_INITIALIZER "0", "0xf1", "0xf2", "0xf3", "0xf5", "0xf6", "0xf7", "0xf8", "0xf9", "0xff",\
  "0xfa", "0xfc", "0xfd", "0xfe", "0xac", "0xbb", "0xca", "0xcb", "other"


// enum SZ_BUCKETS { SZ_BYTE = 0, SZ_WORD, SZ_DWORD, SZ_QWORD, SZ_QQWORD, SZ_32,
//  SZ_48, SZ_64, SZ_96, SZ_128, SZ_OTHER, SZ_MAX };
enum SZ_BUCKETS { SZ_BYTE = 0, SZ_WORD, SZ_DWORD, SZ_QWORD, SZ_QQWORD, SZ_32,
  SZ_48, SZ_64, SZ_96, SZ_128, SZ_256, SZ_OTHER, SZ_MAX };

#define SZ_BUCKET_NAME_MAXSZ 17
// #define SZ_BUCKET_NAME_INITIALIZER "1", "2", "4", "8", "16", "32", "48", "64", "96",\
//  "128", "other"
#define SZ_BUCKET_NAME_INITIALIZER "1", "2", "4", "8", "16", "32", "48", "64", "96",\
  "128", "256", "other"
#define SZ_BYTES_MAX 3

struct PoisonStats {
  unsigned long val_bucket[VAL_MAX]; // What poison values are used
  unsigned long sz_bucket[SZ_MAX]; // What region sizes are poisoned
  // How many bytes are needed to store the size
  unsigned long sz_bytes[SZ_BYTES_MAX]; 

  void Print();
};
extern struct PoisonStats pstats;


static inline int StatsGetSizeClass(uptr size)
{
  int sizeclass;

  switch (size) {
  case 1:
    sizeclass = SZ_BYTE;
    break;
  case 2:
    sizeclass = SZ_WORD;
    break;
  case 4:
    sizeclass = SZ_DWORD;
    break;
  case 8:
    sizeclass = SZ_QWORD;
    break;
  case 16:
    sizeclass = SZ_QQWORD;
    break;
  case 32:
    sizeclass = SZ_32;
    break;
  case 48:
    sizeclass = SZ_48;
    break;
  case 64:
    sizeclass = SZ_64;
    break;
  case 96:
    sizeclass = SZ_96;
    break;
  case 128:
    sizeclass = SZ_128;
    break;
  case 256:
    sizeclass = SZ_256;
    break;
  default:
    sizeclass = SZ_OTHER;
    break;
  }
  return sizeclass;
}


static inline int StatsGetSizeBytes(uptr size)
{
  int bytesclass;

  if (size < 256)
  bytesclass = 0;
  else if (size < 65536)
  bytesclass = 1;
  else
  bytesclass = 2;

  return bytesclass;
}


// TODO: This is not thread safe. Needs to be fixed before running on threaded
// code
static inline void PoisonStatsAdd(uptr addr, uptr size, u8 value) {
  int sizeclass = StatsGetSizeClass(size);
  int bytesclass = StatsGetSizeBytes(size);

  int valclass;
  switch (value) {
  case 0:
    valclass = VAL_ZERO;
    break;
  // edited by shan
  case 0xf1:
    valclass = VAL_F1;
    break;
  case 0xf2:
    valclass = VAL_F2;
    break;
  case 0xf3:
    valclass = VAL_F3;
    break;
  case 0xf5:
    valclass = VAL_F5;
    break;
  case 0xf6:
  valclass = VAL_F6;
  break;
  case 0xf7:
  valclass = VAL_F7;
  break;
  case 0xf8:
    valclass = VAL_F8;
    break;
  case 0xf9:
    valclass = VAL_F9;
    break;
  case 0xff:
    valclass = VAL_FF;
    break;
  case 0xfa:
    valclass = VAL_FA;
    break;
  case 0xfc:
    valclass = VAL_FC;
    break;
  case 0xfd:
    valclass = VAL_FD;
    break;
  case 0xfe:
    valclass = VAL_FE;
    break;
  case 0xac:
    valclass = VAL_AC;
    break;
  case 0xbb:
    valclass = VAL_BB;
    break;
  case 0xca:
    valclass = VAL_CA;
    break;
  case 0xcb:
    valclass = VAL_CB;
    break;       
  default:
    valclass = VAL_OTHER;
    break;
  }

  pstats.val_bucket[valclass]++;
  pstats.sz_bucket[sizeclass]++;
  pstats.sz_bytes[bytesclass]++;
}


struct CheckStats {
  unsigned long v_bucket[VIOLATION_MAX]; // What violations are checked
  unsigned long sz_bucket[SZ_MAX]; // What region sizes are checked
  // How many bytes are needed to store the size
  unsigned long sz_bytes[SZ_BYTES_MAX];

  void Print();
};
extern struct CheckStats cstats;

struct AllocateStats {
  unsigned long sz_bucket[SZ_MAX]; // the size will be allocated on heap
  // How many bytes are needed to store the size
  unsigned long sz_bytes[SZ_BYTES_MAX];
  void Print();
};
extern struct AllocateStats astats;

// TODO: This is not thread safe. Needs to be fixed before running on threaded
// code
static inline void CheckStatsAdd(uptr addr, uptr size, vio_t violation) 
{
  int sizeclass = StatsGetSizeClass(size);
  int bytesclass = StatsGetSizeBytes(size);

  cstats.v_bucket[violation]++;
  cstats.sz_bucket[sizeclass]++;
  cstats.sz_bytes[bytesclass]++;
}
// added by Shan: allocate stats checking
// code
static inline void AllocateStatsAdd(uptr allocated_size)
{
  int sizeclass = StatsGetSizeClass(allocated_size);
  int bytesclass = StatsGetSizeBytes(allocated_size);
  
  astats.sz_bucket[sizeclass]++;
  astats.sz_bytes[bytesclass]++; 
}

#else
#define FUNC_CALLED(name) do { } while(0)
#define FUNC_COUNTER(name) 0
#define PoisonStatsAdd(a, s, v) do { } while (0)
#define CheckStatsAdd(a, s, v) do { } while (0)
#define AllocateStatsAdd(s) do { } while (0)
#endif // ASAN_PERF_ANALYZE


# if __LP64__
#  define STIM_OFFSET  0x7fff84a00000
# else // LP32
#  define STIM_OFFSET  0x7FF2a000
# endif // !USE_PTWRITE
#  define D64_PORT     (STIM_OFFSET+0x18)


#ifdef NOMESSAGES
# define msg_write(data) do { } while (0)
# define msg_write32(data) do { } while (0)
#else // NOMESSAGES

#ifdef USE_PTWRITE

static inline void msg_write(u64 data)
{
#if defined(__x86_64__)
  asm volatile ("ptwrite %0\n" : : "r" (data));
#else
  Printf("PTWRITE attempted but no x86_64!\n");
  Die();
#endif
}

static inline void msg_write32(u32 data)
{
#if defined(__x86_64__)
  asm volatile ("ptwritel %0\n" : : "r" (data));
#else
  Printf("PTWRITE attempted but no x86_64!\n");
  Die();
#endif
}

#else // USE_PTWRITE
 
static inline void msg_write(u64 data)
{
  *(volatile u64 *)D64_PORT = data;
}

static inline void msg_write32(u32 data)
{
  *(volatile u32 *)D64_PORT = data;
}
#endif // !USE_PTWRITE

#endif // !NOMESSAGES



/************ Poisoning/Unpoisoning APIs ******************/

#ifdef ENABLE_POISONING_API
#define MONITOR_POISON_BODY(addr, size, value, opcode)        \
    u64 data;                                                 \
    PoisonStatsAdd(addr, size, value);                        \
    /*                                                        \
     * prepare first 64bit packet                             \
     * | opcode(8) | val(8) | addr(48) |                      \
     */                                                       \
    data = (u64) addr << 16 | value << 8 | opcode;            \
    /* write 1 64bit and 1 32bit stm packets */               \
    msg_write(data);                                          \
    msg_write32((u32)size);

ALWAYS_INLINE void MonitorPoisonShadow(uptr addr, uptr size, u8 value) 
{
  MONITOR_POISON_BODY(addr, size, value, POISON_WITH_VALUE)
}

/* TODO: Do we want to support this? */
ALWAYS_INLINE void MonitorUserPoisonShadow(uptr addr, uptr size) 
{
  MONITOR_POISON_BODY(addr, size, 0xf7, USER_POISON_WITH_VALUE)
}

ALWAYS_INLINE void MonitorUserUnpoisonShadow(uptr addr, uptr size) 
{
  MONITOR_POISON_BODY(addr, size, 0x00, USER_POISON_WITH_VALUE)
}

ALWAYS_INLINE void MonitorPoisonShadowPRR(uptr addr, uptr size, uptr redzone_size,
    u8 value) 
{
  u64 data;

  PoisonStatsAdd(addr, size, value);

  /*
   * prepare and send first 64bit packet
   * | opcode(8) | val(8) | addr(48) |
   */
  data = (u64)addr << 16 | (u64)value << 8 | POISON_PRR;
  msg_write(data);

  /*
   * prepare and send second 64bit packet
   * | size(32) | rz_size(32) |
   */
  data = (u64)redzone_size << 32 | (u32)size;
  msg_write(data);
}

ALWAYS_INLINE void MonitorPoisonAlignedStackMemory(uptr addr, uptr size, bool poison) 
{
  unsigned long data;

  PoisonStatsAdd(addr, size, (poison)? kAsanStackUseAfterScopeMagic : 0);

  /*
   * prepare and send first packet
   * | opcode(8) | unused(7) | poison(1) | addr(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) addr << 16 | poison << 15 | POISON_ALIGNED_STACK_MEM;
  msg_write(data);

  /* send secod packet */
  msg_write32((u32)size);
}

ALWAYS_INLINE void MonitorPoisonOrUnpoisonIntraObjectRedzone(uptr addr,
  uptr size, bool poison) 
{
  unsigned long data;

  PoisonStatsAdd(addr, size, (poison)? kAsanIntraObjectRedzone : 0);

  /*
   * prepare and send first packet
   * | opcode(8) | unused(7) | poison(1) | addr(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) addr << 16 | poison << 15 | POISON_INTRA_OBJ_REDZONE;
  msg_write(data);

  /* send secod packet */
  msg_write32((u32)size);
}

ALWAYS_INLINE void MonitorSanitizerAnnotateContiguousContainer(
    const void *beg_p, const void *end_p, const void *old_mid_p,
    const void *new_mid_p) 
{
  unsigned long data;

  /*
   * prepare and send first packet
   * | opcode(8) | unused(8) | beg_p(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) beg_p << 16 | SANITIZER_ANNOTATE_CONTIGUOUS;
  msg_write(data);

  /*
   * prepare and send second packet
   * | unused(16) | end_p(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) end_p << 16;
  msg_write(data);

  /*
   * prepare and send third packet
   * | unused(16) | old_mid_p(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) old_mid_p << 16;
  msg_write(data);

  /*
   * prepare and send fourth packet
   * | unused(16) | new_mid_p(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) new_mid_p << 16;
  msg_write(data);
}
#else // ENABLE_POISONING_API
#define MonitorSanitizerAnnotateContiguousContainer(a, b, c, d) do { } while (0)
#define MonitorPoisonShadowPRR(a, b, c, d) do { } while (0)
#define MonitorPoisonOrUnpoisonIntraObjectRedzone(a, b, c) do { } while (0)
#define MonitorPoisonAlignedStackMemory(a, b, c) do { } while (0)
#define MonitorPoisonShadow(a, b, c) do { } while (0)
#define MonitorUserPoisonShadow(a, b) do { } while (0)
#define MonitorUserUnpoisonShadow(a, b) do { } while (0)
#endif // !ENABLE_POISONING_API


/************ Checking APIs ******************/

#ifdef ENABLE_CHECKING_API
ALWAYS_INLINE void MonitorCheckSmallRegion(uptr addr, uptr size, bool is_write)
{
  unsigned long data;
  bool fatal;
  u8 small_size = 0x0;

  small_size |= size & 0x3f;

  CheckStatsAdd(addr, size, (is_write)? VIOLATION_WRITE : VIOLATION_READ);

  fatal = flags()->halt_on_error;

  /*
   * prepare packet
   * | opcode(8) | size(7) | isWrite (1) | addr(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) addr << 16 | is_write << 15 | fatal << 14 | small_size << 8 | CHECK_SMALL_REGION;

  /* write packet to stm */
  msg_write(data);
}

ALWAYS_INLINE void MonitorCheckLargeRegion(uptr addr, uptr size, bool is_write) 
{
  u64 data;
  bool fatal;

  CheckStatsAdd(addr, size, (is_write)? VIOLATION_WRITE : VIOLATION_READ);

  fatal = flags()->halt_on_error;

  /*
   * prepare packet
   * | opcode(8) | unused(6) | fatal(1) | isWrite(1) | addr(48) | size(32) |
   * store opcode in LS byte
   */
  data = addr << 16 | is_write << 15 | fatal << 14 | CHECK_LARGE_REGION;

  /* write packet to stm */
  msg_write(data);
  msg_write32((u32)size);
}

ALWAYS_INLINE void MonitorCheckODR(uptr addr, uptr size) 
{
  unsigned long data;

  CheckStatsAdd(addr, size, VIOLATION_ODR);

  /*
   * prepare packet
   * | opcode(8) | unused(8) | addr(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) addr << 16 | CHECK_ODR;

  /* write packet to stm */
  msg_write(data);
  msg_write32((u32)size);
}

ALWAYS_INLINE void MonitorCheckRegionCopy(uptr from, uptr to, uptr size) 
{
  unsigned long data;
  bool fatal;

  CheckStatsAdd(-1, size, VIOLATION_COPY);

  fatal = flags()->halt_on_error;

  /*
   * prepare and send first 64bit packet
   * | opcode(8) | unused(7) | fata(1) | from(48) |
   * store opcode in LS byte
   */
  data = (unsigned long) from << 16 | fatal << 15 | CHECK_REGION_COPY;
  msg_write(data);

  /*
   * prepare and send second 64bit packet
   * | unused(16) | to(48) |
   */
  data = (unsigned long) to << 16;
  msg_write(data);

  /* write 32bit packet */
  msg_write32((u32)size);
}

ALWAYS_INLINE void MonitorCheckLargeRegionExp(uptr addr, uptr size, u32 exp, bool is_write) 
{
  u64 data;
  bool fatal;

  CheckStatsAdd(addr, size, (is_write)? VIOLATION_WRITE : VIOLATION_READ);

  fatal = flags()->halt_on_error;

  /*
   * prepare packet
   * | opcode(8) | unused(6) | fatal(1) | isWrite(1) | addr(48) | size(32) |
   * store opcode in LS byte
   */
  data = addr << 16 | is_write << 15 | fatal << 14 | CHECK_LARGE_REGION_EXP;
  msg_write(data);

  data = (u64)exp << 32 | (u32)size;
  msg_write(data);
}


#define MONITOR_CHECK_DEFINE(size) \
ALWAYS_INLINE void MonitorCheckAccess ## size(uptr addr, bool isWrite, bool fatal)    \
{                                                                                 \
  unsigned long data;               \
  CheckStatsAdd(addr, size, (isWrite)? VIOLATION_WRITE : VIOLATION_READ);   \
  data = (unsigned long) addr << 16 | isWrite << 15 | fatal << 14 |     \
                CHECK_ACCESS_##size;  \
  msg_write(data);                \
}                                                                                     \
ALWAYS_INLINE void MonitorCheckAccessExp ## size(uptr addr, bool isWrite, u32 exp, bool fatal)\
{                                                                                     \
  unsigned long data;               \
  CheckStatsAdd(addr, size, (isWrite)? VIOLATION_WRITE : VIOLATION_READ);       \
  data = (unsigned long) addr << 16 | isWrite << 15 | fatal << 14 | CHECK_ACCESS_EXP_##size;\
  msg_write(data);                \
  msg_write32(exp);               \
}

ALWAYS_INLINE void MonitorCheckCXXArrayCookie(uptr addr) 
{
  unsigned long data;

  CheckStatsAdd(addr, sizeof(uptr), VIOLATION_CXX_ARRAY_COOKIE);

  /*
   * prepare 64bit packet
   * | opcode(8) | unused(8) | addr(48) |
   */
  data = (unsigned long) addr << 16 | CHECK_CXX_ARRAY_COOKIE;

  /* write 32bit packet to stm */
  msg_write(data);
}
#else // ENABLE_CHECKING_API
#define MONITOR_CHECK_DEFINE(size) \
ALWAYS_INLINE void MonitorCheckAccess ## size(uptr addr, bool isWrite, bool fatal) { }\
ALWAYS_INLINE void MonitorCheckAccessExp ## size(uptr addr, bool isWrite, u32 exp, bool fatal) { }
#define MonitorCheckCXXArrayCookie(addr) do { } while (0)
#define MonitorCheckLargeRegionExp(addr, size, exp, is_write) do { } while (0)
#define MonitorCheckRegionCopy(a, b, c) do { } while (0)
#define MonitorCheckODR(a, b) do { } while (0)
#define MonitorCheckLargeRegion(a, b, c) do { } while (0)
#define MonitorCheckSmallRegion(a, b, c) do { } while (0)
#endif // !ENABLE_CHECKING_API

MONITOR_CHECK_DEFINE(1)
MONITOR_CHECK_DEFINE(2)
MONITOR_CHECK_DEFINE(4)
MONITOR_CHECK_DEFINE(8)
MONITOR_CHECK_DEFINE(16)


/************ Miscellaneous APIs ******************/

#ifdef ENABLE_MISC_API
ALWAYS_INLINE void MonitorAllocate(uptr size, uptr alignment, uptr allocated,
    uptr allocated_size) 
{
  u64 data;

  AllocateStatsAdd(allocated_size);

  if (LIKELY(alignment == 0x8)) { /* align by 8 */
    /*
     * prepare and send first 64bit packet
     * | opcode(8) | unused(8) | allocated(48) |
     */
    data = (u64)allocated << 16 | ALLOCATE_SHORT;
    msg_write(data);

    /*
     * prepare and send second 64bit packet
     * | size(32) | allocated_size(32) |
     */
    data = (u64)allocated_size << 32 | (u32)size;
    msg_write(data);
  } else { /* long alignment */
    /*
     * prepare and send first 64bit packet
     * | opcode(8) | unused(8) | alignment(48) |
     */
    data = (u64)alignment << 16 | ALLOCATE_LONG;
    msg_write(data);

    /*
     * prepare and send second 64bit packet
     * | unused(16) | allocated(48) |
     */
    data = (u64)allocated << 16;
    msg_write(data);

    /*
     * prepare and send third 64bit packet
     * | size(32) | allocated_size(32) |
     */
    data = (u64)allocated_size << 32 | (u32)size;
    msg_write(data);
  }
}

ALWAYS_INLINE void MonitorFlushUnneededShadow(uptr addr, uptr size) 
{
  unsigned long data;

  /*
   * prepare first 64bit packet
   * | opcode(8) | unused(8) | addr(48) |
   */
  data = (unsigned long) addr << 16 | FLUSH_UNNEEDED;

  /* write 1 64bit and 1 32bit packets to stm */
  msg_write(data);
  msg_write32((u32)size);
}

ALWAYS_INLINE void MonitorCheckForInvalidPointerPair(uptr a, uptr b) 
{
  unsigned long data;

  /*
   * prepare and send first 64bit packet
   * | opcode(8) | unused(8) | a(48) |
   */
  data = (unsigned long) a << 16 | CHECK_FOR_INVALID_PAIR;
  msg_write(data);

  /*
   * prepare and send second 64bit packet
   * | unused(16) | b(48) |
   */
  data = (unsigned long) b << 16;
  msg_write(data);
}
#else // ENABLE_MISC_API
#define MonitorAllocate(size, alignment, allocated, allocated_size) do { } while (0)
#define MonitorFlushUnneededShadow(addr, size) do { } while (0)
#define MonitorCheckForInvalidPointerPair(a, b) do { } while (0)
#endif // !ENABLE_MISC_API

#define FUNC_DISABLED() \
  do {\
    Report("%s() no longer callable: %s:%d\n", __FUNCTION__, __FILE__, __LINE__);\
    Die();\
  } while (0)

} // namespace __asan

#endif
