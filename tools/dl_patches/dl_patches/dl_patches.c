#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "patchinfo.h"

char patchBase[MAX_PATH];
char outName[MAX_PATH];

DWORD dataSize;
char* consoleTypes[] = {
	"XENON",
	"ZEPHYR",
	"FALCON",
	"JASPER",
	"TRINITY",
	"CORONA",
	"WINCHESTER"
};

PATCHES_LIST fileList[] = {
	{NULL, 0,"patches_xenon.bin", NULL, (PATCH_MASK_JTAG|CONSOLE_XENON)},
	{NULL, 0,"patches_zephyr.bin", NULL, (PATCH_MASK_JTAG|CONSOLE_ZEPHYR)},
	{NULL, 0,"patches_falcon.bin", NULL, (PATCH_MASK_JTAG|CONSOLE_FALCON)},
	{NULL, 0,"patches_jasper.bin", NULL, (PATCH_MASK_JTAG|CONSOLE_JASPER)},
	{NULL, 0,"patches_g2xenon_dl.bin", NULL, (PATCH_MASK_GLITCH2|CONSOLE_XENON)},
	{NULL, 0,"patches_g2zephyr_dl.bin", NULL, (PATCH_MASK_GLITCH2|CONSOLE_ZEPHYR)},
	{NULL, 0,"patches_g2falcon_dl.bin", NULL, (PATCH_MASK_GLITCH2|CONSOLE_FALCON)},
	{NULL, 0,"patches_g2jasper_dl.bin", NULL, (PATCH_MASK_GLITCH2|CONSOLE_JASPER)},
	{NULL, 0,"patches_g2trinity_dl.bin", "patches_trinity_dl.bin", (PATCH_MASK_GLITCH|PATCH_MASK_GLITCH2|CONSOLE_TRINITY)},
	{NULL, 0,"patches_g2corona_dl.bin", NULL, (PATCH_MASK_GLITCH2|CONSOLE_CORONA)},
	{NULL, 0,"patches_g2winchester_dl.bin", NULL, (PATCH_MASK_GLITCH2|CONSOLE_WINCHESTER)},
	{NULL, 0,"patches_g2mtrinity_dl.bin", NULL, (PATCH_MASK_GLITCH2MFG|CONSOLE_TRINITY)},
	{NULL, 0,"patches_g2mcorona_dl.bin", NULL, (PATCH_MASK_GLITCH2MFG|CONSOLE_CORONA)},
	{NULL, 0,"patches_fat_dl.bin", NULL, (PATCH_MASK_GLITCH)},
};
#define NUM_FILES	(sizeof(fileList)/sizeof(PATCHES_LIST))


void showType(DWORD type)
{
	printf(" - %08x (", type);
	if(type&PATCH_MASK_JTAG)
	{
		printf("JTAG|");
		printf("%s)", consoleTypes[type&0xF]);
	}
	else
	{
		if(type&PATCH_MASK_GLITCH2)
			printf("GLITCH2|");
		if(type&PATCH_MASK_GLITCH)
			printf("GLITCH|");
		if(type&PATCH_MASK_GLITCH2MFG)
			printf("GLITCH2M|");
		if(type&0xF)
			printf("%s)", consoleTypes[type&0xF]);
		else
			printf("FAT)");
	}
}

BOOL IsFileExist(PCHAR path)
{
	HANDLE file = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(file == INVALID_HANDLE_VALUE)
	{
		if(GetLastError() != 5) // inaccessible means it exists but is probably open somewhere else
			return FALSE;
	}
	CloseHandle(file);
	return TRUE;
}

BOOL getItemData(int item)
{
	HANDLE hFile;
	DWORD bRead;
	char filePath[MAX_PATH];
	strcpy(filePath, patchBase);
	strcat(filePath, fileList[item].filename);
	hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		printf("**could not read %s\n", filePath);
		if(fileList[item].altname == NULL)
			return FALSE;
		else
		{
			strcpy(filePath, patchBase);
			strcat(filePath, fileList[item].altname);
			hFile = CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				printf("**could not read alternate %s\n", filePath);
				return FALSE;
			}
		}
	}

	fileList[item].len = GetFileSize(hFile, NULL);
	dataSize+= fileList[item].len;
	fileList[item].data = (PBYTE)malloc(fileList[item].len);
	ReadFile(hFile, fileList[item].data, fileList[item].len, &bRead, NULL);
	if(bRead != fileList[item].len)
		printf("ERROR: reading %x bytes only read %x bytes!!!!", fileList[item].len, bRead);
	CloseHandle(hFile);
	printf("read %x bytes from %s\n", fileList[item].len, filePath);
// 	printf("read: %s len 0x%x\n", fileList[item].filename, fileList[item].len);
	return TRUE;
}

int main(int argc, char* argv[])
{
	int i;
	int cnt = 0;
	HANDLE fOut;
	DWORD headSize, currOff = 0, bWrote;
	PPATCHINFO_HEADER pHeader;
	dataSize = 0;
	//for(i = 0; i < argc; i++)
	//	printf("%d:%s\n",i , argv[i]);
	//strcpy(patchBase, ".\\");
	//strcat(patchBase, argv[1]);
	//strcat(patchBase, "\\bin\\");
	//strcat(outName, argv[2]);
	//strcat(outName, ".bin");
	strcpy(patchBase, argv[1]);
	strcpy(outName, argv[2]);

	printf("creating %s\n", outName);
	fOut = CreateFile(outName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(fOut == INVALID_HANDLE_VALUE)
	{
		printf("ERROR: could not create output file %s (err: %08x)", outName, GetLastError());
		return 2;
	}
	// find out how many items are gonna be in the header
	printf("num items in list: %d\n", NUM_FILES);
	for(i = 0; i < NUM_FILES; i++)
	{
		if(getItemData(i))
			cnt++;
	}
	printf("found %d items total size 0x%x\n", cnt, dataSize);
	// allocate for the header
	headSize = sizeof(PATCHINFO_HEADER)+(sizeof(PATCHES_INFO)*(cnt-1));
	//printf("header size 0x%x (header %x, info %x)\n", headSize, sizeof(PATCHINFO_HEADER), sizeof(PATCHES_INFO));
	pHeader = (PPATCHINFO_HEADER)malloc(headSize);
	if(pHeader == NULL)
	{
		printf("ERROR: could not alloc header!\n");
		return 1;
	}
	memset(pHeader, 0xFF, headSize);
	pHeader->headerSize = u32Rev(headSize);
	pHeader->numEntries = u32Rev(cnt);
	// add items into the header and write it to output file
	for(i = 0, cnt = 0; i < NUM_FILES; i++)
	{
		if(fileList[i].data != NULL)
		{
			pHeader->inf[cnt].consoleType = u32Rev(fileList[i].type);
			pHeader->inf[cnt].size = u32Rev(fileList[i].len);
			pHeader->inf[cnt].offset = u32Rev(currOff);
			currOff+=fileList[i].len;
			cnt++;
		}
	}
	WriteFile(fOut, pHeader, headSize, &bWrote, NULL);
	// add item content
	for(i = 0; i < NUM_FILES; i++)
	{
		if(fileList[i].data != NULL)
		{
			printf("file added:");
			showType(fileList[i].type);
			printf(" %s%s len 0x%x\n", patchBase, fileList[i].filename, fileList[i].len);
			WriteFile(fOut, fileList[i].data, fileList[i].len, &bWrote, NULL);
			free(fileList[i].data);
		}
	}

	CloseHandle(fOut);
	free(pHeader);
	return 0;
}

