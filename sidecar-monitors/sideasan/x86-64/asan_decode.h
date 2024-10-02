#ifndef ASAN_DECODE
#define ASAN_DECODE

#include <stdint.h>

#include "asan_decouple.h"
#include "asan_api.h"

#define PKT_PRINT			1

typedef void(*asan_parser_t)(uint64_t);

/* base pkt parser */
void asan_opcode_parser(uint64_t pkt);

/* check small region parsers */
void asan_csr_l1(uint64_t pkt);
void asan_csre_l1(uint64_t pkt);

/* check large region parsers */
void asan_clr_l1(uint64_t pkt);
void asan_clre_l1(uint64_t pkt);

/* check ODR parsers */
void asan_co_l1(uint64_t pkt);

/* check region copy parsers */
void asan_crc_l1(uint64_t pkt);
void asan_crc_l2(uint64_t pkt);

/* check specific sizes parsers */
void asan_ca1_l1(uint64_t pkt);
void asan_ca2_l1(uint64_t pkt);
void asan_ca4_l1(uint64_t pkt);
void asan_ca8_l1(uint64_t pkt);
void asan_ca16_l1(uint64_t pkt);

/* check specific sizes exp parsers */
void asan_cae1_l1(uint64_t pkt);
void asan_cae2_l1(uint64_t pkt);
void asan_cae4_l1(uint64_t pkt);
void asan_cae8_l1(uint64_t pkt);
void asan_cae16_l1(uint64_t pkt);

/* check cxx cookie parser */
void asan_ccac_l1(uint64_t pkt);

/* allocate long parsers */
void asan_al_l1(uint64_t pkt);
void asan_al_l2(uint64_t pkt);

/* allocate short parsers */
void asan_as_l1(uint64_t pkt);

/* poison with value parsers */
void asan_upwv_l1(uint64_t pkt);

/* poison with value parsers */
void asan_pwv_l1(uint64_t pkt);

/* poison partial right redzone parsers */
void asan_pprr_l1(uint64_t pkt);

/* poison aligned stack mem parsers */
void asan_pasm_l1(uint64_t pkt);

/* poison intra object redzone parsers */
void asan_pior_l1(uint64_t pkt);

/* sanitizer annotate contig parsers */
void asan_sac_l1(uint64_t pkt);
void asan_sac_l2(uint64_t pkt);
void asan_sac_l3(uint64_t pkt);

/* flush unneeded parsers */
void asan_fu_l1(uint64_t pkt);

/* check for invalid pair parsers */
void asan_cfip_l1(uint64_t pkt);

/* poison stack entry */
void asan_pse_l1(uint64_t pkt);
void asan_pse_ln(uint64_t pkt);

/* poison stack entry bytes */
void asan_pseb_l1(uint64_t pkt);
void asan_pseb_ln(uint64_t pkt);

/* parser function pointer array */ 
asan_parser_t parser[] = {
	asan_csr_l1,
	asan_csre_l1,
	asan_clr_l1,
	asan_clre_l1,
	asan_co_l1,
	asan_crc_l1,
	asan_ca1_l1,
	asan_ca2_l1,
	asan_ca4_l1,
	asan_ca8_l1,
	asan_ca16_l1,
	asan_cae1_l1,
	asan_cae2_l1,
	asan_cae4_l1,
	asan_cae8_l1,
	asan_cae16_l1,
	asan_ccac_l1,
	asan_al_l1,
	asan_as_l1,
	asan_upwv_l1,
	asan_pwv_l1,
	asan_pprr_l1,
	asan_pasm_l1,
	asan_pior_l1,
	asan_sac_l1,
	asan_fu_l1,
	asan_cfip_l1,
	asan_pse_l1,
	asan_pseb_l1,
	asan_crc_l2,
	asan_al_l2,
	asan_sac_l2,
	asan_sac_l3,
	asan_pse_ln,
	asan_pseb_ln,
	asan_opcode_parser,  
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

#define PARSER_DEFINE(name, next) 				\
	void asan_##name(uint64_t pkt)				\
{								\
		/* store payload to array */			\
		asdec.pkt[asdec.pkt_index++] = pkt;		\
								\
		/* link to the next level parser */		\
		asdec.parser_index = next;	 		\
}

#define PARSER_LAST_DEFINE(name, api) 				\
	void asan_##name(uint64_t pkt)				\
{								\
		/* store payload to array */			\
		asdec.pkt[asdec.pkt_index++] = pkt;		\
								\
		/* enable payload parsing */			\
		asdec.parser_index = OPCODE_PARSER;		\
								\
		/* call asan API */				\
		asan_api_##api((uint64_t *)&asdec.pkt);		\
}

PARSER_LAST_DEFINE(csr_l1, csr)
PARSER_LAST_DEFINE(csre_l1, csre)

PARSER_LAST_DEFINE(clr_l1, clr)
PARSER_LAST_DEFINE(clre_l1, clre)

PARSER_LAST_DEFINE(co_l1, co)

PARSER_DEFINE(crc_l1, CRC_L2)
PARSER_LAST_DEFINE(crc_l2, crc)

PARSER_LAST_DEFINE(ca1_l1, ca1)
PARSER_LAST_DEFINE(ca2_l1, ca2)
PARSER_LAST_DEFINE(ca4_l1, ca4)
PARSER_LAST_DEFINE(ca8_l1, ca8)
PARSER_LAST_DEFINE(ca16_l1, ca16)

PARSER_LAST_DEFINE(cae1_l1, cae1)
PARSER_LAST_DEFINE(cae2_l1, cae2)
PARSER_LAST_DEFINE(cae4_l1, cae4)
PARSER_LAST_DEFINE(cae8_l1, cae8)
PARSER_LAST_DEFINE(cae16_l1, cae16)

PARSER_LAST_DEFINE(ccac_l1, ccac)

PARSER_DEFINE(al_l1, AL_L2)
PARSER_LAST_DEFINE(al_l2, al)

PARSER_LAST_DEFINE(as_l1, as)

PARSER_LAST_DEFINE(upwv_l1, upwv)

PARSER_LAST_DEFINE(pwv_l1, pwv)

PARSER_LAST_DEFINE(pprr_l1, pprr)

PARSER_LAST_DEFINE(pasm_l1, pasm)

PARSER_LAST_DEFINE(pior_l1, pior)

PARSER_DEFINE(sac_l1, SAC_L2)
PARSER_DEFINE(sac_l2, SAC_L3)
PARSER_LAST_DEFINE(sac_l3, sac)

PARSER_LAST_DEFINE(fu_l1, fu)

PARSER_LAST_DEFINE(cfip_l1, cfip)

void asan_pse_l1(uint64_t pkt)                     
{                                                               
	/* store payload to array */                    
	asdec.pkt[asdec.pkt_index++] = pkt;

	/* calculate total packets */
	asdec.total_pkt = (asdec.pkt[0] >> 8 & 0xff) + 2;

	/* pass to next level */
	asdec.parser_index = PSE_LN; 
}

void asan_pse_ln(uint64_t pkt)                     
{                                                               
	/* store payload to array */                    
	asdec.pkt[asdec.pkt_index++] = pkt;

	/* check if there are more pkts */
	if (asdec.pkt_index < asdec.total_pkt) {
		/* link to the next level parser */             
		asdec.parser_index = PSE_LN;
	} else {
		/* link back to payload parser */ 
		asdec.parser_index = OPCODE_PARSER; 

		/* call asan api */
		asan_api_pse((uint64_t *)&asdec.pkt, asdec.pkt_index);
	}
}


void asan_pseb_l1(uint64_t pkt)                     
{                                                               
	/* store payload to array */                    
	asdec.pkt[asdec.pkt_index++] = pkt;

	/* calculate total packets */
	asdec.total_pkt = (asdec.pkt[0] >> 8 & 0xff) + 2;

	/* pass to next level */
	asdec.parser_index = PSEB_LN; 
}

void asan_pseb_ln(uint64_t pkt)                     
{                                                               
	/* store payload to array */                    
	asdec.pkt[asdec.pkt_index++] = pkt;

	/* check if there are more pkts */
	if (asdec.pkt_index < asdec.total_pkt) {
		/* link to the next level parser */             
		asdec.parser_index = PSEB_LN; 
	} else {
		/* link back to payload parser */ 
		asdec.parser_index = OPCODE_PARSER; 

		/* call asan api */
		asan_api_pseb((uint64_t *)&asdec.pkt, asdec.pkt_index);
	}
}

#endif
