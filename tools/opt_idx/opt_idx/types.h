#ifndef _TYPES_H
#define _TYPES_H

#ifndef _MSC_VER
	#define _fseeki64 fseeko64
	#define _ftelli64 ftello64
#endif

#define u16Rev(x) (((x&0xFF)<<8)+(((x&0xFF00)>>8)))
#define u32Rev(x) ((((x&0xFF)<<24))+(((x&0xFF00)<<8))+(((x&0xFF0000)>>8))+(((x&0xFF000000)>>24)))
#define u64Rev(x) (((x&0xFF)<<56)+((x&0xFF00)<<40)+((x&0xFF0000)<<24)+((x&0xFF000000)<<8)+((x>>8)&0xFF000000)+((x>>24)&0xFF0000)+((x>>40)&0xFF00)+((x>>56)&0xFF))
#define bswap32(x) u32Rev(x)
#define bswap16(x) u16Rev(x)

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
