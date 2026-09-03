#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "xecrypt.h"
#include "util.h"
#include "xcontent.h"

u32 ttlFilesChecked = 0;
u32 ttlFilesOk = 0;
u32 ttlConChecked = 0;
u32 ttlConOk = 0;
BOOL copyToFolder = FALSE;
char outbasepath[MAXPATHLEN];
char outfilepath[MAXPATHLEN];

#define MANIFEST_BASE "$SystemUpdate\\"
#define SU_PACKAGE	  "su20076000_00000000"
#define PACKAGE_BASE "Content\\0000000000000000\\" //FFFE07DF\\00008000"
#define MAGIC_XMAN ((u32)0x786d616e) // 'xman' - version 1 manifest
#define MAGIC_XSYM ((u32)0x7873796D) // 'xsym' - version 3 manifest
#define MAGIC_XONM ((u32)0x786F6E6D) // 'xonm' - version 3 manifest
#define MAGIC_XMNP ((u32)0x584D4E50) // 'XMNP'

#pragma pack(push, 1)
typedef struct _SU_MAINIFEST_HEADER {
	DWORD dwMagic; // 0 'XMNP'
	BYTE baUnk1[0x20]; // 0x4
	BYTE baDataSha[0x14]; // 0x24
	BYTE baSignature[0x100]; // 0x38
}SU_MAINIFEST_HEADER, *PSU_MAINIFEST_HEADER; // sz 0x138
C_ASSERT(sizeof(SU_MAINIFEST_HEADER) == 0x138);

typedef struct _SU_CONTENT_HEADER {
	DWORD dwMagic; // 0x0 'xsym' or 'xonm'
	DWORD size; // 0x4
	DWORD unk1; // 0x8 must be 1?
	DWORD unk2; // 0xC
	DWORD dwSchemaVer; // 0x10
	DWORD dwFlashVer; // 0x14
	QWORD qwTimeStamp; // 0x18
	BYTE pad[8]; // 0x20
} SU_CONTENT_HEADER, *PSU_CONTENT_HEADER; // sz 0x28
C_ASSERT(sizeof(SU_CONTENT_HEADER) == 0x28);

typedef struct _SU_ITEM_ENTRY {
	DWORD dwSrcNameOff; // 0 source name
	DWORD dwPad1; // 0x4
	DWORD dwFileSize; // 0x8
	DWORD dwVersionOff; // 0xC pointer to version, used for folder on non-containers
	DWORD dwFlags; // 0x10 flags?
	DWORD dwPad2; // 0x14
	DWORD dwEntryType; // 0x18
	DWORD dwIntNameOff; // 0x1C internal name?
	DWORD dwDestNameOff; // 0x20 destination name
	BYTE unkHash[0x14]; // 0x24
	BYTE dataHash[0x14]; // 0x38
	DWORD dwContentType; // 0x4C ie 0x00008000
	DWORD dwContentTitleId; // 0x50 ie 0xFFFE07DF
	DWORD dwPad3; // 0x54
	DWORD dwPad4; // 0x58
	DWORD dwXContItemListOff; // 0x5C num items in container, followed by entry offsets for each item
} SU_ITEM_ENTRY, *PSU_ITEM_ENTRY; // sz 0x60
C_ASSERT(sizeof(SU_ITEM_ENTRY) == 0x60);

typedef struct _SU_ITEM_ENTRY_V1 {
	DWORD dwSrcNameOff; // 0 source name
	DWORD dwPad1; // 0x4
	DWORD dwFileSize; // 0x8
	DWORD dwVersionOff; // 0xC pointer to version, used for folder on non-containers
	DWORD dwFlags; // 0x10 flags?
	DWORD dwContentType; // 0x14 ie 0x00008000
	DWORD dwContentTitleId; // 0x18 ie 0xFFFE07DF
	BYTE dwPad2[8];
	DWORD dwIntNameOff;
	DWORD dwDestNameOff;
	DWORD dwPad3;
	DWORD dwXContItemListOff;
	BYTE dwPad4[16];
} SU_ITEM_ENTRY_V1, *PSU_ITEM_ENTRY_V1; // sz 0x44
C_ASSERT(sizeof(SU_ITEM_ENTRY_V1) == 0x44);

typedef struct _SU_ITEM_LIST_V1 {
	DWORD dwSizeOfStruct;
	DWORD dwNumEntries;
	SU_ITEM_ENTRY_V1 ents[1];
} SU_ITEM_LIST_V1, *PSU_ITEM_LIST_V1; // sz 0x68
C_ASSERT(sizeof(SU_ITEM_LIST_V1) == 0x4C);

typedef struct _SU_MANIFEST_BODY_V1 {
	SU_CONTENT_HEADER cont;
	SU_ITEM_LIST_V1 items;
} SU_MANIFEST_BODY_V1, *PSU_MANIFEST_BODY_V1;

typedef struct _SU_MANIFEST_V1 {
	SU_MAINIFEST_HEADER header;
	SU_MANIFEST_BODY_V1 body;
} SU_MANIFEST_V1, *PSU_MANIFEST_V1;

typedef struct _SU_ITEM_LIST {
	DWORD dwSizeOfStruct;
	DWORD dwNumEntries;
	SU_ITEM_ENTRY ents[1];
} SU_ITEM_LIST, *PSU_ITEM_LIST; // sz 0x68
C_ASSERT(sizeof(SU_ITEM_LIST) == 0x68);

typedef struct _SU_MANIFEST_BODY {
	SU_CONTENT_HEADER cont;
	SU_ITEM_LIST items;
} SU_MANIFEST_BODY, *PSU_MANIFEST_BODY;

typedef struct _SU_MANIFEST {
	SU_MAINIFEST_HEADER header;
	SU_MANIFEST_BODY body;
} SU_MANIFEST, *PSU_MANIFEST;
#pragma pack(pop)

BOOL verifyManifestHeader(PSU_MANIFEST man)
{
	BOOL ret = TRUE;
	u8 shaBuf[0x14];
	u32 ver = bswap32(man->body.cont.dwSchemaVer);
	if (ver == 1)
	{
		if ((bswap32(man->body.cont.dwMagic) != MAGIC_XMAN))
		{
			printf("manifest revision 1 magic 0x%08x != xman!\n", bswap32(man->body.cont.dwMagic));
			return FALSE;
		}
	}
	else if (ver == 3)
	{
		if ((bswap32(man->body.cont.dwMagic) != MAGIC_XSYM) && (bswap32(man->body.cont.dwMagic) != MAGIC_XONM))
		{
			printf("manifest revision 3 magic 0x%08x != xonm|xsym!\n", bswap32(man->body.cont.dwMagic));
			return FALSE;
		}
	}
	else
	{
		printf("manifest schema %08x != 3 or 1!\n", ver);
		return FALSE;
	}

	if(bswap32(man->header.dwMagic) != MAGIC_XMNP)
	{
		printf("manifest magic 0x%08x != XMNP!\n", bswap32(man->header.dwMagic));
		ret = FALSE;
	}
	XeCryptSha((u8*)&man->body.cont.dwMagic, bswap32(man->body.cont.size), NULL, 0, NULL, 0, shaBuf, 0x14);
	if(memcmp(shaBuf, man->header.baDataSha, 0x14) != 0)
	{
		printf("manifest checksum failed!\n");
		ret = FALSE;
	}
	return ret;
}

int getManifestString(BYTE* sdat, char* dest)
{
	WORD len;
	len = getBe16(sdat);
	if(len != 0)
	{
		memcpy(dest, &sdat[2], len);
		dest[len] = 0x0;
	}
	else
		printf("err len = 0!\n");
	return len;
}

//BYTE dataHash[0x14]; // 0x38
//DWORD dwContentType; // 0x4C ie 0x00008000
//DWORD dwContentTitleId; // 0x50 ie 0xFFFE07DF
//DWORD dwXContItemListOff; // 0x5C num items in container, followed by entry offsets for each item

void verifyFile(char* cfilename, int len, BYTE* hdat)
{
	if(len != 0)
	{
		BYTE* fdata;
		int flen;
		fdata = readFileToBuf(cfilename, &flen);
		if(fdata != NULL)
		{
			if(flen != 0)
			{
				BYTE hashbuf[0x14];
				XeCryptSha(fdata, len, NULL, 0, NULL, 0, hashbuf, 0x14);
				ttlFilesChecked++;
				if (memcmp(hashbuf, hdat, 0x14) == 0)
				{
					printf("file hash check OK\n");
					ttlFilesOk++;
					if (copyToFolder)
						dump_buffer_hex(outfilepath, fdata, flen);
				}
				else
					printf("file corrupt, hash check FAIL!!\n");
			}
			else
				printf("file hash check FAIL! read file is 0 len!\n");
			free(fdata);
		}
		else
			printf("file hash check FAIL! could not read file!\n");
	}
	else
		printf("file hash check FAIL! input len is 0!\n");

}

BOOL conCheckHashes(PXCONTENT_FULL_HEADER xcfh, BYTE* conData, DWORD conDataStart, DWORD conDataLen, u32* blockInfo)
{
	PSTF_DIRECTORY_ENTRY dent;
	int i, j = 0;
	u32 currBlock = 0;
	u8 hashout[XECRYPT_SHA_DIGEST_SIZE];
	XeCryptSha(&conData[sizeof(XCONTENT_HEADER)], conDataStart-sizeof(XCONTENT_HEADER), NULL, 0, NULL, 0, hashout, XECRYPT_SHA_DIGEST_SIZE);
	// 	display_buffer_hex(xcfh->Header.ContentId, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
	// 	display_buffer_hex(hashout, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
	// checking header hash
	if(memcmp(xcfh->Header.ContentId, hashout, XECRYPT_SHA_DIGEST_SIZE) == 0)
	{
		BOOL contProcess = TRUE;
		u32 currOffset = conDataStart+sizeof(STF_HASH_BLOCK);
		PSTF_HASH_BLOCK hb = (PSTF_HASH_BLOCK)&conData[conDataStart];
		PSTF_HASH_BLOCK mhb = NULL;
		dent = (PSTF_DIRECTORY_ENTRY)&conData[currOffset];
		printf("header hash is OK, checking content hashes...\n");
		// 		XeCryptSha(&conData[conDataStart], 0x1000, NULL, 0, NULL, 0, hashout, XECRYPT_SHA_DIGEST_SIZE);
		// 		cprintf(VERB_LV0, "master table hash (0x%x):", conDataStart);
		// 		display_buffer_hex(hashout, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
		// 		cprintf(VERB_LV0, "\n");
		// checking fragment hashes
		while(contProcess)
		{
			for(i = 0; i < 0xAA; i++) // this fragment is complete at 0xAA
			{
				if(checkZeros(&hb->Entries[i], 0x18))
				{
					i = 0xAA;
					contProcess = FALSE;
				}
				else
				{
					XeCryptSha(&conData[currOffset], 0x1000, NULL, 0, NULL, 0, hashout, XECRYPT_SHA_DIGEST_SIZE);
					if(memcmp(hashout, hb->Entries[i].Hash, XECRYPT_SHA_DIGEST_SIZE))
					{
						//cprintf(VERB_LV0, "FAIL entry 0x%x block offset 0x%x level 0x%x\n", i, currOffset, u32Rev(hb->Entries[i].LevelAsULONG));
						//fgetc(stdin);
						return FALSE;
					}
					//else
					//	cprintf(VERB_LV0, "success entry 0x%x block offset 0x%x level 0x%x\n", i, currOffset, u32Rev(hb->Entries[i].LevelAsULONG));
					blockInfo[currBlock] = currOffset;
					currBlock++;
					currOffset += 0x1000;
				}
			}
			if((currOffset+0x2000) >= conDataLen)
			{
				contProcess = FALSE;
			}
			else
			{
				hb = (PSTF_HASH_BLOCK)&conData[currOffset];
				j++;
// 				XeCryptSha(&conData[currOffset], 0x1000, NULL, 0, NULL, 0, hashout, XECRYPT_SHA_DIGEST_SIZE);
// 				cprintf(VERB_LV0, "table %d hash (0x%x):", j, currOffset);
// 				display_buffer_hex(hashout, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
// 				cprintf(VERB_LV0, "\n");

				if(hb->Entries[0].LevelAsULONG == 0)
				{
					mhb = hb; // the main hash block is backed by hash in header, and hashes all the other hash blocks
					XeCryptSha(&conData[currOffset], 0x1000, NULL, 0, NULL, 0, hashout, XECRYPT_SHA_DIGEST_SIZE);
					if(memcmp(xcfh->Meta.Volume.StfsVolumeDescriptor.RootHash, hashout, XECRYPT_SHA_DIGEST_SIZE))
					{
// 						display_buffer_hex(xcfh->Meta.Volume.StfsVolumeDescriptor.RootHash, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
// 						display_buffer_hex(hashout, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
// 						cprintf(VERB_LV0, "FAIL StfsVolumeDescriptor.RootHash\n");
// 						fgetc(stdin);
						return FALSE;
					}

					XeCryptSha(&conData[conDataStart], 0x1000, NULL, 0, NULL, 0, hashout, XECRYPT_SHA_DIGEST_SIZE);
					if(memcmp(hashout, mhb->Entries[0].Hash, XECRYPT_SHA_DIGEST_SIZE))
					{
// 						display_buffer_hex(mhb->Entries[0].Hash, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
// 						display_buffer_hex(hashout, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
// 						cprintf(VERB_LV0, "FAIL mhb->Entries[0]\n");
// 						fgetc(stdin);
						return FALSE;
					}

					currOffset += 0x1000;
					hb = (PSTF_HASH_BLOCK)&conData[currOffset];

				}
				if(mhb != NULL)
				{
					XeCryptSha(&conData[currOffset], 0x1000, NULL, 0, NULL, 0, hashout, XECRYPT_SHA_DIGEST_SIZE);
					if(memcmp(hashout, mhb->Entries[j].Hash, XECRYPT_SHA_DIGEST_SIZE))
					{
						// 						cprintf(VERB_LV0, "FAIL mhb->Entries[%d]\n", j);
						// 						fgetc(stdin);
						return FALSE;
					}
				}

				currOffset += 0x1000;
			}
		}
		//display_buffer_hex(hashout, XECRYPT_SHA_DIGEST_SIZE, VERB_LV0);
		printf("content hashes seem OK, everything looks good!\n");

		return TRUE;
	}
	return FALSE;
}

BOOL conCheckHeader(BYTE* conData, DWORD conDataLen, DWORD expVer, BOOL isSupd)
{
	u32* blockInfo;
	u32 conDataStart;
	PXCONTENT_FULL_HEADER xcfh = (PXCONTENT_FULL_HEADER)conData;
	u32 version;
	conDataStart = XCONTENT_ROUND_UP_TO_ALIGNMENT((bswap32(xcfh->Header.SizeOfHeaders)));//((u32Rev(xcfh->Header.SizeOfHeaders))+0xFFF)&~0xFFF;
	if (isSupd)
		version = bswap32(xcfh->Content.Installer.MetaData.supd.NewVersion);
	else
		version = bswap32(xcfh->Meta.ExecutionId.Version);
	if (expVer != 0)
	{
		if (version != expVer)
		{
			printf("warning, expected container version 0x%x but found instead 0x%x (isSup %d)\n", expVer, version, isSupd);
			return FALSE;
		}
	}
// 	printf("content type: %X\n", u32Rev(xcfh->Meta.ContentType));
// 	printf("content size: %016I64x\n", u64Rev(xcfh->Meta.ContentSize.QuadPart));
// 	printf("content tid : %X\n", u32Rev(xcfh->Meta.ExecutionId.Tid.TitleID));
// 	printf("content ctyp: %X\n", u32Rev(xcfh->Meta.VolumeType));
// 	printf("meta type   : %X\n", u32Rev(xcfh->Content.Installer.MetaDataType));
// 	printf("dataStart   : %X\n", conDataStart);
//	printf("version     : %X (%d.%d.%04d.%02d)\n", version, (version>>28)&0xF, (version>>24)&0xF, (version>>8)&0xFFFF, version&0xFF);
	//if(bswap32(xcfh->Content.Installer.MetaDataType) == INSTALLER_METADATA_TYPE_SYSTEM_UPDATE)
	//{
	//	if(bswap32(xcfh->Meta.ContentType) == 0xB0000)
	//	{
	//		if(bswap32(xcfh->Meta.ExecutionId.Tid.TitleID) == 0xFFFE07D1)
			{
				u32 numBlocks = (conDataLen/0x1000)+1;
				blockInfo = (u32*)malloc((numBlocks*4));
				if(blockInfo != NULL)
				{
					printf("header seems valid, version %d.%d.%04d.%02d\n", (version>>28)&0xF, (version>>24)&0xF, (version>>8)&0xFFFF, version&0xFF);
					if(conCheckHashes(xcfh, conData, conDataStart, conDataLen, blockInfo))
					{
						return TRUE;
					}
				}
			}
	//	}
	//}
	return FALSE;
}

void verifyContainer(char* cfilename, int len, DWORD expVer, BOOL doCopy, BOOL isSupd)
{
	if(len != 0)
	{
		BYTE* fdata;
		int flen;
		fdata = readFileToBuf(cfilename, &flen);
		printf("checking container %s\n", cfilename);
		if(fdata != NULL)
		{
			if(flen != 0)
			{
				ttlConChecked++;
				if (conCheckHeader(fdata, flen, expVer, isSupd))
				{
					printf("container verified as OK\n");
					ttlConOk++;
					if ((doCopy)&&(copyToFolder))
					{
						dump_buffer_hex(outfilepath, fdata, flen);
					}
				}
				else
					printf("container is corrupt!\n");
			}
			else
				printf("file hash check FAIL! read file is 0 len!\n");
			free(fdata);
		}
		else
			printf("file hash check FAIL! could not read file!\n");
	}
	else
		printf("file hash check FAIL! input len is 0!\n");
}


void verifyManifestItems(BYTE* bodydat, int maxlen)
{
	char sbuf[512];
	char srcFile[MAX_PATH];
	char dstFile[MAX_PATH];
	PSU_MANIFEST_BODY bod = (PSU_MANIFEST_BODY)bodydat;
	DWORD i, tmp;
	printf("parsing %d items\n", bswap32(bod->items.dwNumEntries));
	for(i = 0; i < bswap32(bod->items.dwNumEntries); i++)
	{
		PSU_ITEM_ENTRY itm;
		DWORD ver = 0;
		itm = &bod->items.ents[i];
		printf("\n --- item %d of %d ---\n", i+1, bswap32(bod->items.dwNumEntries));
		if(bswap32(itm->dwSrcNameOff) != 0)
		{
			getManifestString(&bodydat[bswap32(itm->dwSrcNameOff)], sbuf);
			printf("\tsource name  : %s\n", sbuf);
			strcpy(srcFile, MANIFEST_BASE);
			strcat(srcFile, sbuf);
		}
		if(bswap32(itm->dwIntNameOff) != 0)
		{
			getManifestString(&bodydat[bswap32(itm->dwIntNameOff)], sbuf);
			printf("\tinternal name: %s\n", sbuf);
		}
		if(bswap32(itm->dwDestNameOff) != 0)
		{
			getManifestString(&bodydat[bswap32(itm->dwDestNameOff)], sbuf);
			printf("\tdest name    : %s\n", sbuf);
		}
		if(bswap32(itm->dwVersionOff) != 0)
		{
			ver = getBe32(&bodydat[bswap32(itm->dwVersionOff)]);
			printf("\tversion      : %d.%d.%d.%d\n", (ver>>28)&0xF, (ver>>24)&0xF, (ver>>8)&0xFFFF, ver&0xF);
		}
		tmp = bswap32(itm->dwEntryType);
		printf("\ttype         : %d ", tmp);
		if(tmp == 2)
			printf("(xcontent)\n");
		else if (tmp == 3)
			printf("(file)\n");
		else
			printf("(unknown)\n");
		printf("\tsize         : 0x%x (%d)\n", bswap32(itm->dwFileSize), bswap32(itm->dwFileSize));
		if(bswap32(itm->dwFlags) != 0)
		{
			tmp = getBe32(&bodydat[bswap32(itm->dwFlags)]);
			printf("\tflags        : 0x%08x\n", tmp);
		}
		if(bswap32(itm->dwEntryType) == 2)
		{
			//DWORD dwContentType; // 0x4C ie 0x00008000
			//DWORD dwContentTitleId; // 0x50 ie 0xFFFE07DF
			//DWORD dwXContItemListOff; // 0x5C num items in container, followed by entry offsets for each item
			printf("\tContentType  : 0x%08x\n", bswap32(itm->dwContentType));
			printf("\tTitleId      : 0x%08x\n", bswap32(itm->dwContentTitleId));
			printf("\tContItemList : 0x%08x\n", bswap32(itm->dwXContItemListOff));
		}
		else
		{
			if(bswap32(itm->dwContentType) != 0)
				printf("\tdwContentType is nonzero, 0x%08x\n", bswap32(itm->dwContentType));
			if(bswap32(itm->dwContentTitleId) != 0)
				printf("\tdwContentTitleId is nonzero, 0x%08x\n", bswap32(itm->dwContentTitleId));
			if(bswap32(itm->dwXContItemListOff) != 0)
				printf("\tdwXContItemListOff is nonzero, 0x%08x\n", bswap32(itm->dwXContItemListOff));
		}

		if(itm->dwPad1 != 0)
			printf("\tdwPad1 is nonzero, 0x%08x\n", bswap32(itm->dwPad1));
		if(itm->dwPad2 != 0)
			printf("\tdwPad2 is nonzero, 0x%08x\n", bswap32(itm->dwPad2));
		if(itm->dwPad3 != 0)
			printf("\tdwPad3 is nonzero, 0x%08x\n", bswap32(itm->dwPad3));
		if(itm->dwPad4 != 0)
			printf("\tdwPad4 is nonzero, 0x%08x\n", bswap32(itm->dwPad4));

		if(bswap32(itm->dwSrcNameOff) != 0)
		{
			getManifestString(&bodydat[bswap32(itm->dwSrcNameOff)], sbuf);
			if(strcmp(sbuf, "flash") != 0)
			{
				getManifestString(&bodydat[bswap32(itm->dwDestNameOff)], sbuf);
				switch(bswap32(itm->dwEntryType))
				{
				case 2: // xcontent
					if (copyToFolder)
					{
						sprintf(outfilepath, "%s\\%s%08x\\%08x\\%s", outbasepath, PACKAGE_BASE, bswap32(itm->dwContentTitleId), bswap32(itm->dwContentType), sbuf);
					}
					verifyContainer(srcFile, bswap32(itm->dwFileSize), ver, TRUE, FALSE);
					sprintf(dstFile, "SEP:\\%s%08x\\%08x\\%s", PACKAGE_BASE, bswap32(itm->dwContentTitleId), bswap32(itm->dwContentType), sbuf);
					break;
				case 3: // file
					if (copyToFolder)
					{
						sprintf(outfilepath, "%s\\%08x", outbasepath, ver);
						createDir(outfilepath, FALSE);
						sprintf(outfilepath, "%s\\%08x\\%s", outbasepath, ver, sbuf);
					}
					verifyFile(srcFile, bswap32(itm->dwFileSize), itm->dataHash);
					sprintf(dstFile, "SEP:\\%08x\\%s", ver, sbuf);
					break;
				default:
					sprintf(dstFile, "~not copied~");
					sprintf(srcFile, "~not copied~");
					break;
				}
				printf("source: Game:\\%s\n", srcFile);
				printf("dest  : %s\n", dstFile);
			}
			else
			{
				strcpy(srcFile, MANIFEST_BASE);
				strcat(srcFile, SU_PACKAGE);
				verifyContainer(srcFile, 1, ver, FALSE, TRUE);
				printf("~no copy~\n");
			}
		}
	}
}

void verifyManifestItemsV1(BYTE* bodydat, int maxlen)
{
	char sbuf[512];
	char srcFile[MAX_PATH];
	char dstFile[MAX_PATH];
	PSU_MANIFEST_BODY_V1 bod = (PSU_MANIFEST_BODY_V1)bodydat;
	DWORD i, tmp;
	printf("parsing %d items\n", bswap32(bod->items.dwNumEntries));
	for (i = 0; i < bswap32(bod->items.dwNumEntries); i++)
	{
		PSU_ITEM_ENTRY_V1 itm;
		DWORD ver = 0;
		itm = &bod->items.ents[i];
		printf("\n --- item %d of %d ---\n", i + 1, bswap32(bod->items.dwNumEntries));
		if (bswap32(itm->dwSrcNameOff) != 0)
		{
			getManifestString(&bodydat[bswap32(itm->dwSrcNameOff)], sbuf);
			printf("\tsource name  : %s\n", sbuf);
			strcpy(srcFile, MANIFEST_BASE);
			strcat(srcFile, sbuf);
		}
		if (bswap32(itm->dwIntNameOff) != 0)
		{
			getManifestString(&bodydat[bswap32(itm->dwIntNameOff)], sbuf);
			printf("\tinternal name: %s\n", sbuf);
		}
		if (bswap32(itm->dwDestNameOff) != 0)
		{
			getManifestString(&bodydat[bswap32(itm->dwDestNameOff)], sbuf);
			printf("\tdest name    : %s\n", sbuf);
		}
		if (bswap32(itm->dwVersionOff) != 0)
		{
			ver = getBe32(&bodydat[bswap32(itm->dwVersionOff)]);
			printf("\tversion      : %d.%d.%d.%d\n", (ver >> 28) & 0xF, (ver >> 24) & 0xF, (ver >> 8) & 0xFFFF, ver & 0xF);
		}

		printf("\tsize         : 0x%x (%d)\n", bswap32(itm->dwFileSize), bswap32(itm->dwFileSize));
		if (bswap32(itm->dwFlags) != 0)
		{
			tmp = getBe32(&bodydat[bswap32(itm->dwFlags)]);
			printf("\tflags        : 0x%08x\n", tmp);
		}
		//DWORD dwContentType; // 0x4C ie 0x00008000
		//DWORD dwContentTitleId; // 0x50 ie 0xFFFE07DF
		//DWORD dwXContItemListOff; // 0x5C num items in container, followed by entry offsets for each item
		printf("\tContentType  : 0x%08x\n", bswap32(itm->dwContentType));
		printf("\tTitleId      : 0x%08x\n", bswap32(itm->dwContentTitleId));
		printf("\tContItemList : 0x%08x\n", bswap32(itm->dwXContItemListOff));


// 		if (itm->dwPad1 != 0)
// 			printf("\tdwPad1 is nonzero, 0x%08x\n", bswap32(itm->dwPad1));
// 		if (itm->dwPad2 != 0)
// 			printf("\tdwPad2 is nonzero, 0x%08x\n", bswap32(itm->dwPad2));
// 		if (itm->dwPad3 != 0)
// 			printf("\tdwPad3 is nonzero, 0x%08x\n", bswap32(itm->dwPad3));
// 		if (itm->dwPad4 != 0)
// 			printf("\tdwPad4 is nonzero, 0x%08x\n", bswap32(itm->dwPad4));

		if (bswap32(itm->dwSrcNameOff) != 0)
		{
			getManifestString(&bodydat[bswap32(itm->dwSrcNameOff)], sbuf);
			if (strcmp(sbuf, "flash") != 0)
			{
				getManifestString(&bodydat[bswap32(itm->dwDestNameOff)], sbuf);
				if (copyToFolder)
				{
					sprintf(outfilepath, "%s\\%s%08x\\%08x\\%s", outbasepath, PACKAGE_BASE, bswap32(itm->dwContentTitleId), bswap32(itm->dwContentType), sbuf);
				}
				verifyContainer(srcFile, bswap32(itm->dwFileSize), ver, TRUE, FALSE);
				sprintf(dstFile, "Hdd:\\%s%08x\\%08x\\%s", PACKAGE_BASE, bswap32(itm->dwContentTitleId), bswap32(itm->dwContentType), sbuf);

				printf("source: Game:\\%s\n", srcFile);
				printf("dest  : %s\n", dstFile);
			}
			else
			{
				strcpy(srcFile, MANIFEST_BASE);
				strcat(srcFile, SU_PACKAGE);
				verifyContainer(srcFile, 1, ver, FALSE, TRUE);
				printf("~no copy~\n");
			}
		}
	}
}

void parseManifest(u8* data, int len)
{
	PSU_MANIFEST man = (PSU_MANIFEST) data;
	if((len > 0x10000)||(len < 0x164))
	{
		printf("manifest size error!\n");
		return;
	}
	if(verifyManifestHeader(man))
	{
		DWORD ver = bswap32(man->body.cont.dwFlashVer);
		u32 mver = bswap32(man->body.cont.dwSchemaVer);

		printf("header ok!\nFlash version: %d.%d.%d.%d\n", (ver>>28)&0xF, (ver>>24)&0xF, (ver>>8)&0xFFFF, ver&0xF);

		if (mver == 1)
		{
			PSU_MANIFEST_V1 man1 = (PSU_MANIFEST_V1)data;
			copyToFolder = FALSE; // I didn't fix this for v1 schema so don't do it
			printf("verify manifest items for schema 1\n");
			verifyManifestItemsV1(&data[sizeof(SU_MAINIFEST_HEADER)], len - sizeof(SU_MAINIFEST_HEADER));
		}
		else if (mver == 3)
		{
			printf("verify manifest items for schema 3\n");
			verifyManifestItems(&data[sizeof(SU_MAINIFEST_HEADER)], len - sizeof(SU_MAINIFEST_HEADER));
		}
		else
			printf("ERROR! CANNOT verify manifest items for schema %d\n", mver);

	}
	else
		printf("manifest header check failed!\n");
}

static char* defaultSysExDirs[] = {
	"%s\\Content",
	"%s\\Content\\0000000000000000",
	"%s\\Content\\0000000000000000\\FFFE07DF",
	"%s\\Content\\0000000000000000\\FFFE07DF\\00008000",
};

void makeDefaultDirs(void)
{
	int i;
	printf("making default directories...\n");
	for (i = 0; i < 4; i++)
	{
		sprintf(outfilepath, defaultSysExDirs[i], outbasepath);
		printf("creating: %s\n", outfilepath);
		createDir(outfilepath, FALSE);
	}
}

int main(int argc, char* argv[])
{
	u8* data;
	int len;
	data = readFileToBuf("$SystemUpdate\\system.manifest", &len);
	if (argc == 2)
	{
		printf("copying flat base to folder %s\n", argv[1]);
		if (createDir(argv[1], TRUE))
		{
			strcpy(outbasepath, argv[1]);
			copyToFolder = TRUE;
			makeDefaultDirs();
		}
	}
	if(data != NULL)
	{
		printf("read 0x%x bytes from file\n", len);
		parseManifest(data, len);
		if (copyToFolder)
		{
			sprintf(outfilepath, "%s\\system.manifest", outbasepath);
			dump_buffer_hex(outfilepath, data, len);
		}
		free(data);
	}
	else
	{
		printf("$SystemUpdate\\system.manifest not found\n");
		if (isFileExist(SU_PACKAGE))
		{
			printf("checking .\\su20076000_00000000 instead\n");
			verifyContainer(SU_PACKAGE, 1, 0, FALSE, TRUE);
		}
		else if (isFileExist(".\\$SystemUpdate\\su20076000_00000000"))
		{
			printf("checking .\\$SystemUpdate\\su20076000_00000000 instead\n");
			verifyContainer(".\\$SystemUpdate\\su20076000_00000000", 1, 0, FALSE, TRUE);
		}
		else
			printf(".\\su20076000_00000000 and .\\$SystemUpdate\\su20076000_00000000 also not found, aborting!\n");
	}
	if ((ttlFilesChecked != 0) || (ttlConChecked != 0))
	{
		if (ttlFilesChecked != 0)
			printf("\nChecked %d files, %d were OK\n", ttlFilesChecked, ttlFilesOk);
		if (ttlConChecked != 0)
			printf("\nChecked %d containers, %d were OK\n", ttlConChecked, ttlConOk);
		printf("\n%d of %d files good!\n", ttlConOk + ttlFilesOk, ttlFilesChecked + ttlConChecked);
	}
	printf("\npress <enter> to quit...\n");
	fgetc(stdin);

	return 0;
}

