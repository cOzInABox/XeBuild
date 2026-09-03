#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "util.h"

#pragma warning (disable:4996)

#define MAX_CONSOLE_TYPES 6
char* consoleTypeStr[] = {
	"Unknown",				// no console type
	"Xenon",		// 1
	"Zephyr",		// 2
	"Falcon",		// 3
	"Jasper",		// 4
	"Trinity",		// 5
	"Corona",		// 6
};

char* getConsoleTypeStr(int num)
{
	if(num <= MAX_CONSOLE_TYPES)
		return consoleTypeStr[num];
	else
		return consoleTypeStr[0];
}

u16 getBeU16(unsigned char* ptr)
{
	return ((ptr[0]&0xFF)<<8)|(ptr[1]&0xFF);
}

u32 getBeU32(unsigned char* ptr)
{
	u32 ret = (ptr[0]&0xFF)<<24;
	ret |= (ptr[1]&0xFF)<<16;
	ret |= (ptr[2]&0xFF)<<8;
	ret |= ptr[3]&0xFF;
	return ret;
}

DWORD getLeU32(void* pval)
{
	unsigned char* ptr = (unsigned char*)pval;
	DWORD ret = (ptr[3]&0xFF)<<24;
	ret |= (ptr[2]&0xFF)<<16;
	ret |= (ptr[1]&0xFF)<<8;
	ret |= ptr[0]&0xFF;
	return ret;
}

unsigned long long getBeU64(unsigned char* ptr)
{
	unsigned long long num = 0;
	int i;
	for(i = 0; i < 8; i++)
	{
		num = (num<<8)&(ptr[7-i]&0xFF);
	}
	return num;
}

void setBeU16(u16 val, unsigned char* ptr)
{
	ptr[0] = (val>>8)&0xFF;
	ptr[1] = val&0xFF;
}

void setBeU32(u32 val, unsigned char* ptr)
{
	ptr[0] = (val>>24)&0xFF;
	ptr[1] = (val>>16)&0xFF;
	ptr[2] = (val>>8)&0xFF;
	ptr[3] = val&0xFF;
}

void setBeU64(unsigned long long num, unsigned char* ptr)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		ptr[7-i] = num&0xFF;
		num = num>>8;
	}
}

void display_buffer_hex(unsigned char *buffer, int size)
{
	int i;
	for (i=0; i<size; i++)
	{
		if (!(i%0x10))
			printf("\n  ");
		printf(" %02X", buffer[i]);
	}
	printf("\n");
}

//dump_buffer_hex("kv.bin", boxdata.blInf[FS_KEYVAULT].data, boxdata.blInf[FS_KEYVAULT].len);
void dump_buffer_hex(char* filename, void* buffer, int size)
{
	FILE* fptr;
// 	printf("dump buffer: n:'%s' b:0x%x s:0x%x\n", filename, buffer, size);
	if((buffer != NULL)&&(filename != NULL)&&(size != 0))
	{
		fptr = fopen(filename, "wb");
		if(fptr != NULL)
		{
			fwrite(buffer, size, 1, fptr);
			fclose(fptr);
		}
		else
			printf("fopen ERROR\n");
	}
	else
		printf("dump buffer arg error\n");
// 	printf("dump OK\n");
}

// converts a ascii hex char to a nibble
unsigned char myatox(char c) // 0x2D
{
	if((c >= 0x61) && (c <= 0x66)) // a thru f
		return (unsigned char)(c-0x57);
	else if((c >= 0x30) && (c <= 0x39)) // 0 thru 9
		return (unsigned char)(c-0x30);
	else if((c >= 0x41) && (c <= 0x46)) // A thru F
		return (unsigned char)(c-0x37);
	return 0xFF;
}

// converts char nibble pair into a uchar
unsigned char myatox8(char* dat)
{
	unsigned char retval; //, tmp;
	retval	= myatox(dat[0]) & 0xF;
	retval = (retval<<4)+(myatox(dat[1]) & 0xF);
	return retval;
}

// converts (up to) 16 uchar nibbles into a u32
u32 myatox32(char* dat, u32 defValue)
{
	u32 retval = 0, i; //, tmp;
	unsigned char res;
	for(i = 0; i < 16; i++)
	{
		if((dat[i]&0xFF) == 0x0)
			i = 16;
		else
		{
			res = (myatox(dat[i]&0xFF));
			if((res&0xFF) == 0xFF) // abort in case of bad value
			{
				i = 16;
				retval = defValue;
			}
			else
				retval = (retval<<4)+(res&0xF);
		}
	}
	return retval;
}

// converts uppercase to lower case
char myatol(char c)
{
	char ret = c;
	if((c>0x40)&&(c<0x5A)) // A thru Z
		ret = c+0x20;
	return ret;
}

// checks chars are valid
BOOL validateString(char* nString)
{
	int i, len;
	//cprintf(VERB_LV1, "validate '%s'\n", nString);
	if(nString == NULL)
	{
		//cprintf(VERB_LV2, "string check failed, pointer invalid\n");
		return FALSE;
	}
	len = strlen(nString);
	//if(nString[0] == 0x0)
	//{
	//	//cprintf(VERB_LV2, "string check failed, first character is NULL\n");
	//}
	//else if(len == 0)
	//{
	//	//cprintf(VERB_LV2, "string check failed, length is 0\n");
	//}
	if((nString[0] != 0x0) && (len != 0))
	{
		for(i = 0; i < len; i++)
		{
			//cprintf(VERB_LV1, "validate '%c' as %x\n", nString[i], myatox(nString[i]));
			if(myatox(nString[i]) == 0xFF)
			{
				//cprintf(VERB_LV1, "string check failed, char %d (%d 0x%x)\n", i, nString[i], nString[i]);
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}
u32 myatou32(char* dat, u32 defValue)
{
	if(((dat[0]&0xFF) == '0')&&(myatol(dat[1]&0xFF) == 'x'))
	{
		if(validateString(&dat[2]))
			return myatox32(&dat[2], defValue);
	}
	else
	{
		if(validateString(dat))
			return atoi((const char*)dat); //myatod32(&boxdata.userIni[off], defValue);
	}
	return defValue;
}

BOOL checkZeros(void* pbuf, u32 len)
{
	unsigned char* buf = (unsigned char*)pbuf;
	u32 i;
	for(i = 0; i<len; i++)
	{
		if(buf[i] != 0x0)
			return FALSE;
	}
	return TRUE;
}

int getFileSize(FILE* fptr)
{
	int len;
	if(fptr == NULL)
	{
		return 0;
	}
	fseek(fptr, 0 , SEEK_END);
	len = ftell(fptr);
	rewind (fptr);
	return len;
}

long long getFileSize64(FILE* fptr)
{
	long long len;
	if(fptr == NULL)
	{
		return 0;
	}
	_fseeki64(fptr, 0, SEEK_END);
	len = _ftelli64(fptr);
	rewind (fptr);
	return len;
}

BOOL FileExists(const char* filename)
{
	FILE* inp;
	inp = fopen(filename, "rb");
	if(inp == NULL)
		return FALSE;
	fclose(inp);
	return TRUE;
}

u8* FileToBuffer(const char* filename, u32* size)
{
	FILE* inp;
	u8* buf = NULL;
	u32 bRead = 0;
	inp = fopen(filename, "rb");
	if(inp != NULL)
	{
		bRead = getFileSize(inp);
		buf = (u8*)malloc(bRead);
		if(buf == NULL)
		{
			printf("unable to allocate file buffer of 0x%x size!\n", bRead);
			bRead = 0;
		}
		else
		{
			fread(buf, bRead, 1, inp);
		}
		fclose(inp);
	}
	if(size != NULL)
		size[0] = bRead;
	return buf;
}

void showFuses(unsigned char * pData)
{
	int i, j = 0;
	for(i = 0; i < 0x60; i++)
	{
		if (!(i%8))
		{
			printf("\nfuseset %02d: ", j);
			j++;
		}
		printf("%02X", pData[i]);
	}
	printf("\n");
}

void showKey(unsigned char * pData)
{
	int i;
	for(i = 0; i < 0x10; i++)
	{
		printf("%02X", pData[i]);
	}
}

DWORD sumBytes(unsigned char* data, int len)
{
	DWORD sum = 0;
	int i;
	for(i = 0; i < len; i++)
		sum+=data[i]&0xFF;
	return sum;
}