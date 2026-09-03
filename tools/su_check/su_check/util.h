#ifndef _UTIL_H
#define _UTIL_H

// dumps a buffer to console
void display_buffer_hex(unsigned char *buffer, unsigned size);
void display_buf(char* msg, unsigned char *buffer, int size);
void display_hash(unsigned char* buffer, int size);
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
BOOL checkZeros(void* buf, u32 len);

// reverse memcpy, copies in bytes backwards starting at len and ending at 0 offset
void memcpyrev(u8* dest, u8* src, int len);

// gets the size of the file and rewinds it to offset 0
int getFileSize(FILE* fptr);

// checks for the presence of hack bytes in SMC data
BOOL checkSmcHacked(unsigned char* data, int len);

// calculates the checksum of config data
u32 calcConfigSum(u8* data);

// checks for smc config data by checksum
BOOL checkSmcConfigSum(u8* data);

// prints a unicode ascii string to screen
void showUnicode(unsigned char* data, int len);

// gets a 32bit value from a plain big endian data buffer
u32 getLe32(unsigned char* data);
u16 getBe16(unsigned char* data);
u32 getBe32(unsigned char* data);
u64 getBe64(unsigned char* data);
void setBe16(unsigned char* data, u16 val);
void setBe32(unsigned char* data, u32 val);
void setBe64(unsigned char* data, u64 val);

// will allocate a buffer and read a file into it, returns NULL on fail
u8* readFileToBuf(char* filename, int* len);

// will return true if a file exists
BOOL isFileExist(char* filename);

BOOL createDir(char* dirname, BOOL showWarn);

#endif // _UTIL_H
