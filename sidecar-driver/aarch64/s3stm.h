#ifndef _S3STM_H_
#define _S3STM_H_

#define S3STM_DEVNAME "s3_stm"	

/* IOCTLs */
#define S3STM_IOCTL_TYPE 'a'
#define GET_RWP 	_IOR(S3STM_IOCTL_TYPE, 'b', int64_t*)
#define GET_RRP 	_IOR(S3STM_IOCTL_TYPE, 'e', int64_t*)
#define GET_FULL 	_IOR(S3STM_IOCTL_TYPE, 'c', int32_t*)
#define CS_ENABLE 	_IOR(S3STM_IOCTL_TYPE, 'd', void*)
#define CS_DISABLE 	_IOR(S3STM_IOCTL_TYPE, 'f', void*)
#define SET_ASANID 	_IOR(S3STM_IOCTL_TYPE, 'g', void*)
#define SET_MONITORID 	_IOR(S3STM_IOCTL_TYPE, 'i', void*)
#define INCR_RRP 	_IOR(S3STM_IOCTL_TYPE, 'j', void*)
#define GET_ETR_SZ 	_IOR(S3STM_IOCTL_TYPE, 'k', void*)
#define GET_BASE 	_IOR(S3STM_IOCTL_TYPE, 'l', void*)


/* ETR modes supported by driver */
typedef enum ETR_MODE_ENUM { ETR_CBUFFER, ETR_SWFIFO } etr_mode_t;


#endif
