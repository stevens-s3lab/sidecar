#ifndef ASAN_DECODE
#define ASAN_DECODE

#include "asan_decouple.h"
#include "asan_api.h"

/* opencsd include */
#include "opencsd/c_api/opencsd_c_api.h"

/* opencsd defines */
#define INPUT_BLOCK_SIZE		1024
#define PACKET_STR_LEN 			1024
#define STMTCSR_TRC_ID_MASK		0x007F0000
#define STMTCSR_TRC_ID_SHIFT		16
#define PKT_PRINT			0

typedef void(*asan_parser_t)(ocsd_stm_pkt *);

/* buffer to handle pkt printing */
static char packet_str[PACKET_STR_LEN];

/* base pkt parser */
void asan_opcode_parser(ocsd_stm_pkt *pkt);

/* check small region parsers */
void asan_csr_l1(ocsd_stm_pkt *pkt);
void asan_csre_l1(ocsd_stm_pkt *pkt);
void asan_csre_l2(ocsd_stm_pkt *pkt);

/* check large region parsers */
void asan_clr_l1(ocsd_stm_pkt *pkt);
void asan_clr_l2(ocsd_stm_pkt *pkt);
void asan_clre_l1(ocsd_stm_pkt *pkt);
void asan_clre_l2(ocsd_stm_pkt *pkt);
void asan_clre_l3(ocsd_stm_pkt *pkt);

/* check ODR parsers */
void asan_co_l1(ocsd_stm_pkt *pkt);
void asan_co_l2(ocsd_stm_pkt *pkt);

/* check region copy parsers */
void asan_crc_l1(ocsd_stm_pkt *pkt);
void asan_crc_l2(ocsd_stm_pkt *pkt);
void asan_crc_l3(ocsd_stm_pkt *pkt);
void asan_crc_l4(ocsd_stm_pkt *pkt);

/* check specific sizes parsers */
void asan_ca1_l1(ocsd_stm_pkt *pkt);
void asan_ca2_l1(ocsd_stm_pkt *pkt);
void asan_ca4_l1(ocsd_stm_pkt *pkt);
void asan_ca8_l1(ocsd_stm_pkt *pkt);
void asan_ca16_l1(ocsd_stm_pkt *pkt);

/* check specific sizes exp parsers */
void asan_cae1_l1(ocsd_stm_pkt *pkt);
void asan_cae1_l2(ocsd_stm_pkt *pkt);
void asan_cae2_l1(ocsd_stm_pkt *pkt);
void asan_cae2_l2(ocsd_stm_pkt *pkt);
void asan_cae4_l1(ocsd_stm_pkt *pkt);
void asan_cae4_l2(ocsd_stm_pkt *pkt);
void asan_cae8_l1(ocsd_stm_pkt *pkt);
void asan_cae8_l2(ocsd_stm_pkt *pkt);
void asan_cae16_l1(ocsd_stm_pkt *pkt);
void asan_cae16_l2(ocsd_stm_pkt *pkt);

/* check cxx cookie parser */
void asan_ccac_l1(ocsd_stm_pkt *pkt);

/* allocate long parsers */
void asan_al_l1(ocsd_stm_pkt *pkt);
void asan_al_l2(ocsd_stm_pkt *pkt);
void asan_al_l3(ocsd_stm_pkt *pkt);
void asan_al_l4(ocsd_stm_pkt *pkt);
void asan_al_l5(ocsd_stm_pkt *pkt);

/* allocate short parsers */
void asan_as_l1(ocsd_stm_pkt *pkt);
void asan_as_l2(ocsd_stm_pkt *pkt);
void asan_as_l3(ocsd_stm_pkt *pkt);

/* poison with value parsers */
void asan_upwv_l1(ocsd_stm_pkt *pkt);
void asan_upwv_l2(ocsd_stm_pkt *pkt);

/* poison with value parsers */
void asan_pwv_l1(ocsd_stm_pkt *pkt);
void asan_pwv_l2(ocsd_stm_pkt *pkt);

/* poison partial right redzone parsers */
void asan_pprr_l1(ocsd_stm_pkt *pkt);
void asan_pprr_l2(ocsd_stm_pkt *pkt);
void asan_pprr_l3(ocsd_stm_pkt *pkt);

/* poison aligned stack mem parsers */
void asan_pasm_l1(ocsd_stm_pkt *pkt);
void asan_pasm_l2(ocsd_stm_pkt *pkt);

/* poison intra object redzone parsers */
void asan_pior_l1(ocsd_stm_pkt *pkt);
void asan_pior_l2(ocsd_stm_pkt *pkt);

/* sanitizer annotate contig parsers */
void asan_sac_l1(ocsd_stm_pkt *pkt);
void asan_sac_l2(ocsd_stm_pkt *pkt);
void asan_sac_l3(ocsd_stm_pkt *pkt);
void asan_sac_l4(ocsd_stm_pkt *pkt);
void asan_sac_l5(ocsd_stm_pkt *pkt);
void asan_sac_l6(ocsd_stm_pkt *pkt);
void asan_sac_l7(ocsd_stm_pkt *pkt);

/* flush unneeded parsers */
void asan_fu_l1(ocsd_stm_pkt *pkt);
void asan_fu_l2(ocsd_stm_pkt *pkt);

/* check for invalid pair parsers */
void asan_cfip_l1(ocsd_stm_pkt *pkt);
void asan_cfip_l2(ocsd_stm_pkt *pkt);
void asan_cfip_l3(ocsd_stm_pkt *pkt);

/* poison stack entry */
void asan_pse_l1(ocsd_stm_pkt *pkt);
void asan_pse_ln(ocsd_stm_pkt *pkt);

/* poison stack entry bytes */
void asan_pseb_l1(ocsd_stm_pkt *pkt);
void asan_pseb_ln(ocsd_stm_pkt *pkt);

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
	asan_csre_l2,
	asan_clr_l2,
	asan_clre_l2,
	asan_clre_l3,
	asan_co_l2,
	asan_crc_l2,
	asan_crc_l3,
	asan_crc_l4,
	asan_cae1_l2,
	asan_cae2_l2,
	asan_cae4_l2,
	asan_cae8_l2,
	asan_cae16_l2,
	asan_al_l2,
	asan_al_l3,
	asan_al_l4,
	asan_al_l5,
	asan_as_l2,
	asan_as_l3,
	asan_upwv_l2,
	asan_pwv_l2,
	asan_pprr_l2,
	asan_pprr_l3,
	asan_pasm_l2,
	asan_pior_l2,
	asan_sac_l2,
	asan_sac_l3,
	asan_sac_l4,
	asan_sac_l5,
	asan_sac_l6,
	asan_sac_l7,
	asan_fu_l2,
	asan_cfip_l2,
	asan_cfip_l3,
	asan_pse_ln,
	asan_pseb_ln,
	asan_opcode_parser,  
};

typedef enum {
	CSRE_L2 = OPCODE_MAX,
	CLR_L2,
	CLRE_L2,
	CLRE_L3,
	CO_L2,
	CRC_L2,
	CRC_L3,
	CRC_L4,
	CAE1_L2,
	CAE2_L2,
	CAE4_L2,
	CAE8_L2,
	CAE16_L2,
	AL_L2,
	AL_L3,
	AL_L4,
	AL_L5,
	AS_L2,
	AS_L3,
	UPWV_L2,
	PWV_L2,
	PPRR_L2,
	PPRR_L3,
	PASM_L2,
	PIOR_L2,
	SAC_L2,
	SAC_L3,
	SAC_L4,
	SAC_L5,
	SAC_L6,
	SAC_L7,
	FU_L2,
	CFIP_L2,
	CFIP_L3,
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
	uint32_t pkt[256];
};

struct asan_decoder asdec;

/* input trace for decoder */
struct stm_decoder {
	uint64_t packets_decoded;
	FILE *trace_in;
	char *data_buffer;
	size_t data_size;
};

#define PARSER_DEFINE(name, next) 				\
	void asan_##name(ocsd_stm_pkt *pkt)			\
{								\
		/* dbg msg */					\
		sprintf(packet_str, "[%.*s->] %s\n", 		\
				asdec.pkt_index,		\
				"--------------",		\
				#name);				\
		ocsd_def_errlog_msgout(packet_str);		\
								\
		/* store payload to array */			\
		asdec.pkt[asdec.pkt_index++] = pkt->payload.D32;\
								\
		/* link to the next level parser */		\
		asdec.parser_index = next;	 		\
}

#define PARSER_LAST_DEFINE(name, api) \
	void asan_##name(ocsd_stm_pkt *pkt)			\
{								\
		/* dbg msg */					\
		sprintf(packet_str, "[%.*s->] %s\n", 		\
				asdec.pkt_index,		\
				"--------------",		\
				#name);				\
		ocsd_def_errlog_msgout(packet_str);		\
								\
		/* store payload to array */			\
		asdec.pkt[asdec.pkt_index++] = pkt->payload.D32;\
								\
		/* enable payload parsing */			\
		asdec.parser_index = OPCODE_PARSER;		\
								\
		/* print pkts for debugging */			\
		for (int i = 0; i < asdec.pkt_index; i++) {	\
			sprintf(packet_str, 			\
					"pkt[%d] %08x\n",	\
					i,			\
					asdec.pkt[i]);		\
			ocsd_def_errlog_msgout(packet_str);	\
		}						\
		sprintf(packet_str, "\n");			\
		ocsd_def_errlog_msgout(packet_str);		\
								\
		/* call asan API */				\
		asan_api_##api((uint32_t *)&asdec.pkt);		\
}

PARSER_LAST_DEFINE(csr_l1, csr)
PARSER_DEFINE(csre_l1, CSRE_L2)
PARSER_LAST_DEFINE(csre_l2, csre)

PARSER_DEFINE(clr_l1, CLR_L2)
PARSER_LAST_DEFINE(clr_l2, clr)
PARSER_DEFINE(clre_l1, CLRE_L2)
PARSER_DEFINE(clre_l2, CLRE_L3)
PARSER_LAST_DEFINE(clre_l3, clre)

PARSER_DEFINE(co_l1, CO_L2)
PARSER_LAST_DEFINE(co_l2, co)

PARSER_DEFINE(crc_l1, CRC_L2)
PARSER_DEFINE(crc_l2, CRC_L3)
PARSER_DEFINE(crc_l3, CRC_L4)
PARSER_LAST_DEFINE(crc_l4, crc)

PARSER_LAST_DEFINE(ca1_l1, ca1)
PARSER_LAST_DEFINE(ca2_l1, ca2)
PARSER_LAST_DEFINE(ca4_l1, ca4)
PARSER_LAST_DEFINE(ca8_l1, ca8)
PARSER_LAST_DEFINE(ca16_l1, ca16)

PARSER_DEFINE(cae1_l1, CAE1_L2)
PARSER_LAST_DEFINE(cae1_l2, cae1)
PARSER_DEFINE(cae2_l1, CAE2_L2)
PARSER_LAST_DEFINE(cae2_l2, cae2)
PARSER_DEFINE(cae4_l1, CAE4_L2)
PARSER_LAST_DEFINE(cae4_l2, cae4)
PARSER_DEFINE(cae8_l1, CAE8_L2)
PARSER_LAST_DEFINE(cae8_l2, cae8)
PARSER_DEFINE(cae16_l1, CAE16_L2)
PARSER_LAST_DEFINE(cae16_l2, cae16)

PARSER_LAST_DEFINE(ccac_l1, ccac)

PARSER_DEFINE(al_l1, AL_L2)
PARSER_DEFINE(al_l2, AL_L3)
PARSER_DEFINE(al_l3, AL_L4)
PARSER_DEFINE(al_l4, AL_L5)
PARSER_LAST_DEFINE(al_l5, al)

PARSER_DEFINE(as_l1, AS_L2)
PARSER_DEFINE(as_l2, AS_L3)
PARSER_LAST_DEFINE(as_l3, as)

PARSER_DEFINE(upwv_l1, UPWV_L2)
PARSER_LAST_DEFINE(upwv_l2, upwv)

PARSER_DEFINE(pwv_l1, PWV_L2)
PARSER_LAST_DEFINE(pwv_l2, pwv)

PARSER_DEFINE(pprr_l1, PPRR_L2)
PARSER_DEFINE(pprr_l2, PPRR_L3)
PARSER_LAST_DEFINE(pprr_l3, pprr)

PARSER_DEFINE(pasm_l1, PASM_L2)
PARSER_LAST_DEFINE(pasm_l2, pasm)

PARSER_DEFINE(pior_l1, PIOR_L2)
PARSER_LAST_DEFINE(pior_l2, pior)

PARSER_DEFINE(sac_l1, SAC_L2)
PARSER_DEFINE(sac_l2, SAC_L3)
PARSER_DEFINE(sac_l3, SAC_L4)
PARSER_DEFINE(sac_l4, SAC_L5)
PARSER_DEFINE(sac_l5, SAC_L6)
PARSER_DEFINE(sac_l6, SAC_L7)
PARSER_LAST_DEFINE(sac_l7, sac)

PARSER_DEFINE(fu_l1, FU_L2)
PARSER_LAST_DEFINE(fu_l2, fu)

PARSER_DEFINE(cfip_l1, CFIP_L2)
PARSER_DEFINE(cfip_l2, CFIP_L3)
PARSER_LAST_DEFINE(cfip_l3, cfip)

void asan_pse_l1(ocsd_stm_pkt *pkt)                     
{                                                               
	/* dbg msg */                                   
	sprintf(packet_str, "[%.*s->] pse_l1\n",            
			asdec.pkt_index,                
			"--------------");                         
	ocsd_def_errlog_msgout(packet_str);             

	/* store payload to array */                    
	asdec.pkt[asdec.pkt_index++] = pkt->payload.D32;

	/* calculate total packets */
	asdec.total_pkt = ((asdec.pkt[0] >> 8 & 0xff) * 2) + 3;

	/* pass to next level */
	asdec.parser_index = PSE_LN; 
}

void asan_pse_ln(ocsd_stm_pkt *pkt)                     
{                                                               
	/* dbg msg */                                   
	sprintf(packet_str, "[%.*s->] pse_l%d\n", 
			asdec.pkt_index,                
			"--------------",
			asdec.pkt_index);                         
	ocsd_def_errlog_msgout(packet_str);             

	/* store payload to array */                    
	asdec.pkt[asdec.pkt_index++] = pkt->payload.D32;

	/* check if there are more pkts */
	if (asdec.pkt_index < asdec.total_pkt) {
		/* link to the next level parser */             
		asdec.parser_index = PSE_LN;
	} else {
		/* link back to payload parser */ 
		asdec.parser_index = OPCODE_PARSER; 

		/* print pkts for debugging */                  
		for (int i = 0; i < asdec.pkt_index; i++) {     
			sprintf(packet_str,                     
					"pkt[%d] %08x\n",       
					i,                      
					asdec.pkt[i]);          
			ocsd_def_errlog_msgout(packet_str);     
		}                                               
		sprintf(packet_str, "\n");                      
		ocsd_def_errlog_msgout(packet_str);  

		/* call asan api */
		asan_api_pse((uint32_t *)&asdec.pkt, asdec.pkt_index);
	}
}


void asan_pseb_l1(ocsd_stm_pkt *pkt)                     
{                                                               
	/* dbg msg */                                   
	sprintf(packet_str, "[%.*s->] pseb_l1\n",            
			asdec.pkt_index,                
			"--------------");                         
	ocsd_def_errlog_msgout(packet_str);             

	/* store payload to array */                    
	asdec.pkt[asdec.pkt_index++] = pkt->payload.D32;

	/* calculate total packets */
	asdec.total_pkt = ((asdec.pkt[0] >> 8 & 0xff) * 2) + 3;

	/* pass to next level */
	asdec.parser_index = PSEB_LN; 
}

void asan_pseb_ln(ocsd_stm_pkt *pkt)                     
{                                                               
	/* dbg msg */                                   
	sprintf(packet_str, "[%.*s->] pseb_l%d\n",            
			asdec.pkt_index,                
			"--------------",
			asdec.pkt_index);                         
	ocsd_def_errlog_msgout(packet_str);             

	/* store payload to array */                    
	asdec.pkt[asdec.pkt_index++] = pkt->payload.D32;

	/* check if there are more pkts */
	if (asdec.pkt_index < asdec.total_pkt) {
		/* link to the next level parser */             
		asdec.parser_index = PSEB_LN; 
	} else {
		/* link back to payload parser */ 
		asdec.parser_index = OPCODE_PARSER; 

		/* print pkts for debugging */                  
		for (int i = 0; i < asdec.pkt_index; i++) {     
			sprintf(packet_str,                     
					"pkt[%d] %08x\n",       
					i,                      
					asdec.pkt[i]);          
			ocsd_def_errlog_msgout(packet_str);     
		}                                               
		sprintf(packet_str, "\n");                      
		ocsd_def_errlog_msgout(packet_str);  

		/* call asan api */
		asan_api_pseb((uint32_t *)&asdec.pkt, asdec.pkt_index);
	}
}

#endif
