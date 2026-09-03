#ifdef _MSC_VER
	#define _CRT_RAND_S
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "util.h"

#ifndef _MSC_VER
	#include <time.h>
#endif

u32 getBeU32(unsigned char* ptr)
{
	u32 ret = (ptr[0]&0xFF)<<24;
	ret |= (ptr[1]&0xFF)<<16;
	ret |= (ptr[2]&0xFF)<<8;
	ret |= ptr[3]&0xFF;
	return ret;
}

u16 getBeU16(unsigned char* ptr)
{
	return ((ptr[0]&0xFF)<<8)|(ptr[1]&0xFF);
}

void display_buffer_hex(unsigned char *buffer, int size)
{
	int i;

	for(i=0; i<size; i++)
	{
		if (!(i%0x10))
			printf("\n  ");
		printf(" %02X", buffer[i]);
	}
	printf("\n");
}

//dump_buffer_hex("kv.bin", bl[FS_KEYVAULT].data, bl[FS_KEYVAULT].len);
void dump_buffer_hex(char* filename, void* buffer, int size)
{
	FILE* fptr;
	//printf("dump buffer: n:'%s' b:0x%x s:0x%x\n", filename, buffer, size);
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
	//printf("dump OK\n");
}

unsigned char* findInst(unsigned char* data, int dataLen, unsigned char* fbytes, int fblen)
{
	int i, j;
	for(i = 0; i < (dataLen-fblen); i++)
	{
		if(data[i] == fbytes[0])
		{
			for(j=0; j < fblen; j++)
			{
				if(data[i+j] != fbytes[j])
					j = fblen+1;
			}
			if(j == fblen)
				return &data[i];
		}
	}
	return NULL;
}

BOOL findInstExists(unsigned char* data, int dataLen, unsigned char* fbytes, int fblen)
{
	if(findInst(data, dataLen, fbytes, fblen) != NULL)
		return TRUE;
	return FALSE;
}

// converts a ascii hex char to a nibble
unsigned char myatox(char c)
{
	if((c>0x60)&&(c<0x67)) // a thru f
		return (unsigned char)(c-0x57);
	else if((c>0x29)&&(c<0x40)) // 0 thru 9
		return (unsigned char)(c-0x30);
	else if((c>0x40)&&(c<0x47)) // A thru F
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


/*
// converts a ascii decimal char
int doatod(char c)
{
	if((c>0x29)&&(c<0x40)) // 0 thru 9
		return (unsigned char)(c-0x30);
	return 0xFF;
}

//// converts (up to) 2147483647 into a u32
u32 myatod32(unsigned char* dat, u32 defValue)
{
	u32 retval = 0, i; //, tmp;
	int res;
	for(i = 0; i < 16; i++)
	{
		if((dat[i]&0xFF) == 0x0)
			i = 16;
		else
		{
			res = (doatod(dat[i]&0xFF));
			if((res&0xFF) == 0xFF) // abort in case of bad value
			{
				i = 16;
				retval = defValue;
			}
			else
				retval = (retval*10)+(res);
		}
	}
	return retval;
}
*/
// checks chars are valid
BOOL validateString(char* nString)
{
	int i, len;
	if(nString == NULL)
	{
		printf("string check failed, pointer invalid\n");
		return FALSE;
	}
	len = strlen(nString);
	if(nString[0] == 0x0)
	{
		printf("string check failed, first character is NULL\n");
	}
	else if(len == 0)
	{
		printf("string check failed, length is 0\n");
	}
	else
	{
		for(i = 0; i < len; i++)
		{
			if(myatox(nString[i]) == 0xFF)
			{
				printf("string check failed, char %d (%d 0x%x)\n", i, nString[i], nString[i]);
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL checkZeros(unsigned char* buf, u32 len)
{
	u32 i;
	for(i = 0; i<len; i++)
	{
		if(buf[i] != 0x0)
			return FALSE;
	}
	return TRUE;
}

void memcpyrev(u8* dest, u8* src, int len)
{
	int i, j;
	for(i=0, j=len-1; i<len; i++, j--)
	{
		dest[i]=src[j];
	}
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

u32 calcConfigSum(u8* data)
{
	u32 i, len, sum = 0;
	data += 0x10;
	for(i=0, len=252; i<len; i++)
		sum += data[i]&0xFF;
	sum = (~sum)&0xFFFF;
	return ((sum&0xFF00)<<8)+((sum&0xFF)<<24);
}

BOOL checkSmcConfigSum(u8* data)
{
	u32 calc;
	u32 src = 0;
	src = ((data[0]&0xFF)<<24)|((data[1]&0xFF)<<16);
	calc = calcConfigSum(data);
	printf("smc sum: %08x calc: %08x\n", src, calc);
	if(calc != src)
		return FALSE;
	return TRUE;
}

u32 sumBytes(u8* data, u32 len)
{
	u32 i, ret=0;
	for(i=0; i<len; i++)
		ret+= (data[i]&0xFF);
	return ret;
}

BOOL readFile(char* filename, PRFILEINF inf)
{
	FILE* inFile;
	u8* buffer;
	u32 filesize;
	u32 bufsize;
	inFile = fopen(filename,"rb");
	if (inFile == NULL)
	{
		printf("***ERROR: Cannot open file '%s'.\n", filename);
		return FALSE;
	}
	filesize = getFileSize(inFile);
	bufsize = (filesize+0xF) &~ 0xF;
	buffer = (u8*)malloc(bufsize);
	if (buffer==NULL)
	{
		printf("***ERROR: Cannot alloc buffer for file '%s'.\n", filename);
		return FALSE;
	}
	fread(buffer, filesize, 1, inFile);
	fclose(inFile);
//	printf("'%s' read OK, %d bytes\n", filename, filesize);
	inf->data = buffer;
	inf->len = filesize;
	return TRUE;
}
