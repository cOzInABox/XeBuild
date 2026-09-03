#ifndef _ISOIO_H
#define _ISOIO_H

#pragma pack(push, 1)
typedef struct _XISO_HEAD{
	char magic[20];
	u32 rootDirSector; // sector of root dir, little endian
	u32 rootDirSize; // directory table size, little endian
	FILETIME creationTime;
	u8 padding[0x7c8];
	char magicTail[20];
} XISO_HEAD, *PXISO_HEAD;

typedef struct _XISO_ENTRYVAL{
	u16 unk1;	// 0
	u16 unk2;	// 2
	u32 sector;	// 4
	u32 size;	// 8
	u8 entType;	// 0xC
	u8 nameSz;	// 0xD
	char name[1]; // 0x11
} XISO_ENTRYVAL, *PXISO_ENTRYVAL;
#pragma pack(pop)


BOOL isoReadSector(u32 sector, u8* buf); // reads 1 0x800 byte sector

BOOL isoToSvod(void);

BOOL openIsoFile(char* fname);
VOID closeIsoFile(void);

#endif // _ISOIO_H
