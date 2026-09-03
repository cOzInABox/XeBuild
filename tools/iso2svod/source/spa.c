#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "util.h"
#include "xcontent.h"
#include "xexInflate.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

extern PXCONTENT_HEADER xHeader;
extern PXCONTENT_METADATA xMeta;

typedef struct _XDBF_HEADER {
	DWORD Magic; // 0x0
	DWORD Version; // 0x4
	DWORD EntryTableLen; // 0x8
	DWORD EntryCount; // 0xC
	DWORD freeMemTablLen; // 0x10
	DWORD freeMemTablEntryCnt; // 0x14
} XDBF_HEADER, *PXDBF_HEADER; // size 0x18

typedef struct _XBDF_ENTRY {
	WORD Type; // 0
	QWORD ident; // 2
	DWORD offset; // 0xA
	DWORD len; // 0xE
} XBDF_ENTRY, *PXBDF_ENTRY; // size 0x12

typedef struct _XSTR_HEADER {
	DWORD Magic; // 0x0
	DWORD Version; // 0x4
	DWORD Size; // 0x8 size including this header
	WORD EntryCount; // 0xC
} XSTR_HEADER, *PXSTR_HEADER; // size 0xE

typedef struct _XSRC_HEADER {
	DWORD Magic; // 0x0
	DWORD Version;
	DWORD Size;
	DWORD FileNameLen;
	BYTE FileName[1]; // of FileNameLen bytes
} XSRC_HEADER, *PXSRC_HEADER;

typedef struct _XSRC_BODY {
	DWORD DecompressedSize;
	DWORD CompressedSize;
	BYTE CompData[1]; // of CompressedSize bytes
} XSRC_BODY, *PXSRC_BODY;

/*
<TitleInfo locale="es-es" Name="name es-ES" />
<TitleInfo locale="ja-jp" Name="name ja-JP" />
<TitleInfo locale="it-it" Name="name it-IT" />
<TitleInfo locale="fr-fr" Name="name fr-FR" />
<TitleInfo locale="ru-ru" Name="name ru-RU" />
<TitleInfo locale="pt-br" Name="name pt-PT" />
<TitleInfo locale="de-de" Name="name de-DE" />
<TitleInfo locale="ko-kr" Name="name ko-KR" />
<TitleInfo locale="zh-tw" Name="name zh-CHT" />
<TitleInfo locale="pl-pl" Name="name pl-PL" />
<TitleInfo locale="zh-cn" Name="name zh-CN" />
*/
char* locales[] = {
	"en-US", // 0 = 1
	"ja-JP", // 1 = 2
	"de-DE", // 2 = 3
	"fr-FR", // 3 = 4
	"es-ES", // 4 = 5
	"it-IT", // 5 = 6
	"ko-KR", // 6 = 7
	"zh-CHT", // 7 = 8
	"pt-PT", // 8 = 9
	"zh-CN", // 9 = 0xa
	"pl-PL", // a = 0xb
	"ru-RU", // b = 0xc
};

static char xstrtemp[2048];
static wchar_t wxstrtemp[2048];

void getXstrHeader(u8* data, PXSTR_HEADER phdr)
{
	phdr->Magic = getBe32(data);
	phdr->Version = getBe32(&data[4]);
	phdr->Size = getBe32(&data[8]);
	phdr->EntryCount = getBe16(&data[0xC]);
}

WCHAR* convertLocaleToName(char* loc)
{
	int i;
	for (i = 0; i < 0xc; i++)
	{
		if (stricmp(locales[i], loc) == 0)
		{
			if (i > 8)
			{
				return xMeta->DisplayNameEx[i - 9];
			}
			return xMeta->DisplayName[i];
		}
	}
	return NULL;
}

WCHAR* convertLocaleToDesc(char* loc)
{
	int i;
	for (i = 0; i < 0xc; i++)
	{
		if (stricmp(locales[i], loc) == 0)
		{
			if (i > 8)
			{
				return xMeta->DescriptionEx[i - 9];
			}
			return xMeta->Description[i];
		}
	}
	return NULL;
}

WCHAR* convertIdentToName(QWORD ident)
{
	if (ident > 9ULL)
	{
		return xMeta->DisplayNameEx[ident - 10];
	}
	return xMeta->DisplayName[ident - 1];
}

WCHAR* convertIdentToDesc(QWORD ident)
{
	if (ident > 9ULL)
	{
		return xMeta->DescriptionEx[ident - 10];
	}
	return xMeta->Description[ident - 1];
}

VOID endianFixWcharBuf(WCHAR* buf, int len)
{
	int i;
	for (i = 0; i < len; i++)
		buf[i] = bswap16(buf[i]);
}

VOID endianCopyWcharBuf(WCHAR* leInput, int maxIn, WCHAR* beOut, int maxOut)
{
	int i;
	int len = maxOut - 1;
	if (len > maxIn)
		len = maxIn;
	memset(beOut, 0, maxOut * 2);
	for (i = 0; i < len; i++)
		beOut[i] = bswap16(leInput[i]);
}

BOOL parseXstrSpa(u8* xstrData, u32 xstrLen, QWORD ident)
{
	BOOL ret = FALSE;
	XSTR_HEADER hdr;
	u32 currOff = 0xE;
	u32 cnt;
	getXstrHeader(xstrData, &hdr);
	if (hdr.Magic != 0x58535452) // 'XSTR'
	{
		printf("error parsing XSTR header, magic 0x%x is not 0x58535452!\n", hdr.Magic);
		return FALSE;
	}

	for (cnt = 0; cnt < hdr.EntryCount; cnt++)
	{
		u16 typ = getBe16(&xstrData[currOff]);
		u16 len = getBe16(&xstrData[currOff + 2]);
		memcpy(xstrtemp, &xstrData[currOff + 4], len);
		xstrtemp[len] = 0;
// 		printf("cnt %d: type: %04x len %04x val: %s\n", cnt, typ, len, xstrtemp);
		if (typ == 0x8000)
		{
			WCHAR* name = convertIdentToName(ident);
			mbstowcs(name, xstrtemp, 0x40-1);
			printf("Game name : %s (%s)\n", xstrtemp, locales[ident - 1]);
			//printf("Game wname: %S\n", name);
			endianFixWcharBuf(name, 0x40);
			ret = TRUE;
		}
		else if (typ == 0x5) // this is wrong, should be getting this info from the .xlast resource section instead
		{
			WCHAR* desc = convertIdentToDesc(ident);
			mbstowcs(desc, xstrtemp, 0x80 - 1);
			printf("Game description : %s\n", xstrtemp);
			printf("Game wdescription: %S\n", desc);
			endianFixWcharBuf(desc, 0x80);
		}
		currOff += len + 4;
	}
	return ret;
}

// just in case titleName wasn't found in XRSC
VOID fixTitleName(VOID)
{
	if (xMeta->TitleName[0] == 0)
	{
		QWORD i;
		for (i = 1; i < 0xD; i++)
		{
			WCHAR* tname = convertIdentToName(i);
			if (tname[0] != 0)
			{
				memcpy(&xMeta->TitleName, tname, 0x80);
				return;
			}
		}
	}
}

VOID copyTitlesToDescription(VOID)
{
	QWORD i;
	for (i = 1; i < 0xD; i++)
	{
		WCHAR* tname = convertIdentToName(i);
		if (tname[0] != 0)
		{
			WCHAR* tdesc = convertIdentToDesc(i);
			memcpy(tdesc, tname, 0x100);
		}
	}
}

BOOL parseStringsXrXml(u8* xmlDat, u32 xmlSz)
{
	int i, wBufLen;//  wBufLen wxstrtemp
	xmlChar *sellString = NULL;
	xmlChar *publisherString = NULL;
	xmlXPathContextPtr xpathCtx;
	xmlXPathObjectPtr xpathObj;
	xmlDocPtr doc;
	xmlNodePtr root_element = NULL;
	xmlInitParser();
	doc = xmlReadMemory(xmlDat, xmlSz, "noname.xml", NULL, 0);
	if (doc == NULL) {
		printf("Failed to parse XML from XRSC\n");
		return FALSE;
	}
	printf("successfully parsed XRSC xml from memory!\n");

	xpathCtx = xmlXPathNewContext(doc);
	if (xmlXPathRegisterNs(xpathCtx, BAD_CAST "x", BAD_CAST "http://www.xboxlive.com/xlast") != 0)
	{
		printf("Error: unable to register NS with prefix");
		xmlXPathFreeContext(xpathCtx);
		xmlFreeDoc(doc);
		return FALSE;
	}

	xpathObj = xmlXPathEvalExpression("/x:XboxLiveSubmissionProject/x:GameConfigProject", xpathCtx);
	if (xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
	{
		printf("No result found for GameConfigProject!\n");
	}
	else 
	{
		xmlChar * titleName = xmlGetProp(xpathObj->nodesetval->nodeTab[0], "titleName");
		if (titleName)
		{
			printf("titleName: %s\n", titleName);
			wBufLen = MultiByteToWideChar(CP_UTF8, 0, titleName, -1, wxstrtemp, 2048);
			if (wBufLen != 0)
			{
				endianCopyWcharBuf(wxstrtemp, wBufLen, xMeta->TitleName, 0x40);
			}
		}
		else
			printf("GameConfigProject does not contain titleName!\n");
	}
	xmlXPathFreeObject(xpathObj);


	xpathObj = xmlXPathEvalExpression("/x:XboxLiveSubmissionProject/x:GameConfigProject/x:ProductInformation", xpathCtx);
	if (xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
	{
		printf("No result found for ProductInformation!\n");
	}
	else
	{
		sellString = xmlGetProp(xpathObj->nodesetval->nodeTab[0], "sellTextStringId");
		if(sellString)
			printf("sellTextStringId: %s\n", sellString);
		else
			printf("sellTextStringId: No sellTextStringId found!\n");

		publisherString = xmlGetProp(xpathObj->nodesetval->nodeTab[0], "publisherStringId");
		if (publisherString == NULL)
		{
			printf("publisherString: No publisherString found!\n");
			publisherString = xmlGetProp(xpathObj->nodesetval->nodeTab[0], "developerStringId");
			if(publisherString)
				printf("developerStringId: %s\n", publisherString);
			else
				printf("developerStringId: No developerStringId found!\n");
		}
		else
			printf("publisherStringId: %s\n", publisherString);
	}
	xmlXPathFreeObject(xpathObj);


//	xpathObj = xmlXPathEvalExpression("/x:XboxLiveSubmissionProject/x:GameConfigProject/x:LocalizedStrings/x:SupportedLocale", xpathCtx);
// 	xpathObj = xmlXPathEvalExpression("/x:XboxLiveSubmissionProject/x:GameConfigProject/x:LocalizedStrings[@defaultLocale='en-US']/x:SupportedLocale", xpathCtx); //friendlyName = "X_STRINGID_TITLENAME"

// 	xpathObj = xmlXPathEvalExpression("/x:XboxLiveSubmissionProject/x:GameConfigProject/x:LocalizedStrings/x:LocalizedString[@friendlyName='X_STRINGID_TITLENAME']/x:Translation", xpathCtx); //friendlyName = "X_STRINGID_TITLENAME"
// 	xpathObj = xmlXPathEvalExpression("/x:XboxLiveSubmissionProject/x:GameConfigProject/x:LocalizedStrings/x:LocalizedString[@friendlyName='X_STRINGID_TITLENAME']/x:Translation[@locale='ja-JP']", xpathCtx); //friendlyName = "X_STRINGID_TITLENAME"
	xpathObj = xmlXPathEvalExpression("/x:XboxLiveSubmissionProject/x:GameConfigProject/x:LocalizedStrings/x:LocalizedString[@id='32768']/x:Translation", xpathCtx); //friendlyName = "X_STRINGID_TITLENAME"


	if (xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
	{
		printf("No result found for game name!\n");
		xmlXPathFreeObject(xpathObj);
		xmlXPathFreeContext(xpathCtx);
		xmlFreeDoc(doc);
		if(sellString)
			xmlFree(sellString);
		if (publisherString)
			xmlFree(publisherString);
		return FALSE;
	}
	else // https://msdn.microsoft.com/en-us/library/dd319072(v=VS.85).aspx
	{
// 		printf("%i results\n", xpathObj->nodesetval->nodeNr);
// 		xmlXPathDebugDumpObject(stdout, xpathObj, 5);
		for (i = 0; i < xpathObj->nodesetval->nodeNr; i++)
		{
			WCHAR* name;
			xmlChar *keyword, *val;
			keyword = xmlGetProp(xpathObj->nodesetval->nodeTab[i], "locale");
// 			val = xmlNodeListGetRawString(doc, xpathObj->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
			val = xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
			name = convertLocaleToName(keyword);
			if (name == NULL)
			{
				printf("ERROR: could not resolve locale %s to a xcontent buffer!\n", keyword);
				xmlXPathFreeObject(xpathObj);
				xmlXPathFreeContext(xpathCtx);
				xmlFreeDoc(doc);
				xmlFree(keyword);
				xmlFree(val);
				if (sellString)
					xmlFree(sellString);
				if (publisherString)
					xmlFree(publisherString);
				return FALSE;
			}
			wBufLen = MultiByteToWideChar(CP_UTF8, 0, val, -1, wxstrtemp, 2048);
			if (wBufLen != 0)
			{
				endianCopyWcharBuf(wxstrtemp, wBufLen, name, 0x40);
				printf("loc: %s - %s (convert ok)\n", keyword, val);
			}
			else
				printf("loc: %s - %s (string convert error %d)\n", keyword, val, GetLastError());

			xmlFree(keyword);
			xmlFree(val);
		}
	}
	xmlXPathFreeObject(xpathObj);

	if (sellString)
	{
		char xpathSS[256];
		sprintf(xpathSS, "/x:XboxLiveSubmissionProject/x:GameConfigProject/x:LocalizedStrings/x:LocalizedString[@id='%s']/x:Translation", sellString);
		xpathObj = xmlXPathEvalExpression(xpathSS, xpathCtx); //friendlyName = "X_STRINGID_TITLENAME"
// 		xpathObj = xmlXPathEvalExpression("/x:XboxLiveSubmissionProject/x:GameConfigProject/x:LocalizedStrings/x:LocalizedString[@friendlyName='SPRINGFIELD_SELLTEXT']/x:Translation", xpathCtx); //friendlyName = "X_STRINGID_TITLENAME"
		if (xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
		{
			QWORD cnter;
			wchar_t phonyDesc[] = L"No Description Provided\0";
			printf("No result found for game description (sellString), using generic 'No Description' text instead!\n");
			for (cnter = 1ULL; cnter < 0xDULL; cnter++)
			{
				WCHAR* desc;
				desc = convertIdentToDesc(cnter);
				for (i = 0; i < 25; i++)
					desc[i] = bswap16(phonyDesc[i]);
			}
		}
		else
		{
			// 		printf("%i results\n", xpathObj->nodesetval->nodeNr);
			// 		xmlXPathDebugDumpObject(stdout, xpathObj, 5);
			for (i = 0; i < xpathObj->nodesetval->nodeNr; i++)
			{
				WCHAR* desc;
				xmlChar *keyword, *val;
				keyword = xmlGetProp(xpathObj->nodesetval->nodeTab[i], "locale");
	// 			val = xmlNodeListGetRawString(doc, xpathObj->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
				val = xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
				desc = convertLocaleToDesc(keyword);
				if (desc == NULL)
				{
					printf("ERROR: could not resolve locale %s to a xcontent buffer!\n", keyword);
					xmlXPathFreeObject(xpathObj);
					xmlXPathFreeContext(xpathCtx);
					xmlFreeDoc(doc);
					xmlFree(keyword);
					xmlFree(val);
				}
				wBufLen = MultiByteToWideChar(CP_UTF8, 0, val, -1, wxstrtemp, 2048);
				if (wBufLen != 0)
				{
					endianCopyWcharBuf(wxstrtemp, wBufLen, desc, 0x80);
					printf("loc: %s - %s (convert ok)\n\n", keyword, val);
				}
				else
					printf("loc: %s - %s (string convert error %d)\n", keyword, val, GetLastError());

				xmlFree(keyword);
				xmlFree(val);
			}
		}
		xmlXPathFreeObject(xpathObj);
	}
	else if (publisherString)
	{
		char xpathSS[256];
		sprintf(xpathSS, "/x:XboxLiveSubmissionProject/x:GameConfigProject/x:LocalizedStrings/x:LocalizedString[@id='%s']/x:Translation", publisherString);
		xpathObj = xmlXPathEvalExpression(xpathSS, xpathCtx); //friendlyName = "X_STRINGID_TITLENAME"
		if (xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
		{
			QWORD cnter;
			wchar_t phonyDesc[] = L"No Description Provided\0";
			printf("No result found for game description (%s), using generic 'No Description' text instead!\n", publisherString);
			for (cnter = 1ULL; cnter < 0xDULL; cnter++)
			{
				WCHAR* desc;
				desc = convertIdentToDesc(cnter);
				for (i = 0; i < 25; i++)
					desc[i] = bswap16(phonyDesc[i]);
			}
		}
		else
		{
			// 		printf("%i results\n", xpathObj->nodesetval->nodeNr);
			// 		xmlXPathDebugDumpObject(stdout, xpathObj, 5);
			for (i = 0; i < xpathObj->nodesetval->nodeNr; i++)
			{
				WCHAR* desc;
				xmlChar *keyword, *val;
				keyword = xmlGetProp(xpathObj->nodesetval->nodeTab[i], "locale");
				// 			val = xmlNodeListGetRawString(doc, xpathObj->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
				val = xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[i]->xmlChildrenNode, 1);
				desc = convertLocaleToDesc(keyword);
				if (desc == NULL)
				{
					printf("ERROR: could not resolve locale %s to a xcontent buffer!\n", keyword);
					xmlXPathFreeObject(xpathObj);
					xmlXPathFreeContext(xpathCtx);
					xmlFreeDoc(doc);
					xmlFree(keyword);
					xmlFree(val);
				}
				wBufLen = MultiByteToWideChar(CP_UTF8, 0, val, -1, wxstrtemp, 2048);
				if (wBufLen != 0)
				{
					endianCopyWcharBuf(wxstrtemp, wBufLen, desc, 0x80);
					printf("loc: %s - %s (convert ok)\n\n", keyword, val);
				}
				else
					printf("loc: %s - %s (string convert error %d)\n", keyword, val, GetLastError());

				xmlFree(keyword);
				xmlFree(val);
			}
		}
		xmlXPathFreeObject(xpathObj);
	}
	else
	{
		printf("No result found for game description, publisherString or developerString; putting display titles into descriptions instead\n");
		copyTitlesToDescription();
	}

	xmlXPathFreeContext(xpathCtx);

	xmlFreeDoc(doc);
	xmlCleanupParser();
	if (sellString)
		xmlFree(sellString);
	if (publisherString)
		xmlFree(publisherString);

	fixTitleName();
	return TRUE;
}

BOOL parseStringsXrsc(u8* xrscData, u32 xrscLen)
{
	u8* decompBuf;
	PXSRC_HEADER hdr = (PXSRC_HEADER)xrscData;
	PXSRC_BODY bod;
	if (bswap32(hdr->Magic) != 0x58535243) // 'XSRC'
	{
		printf("XSRC magic error! 0x%x is not 0x58535243\n", bswap32(hdr->Magic));
		return FALSE;
	}
	bod = (PXSRC_BODY)&xrscData[0x10 + bswap32(hdr->FileNameLen)];
	decompBuf = (u8*)malloc(bswap32(bod->DecompressedSize));
	if (decompBuf != NULL)
	{
		if (unpackGzipStreamData(bod->CompData, bswap32(bod->CompressedSize), decompBuf, bswap32(bod->DecompressedSize)))
		{
			printf("XSRC decompressed OK\n");
// 			dump_buffer_hex("out.XSRC.bin", decompBuf, bswap32(bod->DecompressedSize));
			if (parseStringsXrXml(decompBuf, bswap32(bod->DecompressedSize)))
			{
				free(decompBuf);
// 				exit(0);
				return TRUE;
			}
		}
		else
			printf("XSRC decompression failed!\n");
		free(decompBuf);
	}
	else
		printf("XSRC unable to allocate 0x%x bytes for decompression!\n", bswap32(bod->DecompressedSize));
	return FALSE;
}

VOID getEntryInfo(u8* entData, PXBDF_ENTRY ent)
{
	ent->Type = getBe16(entData);
	ent->ident = getBe64(&entData[2]);
	ent->offset = getBe32(&entData[0xA]);
	ent->len = getBe32(&entData[0xE]);
}

BOOL getSpaInfo(u8* spaData, u32 spaLen)
{
	BOOL hasIcon = FALSE;
	BOOL hasName = FALSE;
	u32 baseOff;
	u32 currOff = 0x18; // start of the data table
	u32 cnt;
	PXDBF_HEADER hdr = (PXDBF_HEADER)spaData;
// 	dump_buffer_hex("out.xbdf.bin", spaData, spaLen);
	if (bswap32(hdr->Magic) != 0x58444246) // 'XDBF'
	{
		printf("error! SPA header magic incorrect! (0x%x != 0x58444246)\n", bswap32(hdr->Magic));
		return FALSE;
	}
	baseOff = (bswap32(hdr->EntryTableLen) * 18) + (bswap32(hdr->freeMemTablLen) * 8) + 0x18;
	if (baseOff > spaLen)
	{
		printf("error! SPA data base 0x%x is not within len 0x%x\n", baseOff, spaLen);
		return FALSE;
	}

	for (cnt = 0; cnt < bswap32(hdr->EntryCount); cnt++)
	{
		XBDF_ENTRY ent;
		getEntryInfo(&spaData[currOff], &ent);
// 		printf("entry %02d: type %04x ident %016I64x offset %08x len %08x\n", cnt, ent.Type, ent.ident, ent.offset, ent.len);
		if ((ent.offset + baseOff + ent.len) > spaLen)
		{
			printf("error in spa data! end at 0x%x is greater than len 0x%x\n", (ent.offset + baseOff + ent.len), spaLen);
			return FALSE;
		}
		if ((ent.Type == 0x2) && (ent.ident == 0x8000ULL))
		{
			printf("icon found! 0x%x bytes\n", ent.len);
			// xMeta->Thumbnail put png data here
			// xMeta->TitleThumbnail  put png data here
			// xMeta->ThumbnailSize put png Size here, max Size is 0x3D00
			// xMeta->TitleThumbnailSize put png Size here, max Size is 0x3D00
			if (ent.len <= 0x3D00)
			{
				memcpy(xMeta->Thumbnail, &spaData[ent.offset + baseOff], ent.len);
				memcpy(xMeta->TitleThumbnail, &spaData[ent.offset + baseOff], ent.len);
				xMeta->ThumbnailSize = bswap32(ent.len);
				xMeta->TitleThumbnailSize = bswap32(ent.len);
				hasIcon = TRUE;
// 				dump_buffer_hex("out.icon.png", &spaData[ent.offset + baseOff], ent.len);
			}
			else
			{
				printf("error! title icon size 0x%x is larger than 0x3D00!\n", ent.len);
				return FALSE;
			}
		}
//		else if ((ent.Type == 0x3) && (ent.ident <= 0xCULL))
//		{
//			//if (ent.ident == 1) // english
//// 			if (ent.ident == 4) // french
//			if (ent.ident >= 1ULL)
//			{
//				//char fname[64];
//				//sprintf_s(fname, 64, "loc.%s.xstr", locales[ent.ident-1]);
//				//printf("found xstr ident %016I64x - %s\n", ent.ident, locales[ent.ident-1]);
//// 				dump_buffer_hex(fname, &spaData[ent.offset + baseOff], ent.len);
//				hasName |= parseXstrSpa(&spaData[ent.offset + baseOff], ent.len, ent.ident);
//			}
//
//		}
		else if ((ent.Type == 1) && (ent.ident == 0x58535243ULL)) // XSRC - xlast utf-16 xml file
		{
			hasName |= parseStringsXrsc(&spaData[ent.offset + baseOff], ent.len);

		}
		currOff += 0x12;
	}

	return ((hasIcon == TRUE) && (hasName == TRUE));
}


