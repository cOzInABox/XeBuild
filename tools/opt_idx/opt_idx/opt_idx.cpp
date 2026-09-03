#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <vector>
#include <string>
#include "types.h"
#include "util.h"

// #define FOR_BASEBUILD 1

#ifdef FOR_BASEBUILD
#define ADDON_BASE_IDX		".\\addonbase.idx"
#define ADDON_OPT_FOLDER	".\\bin\\"
#define ADDON_OPT_OUTFILE	".\\addonbase.idx"
#else
#define ADDON_BASE_IDX		".\\base\\addonbase.idx"
#define ADDON_OPT_FOLDER	".\\opt\\"
#define ADDON_OPT_OUTFILE	".\\opt\\addon.idx"
#endif

using namespace std;

typedef struct _SUPERCEDE_NAMES{
	char* oldName;
	char* newName;
}SCEDE_NAMES, *PSCEDE_NAMES;

typedef struct _PATCH_ITEM{
	DWORD offset;
	DWORD size;
}PATCH_ITEM, *PPATCH_ITEM;

typedef struct _PATCH_FILE{
	string fname;
	vector<PATCH_ITEM> pitm;
} PATCH_FILE, *PPATCH_FILE;

vector<PATCH_FILE> pfiles;

SCEDE_NAMES scedeName[] = {
	{ "nomu", "nointmu" },
	{ "notrinmu", "nointmu" },
};
#define NUM_SCEDE_NAMES	(sizeof(scedeName)/sizeof(SCEDE_NAMES))


/* basic layout of idx file:
	DWORD number of items OF
		DWORD number OF (DWORD offset, DWORD size)
		char nameAsAddon[16]
*/
void loadAddonBaseIdx(void)
{
	u32 len = 0;
	BYTE* buf = FileToBuffer(ADDON_BASE_IDX, &len);
	if((buf != NULL)&&(len != 0))
	{
		if((len%4)==0)
		{
			DWORD i;
			DWORD curroff = 4;
			DWORD ttl = getBeU32(buf);
			printf("processing %s, total patches %d\n", ADDON_BASE_IDX, ttl);
			for(i = 0; i < ttl; i++)
			{
				PATCH_FILE pfl;
				DWORD j;
				DWORD pttl = getBeU32(&buf[curroff]);
				curroff+=4;
				for(j = 0; j < pttl; j++)
				{
					PATCH_ITEM pitm;
					pitm.offset = getBeU32(&buf[curroff]);
					pitm.size = getBeU32(&buf[curroff]+4);
					curroff+=8;
					pfl.pitm.push_back(pitm);
				}
				pfl.fname = (char*)&buf[curroff];
				curroff += 16;
				pfiles.push_back(pfl);
				printf("added %s, with %d patches\n", pfl.fname.c_str(), pfl.pitm.size());
			}
			printf("done importing %d patch file entries from %s\n", pfiles.size(), ADDON_BASE_IDX);
		}
		else
			printf("****** ERROR ***** %s size 0x%x is not a multiple of 4 bytes!", ADDON_BASE_IDX, len);
		free(buf);
	}
	else
		printf("file %s not present, skipping\n", ADDON_BASE_IDX);
}

#ifdef FOR_BASEBUILD
char* goodNames[] = {
	"nomu",
	"notrinmu",
	"nofcrt",
	"nohdd",
	"nohdmiwait",
	"notrinmu",
};
#define NUM_GOODNAMES	(sizeof(goodNames)/sizeof(char*))
#endif

void fixupName(const char* fname, char* outname)
{
	int i;
	string inName = fname;
	string outName;
	int fpos = inName.find_last_of('\\')+1;
	int lpos = inName.find_last_of('.');
	outName = inName.substr(fpos, lpos-fpos);
#ifdef FOR_BASEBUILD
	for(i = 0; i < NUM_GOODNAMES; i++)
	{
		if(strnicmp(outName.c_str(), goodNames[i], strlen(goodNames[i])) == 0)
		{
// 			printf("replacing %s with %s\n", outName.c_str(), goodNames[i]);
			outName = goodNames[i];
			i = NUM_GOODNAMES;
		}
	}
#endif
// 	printf("fixup %s is %s\n", fname, outName.c_str());
	for(i = 0; i < NUM_SCEDE_NAMES; i++)
	{
		if(stricmp(outName.c_str(), scedeName[i].oldName) == 0)
			outName = scedeName[i].newName;
	}
	strcpy(outname, outName.c_str());
}

BOOL shouldAdd(PPATCH_FILE ci)
{
	DWORD i;
	for(i = 0; i < pfiles.size(); i++) // iterate through all the patch files already added
	{
		if(pfiles.at(i).pitm.size() == ci->pitm.size()) // if one of them matches the size, check patch item contents for a match
		{
			int mtch = 0;
			DWORD j;
			for(j = 0; j < pfiles.at(i).pitm.size(); j++)
			{
				DWORD off = pfiles.at(i).pitm.at(j).offset;
				DWORD sz = pfiles.at(i).pitm.at(j).size;
				if((off == ci->pitm.at(j).offset)&&(sz == ci->pitm.at(j).size))
				{
					mtch++;
				}
			}
			if(mtch == ci->pitm.size())
			{
				printf("previously added item for %s is already named %s\n", ci->fname.c_str(), pfiles.at(i).fname.c_str());
				return FALSE;
			}
		}
	}
	return TRUE;
}

void processPatchFile(const char* fname)
{
	u32 len = 0;
	BYTE* buf = FileToBuffer(fname, &len);
	if((buf != NULL)&&(len != 0))
	{
		if((len%4)==0)
		{
			DWORD curroff = 0;
			PATCH_FILE pfl;
			while(curroff < len-8)
			{
				PATCH_ITEM pitm;
				pitm.offset = getBeU32(&buf[curroff]);
				pitm.size = getBeU32(&buf[curroff+4]);
				curroff+=(pitm.size*4)+8;
				pfl.pitm.push_back(pitm);
				//printf("added patch instance\n", pfl.fname.c_str());
			}
			if(pfl.pitm.size())
			{
				char outName[16];
				fixupName(fname, outName);
				pfl.fname = outName;
				if(shouldAdd(&pfl))
				{
					pfiles.push_back(pfl);
					printf("added patch set %s, %d patches\n", pfl.fname.c_str(), pfl.pitm.size());
				}
			}
		}
		else
			printf("****** ERROR ***** %s size 0x%x is not a multiple of 4 bytes!", ADDON_BASE_IDX, len);
		free(buf);
	}
	else
		printf("****** ERROR ***** file is 0 size or unable to read, skipping\n");
}

void addCurrentOptPatches(void)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind;
	string findPath = ADDON_OPT_FOLDER;
	findPath += "*";
	memset(&findData, 0, sizeof(WIN32_FIND_DATA));
	hFind = FindFirstFile(findPath.c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	do {
		if((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			if(stricmp(findData.cFileName, "addon.idx") != 0)
			{
				string fname = ADDON_OPT_FOLDER;
				fname += findData.cFileName;
// 				printf("found file %s\n", fname.c_str());
				processPatchFile(fname.c_str());
			}
		}
	} while (FindNextFile(hFind, &findData));
	CloseHandle(hFind);
}

/* basic layout of idx file:
	DWORD number of items OF
		DWORD number OF (DWORD offset, DWORD size)
		char nameAsAddon[16]
*/
void outputIdx(void)
{
	unsigned char* outdat = NULL;
	DWORD curroff = 0;
	DWORD ttlsz = 4;
	DWORD i;
	for(i = 0; i < pfiles.size(); i++)
	{
		ttlsz += pfiles.at(i).pitm.size() * 8; // 4 bytes each, offset, size
		ttlsz += 20; // 4 bytes for number count, 16 for name
	}
	//printf("out total size is 0x%x\n", ttlsz);
	outdat = (unsigned char*)malloc(ttlsz);
	if(outdat)
	{
		memset(outdat, 0, ttlsz);
		setBeU32(pfiles.size(), &outdat[curroff]);
// 		printf("num items in file: %x\n", pfiles.size());
		curroff+=4;
		for(i = 0; i < pfiles.size(); i++)
		{
			DWORD j;
			PPATCH_FILE ppi = &pfiles.at(i);
			setBeU32(ppi->pitm.size(), &outdat[curroff]);
// 			printf("num patches in item: %x\n", ppi->pitm.size());
			curroff+=4;
			for(j = 0; j < ppi->pitm.size(); j++)
			{
				setBeU32(ppi->pitm.at(j).offset, &outdat[curroff]);
				curroff+=4;
				setBeU32(ppi->pitm.at(j).size, &outdat[curroff]);
				curroff+=4;
// 				printf("%d: offset %08x size %08x\n", j+1, ppi->pitm.at(j).offset, ppi->pitm.at(j).size);
			}
			strcpy((char*)&outdat[curroff], ppi->fname.c_str());
// 			printf("name: %s\n", (char*)&outdat[curroff]);
			curroff+=16;
		}
// 		printf("processed %x of expected %x bytes\n", curroff, ttlsz);
		if(curroff == ttlsz)
		{
			dump_buffer_hex(ADDON_OPT_OUTFILE, outdat, ttlsz);
		}
		else
			printf("***** ERROR ***** expected size is not equal to processed size!\n");
		free(outdat);
	}
	else
		printf("***** ERROR ***** could not allocate 0x%x bytes for output buffer!\n", ttlsz);
}

int main(int argc, char* argv[])
{
	// load .\base\addonbase.idx if it exists
	loadAddonBaseIdx();
	// iterate .\opt for addon binaries, ?dedupe?, update to new names?
	addCurrentOptPatches();
	// output to .\opt folder AND back to base folder
	if(pfiles.size())
		outputIdx();
	else
		printf("***** ERROR ***** nothing to output!!!\n");

//#ifndef FOR_BASEBUILD
	//printf("\npress <enter> to quit...\n");
	//fgetc(stdin);
//#endif
	return 0;
}
