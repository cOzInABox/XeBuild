// created for retail 17150
#include <idc.idc>
#include "generic_functions.idc"

static segHvConvert64ValToStr(val)
{
	auto str, cnt;
	str = form("");
	for(cnt = 7; cnt >= 0; cnt = cnt-1)
	{
		str = form("%s %02X", str, (val>>(cnt*8)&0xFF));
	}
	return str;
}

static segHvNameSegPointer(name, val)
{
	auto offset, rname, cnt, bytes;
	cnt = 0;
	rname = form("%s", name);
	bytes = segHvConvert64ValToStr(val);
	// Message(form("seeking %s (name %s)\n", bytes, name));
	offset = FindBinary(0, SEARCH_DOWN, bytes);
	while(offset != BADADDR)
	{
		if(offset < 0x40000)
		{
			if(cnt != 0)
				rname = form("%s_%d", name, cnt);
			Message(form("marking %s at 0x%x\n", rname, offset));
			makeNamedQword(offset, rname);
			offset = FindBinary(offset+8, SEARCH_DOWN, bytes);
			cnt = cnt+1;
		}
		else
			offset = BADADDR;
	}
}

static segMakeKeyScan(bytes, name, startAddr, endAddr)
{
	auto cqw, stype, offset, ret, end;
	if(endAddr == 0)
		end = 0xFFFFFFFFFFFFFFFF;
	else
		end = endAddr;
	offset = FindBinary(startAddr, SEARCH_DOWN, bytes);
	if(offset != BADADDR)
	{
		cqw = Dword(offset);
		if(offset < end-((cqw*8)+0x10))
		{
			if(cqw == 0x10)
				stype = form("XECRYPT_RSAPUB_1024");
			else if(cqw == 0x18)
				stype = form("XECRYPT_RSAPUB_1536");
			else if(cqw == 0x20)
				stype = form("XECRYPT_RSAPUB_2048");
			else if(cqw == 0x40)
				stype = form("XECRYPT_RSAPUB_4096");
			else
			{
				Message(form("Could not make %s struct offset: 0x%08x, unknown CQW 0x%x\n", name, offset, cqw));
				return;
			}
			ret = makeStructNamed(offset, stype, name);
			if(ret == 0)
				Message(form("Could not make %s struct type %s offset: 0x%08x err: %x\n", name, stype, offset, ret));
			else
				Message(form("Key struct %s marked at offset: 0x%08x\n", name, offset));
			MakeName(offset, name);
			offset = offset+4;
		}
		else
		{
			Message(form("Found key %s outside of range 0x%x-0x%x\n", name, startAddr, end));
			return BADADDR;
		}
	}
	// else
		// Message(form("Did not find %s\n", name));
	return offset;
}

static segNameKeys(base, end)
{
	nameBinaryScan(base, end, "A2 6C 10 F7 1F D9 35 E9 8B 99 92 2C E9 32 15 72", 16, "HvcXex1Key");
	nameBinaryScan(base, end, "E4 89 E0 1A AA 36 4B 7A 4D 95 13 A8 53 86 03 77", 16, "HvcAp251Key");
	nameBinaryScan(base, end, "79 AD 7F 10 0B C9 E9 09 B7 66 84 4F EB D8 7B 69", 16, "HvcDvdAuth1Key");
	nameBinaryScan(base, end, "97 49 35 7E 18 BC 99 74 C7 A6 1C 83 69 94 8C 2A", 16, "HvcSystemLink1Key");
	nameBinaryScan(base, end, "20 B1 85 A5 9D 28 FD C3 40 58 3F BB 08 96 BF 91", 16, "HvcXex2Key");
	nameBinaryScan(base, end, "DD 88 AD 0C 9E D6 69 E7 B5 67 94 FB 68 56 3E FA", 16, "HvcBL1Key"); // live dumps have this twice
	segMakeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 DD 5F 49 6F 99 4D 37 BB", "HvcCONSTANT_MASTER_KEY", base, end); // DD5F496F994D37BB
	segMakeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 F2 E5 3E 3F 03 75 C2 B3", "HvcCONSTANT_LIVE_KEY", base, end); // F2E53E3F0375C2B3
	segMakeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 E6 D7 3D 66 E5 4E D7 D3", "HvcCONSTANT_XB1_GREEN_KEY", base, end); // E6D73D66E54ED7D3
	segMakeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 F4 30 E3 52 4D 53 1C A1", "HvcCONSTANT_SATA_DISK_SECURITY_KEY", base, end); //F430E3524D531CA1
	segMakeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 E6 3B 32 B2 8D 9E 9E E7", "HvcCONSTANT_PIRS_KEY", base, end); // E63B32B28D9E9EE7
	segMakeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 C9 1C 35 77 C8 BF A0 6B", "HvcCONSTANT_PIRS_KEY_DEV", base, end); // C91C3577C8BFA06B
	segMakeKeyScan("00 00 00 18 00 00 00 03 00 00 00 00 00 00 00 00 67 39 40 07 BB 3C A7 09", "HvcCONSTANT_DEVICE_REVOCATION_KEY", base, end); // 67394007BB3CA709
	segMakeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 3D A2 69 7C 4E E3 7C 9F", "HvcCONSTANT_XMACS_KEY", base, end); // 3DA2697C4EE37C9F

	segMakeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 81 A1 17 26 B3 70 8A D9", "HvcCONSTANT_FACTORY_KEY", base, end); // 81A11726B3708AD9
	segMakeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 47 3E 39 88 84 AF 0B 61", "HvcCONSTANT_SECONDARY_ACTIVATION_KEY", base, end); // 473E398884AF0B61


	segMakeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 CB B9 93 CD C1 7D 10 B5", "HvcCONSTANT_ALT_FACTORY_KEY", base, end); // CBB993CDC17D10B5
	segMakeKeyScan("00 00 00 40 00 01 00 01 00 00 00 00 00 00 00 00 6D AB 60 5C B3 45 75 D1", "HvcCONSTANT_DEBUG_UNLOCK_KEY", base, end); // 6DAB605CB34575D1
	segMakeKeyScan("00 00 00 20 00 01 00 01 fd 33 6b 67 93 6a a2 11 e9 8d b5 dc af 38 8e f1", "HvcCONSTANT_1BL_RSA_KEY", base, end); // e98db5dcaf388ef1
	segMakeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 e9 8d b5 dc af 38 8e f1", "HvcCONSTANT_1BL_RSA_KEY_FORM2", base, end); // e98db5dcaf388ef1
	
	// some new stuff I've not puzzled over what they are for yet
	segMakeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 7E 17 D7 46 E2 47 38 DB", "HvcCONSTANT_NEW_KEY1", base, end); // 7E17D746E24738DB
	segMakeKeyScan("00 00 00 10 00 01 00 01 00 00 00 00 00 00 00 00 04 D0 55 50 79 19 95 27", "HvcCONSTANT_NEW_KEY2", base, end); // 04D0555079199527
	segMakeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 E1 82 06 A6 73 EF E2 71", "HvcCONSTANT_DATACENTER_RSA_KEY", base, end); // E18206A673EFE271
	nameBinaryScan(base, end, "E5 1C F0 B7 52 72 2C BA F0 BA DE CB 07 6E 9D E5", 16, "HvcKEY_BYTES1"); // 
	nameBinaryScan(base, end, "DA 39 A3 EE 5E 6B 4B 0D 32 55 BF EF 95 60 18 90 AF D8 07 09", 0x14, "HvcKEY_BYTES2");

}

// for page 0x6401f10000 or 0x8e030000 in kernel, bulk of key copies like PIRS are here
static setupDiskCacheSection(base)
{
	auto tbase;
	Message("setup disk cache section\n");
	MakeRptCmt(base, "0x64_01f10000 physical, 0x8e030000 virtual");
	
	makeStructNamed(base, "DYNAMIC_REVOCATION_LIST", "HvDynamicCrlData"); // 8E030000 6401F10000 my guess is this is used for crl sent from live
	
	makeStructNamed(base+0x8000, "CONSOLE_ID_HASH_CACHE", "HvIdHashCache"); // 6401F18000
	makeStructNamed(base+0x8400, "HDD_SECURITY_BLOB", "HvHddSecurityBlobCache_XeKey0x1001"); // 8E038400
	
	makeStructNamed(base+0x8600, "SECURITY_INFO_CACHE", "HvSecurityInfoCache"); // 8E038600 6401f18600
		MakeRptCmt(base+0x8600, "HvKvPolicyFlashSizeCopy and HvKvPolicyBuiltInMuSizeCopy only populated on machines with bootloaders that flag kv signature required");
	// makeNamedWord(base+0x8600, "HvHeaderFlagsCache"); // 6401f18600	hvHeader.flags|0x8000
		// MakeRptCmt(base+0x8600, "is specifically flags|0x8000 in some cases");
	// makeNamedWord(base+0x8604, "HvKvOddFeatureCache"); // 6401f18604	(HVKEYVAULT.oddFeatures)
	// makeNamedDword(base+0x8610, "HvKeyStatusCache"); // 6401f18610 	hv KeyStatus mirror
	// makeNamedDword(base+0x8618, "HvKvPolicyFlashSizeCopy"); // 8E038618	6401f18618 HVKEYVAULT.policyFlashSize (only on signature required kv machines)
		// MakeRptCmt(base+0x8618, "only populated on machines with bootloaders that flag kv signature required");
	// makeNamedDword(base+0x8620, "HvKvPolicyBuiltInMuSizeCopy"); // 6401F18620 HVKEYVAULT.policyBuiltInMuSize  (only on signature required kv machines)
		// MakeRptCmt(base+0x8620, "only populated on machines with bootloaders that flag kv signature required");
	// makeNamedQword(base+0x8630, "HvKvRestrictedPrivsCopy"); // 8E038630	6401f18630 HVKEYVAULT.restrictedPrivFlags
	// makeNamedQword(base+0x8638, "HvSecDataSecurityDetectedErrorCopy"); // 8E038638	6401f18638 loaded from 7601EEF828	secdata cSecurityDetectedError:LARGE_INTEGER ?
	// makeNamedQword(base+0x8640, "HvSecDataSecurityActivatedErrorCopy"); // 8E038640	6401f18640 loaded from 7601EEF830	secdata cSecurityActivatedError:LARGE_INTEGER ?
	
	makeStructNamed(base+0x8700, "SECDATA_BLOB_CACHECOPY", "HvSecDataPartCopy"); // 6401F18700
	makeStructNamed(base+0x8780, "MEDIA_INFO_CACHE", "HvMediaInfoCache"); // 8E038780 6401F18780
		MakeRptCmt(base+0x8780, "media info, first 0x14 bytes is hash of following 0x8C bytes");
	
	makeStructNamed(base+0xA000, "HVKV_CONSOLE_INFO", "HvKvConsoleInfoCopy"); // 6401F1A000
	makeNamedByteArray(base+0xA810, 0x110, "HvcCONSTANT_DEVICE_ALT_KEY"); // 6401F1A810 one of the alternate RSA keys get copied here
	makeNamedByteArray(base+0xA920, 0x110, "HvcCONSTANT_XSIGNER2_ALT_RSA_KEY"); // 8E03A920 
	makeNamedByteArray(base+0xA700, 0x110, "HvcCONSTANT_PIRS_ALT_KEY"); // 8E03A700
	
	makeStructNamed(base+0xAA30, "HV_KEY_HEADER_INFO", "HvKeyInfo"); // 6401F1AA30
	// makeNamedByteArray(base+0xAA30, 0x10, "HvCpuKeyShaCache"); // 6401F1AA30
	// makeNamedByteArray(base+0xAA40, 0x10, "HvKvHmacShaCache"); // 6401F1AA40
	// makeNamedByteArray(base+0xAA50, 0x10, "HvZeroEncryptedWithConsoleType"); // 6401F1AA50
	// makeStructNamed(base+0xAA60, "BLDR_FLASH", "HvFlashHeaderCache"); // 8E03AA60
		
	makeStructNamed(base+0xC000, "FCRT_HEADER", "HvFcrtCache"); // 8E03C000 6401F1C000 HvFcrtCache
	if(Byte(base+0xC000) != 0) // signature is replaced with a single byte to indicated loaded
	{
		if(Dword(base+0xC11C) != 0)
		{
			tbase = Dword(base+0xC11C)+base+0xC000; // 0x118 = dwDataSize, 0x11C = dwHeaderSize
			Message(form("fcrt found at 0x%x, labeling payload at 0x%x for 0x%x bytes\n", base+0xC000, tbase, Dword(base+0xC118)));
			makeNamedByteArray(tbase, Dword(base+0xC118), "HvFcrtDataCache");
		}
	}

	
	segNameKeys(base, base+0x10000);

}

// for page 0x8000030001ef0000 or 0x8e000000 in kernel
static setupCrlCacheSection(base)
{
	Message("setup CRL section\n");
	MakeRptCmt(base, "0x80000300_01ef0000 physical, 0x8e000000 virtual");
	makeStructNamed(base, "DYNAMIC_REVOCATION_LIST", "HvCrlData");
}

// for page 0x6C01F40000 not mapped in kernel
static setupDaeCacheSection(base)
{
	auto size, curraddr, cnt, tname;
	curraddr = base;
	cnt = 0;
	Message("setup DAE section\n");
	while(Dword(curraddr) == 0x44414550) // 'DAEP'
	{
		size = Word(curraddr+4);
		tname = form("DaeCacheHeader%d", cnt);
		makeStructNamed(curraddr, "DVD_AUTH_EX_HEADER", tname);
		tname = form("DaeCacheData%d", cnt);
		makeNamedByteArray(curraddr+0x150, size-0x150, tname);
		cnt = cnt+1;
		curraddr = curraddr+size;
	}
}

// 0x7601ee0000 not mapped to virtual
static setupHvKvCacheSection(base)
{
	auto i, name;
	MakeRptCmt(base, "0x76_01ee0000 physical, no virtual");
	makeStructNamed(base, "HVKEYVAULT", "HvKeyVaultDataCache");
	makeStructNamed(base+0x4000, "EXTENDED_BIN_DATA", "HvExKeyVaultDataCache");
		MakeRptCmt(base+0x4000, form("extended bin area also used for scratch buffer before extended.bin is loaded"));
	makeNamedDword(base+0x8000, "hvRevocationListLock"); // 7601EE8000
	makeNamedByteArray(base+0xC000, 0x2800, "hvXeCryptAllocMem"); // 7601EEC000 hvXeCryptAllocMem for 0x2800 MAX
		MakeRptCmt(base+0xC000, form("0x2800 bytes used strictly by XeCryptMemAlloc and XeCryptMemFree"));
	makeStructNamed(base+0xF800, "SECDATA_BLOB", "HvSecDataCache"); // 7601EEF800
	for(i = 0; i < 8; i++) // 7601EEFD80 for 0x80, title keys E0 thru E7 0x10 each
	{
		name = form("HvTitleKey0xE%d", i);
		makeNamedByteArray(base+0xFD80+(i*0x10), 0x10, name);
	}
	makeNamedByteArray(base+0xFE00, 0x21, "HvTitleKeyProperties"); // 7601EEFE00 for 0x21 bytes, key props

	// not really enough info on these
	makeNamedDword(base+0xE800, "hvXexResolveImportBuf"); // 7601EEE800
		MakeArray(base+0xE800, 0x400);
	makeNamedByteArray(base+0xFC00, 0x1, "hvIptvSessionBase"); // 7601EEFC00 ?? len
	

}

static makeXexHeaderStringField(entadr, head, adr, name, key)
{
	auto tmp, tname;
	tmp = Dword(adr);
	tname = form("%sSz", name);
	MakeRptCmt(entadr, form("%s key 0x%x offset 0x%x (size: 0x%x)", head, key, adr, tmp));
	makeNamedDword(adr, tname);
	makeNamedString(adr+4, (tmp-4), name);
}

static parseXexDirectoryEntry(base, entadr, cnt)
{
	auto key, size, val, tname, tmp;
	tname = form("HvXexDirectoryInfo_%d", cnt);
	makeStructNamed(entadr, "IMAGE_XEX_DIRECTORY_ENTRY", tname);
	key = Dword(entadr);
	size = key&0xFF;
	if(size != 0xFF)
		size = size << 2;
	key = key >> 8;
	val = Dword(entadr+4);
	if(key == 0x2) // XEX_HEADER_SECTION_TABLE					XEX_HEADER_SIZEDSTRUCT(0x0002)
	{
		tmp = Dword(val+base);
		MakeRptCmt(entadr, form("XEX_HEADER_SECTION_TABLE key 0x%x offset 0x%x (size: 0x%x)", key, val+base, tmp));
		makeNamedDword(val+base, "HvXexSectionTableInfoSz");
		makeStructNamed(val+base+4, "XEX_SECTION_HEADER", "HvXexSectionTableInfo");
		MakeArray(val+base+4, (tmp-4)/0x10);
	}
	else if(key == 0x3) // XEX_FILE_DATA_DESCRIPTOR_HEADER		XEX_HEADER_SIZEDSTRUCT(0x0003)
	{
		tmp = Dword(val+base);
		MakeRptCmt(entadr, form("XEX_FILE_DATA_DESCRIPTOR_HEADER key 0x%x offset 0x%x (size: 0x%x)", key, val+base, tmp));
		makeStructNamed(val+base, "XEX_FILE_DATA_DESCRIPTOR", "HvXexDataDescriptorInfo");
		tmp = Word(val+base+6);
		//Message(form("offset 0x%x format %x\n", val+base, tmp));
		if(tmp == 1) // XEX_DATA_FORMAT_RAW
			makeStructNamed(val+base+8, "XEX_RAW_DATA_DESCRIPTOR", "HvXexDataDescriptorRawInfo");
		else if((tmp == 2)||(tmp == 3)) // XEX_DATA_FORMAT_COMPRESSED
			makeStructNamed(val+base+8, "XEX_COMPRESSED_DATA_DESCRIPTOR", "HvXexDataDescriptorCompressedInfo");
	}
	// else if(key == 0x4) // XEX_PATCH_FILE_BASE_REFERENCE		XEX_HEADER_FIXED_SIZE(0x0004, 0x14)
	// {
		// MakeRptCmt(entadr, form("XEX_PATCH_FILE_BASE_REFERENCE key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		// tname = form("XEX_PATCH_FILE_BASE_REFERENCE");
		// makeNamedByteArray(val+base, size, tname);
	// }
	else if(key == 0x40) // XEX_HEADER_KEY_VAULT_PRIVS			XEX_HEADER_STRUCT(0x0040, XEX_KEY_VAULT_PRIVILEGES)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_KEY_VAULT_PRIVS key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeStructNamed(val+base, "XEX_KEY_VAULT_PRIVILEGES", "HvXexKeyVaultPrivilegesInfo");
	}
	else if(key == 0x41) // XEX_HEADER_TIME_RANGE				XEX_HEADER_STRUCT(0x0041, XEX_SYSTEM_TIME_RANGE)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_TIME_RANGE key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeStructNamed(val+base, "XEX_SYSTEM_TIME_RANGE", "HvXexSystemTimeRangeInfo");
	}
	else if(key == 0x42) // XEX_HEADER_CONSOLE_ID_TABLE			XEX_HEADER_SIZEDSTRUCT(0x0042) // lists disallowed console IDs
	{
		tmp = Dword(val+base);
		MakeRptCmt(entadr, form("XEX_HEADER_CONSOLE_ID_TABLE key 0x%x offset 0x%x (size: 0x%x)", key, val+base, tmp));
		makeNamedDword(val+base, "HvXexConsoleIdTableInfoSz");
		makeNamedByteArray(val+base+4, (tmp-4), "HvXexConsoleIdTableInfo");
	}
	else if(key == 0x80) // XEX_HEADER_BOUND_PATH				XEX_HEADER_STRING_FIELD(0x0080) == XEX_HEADER_SIZEDSTRUCT(key)
	{
		makeXexHeaderStringField(entadr, "XEX_HEADER_BOUND_PATH", val+base, "HvXexBoundPath", key);
	}
	else if(key == 0x81) // XEX_HEADER_DEVICE_ID				XEX_HEADER_FIXED_SIZE(0x0081, 0x14)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_DEVICE_ID key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeNamedByteArray(val+base, size, "HvXexDeviceIdInfo");
	}

	else if(key == 0x100) // XEX_HEADER_ORIGINAL_BASE_ADDRESS	XEX_HEADER_ULONG(0x0100)
		MakeRptCmt(entadr, form("XEX_HEADER_ORIGINAL_BASE_ADDRESS = 0x%x", val));
	else if(key == 0x101) // XEX_HEADER_ENTRY_POINT				XEX_HEADER_FLAG(0x0101)
		MakeRptCmt(entadr, form("XEX_HEADER_ENTRY_POINT = 0x%x", val));
	else if(key == 0x102) // XEX_HEADER_PE_BASE					XEX_HEADER_ULONG(0x0102)
		MakeRptCmt(entadr, form("XEX_HEADER_PE_BASE = 0x%x", val));
	else if(key == 0x103) // XEX_HEADER_IMPORTS					XEX_HEADER_SIZEDSTRUCT(0x0103)
	{
		tmp = Dword(val+base); // name table size
		MakeRptCmt(entadr, form("XEX_HEADER_IMPORTS key 0x%x offset 0x%x (size: 0x%x)", key, val+base, tmp));
		makeStructNamed(val+base, "XEX_IMPORT_DESCRIPTOR", "HvXexImportDescriptorInfo");
		// MakeArray(val+base+4, (tmp-4)/0x24);
		
	}
	else if(key == 0x180) // XEX_HEADER_VITAL_STATS			XEX_HEADER_STRUCT(0x0180, XEX_VITAL_STATS)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_VITAL_STATS key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeStructNamed(val+base, "XEX_VITAL_STATS", "HvXexVitalStatsInfo");
	}
	else if(key == 0x0181) // XEX_HEADER_CALLCAP_IMPORTS		XEX_HEADER_STRUCT(0x0181, XEX_CALLCAP_IMPORTS)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_CALLCAP_IMPORTS key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeStructNamed(val+base, "XEX_CALLCAP_IMPORTS", "HvXexCallcapImportsInfo");
	}
	else if(key == 0x182) // XEX_HEADER_FASTCAP_ENABLED			XEX_HEADER_FLAG(0x0182)
		MakeRptCmt(entadr, form("XEX_HEADER_FASTCAP_ENABLED = 0x%x", val));
	else if(key == 0x183) // XEX_HEADER_PE_MODULE_NAME			XEX_HEADER_STRING_FIELD(0x0183)
		makeXexHeaderStringField(entadr, "XEX_HEADER_PE_MODULE_NAME", val+base, "HvXexPeName", key);
	else if(key == 0x200) // XEX_HEADER_BUILD_VERSIONS			XEX_HEADER_SIZEDSTRUCT(0x0200)
	{
		tmp = Dword(val+base); // name table size
		MakeRptCmt(entadr, form("XEX_HEADER_IMPORTS key 0x%x offset 0x%x (size: 0x%x)", key, val+base, tmp));
		makeNamedDword(val+base, "HvXexLibraryInfoSz");
		makeStructNamed(val+base+4, "XEXIMAGE_LIBRARY_VERSION", "HvXexLibraryInfo");
		MakeArray(val+base+4, (tmp-4)/0x10);
		
	}
	else if(key == 0x201) // XEX_HEADER_TLS_DATA				XEX_HEADER_STRUCT(0x201, XEX_TLS_DATA)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_TLS_DATA key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeStructNamed(val+base, "XEX_TLS_DATA", "HvXexTlsDataInfo");
	}

	else if(key == 0x202) // XEX_HEADER_STACK_SIZE				XEX_HEADER_FLAG(0x0202)
		MakeRptCmt(entadr, form("XEX_HEADER_STACK_SIZE = 0x%x", val));
	else if(key == 0x203) // XEX_HEADER_FSCACHE_SIZE			XEX_HEADER_ULONG(0x203)
		MakeRptCmt(entadr, form("XEX_HEADER_FSCACHE_SIZE = 0x%x", val));
	else if(key == 0x204) // XEX_HEADER_XAPI_HEAP_SIZE			XEX_HEADER_ULONG(0x204)
		MakeRptCmt(entadr, form("XEX_HEADER_XAPI_HEAP_SIZE = 0x%x", val));
	else if(key == 0x0280) // XEX_HEADER_PAGE_HEAP_SIZE_FLAGS	XEX_HEADER_STRUCT(0x0280, XEX_PAGE_HEAP_OPTIONS)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_PAGE_HEAP_SIZE_FLAGS key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeStructNamed(val+base, "XEX_PAGE_HEAP_OPTIONS", "HvXexPageHeapOptionsInfo");
	}
	else if(key == 0x300) // XEX_HEADER_PRIVILEGE(priv)			(XEX_HEADER_FLAG(0x0300)+((priv&~0x1f)<<3))
		MakeRptCmt(entadr, form("XEX_HEADER_ADDITIONAL_TITLE_MEM = 0x%x", val));
	else if(key == 0x402) // XEX_HEADER_WORKSPACE_SIZE			XEX_HEADER_ULONG(0x402)
		MakeRptCmt(entadr, form("XEX_HEADER_WORKSPACE_SIZE = 0x%x", val));
	else if(key == 0x400) // XEX_HEADER_EXECUTION_ID 			XEX_HEADER_STRUCT(0x400, XEX_EXECUTION_ID)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_EXECUTION_ID key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeStructNamed(val+base, "XEX_EXECUTION_ID", "HvXexExecutionIdInfo");
		if(base >= 0x6601F20000)
			segHvNameSegPointer("pHvXexExecutionIdInfo", val+base); 			// .quad HvXexExecutionIdInfo      # 6601F20AF4 << VARIABLE
	}
	else if(key == 0x403) // XEX_HEADER_GAME_RATINGS			XEX_HEADER_FIXED_SIZE(0x403, XEX_NUMBER_GAME_RATING_SYSTEMS)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_GAME_RATINGS key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeNamedByteArray(val+base, size, "HvXexGameRatingsInfo");
	}
	else if(key == 0x404) // XEX_HEADER_LAN_KEY					XEX_HEADER_FIXED_SIZE(0x404, XEX_LAN_KEY_SIZE)
	{
		MakeRptCmt(entadr, form("XEX_HEADER_LAN_KEY key 0x%x offset 0x%x size 0x%x", key, val+base, size));
		makeNamedByteArray(val+base, size, "HvXexLanKeyInfo");
	}
	else if(key == 0x408) // XEX_HEADER_ADDITIONAL_TITLE_MEM	XEX_HEADER_ULONG(0x408)
		MakeRptCmt(entadr, form("XEX_HEADER_ADDITIONAL_TITLE_MEM = 0x%x", val));
	else
	{
		if(size <= 4) // fixed size stored in val
		{
			MakeRptCmt(entadr, form("FLAG/DWORD key 0x%x val 0x%x", key, val));
		}
		else if(size != 0xFF) // fixed size array offset by val
		{
			MakeRptCmt(entadr, form("FIXED_SZ_STRUCT key 0x%x offset 0x%x size 0x%x", key, val+base, size));
			tname = form("XEX_DIRECTORY_FIXED_SZ_STRUCT_KEY_%x", key);
			makeNamedByteArray(val+base, size, tname);
		}
		else // fixed size array with size at offset by val
		{
			tmp = Dword(val+base);
			MakeRptCmt(entadr, form("SIZED_STRUCT key 0x%x offset 0x%x size 0x%x (dsize: 0x%x)", key, val+base, size, tmp));
			tname = form("XEX_DIRECTORY_SIZED_STRUCT_KEY_%x_SZ", key);
			makeNamedDword(val+base, tname);
			tname = form("XEX_DIRECTORY_SIZED_STRUCT_KEY_%x", key);
			makeNamedByteArray(val+base+4, tmp-4, tname);
			
		}
	}
}

// for page 0000006601f20000 or 0x8e050000 in kernel
static setupXexCacheSection(base)
{
	auto addr, tmp, i;
	MakeRptCmt(base, "0x66_01f20000 physical, 0x8e050000 virtual");
	makeStructNamed(base, "IMAGE_XEX_HEADER", "HvTitleXexInfo");
	// directory info right after header
	tmp = Dword(base+0x14); // IMAGE_XEX_HEADER.HeaderDirectoryEntryCount
	addr = base+0x18;
	for(i = 0; i < tmp; i++)
	{
		parseXexDirectoryEntry(base, addr, i);
		addr = addr+8;
	}
	
	
	// security info from pointer in header
	addr = Dword(base+0x10);
	makeStructNamed(addr, "XEX_SECURITY_INFO", "HvXexSecurityInfo");
	tmp = Dword(addr+0x180); // IMAGE_XEX_HEADER->SecurityInfo.PageDescriptorCount
	addr = addr+0x184;
	makeStructNamed(addr, "HV_PAGE_INFO", "HvXexPageInfo");
	MakeArray(addr, tmp);
	

	
}

// for page 0x01f00000 0x8e010000
	// loadSectionFile(0x8e010000, "8e010000.bin");
	// loadSectionFile(0x90001000, "90001000.bin");
	// loadSectionFile(0x90002000, "90002000.bin");

static segHvSetupSegPointers()
{
	auto tmp;
	segHvNameSegPointer("pDynamicCrlData", 0x6401F10000); 		// 6401F10000 my guess is this is used for crl sent from live
	segHvNameSegPointer("pDynamicCrlDataHead", 0x6401F10150); 	// 6401F10150
	segHvNameSegPointer("pHvConsoleIdHash", 0x6401F18000); 		// p6401F18000:.quad HvConsoleIdHash       # 6401F18000
//	segHvNameSegPointer("pHvFlagsCacheBase", 0x6401F18000); 	// pHvFlagsCacheBase:.quad HvConsoleIdHash # 6401F18000
	segHvNameSegPointer("pHvSecDataCache", 0x6401F18700); 		// 6401F18700
	segHvNameSegPointer("pHvKvConsoleInfoCopy", 0x6401F1A000); 	// 6401F1A000
	segHvNameSegPointer("pHvFcrtCache", 0x6401F1C000); 			// 6401F1C000

	segHvNameSegPointer("pHvTitleXexInfo", 0x6601F20000); 		// .quad HvTitleXexInfo            # 6601F20000
//	segHvNameSegPointer("pHvXexExecutionIdInfo", 0x6601F20AF4); // .quad HvXexExecutionIdInfo      # 6601F20AF4 << VARIABLE, its named when xex header is parsed

	segHvNameSegPointer("pDaeCacheAddr", 0x6C01F40000); 		// DaeCacheAddr:.quad aDaepo0              # 6C01F40000
	tmp = Dword(0x6C01F40000);
	if(tmp == 0x44414550) // 'DAEP'
	{
		tmp = 0x6C01F40000+(Word(0x6C01F40004));
		segHvNameSegPointer("pDaeH2CacheAddr", tmp); 			// .quad 6C01F46F30            # DAE second header VARIABLE
		
	}

	segHvNameSegPointer("pHvKeyVault", 0x7601EE0000); 			// pHvKeyVault:.quad HvKeyVaultData        # 7601EE0000
	segHvNameSegPointer("pHvExKeyVault", 0x7601EE4000); 		// phvKeyScratchBuffer:.quad hvKeyScratchBuffer # 7601EE4000
	segHvNameSegPointer("phvRevocationListLock", 0x7601EE8000);	// 7601EE8000 hvRevocationListLock
	segHvNameSegPointer("phvXeCryptAllocMem", 0x7601EEC000); 	// 7601EEC000 hvXeCryptAllocMem for 0x2800 MAX
	segHvNameSegPointer("phvXexResolveImportBuf", 0x7601EEE800);// 7601EEE800 - 7601EEF800 xex import resolve/buffer used by HvxResolveImports
	segHvNameSegPointer("pHvSecDataCache", 0x7601EEF800); 		// 7601EEF800
	segHvNameSegPointer("phvIptvSessionBase", 0x7601EEFC00); 	// 7601EEFC00
	segHvNameSegPointer("pHvTitleKeys", 0x7601EEFD80); 			// 7601EEFD80
	segHvNameSegPointer("pHvTitleKeyProps", 0x7601EEFE00); 		// 7601EEFE00

	segHvNameSegPointer("pCrlData", 0x8000030001EF0000); 		// pCrlData:.quad HvCrlData                # 8000030001EF0000
	segHvNameSegPointer("pCrlDataHead", 0x8000030001EF0150); 	// pCrlDataHead:.quad HvCrlData.DataHeader # 8000030001EF0150

	
	segHvNameSegPointer("p6601F20108", 0x6601F20108); 			// 6601F20108
	segHvNameSegPointer("p6801F54400", 0x6801F54400); 			// 6801F54400
	segHvNameSegPointer("p6801F5440C", 0x6801F5440C); 			// 6801F5440C
	segHvNameSegPointer("p6801F56800", 0x6801F56800); 			// 6801F56800
	segHvNameSegPointer("p6801F56808", 0x6801F56808); 			// 6801F56808
	
	segHvNameSegPointer("pHvExpansionBase", 0x6E01F30000); 		// 6E01F30000 expansion data
//	segHvNameSegPointer("pHvEncryptedAllocations", 0x6a01f6c000);// 6a01f6c000 encrypted page allocations via build_6a_01f6c000
	
}

/*
hv pte pages for 64k pages are stored like this
struct{ // (pte&FFFFFFC0)<<10 == full address
	DWORD PageHigh : 10; // 0x80000XXX_00000000 FFC00000 (addr&0xFFF00000000)>>14
	DWORD PageLow : 16;  // 0x80000000_XXXX0000 003FFFC0 (addr&0xFFFF0000)>> 10
	DWORD EndOfImage : 1;
	DWORD StartOfImage : 1;
	DWORD Valid : 1;
	DWORD NoExecute : 1;
	DWORD Data : 1;
	DWORD ReadOnly : 1;
};
the 4k pages are actually 64k pages with 16 nibbles for per 4k page perms stored elsewhere
*/

// note that the 4k page perms are stored in nibbles, where the second half of the byte 0x0F is the first page and the first is the second page 0xF0
static segMap4kPtePerms(base, baseaddr)
{
	auto i, fstr, sstr, tby, virt;
	auto eoi, soi, val, nx, data, ro;
	MakeUnknown(base, 8, 2);
	for(i = 0; i < 8; i++)
	{
		virt = baseaddr+(i*0x2000);
		tby = Byte(base+i);
		MakeByte(base+i);
		if((tby&0xF) != 0)
		{
			val = (tby>>3)&1;
			nx = (tby>>2)&1;
			data = (tby>>1)&1;
			ro = tby&1;
			fstr = form("virt: 0x%08x Unk:%d NoEx:%d Data:%d ReadOnly:%d", virt, val, nx, data, ro);
		}
		else
			fstr = form("virt: 0x%08x (not mapped)", virt);
		virt = virt+0x1000;
		if((tby&0xF0) != 0)
		{
			val = (tby>>7)&1;
			nx = (tby>>6)&1;
			data = (tby>>5)&1;
			ro = (tby>>4)&1;
			sstr = form("virt: 0x%08x Unk:%d NoEx:%d Data:%d ReadOnly:%d", virt, val, nx, data, ro);
		}
		else
			sstr = form("virt: 0x%08x (not mapped)", virt);
		MakeRptCmt(base+i, form("%s\n%s", fstr, sstr));
	}
}

// phy 6801F50000 not mapped to virtual
static segHvSetupPteTable(base)
{
	auto i, addr, tdw, phy, virt;
	auto eoi, soi, val, nx, data, ro;

	// physical address = (pteval&0xFFFFFFC0)<<10
	for(i = 0; i < 0x2000; i++)
	{
		addr = (i*4)+base;
		MakeUnknown(addr, 4, 2);
		MakeDword(addr);
		tdw = Dword(addr);
		virt = 0x80000000+(i*0x10000);
		if(tdw != 0)
		{
			phy = (tdw&0xFFFFFFC0)<<10;
			eoi = (tdw>>5)&1;
			soi = (tdw>>4)&1;
			val = (tdw>>3)&1;
			nx = (tdw>>2)&1;
			data = (tdw>>1)&1;
			ro = tdw&1;
			if(i < 0x1000)
				MakeRptCmt(addr, form("phy: 0x%016x virt: 0x%08x EOI:%d SOI:%d Valid:%d NoEx:%d Data:%d ReadOnly:%d", phy, virt, eoi, soi, val, nx, data, ro));
			else
			{
				tdw = ((i-0x1000)*8)+(base+0x8000);
				MakeRptCmt(addr, form("phy: 0x%016x virt: 0x%08x EOI:%d SOI:%d Valid:%d NoEx:%d Data:%d ReadOnly:%d page perms 0x%x", phy, virt, eoi, soi, val, nx, data, ro, tdw));
				segMap4kPtePerms(tdw, virt);
			}
		}
		else
		{
			MakeRptCmt(addr, form("virt: 0x%08x (unmapped)", virt));
			if(i >= 0x1000)
			{
				tdw = ((i-0x1000)*8)+(base+0x8000);
				segMap4kPtePerms(tdw, virt);
			}				
		}
			
	}
	MakeName(base, "HvPteTable64k");
	MakeName(base+0x8000, "HvPteTable4kPerms");
	
}
/*
Virtual: 0x90000000 Physical: 0x01f30000 NOT MAPPED
Virtual: 0x90001000 Physical: 0x01f31000 Protect: 0x0002 (Image 4K)
Virtual: 0x90002000 Physical: 0x01f32000 Protect: 0x0004 (Image 4K)
Virtual: 0x91000000 Physical: 0x00980000 Protect: 0x0002 (Image 4K)
Virtual: 0x91001000 Physical: 0x00981000 Protect: 0x0002 (Image 4K)
Virtual: 0x91002000 Physical: 0x00982000 Protect: 0x0002 (Image 4K)
Virtual: 0x91003000 Physical: 0x00983000 Protect: 0x0002 (Image 4K)

_0000006801f50000.bin:6801F54000         .long 0x5B807CCF                # phy: 0x0000016e01f30000 virt: 0x90000000 EOI:0 SOI:0 VAL:1 NX:1 D:1 RO:1

_0000006801f50000.bin:6801F54400 dword_6801F54400:.long 0xC000261E       # phy: 0x0000030000980000 virt: 0x91000000 EOI:0 SOI:1 VAL:1 NX:1 D:1 RO:0
_0000006801f50000.bin:6801F54404         .long 0xC00025CE                # phy: 0x0000030000970000 virt: 0x91010000 EOI:0 SOI:0 VAL:1 NX:1 D:1 RO:0
_0000006801f50000.bin:6801F54408         .long 0x258E                    # phy: 0x0000000000960000 virt: 0x91020000 EOI:0 SOI:0 VAL:1 NX:1 D:1 RO:0
_0000006801f50000.bin:6801F5440C dword_6801F5440C:.long 0x256E           # phy: 0x0000000000950000 virt: 0x91030000 EOI:1 SOI:0 VAL:1 NX:1 D:1 RO:0

_0000006801f50000.bin:6801F58000 HvPteTable4kPerms:.byte 0xF0 # = aka F0 0E 00 00 00 00
_0000006801f50000.bin:6801F58001         .byte  0xE
_0000006801f50000.bin:6801F58002         .byte    0
_0000006801f50000.bin:6801F58003         .byte    0
_0000006801f50000.bin:6801F58004         .byte    0
*/

// phy 6a01f60000
static segSetupEncryptedInfo(base)
{
	auto end;
	end = base+0x10000;
/* these only seem to occur in 0000006a01f60000 */
	nameBinaryScan(base, end, "58 42 4F 58 5F 52 4F 4D 5F 42", 10, "HveROMBSalt"); // XBOX_ROM_B (there is more than one !)
	nameBinaryScan(base, end, "58 42 4F 58 5F 42 4C 50 52 56", 10, "HveBLPRVSalt"); // XBOX_BLPRV
	nameBinaryScan(base, end, "58 42 4F 58 33 36 30 58 45 58", 10, "HveXEXSalt"); // XBOX360XEX
	nameBinaryScan(base, end, "58 42 4F 58 52 45 56 58 45 58", 10, "HveREVXEXSalt"); // XBOXREVXEX
	nameBinaryScan(base, end, "44 56 44 2D 58 47 44 32", 8, "HveXGD2str"); // DVD-XGD2
	nameBinaryScan(base, end, "58 42 4F 58 33 36 30 45 58 50", 10, "HveEXPSalt"); // XBOX360EXP
	nameBinaryScan(base, end, "47 45 4E 55 49 4E 45 20 58 42 4F 58 20 4F 44 44", 16, "HveGenericOddStr"); // GENUINE XBOX ODD
	
	nameBinaryScan(base, end, "58 42 4F 58 5F 45 58 5F 30 31", 10, "HveEX01Salt"); // XBOX_EX_01
	nameBinaryScan(base, end, "50 4C 44 53 20 20 20 20 44 47 2D 31 36 44 34 53 20 20 20 20 20 20 20 20 39 35 30 34", 28, "HveLITE9504Sig"); // "PLDS    DG-16D4S        9504"
	
	makeNamedByte(base+0xc000, "HvEncryptedAllocationPerms");

}

// phy 8000020000000000 the first half of ROM is a mirror of the second half, 0x8000 each
static segSetupSoCROM(base)
{
	auto tdw;
	makeStructNamed(base, "BLDR_1BLROM", "HvBootRom");
	tdw = Dword(base+0xFC) + base;
	// everything else in between header and table is code, everything after is unused
	makeStructNamed(tdw, "BLDR_1BLROM_TABLE", "Rom1BlTable");
	// tdw = Dword(base+0x8) + base; // should always be 0x100 because of the way PPC reset vectors work
	// makeNamedFunction(tdw, "Rom1blEntryPoint");
	nameVectors(base, "_soc");
	
	makeNamedByteArray(base+0x8000, 0x8000, "HvBootRomMirror");
}

// phy 8000020000010000
static segSetupSoCSRAM(base)
{
	// this entire segment is SRAM on the CPU die
	makeStructNamed(base, "SOCSECRAM_BLOCK", "SocSram");
}

// phy 8000020000020000
static segSetupSoCRegs(base)
{
	makeStructNamed(base, "SOCSECOTP_BLOCK", "SocSecOtpBlock");
	makeStructNamed(base+0x4000, "SOCSECENG_BLOCK", "SocSecEngBlock");
	// makeNamedQword(base+0x50b0, "Bl1TransformMagic");
		// MakeRptCmt(base+0x50b0, "This value is copied to Hv space at 0x18 on hv startup");

	// makeNamedQword(base+0x6008, "PrngHardwareRegister");
	makeStructNamed(base+0x6000, "SOCSECRNG_BLOCK", "SocSecRngBlock");
	
	makeStructNamed(base+0x8000, "SOCCBI_BLOCK", "SocCbiBlock");
	
}

// phy 8000020000030000
static segSetupSoCFsbTxxRegs(base)
{
	makeStructNamed(base, "SOCFSBTTX_BLOCK", "SocFsbTtxBlock");
	makeStructNamed(base+0x8000, "SOCFSBTRX_BLOCK", "SocFsbTrxBlock");
}

// phy 8000020000040000
static segSetupSoCFsbLxxRegs(base)
{
	makeStructNamed(base, "SOCFSBLTX_BLOCK", "SocFsbLtxBlock");
	makeStructNamed(base+0x8000, "SOCFSBLRX_BLOCK", "SocFsbLrxBlock");
}

// phy 8000020000050000
static segSetupSoCIntRegs(base)
{
	makeStructNamed(base, "SOCINTS_BLOCK", "SocIntsBlock");
}

// phy 8000020000060000
static segSetupSoCPwmPrvRegs(base)
{
	makeStructNamed(base, "SOCPMW_BLOCK", "SocPwmBlock");
	makeStructNamed(base+0x1000, "SOCPRV_BLOCK", "SocPrvBlock");
}

static segRelocateHvBasePages()
{
#ifdef __EA64__
	//SetupSection
#endif

}

// these segments are difficult if not impossible to dump due to the fact
// that many of the registers cause hardware effects on reads
// ie: reading an interrupt off the ints register causes the interrupt controller to push to the next interrupt
static segCreateSocRegs()
{
#ifdef __EA64__
	SetupSection(0x8000020000030000, 0x8000020000030000+0x10000, "DATA", 4|2, "SoCFsbTxx", 1);
	segSetupSoCFsbTxxRegs(0x8000020000030000);
	SetupSection(0x8000020000040000, 0x8000020000040000+0x10000, "DATA", 4|2, "SoCFsbLxx", 1);
	segSetupSoCFsbLxxRegs(0x8000020000040000);
	SetupSection(0x8000020000050000, 0x8000020000050000+0x10000, "DATA", 4|2, "SoCInts", 1);
	segSetupSoCIntRegs(0x8000020000050000);
	SetupSection(0x8000020000060000, 0x8000020000060000+0x10000, "DATA", 4|2, "SoCPwmPrv", 1);
	segSetupSoCPwmPrvRegs(0x8000020000060000);
	
#endif
}





