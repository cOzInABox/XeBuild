#ifndef _TYPES_H
#define _TYPES_H

#define bswap16(x) (((x&0xFF)<<8)+(((x&0xFF00)>>8)))
#define bswap32(x) ((((x&0xFF)<<24))+(((x&0xFF00)<<8))+(((x&0xFF0000)>>8))+(((x&0xFF000000)>>24)))
#define bswap64(x) (_byteswap_uint64(x))

typedef unsigned long long	QWORD;		
typedef unsigned long 		DWORD;		
typedef unsigned char 		BYTE, *PBYTE;
typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef char				s8;
typedef short				s16;
typedef int					s32;
typedef long long			s64;
typedef int                 BOOL;


#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif


#endif // _TYPES_H
