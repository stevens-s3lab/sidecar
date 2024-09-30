// S3LAB
// Header for shared constants between pass and compiler-rt
//
// vim: ts=2:sw=2:expandtab
#ifndef DECOUPLE_H
#define DECOUPLE_H

#if __LP64__
# define STIM_OFFSET  0x7fff84a2a000
# define D64_PORT     0x7fff84a2a018
#else // LP32
# define STIM_OFFSET  0x7FF2a000
# define D64_PORT     0x7FF2a018
#endif

#define CHECK_SMALL_REGION_THRESHOLD


typedef enum {
	PUSH,
	POP,
	OPCODE_MAX
} opcode_t;

#endif  // DECOUPLE_H
