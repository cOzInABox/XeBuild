#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdarg.h>
#include "types.h"
#include "util.h"
#include "isoio.h"
#include "xcontent.h"
#include "xexinfo.h"
#include "xecrypt.h"

extern PXCONTENT_HEADER xHeader;
extern PXCONTENT_METADATA xMeta;
extern char baseOutPath[MAXPATHLEN];

#define FSD_DEBUG	0
#define FSD(fmt, ...) do { if (FSD_DEBUG) printf(fmt, __VA_ARGS__); } while (0)

#define XEX_FILE_NAME		"default.xex"
#define XGD_IMAGE_MAGIC		"MICROSOFT*XBOX*MEDIA"
#define XGD_MAGIC_SIZE		20
#define XGD_SECTOR_SIZE		0x800

#define XGD_ISO_BASE_OFFSET		0x10000
#define XGD_MAGIC_OFFSET_XDKI	XGD_ISO_BASE_OFFSET		// images made with the xdk image builder

#define XGD_MAGIC_OFFSET_XGD2	0xFDA0000
#define XGD2_PFI_OFFSET			0xFD8E800
#define XGD2_DMI_OFFSET			0xFD8F000
#define XGD2_SS_OFFSET			0xFD8F800

#define XGD_MAGIC_OFFSET_XGD3	0x2090000
#define XGD3_PFI_OFFSET			0x2076800
#define XGD3_DMI_OFFSET			0x2077000
#define XGD3_SS_OFFSET			0x2077800

#define SVOD_START_SECTOR		(XGD_ISO_BASE_OFFSET/XGD_SECTOR_SIZE)

static FILE* isoFile = NULL;
static s64 baseOffset = 0;
static s64 maxOffset = 0;
static u32 maxSector = 0x108; // last sector to contain game data
static u32 minSector = 0xFFFFFFFF; // first sector to contain game data
static u8 sectorBuffer[0x800];
static u32 rootDirSector; // sector of root dir, little endian
static u32 rootDirSize; // directory table size, little endian
static FILETIME creationTime;
static u32 defaultXexSector;
static u32 defaultXexSize;

BOOL setIsoSector(u32 sector)
{
	s64 offset = sector;
	offset = (offset * XGD_SECTOR_SIZE) + baseOffset;
// 	printf("seeking iso to sector 0x%x offset %016I64x\n", sector, offset);
	if (_fseeki64(isoFile, offset, SEEK_SET) != 0)
		return FALSE;
	return TRUE; // should have error handling
}

BOOL readIsoData(u8* dest, int len)
{
	int cnt = fread(dest, len, 1, isoFile);
	if (cnt != 1)
		return FALSE;
	return TRUE;
}

BOOL isoReadInternal(s64 offset, u8* dest, int len)
{
	_fseeki64(isoFile, offset, SEEK_SET);
	fread(dest, len, 1, isoFile);
	return TRUE; // should have error handling
}

BOOL isoReadSector(u32 sector, u8* buf)
{
	setIsoSector(sector);
	fread(buf, 0x800, 1, isoFile);
	return TRUE; // should have error handling
}

BOOL getXgdBaseOffset(VOID)
{
	BOOL ret = FALSE;
	PXISO_HEAD hdr = (PXISO_HEAD)sectorBuffer;
	memset(sectorBuffer, 0, XGD_SECTOR_SIZE);

	if (maxOffset > (XGD_MAGIC_OFFSET_XDKI + XGD_MAGIC_SIZE))
	{
		isoReadInternal(XGD_MAGIC_OFFSET_XDKI, sectorBuffer, XGD_SECTOR_SIZE); // xdk images
		if ((memcmp(hdr->magic, XGD_IMAGE_MAGIC, XGD_MAGIC_SIZE) == 0) && (memcmp(hdr->magicTail, XGD_IMAGE_MAGIC, XGD_MAGIC_SIZE) == 0))
		{
			baseOffset = XGD_MAGIC_OFFSET_XDKI - XGD_ISO_BASE_OFFSET; // should be 0
			ret = TRUE;
		}
	}
	if ((ret == FALSE) && (maxOffset > (XGD_MAGIC_OFFSET_XGD3 + XGD_MAGIC_SIZE)))
	{
		isoReadInternal(XGD_MAGIC_OFFSET_XGD3, sectorBuffer, XGD_SECTOR_SIZE); // XGD3 images
		if (memcmp(sectorBuffer, XGD_IMAGE_MAGIC, XGD_MAGIC_SIZE) == 0)
		{
			baseOffset = XGD_MAGIC_OFFSET_XGD3 - XGD_ISO_BASE_OFFSET;
			ret = TRUE;
		}
	}
	if ((ret == FALSE) && (maxOffset > (XGD_MAGIC_OFFSET_XGD2 + XGD_MAGIC_SIZE)))
	{
		isoReadInternal(XGD_MAGIC_OFFSET_XGD2, sectorBuffer, XGD_SECTOR_SIZE); // XGD2 images
		if (memcmp(sectorBuffer, XGD_IMAGE_MAGIC, XGD_MAGIC_SIZE) == 0)
		{
			baseOffset = XGD_MAGIC_OFFSET_XGD2 - XGD_ISO_BASE_OFFSET;
			ret = TRUE;
		}
	}
	if (ret)
	{
		rootDirSector = getLe32(&hdr->rootDirSector);
		rootDirSize = getLe32(&hdr->rootDirSize);
		creationTime.dwHighDateTime = getLe32(&hdr->creationTime.dwHighDateTime);
		creationTime.dwLowDateTime = getLe32(&hdr->creationTime.dwLowDateTime);
	}
	return ret;
}

BOOL getMaxDirectory(PXISO_ENTRYVAL xent)
{
	BOOL ret = FALSE;
	u32 size = (xent->size + 0x7FF)&~0x7FF;
	u8* buf;
	if (setIsoSector(xent->sector) == FALSE)
	{
		printf("unable to seek to sector 0x%x!\n", xent->sector);
		return FALSE;
	}
	if (size == 0)
	{
		printf("read size is 0\n");
		return TRUE;
	}
	buf = (u8*)malloc(size);
	if (buf == NULL)
	{
		printf("unable to alloc 0x%x bytes for iso read!\n", size);
		return FALSE;
	}
// 	printf("** reading sector %x\n", xent->sector);
	if (readIsoData(buf, size))
	{
		u32 currOff = 0;
		while (currOff < size)
		{
			PXISO_ENTRYVAL pent = (PXISO_ENTRYVAL)&buf[currOff];
			if ((getLe16(&pent->unk1) == 0xFFFF) || (getLe16(&pent->unk2) == 0xFFFF))
			{
				currOff = (currOff+0x7FF) &~ 0x7FF;
				FSD("### unk1 0x%x unk2 0x%x\n", pent->unk1, pent->unk2);
			}
			else
			{
				u32 ssize = (((getLe32(&pent->size) + 0x7FF) &~0x7FF) >> 11) & 0x1FFFFF; // size in 0x800 byte sectors
				char temOutName[128];
				memset(temOutName, 0, 128);
				memcpy(temOutName, pent->name, pent->nameSz);
				if ((ssize + getLe32(&pent->sector)) > maxSector)
				{
					FSD("~~~ updating max sector 0x%x to ", maxSector);
					maxSector = (ssize + getLe32(&pent->sector));
					FSD("0x%x\n", maxSector);
				}
				if (getLe32(&pent->sector) < minSector)
				{
					FSD("+++ updating min sector 0x%x to ", minSector);
					minSector = getLe32(&pent->sector);
					FSD("0x%x\n", minSector);
				}
				if ((pent->entType & 0x10) == 0x10) // its a directory entry
				{
					if (getLe32(&pent->size) != 0) // it is not an empty directory
					{
						FSD("> recursive parse for subdir %s (size 0x%x)\n", temOutName, getLe32(&pent->size));
						getMaxDirectory(pent);

					}
					else
						FSD("> subdir %s is empty!\n", temOutName);

				}
				else
				{
					FSD("file: %s sector 0x%x size 0x%x ssize 0x%x\n", temOutName, getLe32(&pent->sector), getLe32(&pent->size), ssize);
					if (pent->nameSz == 11)
					{
						if (strnicmp(pent->name, XEX_FILE_NAME, 11) == 0)
						{
							defaultXexSize = getLe32(&pent->size);
							defaultXexSector = getLe32(&pent->sector);
							printf("default.xex found at sector 0x%x size 0x%x\n", defaultXexSector, defaultXexSize);
						}
					}
				}
				currOff = currOff + ((pent->nameSz + 0x11) &~ 3); // next PXISO_ENTRYVAL in the directory is 4 byte aligned, and min 0x11 bytes in size 
			}
		}
		ret = TRUE;
	}
	else
		printf("unable to read 0x%x bytes from sector 0x%x!\n", size, xent->sector);
	free(buf);
	setIsoSector(SVOD_START_SECTOR);
	return ret;
}

BOOL getDiskEnd(VOID)
{
	BOOL ret = FALSE;
	XISO_ENTRYVAL baseEntry;
	defaultXexSector = 0;
	defaultXexSize = 0;
	maxSector = rootDirSector + ((rootDirSize + 0x7ff) &~0x7FF);
	if (maxSector < 0x108)
		maxSector = 0x108;
	minSector = rootDirSector;
	baseEntry.sector = rootDirSector;
	baseEntry.size = rootDirSize;
	ret = getMaxDirectory(&baseEntry);
	if (ret)
	{
		minSector = minSector & ~1; // needs to be rounded down to compensate for 0x1000 byte sector tracking
		maxSector = (maxSector + 1) & ~1; // needs to be rounded up to compensate for 0x1000 byte sector tracking
	}
	return ret;
}

BOOL dumpDefaultXex(void)
{
	BOOL ret = FALSE;
	u8* buf;
	u32 bufsz = (defaultXexSize + 0x7ff)&~0x7FF;
	if ((defaultXexSector == 0) || (defaultXexSize == 0))
	{
		printf("cannot dump default.xex, it was not found during parse!\n");
		return FALSE;
	}
	buf = (u8*)malloc(bufsz);
	if (buf != NULL)
	{
		if (setIsoSector(defaultXexSector))
		{
			if (readIsoData(buf, bufsz))
			{
				ret = getXexInfoData(buf, defaultXexSize);

				//dump_buffer_hex("default.xex", buf, defaultXexSize);
				//ret = TRUE;
			}
			else
				printf("error reading default.xex from iso!\n");
		}
		else
			printf("error allocating buffer for default.xex!\n");
		free(buf);
	}
	else
		printf("error allocating buffer for default.xex!\n");
	setIsoSector(SVOD_START_SECTOR);
	return ret;
}



// each data file has a master table of 0x1000 bytes
// this table has 0xCC hash entries, 0xCB are for subsequent hash tables, the last hash is the hash of the next file's first table
// each of the 0xCB sub tables has 0xCC hash entires, corresponding to a 0x1000 (2 sector) data block
// each block represents a max data payload of 0xCB000, each file a max data payload of 0xA1C4000
// each file is A290000
// 0xCB blocks is A28F000 (with hashes)
// each block is CD000
// A290000*0x1b + a89000 = 112FB9000 (1c files)
VOID calcDataFileSizes(VOID)
{
	u32 dataSectors = (((maxSector - minSector) + 1) / 2) + 1; // round up to 0x1000 sector size, +1 for the first 0x1000 being the XISO header
	u32 dataBlocks;
	u32 fileCnt;
	u64 instSz = dataSectors;
	instSz = instSz * (XGD_SECTOR_SIZE*2);
	//printf("data size      : %016I64x\n", instSz);
	//printf("data sectors   : %08x\n", dataSectors);
	dataBlocks = dataSectors / 0xCC;
	if ((dataSectors % 0xCC) != 0)
		dataBlocks++;
	//printf("data blocks    : %08x\n", dataBlocks);
	fileCnt = dataBlocks / 0xCB;
	if ((dataBlocks % 0xCB) != 0)
		fileCnt++;
	printf("file count     : %08x\n", fileCnt);
	xMeta->DataFiles = bswap32(fileCnt);

	instSz = ((u64)fileCnt) * 0x1000; // file hashes
	instSz += ((u64)dataSectors) * 0x1000; // data payload
	instSz += ((u64)dataBlocks) * 0x1000; // block hashes
	printf("install filesize: %016I64x\n", instSz);

	xMeta->DataFilesSize = bswap64(instSz);
}

VOID calcDataBlocks(VOID)
{
	u32 rootXstrt = (minSector / 2); 
	s64 rootXsz;
	s64 instSz = maxSector;
	instSz = instSz * XGD_SECTOR_SIZE;
	printf("Disk min sector: %08x\n", minSector);
	printf("Disk max sector: %08x\n", maxSector);
	rootXsz = ((maxSector - minSector) / 2);// 1 extra sector for the MICROSOFT* 0x1000 bytes at very start
	instSz = (s64)(maxSector - minSector + 2);
	instSz = instSz * XGD_SECTOR_SIZE; // divide this by 0x1000 for the 3 byte value in xiso header, NumberOfDataBlocks
	printf("install size   : %08x sectors (size %016I64x)\n", maxSector - minSector + 2, instSz);
	printf("start data blk : %08x\n", minSector);
	printf("NumberOfDataBlocks: %06llx\n", rootXsz);
	printf("StartingDataBlock : %06x\n", rootXstrt);
	// a = (minSector) / 2
	xMeta->Volume.SvodVolumeDescriptor.StartingDataBlock2 = (rootXstrt >> 16) & 0xFF;
	xMeta->Volume.SvodVolumeDescriptor.StartingDataBlock1 = (rootXstrt >> 8) & 0xFF;
	xMeta->Volume.SvodVolumeDescriptor.StartingDataBlock0 = rootXstrt & 0xFF;
	// (maxSector - minSector)/2 = a ; size in 0x1000 byte chunks instead of XGD_SECTOR_SIZE
	xMeta->Volume.SvodVolumeDescriptor.NumberOfDataBlocks2 = (rootXsz >> 16) & 0xFF;
	xMeta->Volume.SvodVolumeDescriptor.NumberOfDataBlocks1 = (rootXsz >> 8) & 0xFF;
	xMeta->Volume.SvodVolumeDescriptor.NumberOfDataBlocks0 = rootXsz & 0xFF;
}



static SVOD_LEVEL1_HASH_BLOCK lv1hash; // first 0x1000 bytes of a file
static SVOD_LEVEL0_BACKING_BLOCKS datablock; // segments in a file
#define SVOD_MAX_DATA_BLOCK_SIZE			0xCC000
#define SVOD_MAX_DATA_BLOCK_COUNT			0xCB
#define SVOD_DISK_SECTORS_PER_DATA_BLOCK	(SVOD_MAX_DATA_BLOCK_SIZE / XGD_SECTOR_SIZE)// the number of 0x800 byte sectors in a data block

void doBlockHashes(int num)
{
	int i;
	for (i = 0; i < num; i++)
	{
		XeCryptSha(&datablock.DataBlocks[i * 0x1000], 0x1000, NULL, 0, NULL, 0, datablock.Level0HashBlock.Entries[i].Hash, XECRYPT_SHA_DIGEST_SIZE);
	}
}

void doLevel1Hash(int num)
{
	XeCryptSha(datablock.Level0HashBlock.Entries[0].Hash, 0x1000, NULL, 0, NULL, 0, lv1hash.Entries[num].Hash, XECRYPT_SHA_DIGEST_SIZE);
}

FILE* startNewFile(int num)
{
	FILE* fptr;
	char curFileName[MAXPATHLEN];
	sprintf_s(curFileName, MAXPATHLEN, "%sData%04d", baseOutPath, num);
	printf("creating file %s\n", curFileName);
	fptr = fopen(curFileName, "wb+");
	if (fptr != NULL)
	{
		memset(&lv1hash, 0, 0x1000);
		// just a dummy at open... gotta go back and write it in again later
		if (fwrite(&lv1hash, 0x1000, 1, fptr) != 1)
		{
			fclose(fptr);
			fptr = NULL;
			printf("\n\n***** ERROR: could not write header to file %s! (error %d)\n", curFileName, GetLastError());
		}
	}
	else
		printf("\n\n***** ERROR: could not create file %s! (error %d)\n", curFileName, GetLastError());
	return fptr;
}

BOOL writeBlockToFile(FILE* fptr, u32 sectorsHashed)
{
	u32 size = (sectorsHashed * 0x1000) + 0x1000;
	if (fwrite(&datablock, size, 1, fptr) != 1)
	{
		printf("\n\n***** ERROR: could not write data to file! (error %d)\n", GetLastError());
	}
	return TRUE;
}

BOOL writeHeaderToFile(FILE* fptr)
{
	rewind(fptr);
	if (fwrite(&lv1hash, 0x1000, 1, fptr) != 1)
	{
		printf("\n\n***** ERROR: could not rewrite header to data file! (error %d)\n", GetLastError());
		return FALSE;
	}
	return TRUE;
}

int writeIsoBody(void)
{
	int curFileNum = 0;
	int curBlock = 0;
	int curOff = 0x1000; // current offset inside the current hash block's data
	u32 sectorsRem = maxSector - minSector;
	FILE* curFile = NULL;

	printf("dumping 0x%x sectors from iso\n", sectorsRem + 2);
	curFile = startNewFile(curFileNum);
	if (curFile == NULL)
		return 0;
	setIsoSector(SVOD_START_SECTOR); // set to MICROSOFT* data
	readIsoData(datablock.DataBlocks, 0x1000); // read MICROSOFT* data
	setIsoSector(minSector); //set to the start of the first iso data on the disk
	while (sectorsRem)
	{
		u32 dataToRead, sectorsToRead, sectorsToHash;
		if (sectorsRem >= (u32)((SVOD_MAX_DATA_BLOCK_SIZE - curOff) / XGD_SECTOR_SIZE))
			dataToRead = (SVOD_MAX_DATA_BLOCK_SIZE - curOff);
		else
			dataToRead = sectorsRem * XGD_SECTOR_SIZE;
		sectorsToRead = dataToRead / XGD_SECTOR_SIZE;
		sectorsToHash = (sectorsToRead + 1) / 2;
		if (curOff != 0)
		{
			sectorsToHash += curOff / (XGD_SECTOR_SIZE * 2);
		}
		// fill the block as much as needed
		if (readIsoData(&datablock.DataBlocks[curOff], dataToRead) == FALSE)
		{
			printf("\n\nError reading ISO data!\n");
			return 0;
		}
		curOff = 0;
		// update the blocks hashes
		doBlockHashes(sectorsToHash);
		sectorsRem -= sectorsToRead;
		// update the master hash table
		doLevel1Hash(curBlock);
		curBlock++;
		// write block to file
		writeBlockToFile(curFile, sectorsToHash);
		memset(&datablock, 0, sizeof(SVOD_LEVEL0_BACKING_BLOCKS));
		// check if at end of current file bounds
		if ((curBlock == SVOD_MAX_DATA_BLOCK_COUNT) || (sectorsRem == 0))
		{
			writeHeaderToFile(curFile);
			fclose(curFile);
			if (sectorsRem != 0)
			{
				curFileNum++;
				curFile = startNewFile(curFileNum);
				if (curFile == NULL)
					return 0;

			}
			curBlock = 0;
		}
	}
	return curFileNum;
}

FILE* getHeaderData(int num)
{
	FILE* fptr;
	char curFileName[MAXPATHLEN];
	sprintf_s(curFileName, MAXPATHLEN, "%sData%04d", baseOutPath, num);
	printf("opening file %s\n", curFileName);
	fptr = fopen(curFileName, "rb+");
	if (fptr != NULL)
	{
		if (fread(&lv1hash, 0x1000, 1, fptr) != 1)
		{
			fclose(fptr);
			fptr = NULL;
			printf("\n\n***** ERROR: could not read header from file %s! (error %d)\n", curFileName, GetLastError());
		}
	}
	else
		printf("\n\n***** ERROR: could not open file %s! (error %d)\n", curFileName, GetLastError());
	return fptr;
}

// starting with the last file, and working backwards we must update the hashes in the first 0x1000 bytes of the file
BOOL fixIsoHeaders(int lastFile)
{
	int i;
	u8 hashInfo[XECRYPT_SHA_DIGEST_SIZE];
	memset(hashInfo, 0, XECRYPT_SHA_DIGEST_SIZE);
	for (i = lastFile; i >= 0; i--)
	{
		FILE* curFile = getHeaderData(i);
		if (curFile != NULL)
		{
			if (i != lastFile)
				memcpy(lv1hash.NextFragmentHashEntry.Hash, hashInfo, XECRYPT_SHA_DIGEST_SIZE);
			XeCryptSha(lv1hash.Entries[0].Hash, 0x1000, NULL, 0, NULL, 0, hashInfo, XECRYPT_SHA_DIGEST_SIZE);
			if (i != lastFile)
				writeHeaderToFile(curFile);
			fclose(curFile);
		}
		else
			return FALSE;
	}
	memcpy(xMeta->Volume.SvodVolumeDescriptor.FirstFragmentHashEntry.Hash, hashInfo, XECRYPT_SHA_DIGEST_SIZE);
	return TRUE;
}

BOOL isoToSvod(void)
{
	int numFiles;
	numFiles = writeIsoBody();
	printf("wrote body data of %d files! Fixing header hashes...\n", numFiles+1);
	if (numFiles != 0)
	{
		fixIsoHeaders(numFiles);
		return TRUE;
	}
	return FALSE;
}

BOOL openIsoFile(char* fname)
{
	BOOL ret = FALSE;
	isoFile = fopen(fname, "rb");
	if (isoFile == NULL)
		return FALSE;
	maxOffset = getFileSize64(isoFile);
	if (getXgdBaseOffset())
	{
		SYSTEMTIME tm;
		s64 offset;
		printf("iso opened OK\n");
		printf("max offset: %016I64x\n", maxOffset);
		printf("XGD offset: %016I64x\n", baseOffset);
		offset = (rootDirSector*rootDirSize) + baseOffset;
		printf("root sect : %08x (iso: %016I64x)\n", rootDirSector, offset); // rootDirSector/2 for the 3 byte value in xiso header, startingDataBlock
		printf("sect size : %08x\n", rootDirSize);
		FileTimeToSystemTime(&creationTime, &tm);
		printf("created   : %d/%d/%d %2.2d:%2.2d\n", tm.wMonth, tm.wDay, tm.wYear, tm.wHour, tm.wMinute);
		if (getDiskEnd())
		{
			calcDataBlocks();
			calcDataFileSizes();
			ret = dumpDefaultXex();
		}
		else
			printf("ERROR! something went wrong finding the last data on the disk!\n");
	}
	else
		printf("iso open failed! file: %s\n", fname);

	return ret;
}

VOID closeIsoFile(void)
{
	if (isoFile != NULL)
		fclose(isoFile);
	isoFile = NULL;
}


