#ifndef _UTIL_H
#define _UTIL_H

typedef struct _RFILEINF{
	u8* data;
	u32 len;
	u32 magic;
} RFILEINF, *PRFILEINF;

// gets a u32 value from a byte buffer
u32 getBeU32(unsigned char* ptr);

// gets a u16 value from a byte buffer
u16 getBeU16(unsigned char* ptr);

// dumps a buffer to console
void display_buffer_hex(unsigned char *buffer, int size);

// dumps a buffer to a file
void dump_buffer_hex(char* filename, void* buffer, int size);

// returns a pointer to an instance of bytes in a buffer
unsigned char* findInst(unsigned char* data, int dataLen, unsigned char* fbytes, int fblen);

// returns true when an instance exists in a buffer
BOOL findInstExists(unsigned char* data, int dataLen, unsigned char* fbytes, int fblen);

// converts a ascii hex char to a hex nibble
unsigned char myatox(char c);

// converts char nibble pair into a uchar
unsigned char myatox8(char* dat);

// converts (up to) 16 uchar nibbles into a u32
u32 myatox32(char* dat, u32 defValue);

// converts uppercase to lower case
char myatol(char c);

// checks char string are all valid hex digits
BOOL validateString(char* nString);

// checks for all 0x0 at an adress for len bytes
BOOL checkZeros(unsigned char* buf, u32 len);

// reverse memcpy, copies in bytes backwards starting at len and ending at 0 offset
void memcpyrev(u8* dest, u8* src, int len);

// gets the size of the file and rewinds it to offset 0
int getFileSize(FILE* fptr);

// calculates the checksum of config data
u32 calcConfigSum(u8* data);

// checks for smc config data by checksum
BOOL checkSmcConfigSum(u8* data);

// sums len number of bytes
u32 sumBytes(u8* data, u32 len);

BOOL readFile(char* filename, PRFILEINF inf);

#endif // _UTIL_H
