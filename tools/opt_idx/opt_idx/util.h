#ifndef _UTIL_H
#define _UTIL_H

// returns model name string, 1= xenon etc
char* getConsoleTypeStr(int num);

// gets a u16 value from a byte buffer
u16 getBeU16(unsigned char* ptr);

// gets a u32 value from a byte buffer
u32 getBeU32(unsigned char* ptr);
DWORD getLeU32(void* pval);

// gets a u64 value from a byte buffer
unsigned long long getBeU64(unsigned char* ptr);

// put a 16bit value into a Be array
void setBeU16(u16 val, unsigned char* ptr);

// put a 32bit value into a Be array
void setBeU32(u32 val, unsigned char* ptr);

// put a 64bit value into a Be array
void setBeU64(unsigned long long num, unsigned char* ptr);

// dumps a buffer to console
void display_buffer_hex(unsigned char *buffer, int size);

// dumps a buffer to a file
void dump_buffer_hex(char* filename, void* buffer, int size);

// converts a ascii hex char to a hex nibble
unsigned char myatox(char c);

// converts char nibble pair into a uchar
unsigned char myatox8(char* dat);

// converts (up to) 16 uchar nibbles into a u32
u32 myatox32(char* dat, u32 defValue);

BOOL validateString(char* nString);

// converts uppercase to lower case
char myatol(char c);

// converts (up to) 16 uchar nibbles into a u32, 0x preceding specifies hex value
u32 myatou32(char* dat, u32 defValue);

// checks for all 0x0 at an address for len bytes
BOOL checkZeros(void* pbuf, u32 len);

// gets the size of the file and rewinds it to offset 0
int getFileSize(FILE* fptr);

// gets the 64bit size of the file and rewinds it to offset 0
long long getFileSize64(FILE* fptr);

// returns true if a file exists
BOOL FileExists(const char* filename);

// returns a pointer to a newly allocated buffer containing file data
// returns NULL if it can't load, optionally puts load size or 0 into size pointer var
u8* FileToBuffer(const char* filename, u32* size);

// takes a pointer to a 0x60 byte bin and shows it as CPU fuses
void showFuses(unsigned char * pData);

// shows a 16 byte key, like CPU key
void showKey(unsigned char * pData);

// sums a series of bytes
DWORD sumBytes(unsigned char* data, int len);

#endif // _UTIL_H
