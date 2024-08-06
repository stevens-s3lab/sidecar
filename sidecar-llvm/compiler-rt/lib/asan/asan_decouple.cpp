// vim: ts=2:sw=2:expandtab

#ifdef ASAN_DECOUPLE

#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include "asan_internal.h"
#include "asan_interface_internal.h"
#include "asan_mapping.h"
#include "asan_decouple.h"

namespace __asan {

extern "C" {
void __asan_poison_stack_entry(uptr addr, u32 frame_sb_size, u32 argsnum, ...)
{
  va_list args;
  u64 data;

  // Printf("Poison stack entry %x:%u %u %u\n", addr,
  //    frame_sb_size << SHADOW_SCALE, frame_sb_size, argsnum);

  /*
  * prepare and send first 64bit packet
  * | opcode(8) | vars(8) | addr(48) |
  */
  data = (u8)argsnum;
  data <<= 8;
  data |= (u64)addr << 16 | POISON_STACK_ENTRY;
  msg_write(data);

  /*
  * prepare and send 32bit packet
  * | unused(16) | sb_size(16) |
  */
  data = (u16)frame_sb_size;
  msg_write32((u32)data);

  va_start(args, argsnum);
  while (argsnum-- > 0) {
    /* var_data
     * | offset(32) | length(32) |
     */
    data = va_arg(args, u64);
    msg_write(data);

#if 0
  /* get offset and length for each var */
  u32 off = data;
  u32 len = data >> 32;
  Printf("[+] var off %u, len %u, data 0x%016llx\n", off, len, data);
#endif
  }
  va_end(args);

  PoisonStatsAdd(addr, frame_sb_size << SHADOW_SCALE, 0xff);
}

void __asan_poison_stack_entry_bytes(uptr addr, u32 frame_sb_size, u32 qwords, ...)
{
  va_list bytes;
  u64 data;

  // Printf("Poison stack entry bytes %x:%u\n", addr, frame_sb_size);

  /*
   * prepare and send first 64bit packet
   * | opcode(8) | qwords(8) | addr(48) |
   */
  data = (u8)qwords;
  data <<= 8;
  data |= (u64)addr << 16 | POISON_STACK_ENTRY_BYTES;
  msg_write(data);

/*
  * prepare and send 32bit packet
  * | unused(16) | sb_size(16) |
  */
  data = (u16)frame_sb_size;
  msg_write32((u32)data);

  va_start(bytes, qwords);
  while (qwords-- > 0) {
    /* get magic bytes */
    data = va_arg(bytes, u64);
    msg_write(data);
#if 0
    Printf("[+] byte 0x%016llx\n", data);
#endif
  }
  va_end(bytes);

  PoisonStatsAdd(addr, frame_sb_size << SHADOW_SCALE, 0xff);
}

void __asan_poison_f8(uptr addr, uptr size)
{
  MonitorPoisonShadow(addr, size, 0xf8);
}

} // extern "C"
} // namespace __asan
#endif // ASAN_DECOUPLE
