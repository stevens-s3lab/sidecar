#ifndef SIDESTACK_DECODE
#define SIDESTACK_DECODE

#include "sidestack_decouple.h"
#include "sidestack_api.h"

/* pkt out */
FILE *pkt_out;

typedef void(*sidestack_parser_t)(uint64_t);

/* base pkt parser */
void sidestack_opcode_parser(uint64_t payload);

/* parser function pointer array */ 
sidestack_parser_t parser[] = {
	sidestack_push,
	sidestack_pop 
};

typedef enum {
	CRC_L2 = OPCODE_MAX,
	AL_L2,
	SAC_L2,
	SAC_L3,
	PSE_LN,
	PSEB_LN,
	OPCODE_PARSER,  
	PARSER_MAX
} parser_t;

/* struct for decoding asan packet */
struct asan_decoder {
	int parser_index;
	int pkt_index;
	int total_pkt;
	/* this might need to be larger */
	uint64_t pkt[256];
};

struct asan_decoder asdec;

/* input trace for decoder */
struct ptw_decoder {
	uint64_t packets_decoded;
	FILE *trace_in;
	char *data_buffer;
	size_t data_size;
};

void
sidestack_opcode_parser(uint64_t payload)
{
	register uint8_t oc;

	/* reset pkt index */
	asdec.pkt_index = 0;

	/* dbg msg */
	fprintf(pkt_out, "[%.*s->] opcode: 0x%lx\n", 
			asdec.pkt_index,
			"--------------", 
			payload & 0xff);

	/* store first pkt */
	asdec.pkt[asdec.pkt_index++] = payload;

	/* set L1 parser based on opcode */
	asdec.parser_index = payload & 0xff;

	oc = payload & 0xff;
	/* all packets have already been parsed */
	// if (oc == CHECK_SMALL_REGION ||
	//   oc == CHECK_ACCESS_1 ||
	//   oc == CHECK_ACCESS_2 ||
	//   oc == CHECK_ACCESS_4 ||
	//   oc == CHECK_ACCESS_8 ||
	//   oc == CHECK_ACCESS_16 ||
	//   oc == CHECK_CXX_ARRAY_COOKIE ) {
	// 	parser[asdec.parser_index](0x0);
	// }
}

#endif
