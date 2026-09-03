#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#include "types.h"
#include "xecrypt.h"
#include "util.h"
#include "xcontent.h"
#include "xexinfo.h"
#include "isoio.h"

#define PROG_NAME		"iso2svod"
#define TEST_ISO_PATH	"Z:\\test_isos\\wet.iso"
#define TEST_XEX_PATH	"default.xex.enc.compressed"
// #define TEST_XEX_PATH	"dash.xex"
//#define TEST_XEX_PATH	"lhelper.xex"

u8 headerBuffer[0xB000];
PXCONTENT_HEADER xHeader = (PXCONTENT_HEADER)headerBuffer;
PXCONTENT_METADATA xMeta = (PXCONTENT_METADATA)&headerBuffer[0x344];
BYTE xexMediaId[0x10];
char baseOutPath[MAXPATHLEN];
char headerPath[MAXPATHLEN];

void showUsage(char* mes)
{
	printf("%s\n\nuseage:\n", mes);
	printf("%s.exe <iso file> <output path>", PROG_NAME);
	printf("\npress <enter> to quit...\n");
	fgetc(stdin);
	exit(1);
}

BOOL createOutputDir(char* dirname)
{
	if (dirname != NULL)
	{
		if (mkdir(dirname) != 0)
		{
			int err = GetLastError();
			if (err == ERROR_ALREADY_EXISTS)// 183
			{
				printf("\n\n***** ERROR: output folder %s already exists!\n", dirname);
				return FALSE;
			}
			else
			{
				printf("\n\n***** ERROR: cannot create %s! (error %d)\n", dirname, err);
				return FALSE;
			}
		}
	}
	return TRUE;
}


void prepareHeaderFile(void)
{
	memset(headerBuffer, 0, 0xB000);
	xHeader->SignatureType = bswap32(LIVE_SIGNED);
	xHeader->SizeOfHeaders = bswap32(0xAD0E);
	xHeader->LicenseDescriptors[0].LicenseeId.AsULONGLONG = 0xFFFFFFFFFFFFFFFFULL;

	xMeta->ContentType = bswap32(XCONTENTTYPE_XBOX360TITLE);
	xMeta->ContentMetadataVersion = bswap32(2);
	xMeta->VolumeType = bswap32(SVOD_VOLUME); // CHECK FIX MAYBE this isn't actually 4 byte aligned, may cause problems on other compilers...

	// DONE! xMeta->ExecutionId parse from xex
	// DONE! xMeta->DataFiles fill in with number of data files created
	// DONE! xMeta->DataFilesSize fill in with cumulative Size of data files
	// DONE! xMeta->DisplayName[0] fill in with wstr from name in xex spa/xstr, there are 9 with 0-8, 0 being english, each 0x100 bytes in len
	// DONE! xMeta->DisplayNameEx[0] for additional languages not planned originally, there are 3 for spa types a-c
	// This is an installed game. To play insert the original disc.
	// DONE! xMeta->Description[0] fill in with wstr from name in xex spa / xstr, there are 9 with 0 - 8, 0 being english, each 0x100 bytes in len
	// DONE! xMeta->DescriptionEx[0] for additional languages not planned originally, there are 3 for spa types a-c
	// xMeta->TitleName fill in with wstr from name in xex/spa, only one with 0x80 bytes
	// DONE! xMeta->Thumbnail put png data here
	// DONE! xMeta->TitleThumbnail  put png data here
	// DONE! xMeta->ThumbnailSize put png Size here, max Size is 0x3D00
	// DONE! xMeta->TitleThumbnailSize put png Size here, max Size is 0x3D00

	xMeta->Volume.SvodVolumeDescriptor.DescriptorLength = 0x24;
	xMeta->Volume.SvodVolumeDescriptor.BlockCacheElementCount = 0x5;
	xMeta->Volume.SvodVolumeDescriptor.WorkerThreadProcessor = 0x5;
	xMeta->Volume.SvodVolumeDescriptor.WorkerThreadPriority = 0x11;
	// DONE! xMeta->Volume.SvodVolumeDescriptor.FirstFragmentHashEntry.Hash hash of first data file first 0x1000 bytes
	xMeta->Volume.SvodVolumeDescriptor.Features.HasEnhancedGDFLayout = 1; // set bit 0x40 of this byte
	// DONE! a = (rootDirSector from MICROSOFT* header) / 2
	// DONE! 	xMeta->Volume.SvodVolumeDescriptor.StartingDataBlock2 = (a >>16) & 0xFF
	// DONE! 	xMeta->Volume.SvodVolumeDescriptor.StartingDataBlock1 = (a >>8) & 0xFF
	// DONE! 	xMeta->Volume.SvodVolumeDescriptor.StartingDataBlock0 = a  & 0xFF
	// DONE! (maxSector - rootDirSector)/2 = a ; size in 0x1000 byte chunks instead of 0x800
	// DONE! 	xMeta->Volume.SvodVolumeDescriptor.NumberOfDataBlocks2 = (a >>16) & 0xFF
	// DONE! 	xMeta->Volume.SvodVolumeDescriptor.NumberOfDataBlocks1 = (a >>8) & 0xFF
	// DONE! 	xMeta->Volume.SvodVolumeDescriptor.NumberOfDataBlocks0 = a  & 0xFF

}

void finalizeHeaderFile(void)
{
	// do when everything else is done, place in xHeader->ContentId
	// sha1 hash 0x344 to 0xB000-0x344
	BYTE shaOut[XECRYPT_SHA_DIGEST_SIZE];
	memset(shaOut, 0, XECRYPT_SHA_DIGEST_SIZE);
	XeCryptSha(&headerBuffer[0x344], 0xB000 - 0x344, NULL, 0, NULL, 0, shaOut, XECRYPT_SHA_DIGEST_SIZE);
	memcpy(xHeader->ContentId, shaOut, XECRYPT_SHA_DIGEST_SIZE);
}

BOOL prepareOutputPath(char* arg)
{
	char tout[MAXPATHLEN];
	char outname[40];
	strcpy_s(tout, MAXPATHLEN, arg);
	if (tout[strlen(tout) - 1] != '\\')
		strcat_s(tout, MAXPATHLEN, "\\");

	sprintf_s(outname, 40, "%08X%08X%08X%08X", getBe32(xexMediaId), getBe32(&xexMediaId[0x4]), getBe32(&xexMediaId[0x8]), getBe32(&xexMediaId[0xC]));
	// <basepath tout><titleID><XCONTENTTYPE_XBOX360TITLE><media ID>
	sprintf_s(baseOutPath, MAXPATHLEN, "%s%08X\\%08X\\%s.data\\", tout, bswap32(xMeta->ExecutionId.Tid.TitleID), XCONTENTTYPE_XBOX360TITLE, outname);
	sprintf_s(headerPath, MAXPATHLEN, "%s%08X\\%08X\\%s", tout, bswap32(xMeta->ExecutionId.Tid.TitleID), XCONTENTTYPE_XBOX360TITLE, outname);

	printf("creating output folder...");
	if (isDirExist(baseOutPath) != 0)
	{
		printf("Failed!\n\n***** ERROR: directory %s already exists!\n", baseOutPath);
		return FALSE;
	}
	createDirRecursive(baseOutPath);
	if (isDirExist(baseOutPath) != 1)
	{
		printf("Failed!\n\n***** ERROR: directory %s was not created!\n", baseOutPath);
		return FALSE;
	}
	printf("success!\n");
	printf("output header: %s\n", headerPath);
	printf("output folder: %s\n", baseOutPath);
	return TRUE;
}

int main(int argc, char* argv[])
{
	int retVal = -1;
	//int i;
	//for (i = 0; i < argc; i++)
	//	printf("arg %d: %s\n", i, argv[i]);
	//getXexInfoFile(TEST_XEX_PATH);
	if (argc != 3)
		showUsage("Invalid command line!");

	//prepareOutputPath(argv[2]);

	prepareHeaderFile();

	if (openIsoFile(argv[1]) == TRUE)
	{
// 		finalizeHeaderFile();
// 		dump_buffer_hex("header.bin", headerBuffer, 0xB000);
		if (prepareOutputPath(argv[2]))
		{
			if (isoToSvod())
			{
				printf("isoToSvod completed!\n");
				finalizeHeaderFile();
				dump_buffer_hex(headerPath, headerBuffer, 0xB000);
				retVal = 0;

			}
			else
				printf("failed isoToSvod!\n");
		}
	}
	else
	{
		printf("\n\n***** ERROR: failed to open iso file %s!\n", argv[1]);
	}


	printf("\npress <enter> to quit...\n");
	fgetc(stdin);
	return retVal;
}

