//===-- asan_interceptors_memintrinsics.h -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//
//
// This file is a part of AddressSanitizer, an address sanity checker.
//
// ASan-private header for asan_interceptors_memintrinsics.cpp
//===---------------------------------------------------------------------===//
#ifndef ASAN_MEMINTRIN_H
#define ASAN_MEMINTRIN_H

#include "asan_interface_internal.h"
#include "asan_internal.h"
#include "asan_mapping.h"
#include "interception/interception.h"

DECLARE_REAL(void*, memcpy, void *to, const void *from, uptr size)
DECLARE_REAL(void*, memset, void *block, int c, uptr size)

namespace __asan {

// S3LAB
#ifndef ASAN_DECOUPLE
// Return true if we can quickly decide that the region is unpoisoned.
// We assume that a redzone is at least 16 bytes.
static inline bool QuickCheckForUnpoisonedRegion(uptr beg, uptr size) {
  if (size == 0) return true;
  if (size <= 32)
    return !AddressIsPoisoned(beg) &&
           !AddressIsPoisoned(beg + size - 1) &&
           !AddressIsPoisoned(beg + size / 2);
  if (size <= 64)
    return !AddressIsPoisoned(beg) &&
           !AddressIsPoisoned(beg + size / 4) &&
           !AddressIsPoisoned(beg + size - 1) &&
           !AddressIsPoisoned(beg + 3 * size / 4) &&
           !AddressIsPoisoned(beg + size / 2);
  return false;
}
#endif //!ASAN_DECOUPLE

struct AsanInterceptorContext {
  const char *interceptor_name;
};

// S3LAB
#ifdef ASAN_DECOUPLE

// memcpy is called during __asan_init() from the internals of printf(...).
// We do not treat memcpy with to==from as a bug.
// See http://llvm.org/bugs/show_bug.cgi?id=11763.
#define ASAN_MEMCPY_IMPL(ctx, to, from, size)                           \
  do {                                                                  \
    if (UNLIKELY(!asan_inited)) return internal_memcpy(to, from, size); \
    if (asan_init_is_running) {                                         \
      return REAL(memcpy)(to, from, size);                              \
    }                                                                   \
    ENSURE_ASAN_INITED();                                               \
    if (flags()->replace_intrin) {                                      \
      if (to != from) {                                                 \
        CHECK_RANGES_OVERLAP("memcpy", to, size, from, size);           \
      }                                                                 \
      MonitorCheckRegionCopy((uptr)from, (uptr)to, (uptr)size);		\
    }                                                                   \
    return REAL(memcpy)(to, from, size);                                \
  } while (0)

#define ASAN_MEMMOVE_IMPL(ctx, to, from, size)                           \
  do {                                                                   \
    if (UNLIKELY(!asan_inited)) return internal_memmove(to, from, size); \
    ENSURE_ASAN_INITED();                                                \
    if (flags()->replace_intrin) {                                       \
      MonitorCheckRegionCopy((uptr)from, (uptr)to, (uptr)size);          \
    }                                                                    \
    return internal_memmove(to, from, size);                             \
  } while (0)

#define ASAN_READ_RANGE(ctx, offset, size) \
  MonitorCheckLargeRegion((uptr)offset, (uptr)size, false)
#define ASAN_WRITE_RANGE(ctx, offset, size) \
  MonitorCheckLargeRegion((uptr)offset, (uptr)size, true)

#else // ! ASAN_DECOUPLE

// We implement ACCESS_MEMORY_RANGE, ASAN_READ_RANGE,
// and ASAN_WRITE_RANGE as macro instead of function so
// that no extra frames are created, and stack trace contains
// relevant information only.
// We check all shadow bytes.
#define ACCESS_MEMORY_RANGE(ctx, offset, size, isWrite) do {            \
    uptr __offset = (uptr)(offset);                                     \
    uptr __size = (uptr)(size);                                         \
    uptr __bad = 0;                                                     \
    /* S3LAB */                                                         \
    FUNC_CALLED(ACCESS_MEMORY_RANGE);                                   \
    if (__offset > __offset + __size) {                                 \
      GET_STACK_TRACE_FATAL_HERE;                                       \
      ReportStringFunctionSizeOverflow(__offset, __size, &stack);       \
    }                                                                   \
    if (!QuickCheckForUnpoisonedRegion(__offset, __size) &&             \
        (__bad = __asan_region_is_poisoned(__offset, __size))) {        \
      AsanInterceptorContext *_ctx = (AsanInterceptorContext *)ctx;     \
      bool suppressed = false;                                          \
      if (_ctx) {                                                       \
        suppressed = IsInterceptorSuppressed(_ctx->interceptor_name);   \
        if (!suppressed && HaveStackTraceBasedSuppressions()) {         \
          GET_STACK_TRACE_FATAL_HERE;                                   \
          suppressed = IsStackTraceSuppressed(&stack);                  \
        }                                                               \
      }                                                                 \
      if (!suppressed) {                                                \
        GET_CURRENT_PC_BP_SP;                                           \
        ReportGenericError(pc, bp, sp, __bad, isWrite, __size, 0, false);\
      }                                                                 \
    }                                                                   \
  } while (0)

// memcpy is called during __asan_init() from the internals of printf(...).
// We do not treat memcpy with to==from as a bug.
// See http://llvm.org/bugs/show_bug.cgi?id=11763.
#define ASAN_MEMCPY_IMPL(ctx, to, from, size)                           \
  do {                                                                  \
    if (UNLIKELY(!asan_inited)) return internal_memcpy(to, from, size); \
    if (asan_init_is_running) {                                         \
      return REAL(memcpy)(to, from, size);                              \
    }                                                                   \
    ENSURE_ASAN_INITED();                                               \
    if (flags()->replace_intrin) {                                      \
      if (to != from) {                                                 \
        CHECK_RANGES_OVERLAP("memcpy", to, size, from, size);           \
      }                                                                 \
      ASAN_READ_RANGE(ctx, from, size);                                 \
      ASAN_WRITE_RANGE(ctx, to, size);                                  \
    }                                                                   \
    return REAL(memcpy)(to, from, size);                                \
  } while (0)

#define ASAN_MEMMOVE_IMPL(ctx, to, from, size)                           \
  do {                                                                   \
    if (UNLIKELY(!asan_inited)) return internal_memmove(to, from, size); \
    ENSURE_ASAN_INITED();                                                \
    if (flags()->replace_intrin) {                                       \
      ASAN_READ_RANGE(ctx, from, size);                                  \
      ASAN_WRITE_RANGE(ctx, to, size);                                   \
    }                                                                    \
    return internal_memmove(to, from, size);                             \
  } while (0)

#define ASAN_READ_RANGE(ctx, offset, size) \
  ACCESS_MEMORY_RANGE(ctx, offset, size, false)
#define ASAN_WRITE_RANGE(ctx, offset, size) \
  ACCESS_MEMORY_RANGE(ctx, offset, size, true)

#endif // ! ASAN_DECOUPLE


// memset is called inside Printf.
#define ASAN_MEMSET_IMPL(ctx, block, c, size)                           \
  do {                                                                  \
    if (UNLIKELY(!asan_inited)) return internal_memset(block, c, size); \
    if (asan_init_is_running) {                                         \
      return REAL(memset)(block, c, size);                              \
    }                                                                   \
    ENSURE_ASAN_INITED();                                               \
    if (flags()->replace_intrin) {                                      \
      ASAN_WRITE_RANGE(ctx, block, size);                               \
    }                                                                   \
    return REAL(memset)(block, c, size);                                \
  } while (0)


// Behavior of functions like "memcpy" or "strcpy" is undefined
// if memory intervals overlap. We report error in this case.
// Macro is used to avoid creation of new frames.
static inline bool RangesOverlap(const char *offset1, uptr length1,
                                 const char *offset2, uptr length2) {
  return !((offset1 + length1 <= offset2) || (offset2 + length2 <= offset1));
}

#ifdef ASAN_DECOUPLE

#define CHECK_RANGES_OVERLAP(name, _offset1, length1, _offset2, length2)   \
  do {                                                                     \
    const char *offset1 = (const char *)_offset1;                          \
    const char *offset2 = (const char *)_offset2;                          \
    if (RangesOverlap(offset1, length1, offset2, length2)) {               \
        ReportStringFunctionMemoryRangesOverlap(name, offset1, length1,    \
                                                offset2, length2, 0); 	   \
    }                                                                      \
  } while (0)

#else // ! ASAN_DECOUPLE

#define CHECK_RANGES_OVERLAP(name, _offset1, length1, _offset2, length2)   \
  do {                                                                     \
    const char *offset1 = (const char *)_offset1;                          \
    const char *offset2 = (const char *)_offset2;                          \
    if (RangesOverlap(offset1, length1, offset2, length2)) {               \
      GET_STACK_TRACE_FATAL_HERE;                                          \
      bool suppressed = IsInterceptorSuppressed(name);                     \
      if (!suppressed && HaveStackTraceBasedSuppressions()) {              \
        suppressed = IsStackTraceSuppressed(&stack);                       \
      }                                                                    \
      if (!suppressed) {                                                   \
        ReportStringFunctionMemoryRangesOverlap(name, offset1, length1,    \
                                                offset2, length2, &stack); \
      }                                                                    \
    }                                                                      \
  } while (0)

#endif // ! ASAN_DECOUPLE

}  // namespace __asan

#endif  // ASAN_MEMINTRIN_H
