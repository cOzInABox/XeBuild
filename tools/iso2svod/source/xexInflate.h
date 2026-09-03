#ifndef _XEXINFLATE_H
#define _XEXINFLATE_H

BOOL unpackXexData(u8* pInData, u32 inLen, u8* pOutData, u32 outLen, DWORD windowSize, DWORD firstDataSize);

BOOL unpackGzipStreamData(u8* pInData, u32 inLen, u8* pOutData, u32 outLen);

#endif // _XEXINFLATE_H
