//===-- asan_stats.cpp ----------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is a part of AddressSanitizer, an address sanity checker.
//
// Code related to statistics collected by AddressSanitizer.
//===----------------------------------------------------------------------===//
#include "asan_interceptors.h"
#include "asan_internal.h"
#include "asan_stats.h"
#include "asan_thread.h"
#include "sanitizer_common/sanitizer_allocator_interface.h"
#include "sanitizer_common/sanitizer_mutex.h"
#include "sanitizer_common/sanitizer_stackdepot.h"

namespace __asan {

// S3LAB
#ifdef ASAN_PERF_ANALYZE

struct FuncCallCounters ffreqc;
struct InstCounters instc;

#define xstr(s) str(s)
#define str(s) #s

static const char fc_counter_name[MAX_COUNTER][80] = {
	FC_COUNTER_NAME_INITIALIZER
};

void FuncCallCounters::Print(void)
{
  Printf("Stats: Function call frequency:\n");
  for (int i = 0; i < MAX_COUNTER;i++) {
    Printf("  %-60s %10zu\n", fc_counter_name[i], this->counter[i]);
  }
}

void InstCounters::Print(void)
{
  Printf("Stats: Instrumentation counters:\n");
  Printf("\tinstrumented operands: %10zu\n", this->inst_op_counter);
}

#ifdef ASAN_DECOUPLE
struct PoisonStats pstats;
struct CheckStats cstats;
// added by shan
struct AllocateStats astats;

static const char ps_val_name[VAL_MAX][VAL_BUCKET_NAME_MAXSZ] = {
	VAL_BUCKET_NAME_INITIALIZER
};

static const char ps_sz_name[SZ_MAX][SZ_BUCKET_NAME_MAXSZ] = {
	SZ_BUCKET_NAME_INITIALIZER
};

static const char violation_name[VIOLATION_MAX][VIOLATION_NAME_MAX] = {
	VIOLATION_NAME_INITIALIZER
};

#define PRINT_SIZES(bucket) \
  do {\
    Printf("\tSizes:\n");\
    for (int i = 0; i < SZ_MAX; i++) {\
	    Printf("\t\t%-10s %10zu\n", ps_sz_name[i], bucket[i]);\
    }\
  } while (0)

#define PRINT_BYTES(bytes) \
  do {\
    Printf("\tSize bytes:\n");\
    Printf("\t\t%zu%-9s %10zu\n", 1, " ", bytes[0]);\
    Printf("\t\t%zu%-9s %10zu\n", 2, " ", bytes[1]);\
    Printf("\t\t%zu%-9s %10zu\n", 4, " ", bytes[2]);\
  } while (0)

void PoisonStats::Print(void)
{
  Printf("Stats: Poison Stats:\n");
  Printf("\tValues:\n");
  for (int i = 0; i < VAL_MAX; i++) {
	  Printf("\t\t%-10s %10zu\n", ps_val_name[i], pstats.val_bucket[i]);
  }
  PRINT_SIZES(pstats.sz_bucket);
  PRINT_BYTES(pstats.sz_bytes);
}


void CheckStats::Print(void)
{
  Printf("Stats: Check Stats:\n");
  Printf("\tViolations:\n");
  for (int i = 0; i < VIOLATION_MAX; i++) {
	  Printf("\t\t%-10s %10zu\n", violation_name[i], cstats.v_bucket[i]);
  }
  PRINT_SIZES(cstats.sz_bucket);
  PRINT_BYTES(cstats.sz_bytes);
}
void AllocateStats::Print(void){
  Printf("Stats: Allocate Stats:\n");
  Printf("\tAllocated Size:\n");
  PRINT_SIZES(astats.sz_bucket);
  PRINT_BYTES(astats.sz_bytes);
}
#endif // ASAN_DECOUPLE

#endif // ASAN_PERF_ANALYZE

AsanStats::AsanStats() {
  Clear();
}

void AsanStats::Clear() {
  CHECK(REAL(memset));
  REAL(memset)(this, 0, sizeof(AsanStats));
}

static void PrintMallocStatsArray(const char *prefix,
                                  uptr (&array)[kNumberOfSizeClasses]) {
  Printf("%s", prefix);
  for (uptr i = 0; i < kNumberOfSizeClasses; i++) {
    if (!array[i]) continue;
    Printf("%zu:%zu; ", i, array[i]);
  }
  Printf("\n");
}

void AsanStats::Print() {
  Printf("Stats: %zuM malloced (%zuM for red zones) by %zu calls\n",
             malloced>>20, malloced_redzones>>20, mallocs);
  Printf("Stats: %zuM realloced by %zu calls\n", realloced>>20, reallocs);
  Printf("Stats: %zuM freed by %zu calls\n", freed>>20, frees);
  Printf("Stats: %zuM really freed by %zu calls\n",
             really_freed>>20, real_frees);
  Printf("Stats: %zuM (%zuM-%zuM) mmaped; %zu maps, %zu unmaps\n",
             (mmaped-munmaped)>>20, mmaped>>20, munmaped>>20,
             mmaps, munmaps);

  PrintMallocStatsArray("  mallocs by size class: ", malloced_by_size);
  Printf("Stats: malloc large: %zu\n", malloc_large);
}

void AsanStats::MergeFrom(const AsanStats *stats) {
  uptr *dst_ptr = reinterpret_cast<uptr*>(this);
  const uptr *src_ptr = reinterpret_cast<const uptr*>(stats);
  uptr num_fields = sizeof(*this) / sizeof(uptr);
  for (uptr i = 0; i < num_fields; i++)
    dst_ptr[i] += src_ptr[i];
}

static BlockingMutex print_lock(LINKER_INITIALIZED);

static AsanStats unknown_thread_stats(LINKER_INITIALIZED);
static AsanStats dead_threads_stats(LINKER_INITIALIZED);
static BlockingMutex dead_threads_stats_lock(LINKER_INITIALIZED);
// Required for malloc_zone_statistics() on OS X. This can't be stored in
// per-thread AsanStats.
static uptr max_malloced_memory;

static void MergeThreadStats(ThreadContextBase *tctx_base, void *arg) {
  AsanStats *accumulated_stats = reinterpret_cast<AsanStats*>(arg);
  AsanThreadContext *tctx = static_cast<AsanThreadContext*>(tctx_base);
  if (AsanThread *t = tctx->thread)
    accumulated_stats->MergeFrom(&t->stats());
}

static void GetAccumulatedStats(AsanStats *stats) {
  stats->Clear();
  {
    ThreadRegistryLock l(&asanThreadRegistry());
    asanThreadRegistry()
        .RunCallbackForEachThreadLocked(MergeThreadStats, stats);
  }
  stats->MergeFrom(&unknown_thread_stats);
  {
    BlockingMutexLock lock(&dead_threads_stats_lock);
    stats->MergeFrom(&dead_threads_stats);
  }
  // This is not very accurate: we may miss allocation peaks that happen
  // between two updates of accumulated_stats_. For more accurate bookkeeping
  // the maximum should be updated on every malloc(), which is unacceptable.
  if (max_malloced_memory < stats->malloced) {
    max_malloced_memory = stats->malloced;
  }
}

void FlushToDeadThreadStats(AsanStats *stats) {
  BlockingMutexLock lock(&dead_threads_stats_lock);
  dead_threads_stats.MergeFrom(stats);
  stats->Clear();
}

void FillMallocStatistics(AsanMallocStats *malloc_stats) {
  AsanStats stats;
  GetAccumulatedStats(&stats);
  malloc_stats->blocks_in_use = stats.mallocs;
  malloc_stats->size_in_use = stats.malloced;
  malloc_stats->max_size_in_use = max_malloced_memory;
  malloc_stats->size_allocated = stats.mmaped;
}

AsanStats &GetCurrentThreadStats() {
  AsanThread *t = GetCurrentThread();
  return (t) ? t->stats() : unknown_thread_stats;
}

static void PrintAccumulatedStats() {
  AsanStats stats;
  GetAccumulatedStats(&stats);
  // Use lock to keep reports from mixing up.
  BlockingMutexLock lock(&print_lock);
  stats.Print();
  StackDepotStats *stack_depot_stats = StackDepotGetStats();
  Printf("Stats: StackDepot: %zd ids; %zdM allocated\n",
         stack_depot_stats->n_uniq_ids, stack_depot_stats->allocated >> 20);
  PrintInternalAllocatorStats();
#ifdef ASAN_PERF_ANALYZE
  ffreqc.Print();
  instc.Print();
# ifdef ASAN_DECOUPLE
  pstats.Print();
  cstats.Print();
  //added by shan
  astats.Print();
# endif
#endif
}

}  // namespace __asan

// ---------------------- Interface ---------------- {{{1
using namespace __asan;

uptr __sanitizer_get_current_allocated_bytes() {
  AsanStats stats;
  GetAccumulatedStats(&stats);
  uptr malloced = stats.malloced;
  uptr freed = stats.freed;
  // Return sane value if malloced < freed due to racy
  // way we update accumulated stats.
  return (malloced > freed) ? malloced - freed : 1;
}

uptr __sanitizer_get_heap_size() {
  AsanStats stats;
  GetAccumulatedStats(&stats);
  return stats.mmaped - stats.munmaped;
}

uptr __sanitizer_get_free_bytes() {
  AsanStats stats;
  GetAccumulatedStats(&stats);
  uptr total_free = stats.mmaped
                  - stats.munmaped
                  + stats.really_freed;
  uptr total_used = stats.malloced
                  + stats.malloced_redzones;
  // Return sane value if total_free < total_used due to racy
  // way we update accumulated stats.
  return (total_free > total_used) ? total_free - total_used : 1;
}

uptr __sanitizer_get_unmapped_bytes() {
  return 0;
}

void __asan_print_accumulated_stats() {
  PrintAccumulatedStats();
}
