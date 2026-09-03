#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "util.h"
#include "mspack/mspack.h"
#include "mspack/lzx.h"
#include "zlib.h"

int getBitSize(DWORD winSize)
{
	switch (winSize)
	{
		case 0x8000:
			return 15;
		case 0x10000:
			return 16;
		case 0x20000:
			return 17;
		case 0x40000:
			return 18;
		case 0x80000:
			return 19;
		case 0x100000:
			return 20;
		case 0x200000:
			return 21;
	}
	return 0;
}

/* use a pointer to a mem_buf structure as "filenames" */
typedef struct _mem_buf {
	void *data;
	size_t length;
	char* name;
} mem_buf, *Pmem_buf;

typedef struct _mem_file {
	unsigned char *data;
	size_t length;
	size_t posn;
	char* name;
} mem_file, *Pmem_file;

static void *mem_alloc(struct mspack_system *self, size_t bytes)
{
	/* put your memory allocator here */
	return malloc(bytes);
}

static void mem_free(void *buffer)
{
	/* put your memory deallocator here */
	free(buffer);
}

static void mem_copy(void *src, void *dest, size_t bytes)
{
	/* put your own memory copy routine here */
	memcpy(dest, src, bytes);
}

static void mem_msg(mem_file *file, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fputc((int) '\n', stderr);
	fflush(stderr);
	/* put your own printf-type routine here, or leave it empty */
}

static mem_file *mem_open(struct mspack_system *self, mem_buf *fn, int mode)
{
	mem_file *fh;
	if (!fn || !fn->data || !fn->length) return NULL;
// 	printf("mem_open %s\n", fn->name);
	if ((fh = (mem_file *)mem_alloc(self, sizeof(mem_file))))
	{
		fh->data = (unsigned char *)fn->data;
		fh->length = fn->length;
		fh->posn = (mode == MSPACK_SYS_OPEN_APPEND) ? fn->length : 0;
		fh->name = fn->name;
	}
	return fh;
}

static void mem_close(mem_file *fh)
{
	if (fh)
		mem_free(fh);
}

static int mem_read(mem_file *fh, void *buffer, int bytes) {
	int todo;
// 	printf("mem_read: 0x%x bytes from %s at 0x%x\n", bytes, fh->name, fh->posn);
	if (!fh || !buffer || bytes < 0) 
		return -1;
	todo = fh->length - fh->posn;
	if (todo > bytes)
		todo = bytes;
	if (todo > 0)
		mem_copy(&fh->data[fh->posn], buffer, (size_t)todo);
	fh->posn += todo;
	return todo;
}

static int mem_write(mem_file *fh, void *buffer, int bytes)
{
	int todo;
// 	printf("mem_write: 0x%x bytes to %s at 0x%x\n", bytes, fh->name, fh->posn);
	if (!fh || !buffer || bytes < 0)
	{
// 		if (!fh)
// 			printf("memfile not filled in\n");
// 		if (!buffer)
// 			printf("buffer not filled in\n");
// 		if (bytes < 0)
// 			printf("bytes < 0\n");
		return -1;
	}
	todo = fh->length - fh->posn;
	if (todo > bytes)
		todo = bytes;
	if (todo > 0)
		mem_copy(buffer, &fh->data[fh->posn], (size_t)todo);
	fh->posn += todo;
	return todo;
}

static int mem_seek(mem_file *fh, off_t offset, int mode)
{
	if (!fh)
		return 1;
	switch (mode)
	{
		case MSPACK_SYS_SEEK_START:
			break;
		case MSPACK_SYS_SEEK_CUR:
			offset += (off_t)fh->posn;
			break;
		case MSPACK_SYS_SEEK_END:
			offset = (off_t)fh->length;
			break;
		default:
			return 1;
	}
	if ((offset < 0) || (offset >(off_t) fh->length))
		return 1;
	fh->posn = (size_t)offset;
	return 0;
}

static off_t mem_tell(mem_file *fh) {
	return (fh) ? (off_t)fh->posn : -1;
}

static struct mspack_system mem_system = {
	(struct mspack_file * (*)(struct mspack_system *, const char *, int)) &mem_open,
	(void(*)(struct mspack_file *)) &mem_close,
	(int(*)(struct mspack_file *, void *, int)) &mem_read,
	(int(*)(struct mspack_file *, void *, int)) &mem_write,
	(int(*)(struct mspack_file *, off_t, int)) &mem_seek,
	(off_t(*)(struct mspack_file *)) &mem_tell,
	(void(*)(struct mspack_file *, const char *, ...)) &mem_msg,
	&mem_alloc,
	&mem_free,
	&mem_copy,
	NULL
};

// lets turn it back into a stream to work with mspack better
u8* rebufInData(u8* pInData, DWORD* finSiz, DWORD inLen, DWORD firstSize)
{
	u8* buf = (u8*)malloc(inLen);
	*finSiz = 0;
	if (buf != NULL)
	{
		DWORD currLen = firstSize;
		DWORD nextLen = 1;

		DWORD compressedSz = 0; // position rebuffered compressed data
		DWORD currPos = 0; // position in input file
		DWORD lastPos = 0;
		while ((currPos < inLen) && (nextLen != 0))
		{
			WORD innerLen = 1;
			nextLen = getBe32(&pInData[currPos]);
// 			printf("nextLen 0x%x at 0x%x (0x%x)\n", nextLen, currPos, currPos + 0x4000);
			currPos += 0x18; // skip over next fragment size and hash data
			do{
				innerLen = getBe16(&pInData[currPos]);
// 				printf("innerlen 0x%x at 0x%x (0x%x)\n", innerLen, currPos, currPos + 0x4000);
				if (innerLen)
				{
					currPos += 2;
					memcpy(&buf[compressedSz], &pInData[currPos], innerLen);
					currPos += innerLen;
					compressedSz += innerLen;
				}
			} while (innerLen);
			currPos = lastPos + currLen;
			currLen = nextLen;
			lastPos = currPos;
		}
		*finSiz = compressedSz;
		return buf;
	}
	return NULL;
}

BOOL doXexUnpack(u8* pInData, u32 inLen, u8* pOutData, u32 outLen, DWORD windowSize)
{
	BOOL ret = TRUE;
	struct mspack_system *sys = &mem_system;
	struct lzxd_stream *lzxd = NULL;
	mem_buf source = { NULL, 0, "source" };
	mem_buf output = { NULL, 0, "output" };
	mem_file* in, *out;

	source.data = pInData; // data after the header
	source.length = inLen;
	output.data = pOutData;
	output.length = outLen;
// 	printf("inlen: 0x%x outlen 0x%x winsz 0x%x\n", inLen, outLen, windowSize);
	in = (mem_file*)sys->open(sys, (const char*)&source, MSPACK_SYS_OPEN_READ);
	out = (mem_file*)sys->open(sys, (const char*)&output, MSPACK_SYS_OPEN_WRITE);

	lzxd = lzxd_init(sys, (struct mspack_file *)in, (struct mspack_file *)out, getBitSize(windowSize), 0, windowSize, (off_t)outLen, 0);
	if (lzxd != NULL)
	{
		int stLzx = lzxd_decompress(lzxd, (off_t)outLen);
		if (stLzx == MSPACK_ERR_OK)
		{
			ret = TRUE;
		}
		lzxd_free(lzxd);
	}
	else
		ret = FALSE;
	sys->close((struct mspack_file *)in);
	sys->close((struct mspack_file *)out);
// 	printf("returning %d\n", ret);

	return ret;
}

BOOL unpackXexData(u8* pInData, u32 inLen, u8* pOutData, u32 outLen, DWORD windowSize, DWORD firstDataSize)
{
	BOOL ret = FALSE;
	u8* rebuf;
	u32 rebufsz;
	rebuf = rebufInData(pInData, &rebufsz, inLen, firstDataSize);
	if (rebuf)
	{
		ret = doXexUnpack(rebuf, rebufsz, pOutData, outLen, windowSize);
		free(rebuf);
	}
	return ret;
}


BOOL unpackGzipStreamData(u8* pInData, u32 inLen, u8* pOutData, u32 outLen)
{
	z_stream strm;
	int ret;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	inflateInit2(&strm, MAX_WBITS | 16);
	strm.avail_in = inLen;
	strm.next_in = pInData;
	strm.avail_out = outLen;
	strm.next_out = pOutData;
	ret = inflate(&strm, Z_FINISH);
	if (ret != Z_STREAM_END)
	{
		printf("gzip inflate error %i\n", ret);
		inflateEnd(&strm);
		return FALSE;
	}
	inflateEnd(&strm);
	return TRUE;
}
