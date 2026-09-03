#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "util.h"
#include "crc32.h"

// #define DEBUG_OUTPUT	1
FLLIST flashFiles[MAX_FLASH];
int inFlash = 0;
FLLIST otherFiles[MAX_FLASH];
int inOther = 0;
FLLIST flashBls[MAX_FLASH];
int inBls = 0;
u16 flashVer = 0;
FILE* outFile;

void dprintf(const char* s, ...)
{
	va_list argp;
	char temp[512];
	va_start(argp, s);
	vsnprintf(temp, 512, s, argp);
	va_end(argp);
	printf(temp);
	fprintf(outFile, temp);
}

u32 checkBlMagic(PRFILEINF fdat)
{
	u32 crclen = getBeU32(&fdat->data[0xC]);
	// u32 crcc = 0;
	if(crclen > fdat->len)
	{
		printf("\n****ERROR: BL len is greater than provided file len!\n");
		return FALSE;
	}
	else
	{
		u16 bltyp = ((fdat->magic>>16)&BL_FULLMASK);
		u16 ver = fdat->magic&0xFFFF;
		switch(bltyp)
		{
			case BLCB: // 0x0 fill: @0x10 for 0x30
				memset(&fdat->data[0x10], 0, 0x30);
				break;

			case BLCC: // 0x0 fill: @0x10 for 0x30
				memset(&fdat->data[0x10], 0, 0x10);
				break;

			case BLCD: // 0x0 fill: @0x10 for 0x10
				memset(&fdat->data[0x10], 0, 0x10);
				break;

			case BLCE: // 0x0 fill: @0x10 for 0x10
				memset(&fdat->data[0x10], 0, 0x10);
				break;

			case BLCF: // 0x0 fill: @0x20 for 0x210
				if(ver > flashVer)
				{
#ifdef DEBUG_OUTPUT
					printf("FLASH VER CUR: %d NEW: %d\n", flashVer, ver);
#endif
					// fgetc(stdin);
					flashVer = ver;
				}
				memset(&fdat->data[0x20], 0, 0x210);
				break;

			case BLCG: // 0x0 fill: @0x10 for 0x10
				memset(&fdat->data[0x10], 0, 0x10);
				break;

			default:
				printf("*****ERROR: improper BL?\n");
				return 0;
		}
		flashBls[inBls].magic = fdat->magic;
		flashBls[inBls].crc = crc32buf(fdat->data, crclen);
		return 1;

	}
	return 0;
}

void processFile(int typ, char* name, PRFILEINF fdat)
{
	char* pname = strrchr(name, '\\')+1;
	if(pname == NULL)
		pname = name;
	switch(typ)
	{
		case FILE_FLASH:
			strncpy(flashFiles[inFlash].name, pname, strlen(pname));
			flashFiles[inFlash].crc = crc32buf(fdat->data, fdat->len);
#ifdef DEBUG_OUTPUT
			printf("flash: %s,%08x\n", flashFiles[inFlash].name, flashFiles[inFlash].crc);
#endif
			inFlash++;
			break;
		case FILE_BL:
			checkBlMagic(fdat);
			strncpy(flashBls[inBls].name, pname, strlen(pname));
#ifdef DEBUG_OUTPUT
			printf("bls  : %s,%08x\n", flashBls[inFlash].name, flashBls[inFlash].crc);
#endif
			inBls++;
			break;
		default:
			strncpy(otherFiles[inOther].name, pname, strlen(pname));
			otherFiles[inOther].crc = crc32buf(fdat->data, fdat->len);
#ifdef DEBUG_OUTPUT
			printf("flash: %s,%08x\n", otherFiles[inOther].name, otherFiles[inOther].crc);
#endif
			inOther++;
			break;
	}
}

void showBls(u32 minver1, u32 maxver1, u32 minver2, u32 maxver2)
{
	int i;
	u16 ver;
	for(i = 0; i < inBls; i++)
	{
		//printf("magic %x:%x\n", flashBls[i].magic, flashBls[i].magic&0xF);
		if(flashBls[i].magic != 0)
		{
			if((((flashBls[i].magic>>16)&0xFF) == 'B')||(((flashBls[i].magic>>16)&0xFF) == 'D'))
			{
				ver = flashBls[i].magic&0xFFFF;
				//printf("ver %d\n", ver);
				if(((ver > minver1)&&(ver < maxver1))||((ver > minver2)&&(ver < maxver2)))
				{
					dprintf("%s,%08x\n", flashBls[i].name, flashBls[i].crc);
					flashBls[i].magic = 0;
				}
			}
		}
	}
}

bool hasBls(u32 minver1, u32 maxver1, u32 minver2, u32 maxver2)
{
	int i;
	u16 ver;
	for(i = 0; i < inBls; i++)
	{
		//printf("magic %x:%x\n", flashBls[i].magic, flashBls[i].magic&0xF);
		if(flashBls[i].magic != 0)
		{
			if((((flashBls[i].magic>>16)&0xFF) == 'B')||(((flashBls[i].magic>>16)&0xFF) == 'D'))
			{
				ver = flashBls[i].magic&0xFFFF;
				//printf("ver %d\n", ver);
				if(((ver > minver1)&&(ver < maxver1))||((ver > minver2)&&(ver < maxver2)))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void displayBls(char* name, u32 minver1, u32 maxver1, u32 minver2, u32 maxver2)
{
	if(hasBls(minver1, maxver1, minver2, maxver2))
	{
		dprintf(name);
		showBls(minver1, maxver1, minver2, maxver2);
	}
}

bool hasCommon(void)
{
	int i;
	for(i = 0; i < inBls; i++)
	{
		if(flashBls[i].magic != 0)
		{
			return true;
		}
	}
	return false;
}

void showCommon(void)
{
	if(hasCommon())
	{
		int i;
		dprintf("\n[commonbl]; you will need to copy these to wherever they are needed\n");
		for(i = 0; i < inBls; i++)
		{
			if(flashBls[i].magic != 0)
				dprintf("%s,%08x\n", flashBls[i].name, flashBls[i].crc);
		}
	}
}

int main(int argc, char* argv[])
{
	int i;
	RFILEINF fdat;
	printf("Processing %d files\n", argc-1);
	for(i = 1; i < argc; i++)
	{
		printf("process %s\n", argv[i]);
		if(readFile(argv[i], &fdat))
		{
			// XEX2, xttf, bls
			fdat.magic = getBeU32(fdat.data);
			if((fdat.magic == XEX2_MASK)|(fdat.magic == XTTF_MASK))
			{
#ifdef DEBUG_OUTPUT
				printf("flash file\n");
#endif
				processFile(FILE_FLASH, argv[i], &fdat);
			}
			else if((fdat.magic&BL_MASK)==BL_MASK)
			{
#ifdef DEBUG_OUTPUT
				printf("bl file %c%c v:%d\n", fdat.data[0], fdat.data[1], (fdat.magic&0xFFFF));
#endif
				processFile(FILE_BL, argv[i], &fdat);
			}
			else
			{
#ifdef DEBUG_OUTPUT
				printf("Unknown file type\n");
#endif
				processFile(FILE_OTHER, argv[i], &fdat);
			}

			free(fdat.data);
			fdat.len = 0;
		}
		else
			printf("ERROR: unable to read file %s\n", argv[i]);
	}

	outFile = fopen("__output.ini", "w");
	printf("\n-------------------------------------ini output------------------------------------------\n");
	if(flashVer != 0)
	{
		dprintf("[version]; simply the highest version of CF found\n%d\n", flashVer);
	}
	if(inBls != 0)
	{
		displayBls("\n[xenonbl]\n", 1000, 2000, 7000, 8000);
		displayBls("\n[zephyrbl]\n", 4000, 5000, 0, 0);
		displayBls("\n[falconbl]\n", 5000, 6000, 0, 0);
		displayBls("\n[jasperbl]\n", 6000, 7000, 0, 0);
		displayBls("\n[trinitybl]\n", 9000, 10000, 0, 0);
		displayBls("\n[coronabl]\n", 13000, 15000, 12000, 12999);

		showCommon();
	}

	if(inFlash != 0)
	{
		dprintf("\n[flash]\n");
		for(i = 0; i < inFlash; i++)
		{
			dprintf("%s,%08x\n", flashFiles[i].name, flashFiles[i].crc);
		}
	}
	if(inOther != 0)
	{
		dprintf("\n[unknown]; these are leftovers that couldn't be classified\n");
		for(i = 0; i < inOther; i++)
		{
			dprintf("%s,%08x\n", otherFiles[i].name, otherFiles[i].crc);
		}
	}
	fclose(outFile);
	printf("\nresults written to __output.ini\n");
	printf("press <enter> to quit...\n");
	fgetc(stdin);
	return 0;
}

/*
cb/cd
1xxx = xenon
4xxx = zephyr
5xxx = falcon
6xxx = jasper
9xxx = trinity
13xxx= corona
ce: always 1888
cf/cg 4532 for exploit

*/
