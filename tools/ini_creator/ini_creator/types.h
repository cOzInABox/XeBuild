#ifndef _TYPES_H
#define _TYPES_H

#define u16Rev(x) (((x&0xFF)<<8)+(((x&0xFF00)>>8)))
#define u32Rev(x) ((((x&0xFF)<<24))+(((x&0xFF00)<<8))+(((x&0xFF0000)>>8))+(((x&0xFF000000)>>24)))
#define bswap32(x) u32Rev(x)
#define bswap16(x) u16Rev(x)

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef char				s8;
typedef short				s16;
typedef int					s32;

#ifndef bool
	typedef enum { false, true } bool;
#endif
// #ifndef BOOL
	// typedef enum { FALSE, TRUE } BOOL;
// #endif
#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef MAXPATHLEN
	#define MAXPATHLEN 1024
#endif

typedef struct _FLLIST {
	char name[64];
	u32 crc;
	u32 magic;
} FLLIST, *PFLLIST;

// 16 bit file type masks
#define XEX2_MASK	(0x58455832)//'XEX2'
#define XTTF_MASK	(0x78747466)//'xttf'
#define BL_MASK		0x43400000 //'C@'
#define BL_FULLMASK	0x4347

#define BLTBASE		(0x4300)
#define BLCB		(BLTBASE|0x42)
#define BLCC		(BLTBASE|0x43)
#define BLCD		(BLTBASE|0x44)
#define BLCE		(BLTBASE|0x45)
#define BLCF		(BLTBASE|0x46)
#define BLCG		(BLTBASE|0x47)

enum {
	FILE_FLASH = 0,
	FILE_BL,
	FILE_OTHER
};

enum {
	XENON = 0,
	ZEPHRY = 1,
	FALCON = 2,
	JASPER = 3,
	TRINITY
};

#define MAX_BLS		0x10
#define MAX_FLASH	0x100

#endif // _TYPES_H
