#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "util.h"
#include "xexinfo.h"
#include "xexheader.h"
#include "xecrypt.h"
#include "xexInflate.h"
#include "keys.h"
#include "spa.h"
#include "xcontent.h"

extern PXCONTENT_HEADER xHeader;
extern PXCONTENT_METADATA xMeta;
extern BYTE xexMediaId[0x10];


u8* getXexHeaderField(u8* xexbuf, u32 field, int numEntries)
{
	int i;
	// first pair is at 0x14, first dword is description, second is val or offset depending on field type
	u8* curBuf = &xexbuf[sizeof(IMAGE_XEX_HEADER)];
// 	printf("finding field %08x in %x entries\n", field, numEntries);
	for (i = 0; i < numEntries; i++)
	{
// 		printf("%d:%x\n", i, getBe32(curBuf));
		if (getBe32(curBuf) == field)
			return curBuf;
		curBuf += 8;
	}
	return NULL;
}

// ***1 - key: 000002 sz:FF (255 bytes) val:00002374  _HEADER_SECTION_TABLE		XEX_HEADER_SECTION_TABLE
//         size: 0x00000024 (36) entries: 2 entry size: 16
//         "425307DB" VAddr: 83555A80 Vsize: 0007AF72 (503666)
//         "A2M_INI " VAddr: 835E0000 Vsize: 00000761 (1889)




// ***11 - key: 000400 sz:06 (024 bytes) val:000024C4  _HEADER_EXECUTION_ID		XEX_HEADER_EXECUTION_ID
BOOL getXexExecInfo(PIMAGE_XEX_HEADER xhdr, int len) // dumb version or imageXexHeaderField
{
	PXEX_EXECUTION_ID pxid;
	u32 tmp32;
	u8* curBuf;
// 	printf("parse field\n");
	curBuf = getXexHeaderField((u8*)xhdr, XEX_HEADER_EXECUTION_ID, bswap32(xhdr->HeaderDirectoryEntryCount));
	if (curBuf == NULL)
	{
		printf("error finding execution ID in default.xex!\n");
		return FALSE;
	}
// 	printf("get offset hdr: %x curbuf: %x\n", xhdr, curBuf);
	tmp32 = getBe32(&curBuf[4]); // offset
	if (tmp32 > (len - sizeof(XEX_EXECUTION_ID)))
	{
		printf("error execution ID is out of bounds!\n");
		return FALSE;
	}
	curBuf = (u8*)xhdr;
	pxid = (PXEX_EXECUTION_ID)&curBuf[tmp32];
	memcpy(&xMeta->ExecutionId, pxid, sizeof(XEX_EXECUTION_ID));
	printf("XEX Execution ID:\n");
	printf("\tMediaID    : 0x%x\n", bswap32(xMeta->ExecutionId.MediaID));
	printf("\tTitleID    : 0x%x\n", bswap32(xMeta->ExecutionId.Tid.TitleID));
	tmp32 = bswap32(xMeta->ExecutionId.Version);
	printf("\tVersion    : %08x (%ld.%ld.%04ld.%02ld)\n", tmp32, (tmp32 >> 28) & 0xF, (tmp32 >> 24) & 0xF, (tmp32 >> 8) & 0xFFFF, tmp32 & 0xFF); // 2022FB00 = 2.0.8955.00
	tmp32 = bswap32(xMeta->ExecutionId.BaseVersion);
	printf("\tBaseVersion: %08x (%ld.%ld.%04ld.%02ld)\n", tmp32, (tmp32 >> 28) & 0xF, (tmp32 >> 24) & 0xF, (tmp32 >> 8) & 0xFFFF, tmp32 & 0xFF); // 2022FB00 = 2.0.8955.00
	printf("\tDisk       : %d of %d\n", xMeta->ExecutionId.DiscNum, xMeta->ExecutionId.DiscsInSet);

	return TRUE;
}

VOID xexDoDecrypt(u8* pXexKey, u8* pImageKey, u8* pIn, u8* pOut, u32 len)
{
	XECRYPT_AES_STATE ctx1;
	XECRYPT_AES_STATE ctx2;
	unsigned char tempkey[16];
	unsigned char aesFeed[16];

	memset(aesFeed, 0, 16);
	XeCryptAesKey(&ctx1, pXexKey);
	XeCryptAesCbc(&ctx1, pImageKey, 0x10, tempkey, aesFeed, FALSE); // decrypt the image key

	memset(aesFeed, 0, 16);
	XeCryptAesKey(&ctx2, tempkey);
	XeCryptAesCbc(&ctx2, pIn, len, pOut, aesFeed, FALSE);// use it to decrypt the data
}

BOOL xexCheckSignature(PXEX_SECURITY_INFO xsec, int len)
{
	u8* dat;
	u8* salt = XexSalt;
	u32 datSz;
	u8 hashBuf[0x14];
	if (bswap32(xsec->ImageInfo.ImageFlags) & XEX_SECURITY_FLAG_REQUIRE_COOKIE)
		salt = RevXexSalt;
	dat = (u8*)&xsec->ImageInfo.InfoSize;
	datSz = bswap32(xsec->ImageInfo.InfoSize) - 0x100;
	XeCryptRotSumSha(dat, datSz, NULL, 0, hashBuf, 0x14);
	printf("signature  : ");
	if (XeCryptBnQwBeSigVerify((PXECRYPT_SIG)xsec->ImageInfo.Signature, hashBuf, salt, (PXECRYPT_RSA)PirsRsaPubKeyDev))
		printf("devkit signed\n");
	else if (XeCryptBnQwBeSigVerify((PXECRYPT_SIG)xsec->ImageInfo.Signature, hashBuf, salt, (PXECRYPT_RSA)PirsRsaPubKey))
		printf("retail signed\n");
	else
		printf("unsigned\n");
	return TRUE;

}

DWORD xexGetSpaOffset(PIMAGE_XEX_HEADER xhdr, char* sectionName, DWORD* spaLen)
{
	u8* curBuf = getXexHeaderField((u8*)xhdr, XEX_HEADER_SECTION_TABLE, xhdr->HeaderDirectoryEntryCount);
	*spaLen = 0;
	if (curBuf != NULL)
	{
		PXEX_SECTION_INFO xsect;
		u32 tmp32, numEnt, i;
		tmp32 = getBe32(&curBuf[4]);
		curBuf = (u8*)xhdr;
		xsect = (PXEX_SECTION_INFO)&curBuf[tmp32];
		numEnt = (bswap32(xsect->Size) - 4) / sizeof(XEX_SECTION_HEADER);
		for (i = 0; i < numEnt; i++)
		{
			if (strnicmp(sectionName, xsect->Section[i].SectionName, 8) == 0)
			{
				*spaLen = bswap32(xsect->Section[i].VirtualSize);
				return bswap32(xsect->Section[i].VirtualAddress);
			}
		}

	}
	printf("unable to find spa offset!\n");
	return 0;
}

// decrypt, decompress, get SPA info
BOOL xexGetSpaInfo(PIMAGE_XEX_HEADER xhdr, PXEX_SECURITY_INFO xsec, int len)
{
	PXEX_FILE_DATA_DESCRIPTOR fdesc;
	BOOL isXexMemGood = FALSE;
	BOOL retVal = FALSE;
	int i;
	u32 tmp32;
	u8* outbuf = NULL;
	u8* curBuf = getXexHeaderField((u8*)xhdr, XEX_FILE_DATA_DESCRIPTOR_HEADER, xhdr->HeaderDirectoryEntryCount);
	outbuf = (u8*)malloc(bswap32(xsec->ImageSize));
	if (curBuf == NULL)
	{
		printf("error finding file data descriptor in default.xex!\n");
		return FALSE;
	}
	if (outbuf == NULL)
	{
		printf("unable to allocate buffer for decrypt/decompress output!\n");
		return FALSE;
	}
	memset(outbuf, 0, bswap32(xsec->ImageSize));

	tmp32 = getBe32(&curBuf[4]); // offset
	curBuf = (u8*)xhdr;
	fdesc = (PXEX_FILE_DATA_DESCRIPTOR)&curBuf[tmp32];
// 	printf("data descriptor found at 0x%x\n", (u8*)fdesc - (u8*)xhdr);
	xexCheckSignature(xsec, len);

	tmp32 = bswap16(fdesc->Flags);
	curBuf = (u8*)xhdr;
	curBuf = &curBuf[bswap32(xhdr->SizeOfHeaders)];
	if (tmp32 & XEX_DATA_FLAG_ENCRYPTED)
	{
		printf("decrypting xex...");
		//printf("xex is encrypted, flags 0x%x\n", bswap32(xsec->ImageInfo.ImageFlags));
		//printf("len: 0x%x sizeofheaders 0x%x\n", len, bswap32(xhdr->SizeOfHeaders));
		if (bswap32(xsec->ImageInfo.ImageFlags) & XEX_SECURITY_FLAG_MFG_SUPPORT)
			xexDoDecrypt(Xex1Key, xsec->ImageInfo.ImageKey, curBuf, curBuf, len - bswap32(xhdr->SizeOfHeaders));
		else
			xexDoDecrypt(Xex2Key, xsec->ImageInfo.ImageKey, curBuf, curBuf, len - bswap32(xhdr->SizeOfHeaders));
		printf("done!\n");
	}

	tmp32 = bswap16(fdesc->Format);
	if (tmp32 == XEX_DATA_FORMAT_RAW)
	{
		printf("parsing xex raw data into memory image layout\n");
		int cnt = (bswap32(fdesc->Size) - 8) / sizeof(XEX_RAW_DATA_DESCRIPTOR); // parse the raw data descriptors for conrol info
		tmp32 = 0;
// 		printf("processing %d raw records\n", cnt);
		for (i = 0; i < cnt; i++)
		{
			DWORD rds = bswap32(fdesc->fmt.raw[i].DataSize);
			DWORD zds = bswap32(fdesc->fmt.raw[i].ZeroSize);
			if (rds)
			{
				memcpy(&outbuf[tmp32], curBuf, rds);
				curBuf = &curBuf[rds];
				tmp32 += rds;
			}
			if (zds)
			{
				tmp32 += zds;
			}
		}
		isXexMemGood = TRUE;
// 		dump_buffer_hex("outbase.bin", outbuf, bswap32(xsec->ImageSize));
	}
	else if (tmp32 == XEX_DATA_FORMAT_COMPRESSED)
	{
		DWORD inSz = len - bswap32(xhdr->SizeOfHeaders);
		DWORD outSz = bswap32(xsec->ImageSize);
		DWORD winSz = bswap32(fdesc->fmt.compressed.WindowSize);
		DWORD firstSz = bswap32(fdesc->fmt.compressed.FirstDescriptor.Size);
		curBuf = (u8*)xhdr;
		curBuf = &curBuf[bswap32(xhdr->SizeOfHeaders)];
		printf("decompressing xex...");
		if (unpackXexData(curBuf, inSz, outbuf, outSz, winSz, firstSz))
		{
			printf("success!\n");
			isXexMemGood = TRUE;
		}
		else
			printf("failed!\n");

// 		dump_buffer_hex("outbase.inf.bin", outbuf, outSz);
		printf("done!\n");
	}
	else
	{
		printf("ERROR: unknown xex data format 0x%x\n", tmp32);
	}

	if (isXexMemGood) // process xbdf
	{
		DWORD spaOff = 0, spaLen = 0;
		char sectionName[32];
		sprintf_s(sectionName, 16, "%X", bswap32(xMeta->ExecutionId.Tid.TitleID));
		spaOff = xexGetSpaOffset(xhdr, sectionName, &spaLen);
		if (spaOff != 0)
		{
			spaOff = spaOff - bswap32(xsec->ImageInfo.LoadAddress);
			if ((spaOff+spaLen) <= bswap32(xsec->ImageSize))
			{
				retVal = getSpaInfo(&outbuf[spaOff], spaLen);
			}
			else
			{
				printf("error, spaOff does not seem to be inside the image!\n");
			}
		}
	}

	free(outbuf);
	return retVal;
}

BOOL getXexInfo(u8* buf, int len)
{
	PIMAGE_XEX_HEADER xhdr = (PIMAGE_XEX_HEADER)buf;
	PXEX_SECURITY_INFO xsec;
	
	if (bswap32(xhdr->Magic) != XEX_HEADER_MAGIC)
	{
		printf("incorrect xex header magic!\n");
		return FALSE;
	}
	if (bswap32(xhdr->SecurityInfo) > (len - sizeof(XEX_SECURITY_INFO)))
	{
		printf("incorrect xex security info offset!\n");
		return FALSE;
	}
	xsec = (PXEX_SECURITY_INFO)&buf[bswap32(xhdr->SecurityInfo)];
	display_buf("Media ID: ", xsec->ImageInfo.MediaID, 0x10);
	memcpy(xexMediaId, xsec->ImageInfo.MediaID, 0x10);
	if (getXexExecInfo(xhdr, len))
		xexGetSpaInfo(xhdr, xsec, len);
	return TRUE;
}

void getXexInfoFile(char* xexpath)
{
	int len = 0;
	u8* dat = readFileToBuf(xexpath, &len);
	if (dat != NULL)
	{
		printf("read %s to 0x%x buf\n", xexpath, len);
		getXexInfo(dat, len);
		free(dat);
	}

}

BOOL getXexInfoData(u8* xexData, int len)
{
	return getXexInfo(xexData, len);
}
