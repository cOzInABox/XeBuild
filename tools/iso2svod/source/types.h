#ifndef _TYPES_H
#define _TYPES_H

#include "xecryptTypes.h"

//#ifndef _MSC_VER
//#define _fseeki64 fseeko64
//#define _ftelli64 ftello64
//#endif


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
