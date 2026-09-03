#include <idc.idc>
#include "generic_functions.idc"
#include "segments.idc"
#include "syscall_names.idc"

static GetHvExp2Name(funcNum) {
	     if(funcNum == 0x0) return "HvpAcquireSpinLock";
	else if(funcNum == 0x1) return "HvpReleaseSpinLock";
	else if(funcNum == 0x2) return "HvpPhysicalToReal";
	else if(funcNum == 0x3) return "HvpRelocateCacheLines";
	else if(funcNum == 0x4) return "HvpRelocatePhysicalToEncrypted";
	else if(funcNum == 0x5) return "HvpRelocateEncryptedToPhysical";
	else if(funcNum == 0x6) return "HvpRelocatePhysicalToProtected";
	else if(funcNum == 0x7) return "HvpRelocateProtectedToPhysical";
	else if(funcNum == 0x8) return "HvpFlagsUpdate";
	else if(funcNum == 0x9) return "HvpPkcs1Verify";
	else if(funcNum == 0xA) return "memcmp";
	else if(funcNum == 0xB) return "memcpy";
	else if(funcNum == 0xC) return "memset";
	else if(funcNum == 0xD) return "XeCryptAesCbc";
	else if(funcNum == 0xE) return "XeCryptAesEcb";
	else if(funcNum == 0xF) return "XeCryptAesKey";
	else if(funcNum == 0x10) return "XeCryptBnDwLePkcs1Format";
	else if(funcNum == 0x11) return "XeCryptBnDwLePkcs1Verify";
	else if(funcNum == 0x12) return "XeCryptBnDw_Copy";
	else if(funcNum == 0x13) return "XeCryptBnDw_SwapLeBe";
	else if(funcNum == 0x14) return "XeCryptBnQwBeSigFormat";
	else if(funcNum == 0x15) return "XeCryptBnQwBeSigVerify";
	else if(funcNum == 0x16) return "XeCryptBnQwNeModExp";
	else if(funcNum == 0x17) return "XeCryptBnQwNeModInv";
	else if(funcNum == 0x18) return "XeCryptBnQwNeModMul";
	else if(funcNum == 0x19) return "XeCryptBnQwNeRsaPrvCrypt";
	else if(funcNum == 0x1A) return "XeCryptBnQwNeRsaPubCrypt";
	else if(funcNum == 0x1B) return "XeCryptBnQw_Zero";
	else if(funcNum == 0x1C) return "XeCryptBnQw_Copy";
	else if(funcNum == 0x1D) return "XeCryptBnQw_SwapDwQwLeBe";
	else if(funcNum == 0x1E) return "XeCryptDes3Cbc";
	else if(funcNum == 0x1F) return "XeCryptDes3Ecb";
	else if(funcNum == 0x20) return "XeCryptDes3Key";
	else if(funcNum == 0x21) return "XeCryptDesEcb";
	else if(funcNum == 0x22) return "XeCryptDesCbc";
	else if(funcNum == 0x23) return "XeCryptDesKey";
	else if(funcNum == 0x24) return "XeCryptDesParity";
	else if(funcNum == 0x25) return "XeCryptHammingWeight";
	else if(funcNum == 0x26) return "XeCryptHmacSha";
	else if(funcNum == 0x27) return "XeCryptHmacShaFinal";
	else if(funcNum == 0x28) return "XeCryptHmacShaInit";
	else if(funcNum == 0x29) return "XeCryptHmacShaUpdate";
	else if(funcNum == 0x2A) return "XeCryptMemAlloc";
	else if(funcNum == 0x2B) return "XeCryptMemFree";
	else if(funcNum == 0x2C) return "XeCryptRandom";
	else if(funcNum == 0x2D) return "XeCryptRc4";
	else if(funcNum == 0x2E) return "XeCryptRc4Ecb";
	else if(funcNum == 0x2F) return "XeCryptRc4Key";
	else if(funcNum == 0x30) return "XeCryptRotSum";
	else if(funcNum == 0x31) return "XeCryptRotSum4";
	else if(funcNum == 0x32) return "XeCryptRotSumSha";
	else if(funcNum == 0x33) return "XeCryptSha";
	else if(funcNum == 0x34) return "XeCryptShaFinal";
	else if(funcNum == 0x35) return "XeCryptShaInit";
	else if(funcNum == 0x36) return "XeCryptShaUpdate";
	else if(funcNum == 0x37) return "XeCryptSwizzle";
	else if(funcNum == 0x38) return "XeCryptUidEccEncode";
	else if(funcNum == 0x39) return "XeCryptUidEccDecode";
	else return form("HvxExp2_%02X", funcNum);
}

static SetupPointerBranches()
{
	auto currAddr, baseaddr, i, str, tabl, temp, ttbl;
	tabl = 0;
	ttbl = 0;
	for(currAddr = 0; currAddr != BADADDR; currAddr = currAddr + 4)
	{
		currAddr = FindBinary(currAddr, SEARCH_DOWN, "7D 8C 58 2E"); // lwzx    %r12, %r12, %r11
		if(currAddr == BADADDR)
			break;
		if((currAddr & 0x3) == 0)
		{
			currAddr = currAddr - 4;
			MakeFunction(currAddr, BADADDR);
			MakeName(currAddr, form("jt%d_jumper", tabl));
			temp = Word(currAddr+2);
			baseaddr = Dword(temp);
			Message(form("branch table at 0x%08X\n",baseaddr));
			MakeNameEx(baseaddr, "HvTbl", 0);
			//SetType(0xAE84, "unsigned int*;");
	
			currAddr = FindBinary(currAddr, SEARCH_DOWN, "39 60 00 00 4B FF"); 
			i = 0;
			while((Word(currAddr)&0x3960) == 0x3960)
			{
				//MakeUnknown(currAddr, 8, 0); // DOUNK_SIMPLE
				temp = Word(currAddr+2);
				MakeDword(baseaddr+temp);
				OpOff(baseaddr+temp, 0, 0);
				//Message(form("table %08x offset %08x curr %08x\n",baseaddr, temp, currAddr));
				str = CommentEx(currAddr, 1);
				if(strlen(str) != 0) // was commented previously, use the function name put there
				{
					ttbl = tabl; // reset this
					MakeNameEx(Dword(baseaddr+temp), str, 0);
					MakeFunction(currAddr, BADADDR);
					SetFunctionCmt(currAddr, form("<- b 0x%X %s", Dword(baseaddr+temp), str), 1);
					// MakeRptCmt(currAddr, form("<- b 0x%X %s", Dword(baseaddr+temp), str));
					// MakeNameEx(currAddr, form("jt%d_%s", tabl, str), 0); // old way doesn't work well with TIL
					while(MakeNameEx(currAddr, form("%s_%d", str, ttbl), SN_NOWARN) != 1)
					{
						ttbl = ttbl+1;
					}
				}
				else
				{
					MakeFunction(currAddr, BADADDR);
					SetFunctionCmt(currAddr, form("<- b 0x%X", Dword(baseaddr+temp)), 1);
					// MakeRptCmt(currAddr, form("<- b 0x%X", Dword(baseaddr+temp)));
				}
					
				MakeFunction(Dword(baseaddr+temp), BADADDR);
				currAddr = currAddr + 8;
			}
			tabl = tabl+1;
		}
	}
}

static SetupSyscallTable()
{
	auto currAddr, testAddr, sctable, scmax, i, scOff, str;

	for(currAddr=0; currAddr != BADADDR; currAddr=currAddr+4)
	{
		currAddr = FindBinary(currAddr, SEARCH_DOWN, "28 00 00");
		if(currAddr == BADADDR)
			break;
		// if((currAddr & 0x3) == 0)
			// break;
		if(currAddr > 0xC00)
			currAddr = BADADDR;
		scmax = Byte(currAddr+3);
		if(scmax > 0x50)
			break;
	}
	if(currAddr != BADADDR)
	{
		testAddr = FindBinary(currAddr, SEARCH_DOWN, "3C 84 00 01");
		if(testAddr != BADADDR)
		{
			sctable = 0x10000+(Word(testAddr+6));
			MakeNameEx(sctable, "_SyscallTable", 0);
			scmax = Byte(currAddr+3);
			Message(form("Total syscalls %d, handler at 0x%0x, table at 0x%08X\n", scmax, currAddr, sctable));
			MakeFunction(currAddr, BADADDR);
			MakeNameEx(currAddr, "HvxHvExpTableHandler", 0);
			MakeRptCmt(currAddr, form("Syscall Table at 0x%x",sctable));

			MakeUnknown(sctable, scmax*4, DOUNK_DELNAMES);// DOUNK_SIMPLE 0 DOUNK_DELNAMES  0x0002
			
			for(i=scmax-1; i>=0; i=i-1)
			{
				MakeDword(sctable+(4*i));
				OpOff(sctable+(4*i), 0, 0);
				scOff = Dword(sctable+(4*i));
				str = myGetSyscallName(i);
				if((Dword(scOff) == 0x38600000) && (Dword(scOff+4) == 0x4E800020))
				{
					MakeRptCmt(sctable+(4*i), form("%s (disabled)", str));
				}
				else if((Word(scOff) == 0x3960) && (Word(scOff+4) == 0x4BFF)) // is a jumptable
				{
					MakeUnknown(scOff, 8, DOUNK_DELNAMES);
					MakeRptCmt(scOff, str);
				}
				else
				{
					MakeUnkn(scOff, DOUNK_EXPAND|DOUNK_DELNAMES);
					MakeFunction(scOff, BADADDR);
					MakeNameEx(scOff, str, 0);
				}
			}
		}
	}
	else
		Message(form("Could not find the syscall table!\n"));
}

static SetupExp2Table()
{
	auto currAddr, testAddr, i, str;
	i = 0;
	MakeDword(0x4c);
	MakeNameEx(0x4c, "pHvExp2", 0);
	currAddr = Dword(0x4c); // the first address in the table
	MakeNameEx(currAddr, "HvExp2", 0);
	while(Dword(currAddr) != 0)
	{
		testAddr = Dword(currAddr); // the address the table is pointing at
		MakeDword(currAddr);
		OpOff(currAddr, 0, 0);
		str = GetHvExp2Name(i);
		if((Word(testAddr) == 0x3960) && (Word(testAddr+4) == 0x4BFF)) // is a jumptable
		{
			MakeRptCmt(testAddr, str);
		}
		else
		{
			MakeUnkn(testAddr, DOUNK_EXPAND|DOUNK_DELNAMES);
			MakeFunction(testAddr, BADADDR);
			MakeNameEx(testAddr, str, 0);
		}
		currAddr = currAddr +4;
		i = i+1;
	}
}

static makeKey(bytes, name, startAddr, endAddr)
{
	auto cqw, stype, offset, ret, end, rend, mask;
	mask = getAddrMask();
	if(endAddr == 0)
		end = MAXADDR;
	else
		end = endAddr;
	offset = FindBinary(startAddr, SEARCH_DOWN, bytes);
	if(offset != BADADDR)
	{
		cqw = Dword(offset);
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
			return (offset+4);
		}
		rend = end-((cqw*8)+0x10);
		if((offset&mask) < rend)
		{
			ret = MakeStructEx(offset, -1, stype);
			if(ret == 0)
				Message(form("Could not make %s struct type %s offset: 0x%08x err: %x\n", name, stype, offset, ret));
			else
				Message(form("Key struct %s marked at offset: 0x%08x\n", name, offset));
//				Message(form("Key struct %s marked at offset: 0x%08x (range 0x%x to 0x%x rend 0x%x)\n", name, offset, startAddr, end, rend));
			MakeName(offset, name);
			offset = offset+4;
		}
		else
		{
			Message(form("Found key %s outside of range 0x%x-0x%x at 0x%x\n", name, startAddr, end, offset));
			return BADADDR;
		}
	}
	// else
		// Message(form("Did not find %s\n", name));
	return offset;
}

static makeKeyScan(bytes, name, startAddr, endAddr)
{
	auto currAddr, count, rename;
	currAddr = startAddr;
	count = 0;
	rename = form("%s", name);
	while(currAddr != BADADDR)
	{
		if(count != 0)
			rename = form("%s_%d", name, count);
		currAddr = makeKey(bytes, rename, currAddr, endAddr);
		count = count+1;
	}
}

static nameBinaryFunction(startAddr, bytes, name)
{
	auto offset;
	offset = FindBinary(startAddr, SEARCH_DOWN, bytes);
	if(offset != BADADDR)
	{
		MakeName(offset, name);
		MakeFunction(offset, BADADDR);
		Message(form("Named function %s at offset: 0x%08x\n", name, offset));	
	}
	else
		Message(form("Did not find binary function %s\n", name));	

	return offset;
}

static nameBinaryFunctionComment(startAddr, bytes, name, comment)
{
	auto currAddr;
	currAddr = nameBinaryFunction(startAddr, bytes, name);
	if(currAddr != BADADDR)
		SetFunctionCmt(currAddr, comment, 1);
}

static nameFunctionByOffset(offset, name)
{
	auto start;
	start = GetFunctionAttr(offset, FUNCATTR_START);
	if(start != BADADDR)
	{
		MakeName(start, name);
		Message(form("Named function %s at offset: 0x%08x\n", name, start));
	}
	else
		Message(form("could not find function start for %s in 0x%x\n", name, offset));	
}

static nameFunctionFindBytes(name, bytes)
{
	auto ea, start;
	ea = FindBinary(0, SEARCH_DOWN, bytes);
	if(ea != BADADDR)
	{	
		start = GetFunctionAttr(ea,FUNCATTR_START);
		if(start != BADADDR)
		{
			MakeName(start, name);
			Message(form("Named function %s at offset: 0x%08x\n", name, start));
			ea = FindBinary(ea+4, SEARCH_DOWN, bytes);
			if(ea != BADADDR)
			{
				Message(form("# WARNING second instance found at 0x%x\n", name, ea));	
			}
		}
		else
			Message(form("could not find function start for %s in 0x%x\n", name, ea));	
	}
	else
		Message(form("Did not find binary function %s\n", name));	
}

static nameFunctionByXref(name, callname, xrnum)
{
	auto ea, ref, start, xrc;
	ea = LocByName(callname);
	if(ea != BADADDR)
	{
		ref = RfirstB(ea); // first xref
		if(ref != BADADDR)
		{
			xrc = 1;
			while(xrc < xrnum)
			{
				ref = RnextB(ea, ref);
				xrc = xrc + 1;
			}
			if(ref != BADADDR)
			{
				start = GetFunctionAttr(ref,FUNCATTR_START);
				if(start != BADADDR)
				{
					MakeName(start, name);
					Message(form("Named function %s at offset: 0x%08x\n", name, start));
				}
				else
					Message(form("cound not find function start for %s at %08x!!\n", name, ref));
			}
			else
				Message(form("cound not find xref #%d calling %s to name %s!!\n", xrnum, callname, name));
		}
	}
	else
		Message(form("cound not find caller %s to name %s!!\n", callname, name));
}

static getFunctionXrefOffset(callname, xrnum)
{
	auto ea, ref, start, xrc;
	ea = LocByName(callname);
	if(ea != BADADDR)
	{
		ref = RfirstB(ea); // first xref
		if(ref != BADADDR)
		{
			xrc = 1;
			while(xrc < xrnum)
			{
				ref = RnextB(ea, ref);
				xrc = xrc + 1;
			}
			if(ref != BADADDR)
				return ref;
			// else
				// Message(form("cound not find xref #%d calling %s!!\n", xrnum, callname));
		}
	}
	else
		Message(form("cound not find caller %s!!\n", callname));
	return BADADDR;
}

// loading segment 0000007601ee0000.bin at 0x7601ee0000
// loading segment 8000030001ef0000.bin at 0x8000030001ef0000 <<virtual 0x8e000000 0x10000 << this is in protected memory, just use virtual dump
// loading segment 0000006401f10000.bin at 0x6401f10000 << virtual 0x8e030000 0x10000 (disk info, some keys)
// loading segment 0000006601f20000.bin at 0x6601f20000 << virtual 0x8e050000 0x10000 (xex2 cache)
// loading segment 0000006e01f30000.bin at 0x6e01f30000
// loading segment 0000006c01f40000.bin at 0x6c01f40000
// loading segment 0000006801f50000.bin at 0x6801f50000
// 		loading segment 0000006801f58000.bin at 0x6801f58000
// loading segment 0000006a01f60000.bin at 0x6a01f60000
// 		loading segment 0000006a01f6c000.bin at 0x6a01f6c000

static loadSectionFile(address, fname)
{
#ifdef __EA64__
	auto fp, len, freal;
	if(address == 0x8000030001ef0000)
		freal = form("8e000000.bin");
	else
		freal = form("%s", fname);
	if((address&0xFFFF) == 0)
	{
		fp = fopen(freal, "rb");
		if(fp != 0)
		{
			len = filelength(fp);
			SetupSection(address, address+len, "DATA", 4|2, fname, 1);
			Message(form("loading segment %s at 0x%x for 0x%x\n", fname, address, len));
			loadfile(fp, 0, address, len);
			fclose(fp);
			if(address == 0x7601EE0000)
				setupHvKvCacheSection(address);
			else if(address == 0x8000030001EF0000) // 0x8e000000 in kernel
				setupCrlCacheSection(address);
			else if(address == 0x6401f10000) // 0x8e030000 in kernel
				setupDiskCacheSection(address);
			else if(address == 0x6601f20000) // 0x8e050000 in kernel
				setupXexCacheSection(address);
			else if(address == 0x6C01F40000) // not in kernel
				setupDaeCacheSection(address);
			else if(address == 0x6801F50000) // pte tables, not in kernel
				segHvSetupPteTable(address);
			else if(address == 0x8000020000000000) // SoC
				segSetupSoCROM(address);
			else if(address == 0x8000020000010000)
				segSetupSoCSRAM(address);
			else if(address == 0x8000020000020000)
				segSetupSoCRegs(address);
			else if(address == 0x6a01f60000)
				segSetupEncryptedInfo(address);
			
		}
		else
			Message(form("could not load %s\n", freal));
	}
// #else
	// Message(form("skipping segment load for %s, ida is not in 64bit mode\n", fname));
#endif
}

//38 60 00 6A 78 63 07 C6 64 63 01 F6 4E 80 00 20 			6A_01F60000
//38 60 00 6C 78 63 07 C6 64 63 01 F4 4E 80 00 20 			6C_01F40000
//38 60 00 6E 78 63 07 C6 64 63 01 F3 4E 80 00 20 			6E_01F30000
//38 60 ||00 64|| 78 63 07 C6 64 63 ||01 F1|| 4E 80 00 20 	64_01F10000
//38 60 00 66 78 63 07 C6 64 63 01 F2 4E 80 00 20 			66_01F20000
//38 60 00 68 78 63 07 C6 64 63 01 F5 4E 80 00 20 			68_01F50000
//															76_01ee0000
static nameBuilders()
{
	auto offset, curroff, name, val, seg;
	offset = 0;
	curroff = 0;
	while(offset != BADADDR)
	{
		seg = 0;
		//Message(form("seeking at %08x from 0x%x\n", curroff,offset));
		offset = FindBinary(curroff, SEARCH_DOWN, "78 63 07 C6 64 63");
		if(offset != BADADDR)
		{
			 //38 60 00 66 78 63 07 C6 64 63 01 F2 4E 80 00 20 66_01F20000
			if(Word(offset-4) == 0x3860)
			{
				if(Dword(offset+8) == 0x4E800020)
				{
					name = form("build_%x_%04x0000", Word(offset-2), Word(offset+6));
					Message(form("Naming %s at 0x%x\n", name, (offset-4)));
					MakeFunction((offset-4), BADADDR);
					MakeNameEx((offset-4), name, 0);
					SetFunctionCmt((offset-4), form("0x0000%04x%04x0000", Word(offset-2), Word(offset+6)), 1);
					// MakeRptCmt((offset-4), form("0x0000%04x%04x0000", Word(offset-2), Word(offset+6)));
					name = form("0000%04x%04x0000.bin", Word(offset-2), Word(offset+6));
					seg = (Word(offset-2)<<32)|(Word(offset+6)<<16);
					loadSectionFile(seg, name);
				}
				//38 60 00 68 ||78 63 07 C6 64 63|| 01 F5 38 63 40 00 38 63 40 00 4E 80 00 20 68_01f58000
				//38 60 00 6A ||78 63 07 C6 64 63|| 01 F6 38 63 60 00 38 63 60 00 4E 80 00 20 6A_01F6C000
				else if(Dword(offset+16) == 0x4E800020)
				{
					val = 0;
					val = (Word(offset+6)<<16);
					val = val+(Word(offset+10));
					val = val+(Word(offset+14));
					name = form("build_%x_%08x", Word(offset-2), val);
					Message(form("Naming %s at 0x%x\n", name, (offset-4)));
					MakeFunction((offset-4), BADADDR);
					MakeNameEx((offset-4), name, 0);
					SetFunctionCmt((offset-4), form("0x0000%04x%08x", Word(offset-2), val), 1);
					// MakeRptCmt((offset-4), form("0x0000%04x%08x", Word(offset-2), val));
					name = form("0000%04x%08x.bin", Word(offset-2), val);
					seg = (Word(offset-2)<<32)|(val);
					loadSectionFile(seg, name);
				}
			}
			else if(Word(offset-8) == 0x3860)
			{
				//38 60 03 00 64 63 80 00 ||78 63 07 C6 64 63|| 01 EF 4E 80 00 20 80000300_01EF0000
				if(Dword(offset+8) == 0x4E800020)
				{
					name = form("build_%04x%04x_%04x0000", Word(offset-2), Word(offset-6), Word(offset+6));
					Message(form("Naming %s at 0x%x\n", name, (offset-8)));
					MakeFunction((offset-8), BADADDR);
					MakeNameEx((offset-8), name, 0);
					SetFunctionCmt((offset-8), form("0x%04x%04x%04x0000", Word(offset-2), Word(offset-6), Word(offset+6)), 1);
					// MakeRptCmt((offset-8), form("0x%04x%04x%04x0000", Word(offset-2), Word(offset-6), Word(offset+6)));
					name = form("%04x%04x%04x0000.bin", Word(offset-2), Word(offset-6), Word(offset+6));
					seg = (Word(offset-2)<<48)|(Word(offset-6)<<32)|(Word(offset+6)<<16);
					loadSectionFile(seg, name);
				}
			}
			curroff = offset+12;	
		}
	}
	loadSectionFile(0x8000020000000000, "8000020000000000.bin");
	loadSectionFile(0x8000020000010000, "8000020000010000.bin");
	loadSectionFile(0x8000020000020000, "8000020000020000.bin");
	segCreateSocRegs();
}

static namePostOutXrefs()
{
	auto addr, ea, cnt, tdw, i;
	addr = 0;
	cnt = 0;
	while(addr != BADADDR)
	{
		addr = getFunctionXrefOffset("HvxPostOutput", cnt);
		if(addr != BADADDR)
		{
			// Message(form("HvxPostOutput ref %d at address 0x%x\n", cnt, addr));
			for(i = 1; i < 6; i++)
			{
				ea = addr-(4*i);
				tdw = Dword(ea);
				if((tdw&0xFFFFFF00) == 0x38600000)
				{
					tdw = tdw&0xFF;
					// Message(form("HvxPostOutput ref %d at address 0x%x does li r3, 0x%x\n", cnt, ea, tdw));
					if(tdw == 0x59)
						nameFunctionByOffset(addr, "HvInitSocMmio");
					else if(tdw == 0x5A)
						nameFunctionByOffset(addr, "HvInitXexTraining");
					else if(tdw == 0x5B)
						nameFunctionByOffset(addr, "HvInitKeyring");
					else if(tdw == 0x5C)
						nameFunctionByOffset(addr, "HvInitKeys");
					else if(tdw == 0x5E)
						nameFunctionByOffset(addr, "HvInitSocInterrupts");
					// else
						// Message(form("skipping HvxPostOutput ref %d for post 0x%x\n", cnt, tdw));
					i = 6;
				}
				// else
					// Message(form("HvxPostOutput ref %d at address 0x%x does not have li r3 instruction\n", cnt, ea));
			}
		}
		cnt = cnt+1;
	}
}

// stuff that needs to be done after auto analysis is complete
static setupFinalFunctions()
{
	auto addr;
	Wait();
	nameFunctionFindBytes("HvpIsBufferZeroed", "89 43 00 00 38 84 FF FF 38 63 00 01 7D 4B 5B 78");
	nameFunctionFindBytes("HvpUpdateManufacturingMode", "81 40 00 30 55 4A 04 A4 2B 0A 00 00");
	nameFunctionFindBytes("HvpGetKeyVaultHmacSha", "7C 6B 1B 78 7C 9E 23 78 3B EB 00 18 7C BD 2B 78");
	nameFunctionByXref("HvpIsKeyVaultSignatureValid", "HvpGetKeyVaultHmacSha", 1);
	nameFunctionFindBytes("XeCryptAesEncrypt", "38 86 04 00 38 E6 08 00 39 06 0C 00 38 63 00 10");
	nameFunctionFindBytes("XeCryptAesDecrypt", "38 86 04 00 38 E6 08 00 39 06 0C 00 38 63 FF F0");
	nameFunctionByXref("XeCryptAesCtr", "XeCryptAesEncrypt", 3);
	nameFunctionByXref("XeCryptAesDmMac", "XeCryptAesEncrypt", 4);
	nameFunctionFindBytes("XeCryptAesKeyTable", "39 65 01 00 39 85 05 00 39 C5 09 00 39 E5 0D 00 38 85 11 10");

	nameBinaryFunction(0, "7E BF FA A6 56 B5 07 7E  28 35 00 00 40 82 00 1C", "HvStartup");
	nameBinaryFunction(0, "7D A8 02 A6", "HvInit");
	nameFunctionFindBytes("HvInitCryptAlloc", "7C 6B 1B 78 3D 42 00 01 3D 6B 00 01 38 6A 68 00");
	nameFunctionFindBytes("HvBuildKeys", "89 68 00 00 39 29 FF FF 54 EA 06 3E 39 08 00 01");
	nameFunctionByXref("HvInitUserMemory", "HvBuildKeys", 1);
	nameFunctionFindBytes("HvpKeysVerifyKeyvault", "39 60 07 12"); // li r11, 0x712
	nameFunctionFindBytes("HvpKvApplyRestrictedPrivs", "55 69 07 FE 2B 09 00 00 41 9A 00 0C 38 60 00 00");
	nameFunctionFindBytes("HvpClearDiskAuthInfo", "91 4B 00 00 91 4B 00 04 91 4B 00 08 91 4B 00 0C");
	
	
// added these from stoker
	nameFunctionFindBytes("HvpDecryptConsoleRevocationXex2Key", "7C 7F 1B 78 3B AB FE B0 38 80 00 54");
	nameFunctionFindBytes("HvpDecryptConsoleRevocationCpuKey", "7C 7F 1B 78 3B AB FE B0 38 80 00 20");
	nameFunctionFindBytes("HvpEncryptConsoleRevocationCpuKey", "7C 7F 1B 78 7C 9C 23 78 3B DF 01 20 38 80 00 10");
	nameFunctionFindBytes("HvpEncryptKeyVault", "7C 7F 1B 78 39 40 07 12 3B DF 00 10 38 80 00 08");
	nameFunctionFindBytes("HvpReadFlashKeyVault", "78 8B 00 20 7C BF 2B 78 39 4B 00 0F 79 4A 06 E4");
	nameFunctionFindBytes("HvpSecurityDecryptSettings", "38 81 00 60 3B A0 00 10 7C 7F 1B 78 39 40 00 00");
	nameFunctionFindBytes("HvpSecurityEncryptSettings", "3D 62 00 01 7C 7F 1B 78 38 80 00 08 3B DF 00 10");
	nameFunctionFindBytes("HvpHashingCountermeasures", "39 6B FF FF 79 6B 17 80 79 6B 07 64 7D 6B D8 2E");
	
	namePostOutXrefs();


	// add from 16529
// study this more: 00000080 pHvTable:.long HvTbl

}

static setupGeneric()
{
	auto currAddr, str;

	// name some generic functions
	nameBinaryFunction(0x200, "7C 7E 4A A6", "MachineCheck");
	nameBinaryFunctionComment(0, "38 60 02 00 64 63 80 00 78 63 07 C6 64 63 C8 00 4E 80 00 20", "HvpGetFlashBaseAddress", "r3 = 0x80000200C8000000");
	nameBinaryFunctionComment(0, "38 80 02 00 64 84 80 00 78 84 07 C6 7C 63 22 14 4E 80 00 20", "HvpGetSocMMIORegs", "r3 = 0x8000020000000000 + r3");
	nameBinaryFunctionComment(0, "38 80 02 00 64 84 80 00 78 84 07 C6 64 84 D0 00 7C 63 22 14 4E 80 00 20", "HvpBuildPciConfigRegs", "r3 = 0x80000200D0000000 + r3");
	nameBinaryFunctionComment(0, "38 80 02 00 64 84 80 00 78 84 07 C6 64 84 E0 00 7C 63 22 14 4E 80 00 20", "HvpGetHostBridgeRegs", "r3 = 80000200E0000000 + r3");
	nameBinaryFunction(0, "28 24 00 00 7C 89 03 A6 4D C2 00 20 7C 20 1F EC 38 63 00 80 43 20 FF F8 4E 80 00 20", "HvpZeroCacheLines");
	nameBinaryFunction(0, "7C 9E 4A A6 78 64 0F AC 4C 00 01 2C 7C 9E 4B A6 4C 00 01 2C 38 60 03 FF 78 63 07 C6 7C 20 1A 24 7C 00 04 AC 4C 00 01 2C 4E 80 00 20", "HvpSetRMCI");
	nameBinaryFunction(0, "7C 78 EB A6 7C 99 EB A6 7C BA EB A6 4E 80 00 20", "HvpLockL2XpsRange");
	nameBinaryFunction(0, "7C 7B EB A6 7C 9C EB A6 7C BD EB A6 4E 80 00 20", "HvpLockL2TitleRange");
	nameBinaryFunction(0, "A1 60 00 06 55 63 DF FE 4E 80 00 20", "HvpIsKeyVaultSignatureRequired");
	nameBinaryFunction(0, "2C 05 00 00 7C A9 03 A6 38 E0 00 00 41 82 00 24 88 C4 00 00 88 A3 00 00 48 00 00 0C 8C C4 00 01 8C A3 00 01 7C C6 2A 78 7C E7 33 78 42 00 FF F0", "XeCryptMemDiff");
	nameBinaryFunction(0, "7C 64 18 50 7C A9 03 A6 80 C4 00 00 80 E4 00 04 78 C7 00 0E 7C E3 21 2A 38 84 00 08 43 20 FF EC 4E 80 00 20", "XeCryptCopyQwVec");
	nameBinaryFunction(0, "78 63 00 20 1C 63 00 32 7C 8C 42 E6 7C AC 42 E6 7C A4 28 50 7C 23 28 40 4C C1 00 20 7F FF FB 78 4B FF FF EC", "HvpStallExecutionProcessor");
	currAddr = nameBinaryFunction(0, "54 63 45 EE 3C 82 00 01 38 84 01 30 7C 63 22 14 4E 80 00 20", "HvpGetUserModeSlb");
	if(currAddr != BADADDR) // this puts slb at 0x10130 for 0x20 entries of 16bytes SLB_CACHE
	{
		makeStructNamed(0x10130, "SLB_CACHE", "HvUserModeSlb");
		MakeArray(0x10130, 0x20);
	}
	currAddr = nameBinaryFunction(0, "54 63 26 F6 3C 82 00 01 38 84 03 30 7C 63 22 14 4E 80 00 20", "HvpGetUserModeTlb");
	if(currAddr != BADADDR)
		makeNamedQword(0x10330, "HvUserModeTlb"); // hardcoded again to the function bytes
	nameBinaryFunction(0, "3D 62 00 01 81 6B 00 00 7F 03 58 40 4D 9A 00 20", "HvpReleaseVerifyStackLock");
	//nameBinaryFunction(0x1000, "3D 62 00 01 81 6B 00 00 7F 03 58 40 4D 9A 00 20", "HvpReleaseVerifyStackLock2");
	nameBinaryFunction(0, "7C 89 03 A6 88 83 00 00 98 83 00 00 7C 00 18 6C 38 63 00 80 43 20 FF F0 7C 00 04 AC 4C 00 01 2C 4E 80 00 20", "HvpLoadStoreCacheLines");
	
	
	nameBinaryFunction(0, "7C 6C 1B 78 38 60 00 00 55 84 27 3E 38 84 FF F8 2B 04 00 01 4D 99 00 20 38 60 00 68 78 63 07 C6 64 63 01 F5 51 83 94 7A 4E 80 00 20", "HvpGetPteOffset");
	nameBinaryFunction(0, "39 63 00 0F 79 6B 06 E4 7F 23 58 40 40 9A 00 30 3D 40 07 FF 39 63 FF 80 61 4A FF 7F 7F 2B 50 40 41 99 00 1C 3D 60 08 00 78 8A 00 20 7D 63 58 50 38 60 00 01 7F 2A 58 40 4D 98 00 20 38 60 00 00 4E 80 00 20", "HvpBldrFlashRange");
	nameBinaryFunction(0, "78 63 00 20 1C 63 00 32 7C 8C 42 E6 7C AC 42 E6 7C A4 28 50 7C 23 28 40 4C C1 00 20 7F FF FB 78 4B FF FF EC", "HvpStallExecutionProcessor");
	nameBinaryFunction(0, "39 60 00 00 78 AB 1F 4C 7D 4B 1A 14 7D 6B 22 14", "XeCryptBnQwNeCompare");
	nameBinaryFunction(0, "39 60 00 00 78 8B 1F 4C 7D 6B 1A 14 48 00 00 18", "XeCryptBnQwNeDigLen");
	nameBinaryFunction(0, "81 60 00 30 55 6B 04 20 2B 0B 00 00 41 9A 00 2C A1 60 00 06", "HvpHasPrivileges");
	nameBinaryFunction(0, "39 60 00 00 2B 04 00 00 41 9A 00 28 89 43 00 00 38 84 FF FF", "HvpPreloadCacheLines");

	
	// static AES and similar stuff
	nameBinary(0, 0x40000, "C6 63 63 A5 F8 7C 7C 84 EE 77 77 99 F6 7B 7B 8D", 0x1000, "g_XeCryptAesE"); 
	nameBinary(0, 0x40000, "51 F4 A7 50 7E 41 65 53 1A 17 A4 C3 3A 27 5E 96", 0x1100, "g_XeCryptAesD");
	currAddr = nameBinary(0, 0x40000, "63 7C 77 7B F2 6B 6F C5 30 01 67 2B FE D7 AB 76", 271, "g_XeCryptAesU_frag");
	if(currAddr != BADADDR)
		nameBinary(currAddr+271, 0x40000, "63 7C 77 7B F2 6B 6F C5 30 01 67 2B FE D7 AB 76", 0x1110, "g_XeCryptAesU");
	nameBinary(0, 0x40000, "14 04 00 05 1A 02 03 0E 2B 05 06 09 30 21 30 00", 0x10, "g_abXeCryptPkcs1ShaEncoding1");
	nameBinary(0, 0x40000, "14 04 1A 02 03 0E 2B 05 06 07 30 1F 30 00 00 00", 0x10, "g_abXeCryptPkcs1ShaEncoding2");
	nameBinary(0, 0x40000, "02 08 08 00 00 08 00 00 02 00 00 02 02 08 08 02", 0x800, "g_XeCryptDesSpbox");
	nameBinary(0, 0x40000, "00 00 00 00 00 00 00 10 20 00 00 00 20 00 00 10", 0x800, "g_XeCryptDesSel");
	nameBinary(0, 0x40000, "00 00 01 01 01 01 01 01 00 01 01 01 01 01 01 00", 0x10, "g_XeCryptDesDoubleShift");
	nameBinary(0, 0x40000, "00 01 01 02 01 02 02 03 01 02 02 03 02 03 03 04", 0x10, "g_XeCryptDesParityTable");
	
	nameBinary(0, 0x40000, "00 00 01 01 01 01 01 01 00 01 01 01 01 01 01 00", 0x10, "g_XeCryptDesDoubleShift");
	nameBinary(0, 0x40000, "00 00 01 01 01 01 01 01 00 01 01 01 01 01 01 00", 0x10, "g_XeCryptDesDoubleShift");
	

	
	// some keys that are built during boot time
	nameBinary(0, 0x40000, "A2 6C 10 F7 1F D9 35 E9 8B 99 92 2C E9 32 15 72", 16, "Xex1Key");
	nameBinary(0, 0x40000, "E4 89 E0 1A AA 36 4B 7A 4D 95 13 A8 53 86 03 77", 16, "Ap251Key");
	nameBinary(0, 0x40000, "79 AD 7F 10 0B C9 E9 09 B7 66 84 4F EB D8 7B 69", 16, "DvdAuth1Key");
	nameBinary(0, 0x40000, "97 49 35 7E 18 BC 99 74 C7 A6 1C 83 69 94 8C 2A", 16, "SystemLink1Key");
	nameBinary(0, 0x40000, "20 B1 85 A5 9D 28 FD C3 40 58 3F BB 08 96 BF 91", 16, "Xex2Key");
	nameBinaryScan(0, 0x40000, "DD 88 AD 0C 9E D6 69 E7 B5 67 94 FB 68 56 3E FA", 16, "BL1Key"); // live dumps have this twice
	
	//nameBinary(0, 0x40000, "", "");

	// some strings that never change...
	nameBinaryStringScan(0, 0x40000, "58 42 4F 58 5F 52 4F 4D 5F 42", 10, "ROMBSalt"); // XBOX_ROM_B (there is more than one !)
	nameBinaryString(0, 0x40000, "58 42 4F 58 5F 42 4C 50 52 56", 10, "BLPRVSalt"); // XBOX_BLPRV
	nameBinaryString(0, 0x40000, "58 42 4F 58 33 36 30 58 45 58", 10, "XEXSalt"); // XBOX360XEX
	nameBinaryString(0, 0x40000, "58 42 4F 58 52 45 56 58 45 58", 10, "REVXEXSalt"); // XBOXREVXEX
	nameBinaryString(0, 0x40000, "44 56 44 2D 58 47 44 32", 8, "XGD2str"); // DVD-XGD2
	nameBinaryString(0, 0x40000, "58 42 4F 58 33 36 30 45 58 50", 10, "EXPSalt"); // XBOX360EXP
	nameBinaryString(0, 0x40000, "47 45 4E 55 49 4E 45 20 58 42 4F 58 20 4F 44 44", 16, "GenericOddStr"); // GENUINE XBOX ODD
	
	nameBinaryString(0, 0x40000, "58 42 4F 58 5F 45 58 5F 30 31", 10, "EX01Salt"); // XBOX_EX_01
	nameBinaryString(0, 0x40000, "50 4C 44 53 20 20 20 20 44 47 2D 31 36 44 34 53 20 20 20 20 20 20 20 20 39 35 30 34", 28, "LITE9504Sig"); // "PLDS    DG-16D4S        9504"


	// name the constant keys
	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 DD 5F 49 6F 99 4D 37 BB", "CONSTANT_MASTER_KEY", 0, 0x40000); // DD5F496F994D37BB
	makeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 F2 E5 3E 3F 03 75 C2 B3", "CONSTANT_LIVE_KEY", 0, 0x40000); // F2E53E3F0375C2B3
	makeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 E6 D7 3D 66 E5 4E D7 D3", "CONSTANT_XB1_GREEN_KEY", 0, 0x40000); // E6D73D66E54ED7D3
	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 F4 30 E3 52 4D 53 1C A1", "CONSTANT_SATA_DISK_SECURITY_KEY", 0, 0x40000); //F430E3524D531CA1
	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 E6 3B 32 B2 8D 9E 9E E7", "CONSTANT_PIRS_KEY", 0, 0x40000); // E63B32B28D9E9EE7
	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 C9 1C 35 77 C8 BF A0 6B", "CONSTANT_PIRS_KEY_DEV", 0, 0x40000); // C91C3577C8BFA06B
	makeKeyScan("00 00 00 18 00 00 00 03 00 00 00 00 00 00 00 00 67 39 40 07 BB 3C A7 09", "CONSTANT_DEVICE_REVOCATION_KEY", 0, 0x40000); // 67394007BB3CA709
	makeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 3D A2 69 7C 4E E3 7C 9F", "CONSTANT_XMACS_KEY", 0, 0x40000); // 3DA2697C4EE37C9F

	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 81 A1 17 26 B3 70 8A D9", "CONSTANT_FACTORY_KEY", 0, 0x40000); // 81A11726B3708AD9
	makeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 47 3E 39 88 84 AF 0B 61", "CONSTANT_SECONDARY_ACTIVATION_KEY", 0, 0x40000); // 473E398884AF0B61


	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 CB B9 93 CD C1 7D 10 B5", "CONSTANT_ALT_FACTORY_KEY", 0, 0x40000); // CBB993CDC17D10B5
	makeKeyScan("00 00 00 40 00 01 00 01 00 00 00 00 00 00 00 00 6D AB 60 5C B3 45 75 D1", "CONSTANT_DEBUG_UNLOCK_KEY", 0, 0x40000); // 6DAB605CB34575D1
	makeKeyScan("00 00 00 20 00 01 00 01 fd 33 6b 67 93 6a a2 11 e9 8d b5 dc af 38 8e f1", "CONSTANT_1BL_RSA_KEY", 0, 0x40000); // e98db5dcaf388ef1
	makeKeyScan("00 00 00 20 00 01 00 01 00 00 00 00 00 00 00 00 e9 8d b5 dc af 38 8e f1", "CONSTANT_1BL_RSA_KEY_FORM2", 0, 0x40000); // e98db5dcaf388ef1
	
	// some new stuff I've not puzzled over what they are for yet
	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 7E 17 D7 46 E2 47 38 DB", "CONSTANT_UNK_NEW_KEY1", 0, 0x40000); // 7E17D746E24738DB
	makeKeyScan("00 00 00 10 00 01 00 01 00 00 00 00 00 00 00 00 04 D0 55 50 79 19 95 27", "CONSTANT_UNK_NEW_KEY2", 0, 0x40000); // 04D0555079199527
	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 E1 82 06 A6 73 EF E2 71", "CONSTANT_DATACENTER_RSA_KEY", 0, 0x40000); // E18206A673EFE271
	makeKeyScan("00 00 00 20 00 00 00 03 00 00 00 00 00 00 00 00 32 B4 EE 99 6A 4A B0 3F", "CONSTANT_UNK_KE_KEY", 0, 0x40000); // 32B4EE996A4AB03F
	
	nameBinary(0, 0x40000, "E5 1C F0 B7 52 72 2C BA F0 BA DE CB 07 6E 9D E5", 16, "UNK_KEY_BYTES1"); // 
	nameBinary(0, 0x40000, "DA 39 A3 EE 5E 6B 4B 0D 32 55 BF EF 95 60 18 90 AF D8 07 09", 0x14, "UNK_KEY_BYTES2");

	//makeKey("", "", 0, 0x40000);
	makeStructNamed(0xFFA0, "CPU_FUSES", "vFuses");
	
}

// AddStrucMember(id,"", -1, FF_BYTE|FF_DATA, -1, 1);
// AddStrucMember(id,"", -1, FF_WORD|FF_DATA, -1, 2);
// AddStrucMember(id,"", -1, FF_DWRD|FF_DATA, -1, 4);
// AddStrucMember(id,"", -1, FF_QWRD|FF_DATA, -1, 8);
static addStructs()
{
	auto id;
	Til2Idb(-1, "HVKEYVAULT");
	Til2Idb(-1, "HVKEYPROPS");

	Til2Idb(-1, "XECRYPT_RSAPUB_1024");
    Til2Idb(-1, "XECRYPT_RSAPUB_1536");
    Til2Idb(-1, "XECRYPT_RSAPUB_2048");
    Til2Idb(-1, "XECRYPT_RSAPUB_4096");
	if (GetStrucIdByName("XECRYPT_RSAPUB_1024") == -1)
	{
		Message("Failed loading \n");
		id = AddStrucEx(-1,"XECRYPT_RSAPUB_1024",0); // -1 next highest, name, 0=struct 1=union; returns id
		AddStrucMember(id,"cqw", -1, FF_DWRD|FF_DATA, -1, 4);
		AddStrucMember(id,"dwPubExp", -1, FF_DWRD|FF_DATA, -1, 4);
		AddStrucMember(id,"qwReserved", -1, FF_QWRD|FF_DATA, -1, 8);
		AddStrucMember(id,"aqwM", -1, FF_QWRD|FF_DATA, -1, 8*0x10);
	}
	if (GetStrucIdByName("XECRYPT_RSAPUB_1536") == -1)
	{
		Message("Failed loading XECRYPT_RSAPUB_1536\n");
		id = AddStrucEx(-1,"XECRYPT_RSAPUB_1536",0); // -1 next highest, name, 0=struct 1=union; returns id
		AddStrucMember(id,"cqw", -1, FF_DWRD|FF_DATA, -1, 4);
		AddStrucMember(id,"dwPubExp", -1, FF_DWRD|FF_DATA, -1, 4);
		AddStrucMember(id,"qwReserved", -1, FF_QWRD|FF_DATA, -1, 8);
		AddStrucMember(id,"aqwM", -1, FF_QWRD|FF_DATA, -1, 8*0x18);
	}
	if (GetStrucIdByName("XECRYPT_RSAPUB_2048") == -1)
	{
		Message("Failed loading XECRYPT_RSAPUB_2048\n");
		id = AddStrucEx(-1,"XECRYPT_RSAPUB_2048",0); // -1 next highest, name, 0=struct 1=union; returns id
		AddStrucMember(id,"cqw", -1, FF_DWRD|FF_DATA, -1, 4);
		AddStrucMember(id,"dwPubExp", -1, FF_DWRD|FF_DATA, -1, 4);
		AddStrucMember(id,"qwReserved", -1, FF_QWRD|FF_DATA, -1, 8);
		AddStrucMember(id,"aqwM", -1, FF_QWRD|FF_DATA, -1, 8*0x20);
	}
	// id = AddStrucEx(-1,"BLDR",0);
	// AddStrucMember(id,"Magic", -1, FF_WORD|FF_DATA, -1, 2);
	// AddStrucMember(id,"Build", -1, FF_WORD|FF_DATA, -1, 2);
	// AddStrucMember(id,"Qfe", -1, FF_WORD|FF_DATA, -1, 2);
	// AddStrucMember(id,"Flags", -1, FF_WORD|FF_DATA, -1, 2);
	// AddStrucMember(id,"Entry", -1, FF_DWRD|FF_DATA, -1, 4);
	// AddStrucMember(id,"Size", -1, FF_DWRD|FF_DATA, -1, 4);
}

static nameBasics()
{
	makeNamedWord(0x0, "Magic");
	makeNamedWord(0x2, "Build");
	makeNamedWord(0x4, "Qfe");
	makeNamedWord(0x6, "Flags");
	makeNamedDword(0x8, "Entry");
	makeNamedDword(0xC, "Size");
	// MakeStructEx(0, -1, "BLDR");
	// MakeName(0, "HvHdr");

	// no idea how long these will remain accurate...
	makeNamedDword(0x10, "BaseKernel");
	makeNamedDword(0x14, "UpdateType");
	makeNamedDword(0x38, "pKeyVault");
	
	makeNamedDword(0x44, "pPteTable");
	MakeByte(Dword(0x44));
	makeNamedDword(Dword(0x44), "PteTable");
	
	makeNamedDword(0x48, "pVersionMirror");
	MakeDword(Dword(0x48));
	makeNamedDword(Dword(0x48), "VersionMirror");
	
	makeNamedDword(0x50, "prKernelHvExportTable");
	makeNamedByte(0x74, "ConsoleType");
	makeNamedByte(0x75, "UpdateSequence");
	makeNamedWord(0x76, "UpdateSequenceAllow");
	makeNamedDword(0x78, "prXboxKrnlBaseVersion");
	makeNamedDword(0x7C, "prHvBaseVersion");
	makeNamedDword(0x80, "pHvTable");
	
	// 0x38 of HVKEYPROPS
	makeNamedDword(0x3C, "pKeyPropertiesTbl"); // aray dword 0x0 terminated
	makeStructNamed(Dword(0x3c), "HVKEYPROPS", "KeyPropertiesTbl");
	MakeArray(Dword(0x3c), 0x38);
	
	makeNamedDword(0x40, "pKeyvaultLookupTbl"); // array short 0x0 terminated
	makeNamedWord(Dword(0x40), "KeyvaultLookupTbl");
	MakeArray(Dword(0x40), 57);

	makeNamedDword(0x30, "KeysStatus");
	makeNamedDword(0x34, "pPirsKey");
	makeNamedDword(0x10000, "g_dwHvpStackLock");

	
	// cpu key is stowed currently at 0x20
	makeNamedByteArray(0x20, 0x10, "CpuKey");
	// xex2 is stowed currently at 0x54
	makeNamedByteArray(0x54, 0x10, "Xex2Key");
	// transform magic at 0x18
	makeNamedByteArray(0x18, 8, "TransformMagic");
	
	makeNamedByteArray(0x10010, 0x10, "HvDiskMediaId");
		MakeRptCmt(0x10010, "media id comes from ss and dmi, is echoed in other memory regions");
	makeNamedByte(0x1000C, "HvDiskAuthState"); // byte stb       r11, unk_1000C@l(r9) disk auth byte
	makeNamedByteArray(0x10020, 0x10, "HvDiskRandomAesKey");
	makeNamedByteArray(0x10030, 0x10, "HvDiskRandomAesFeed");
	makeNamedByteArray(0x10040, 0x80, "HvPubKeyCryptedData");
	// may just be a cache of kv hmacsha
//	makeNamedByteArray(0x10050, 0x14, "HvKeyVaultHmacSha");
	makeNamedByteArray(0x100C0, 0x40, "HvMmioShaObfuscation");
	// makeNamedByteArray(0x100C0, 0x10, "MMIO_SHA");
	// makeNamedByteArray(0x100D0, 0x10, "PCI_SHA");
	// makeNamedByteArray(0x100E0, 0x10, "HRMORMSR_SHA");
	// makeNamedByteArray(0x100F0, 0x10, "CPUECC_SHA");
	makeNamedByteArray(0x10100, 0x30, "HvRandomData");
	
}

static FinalPass()
{
	auto offset;
	offset = MAXADDR;
	while(offset != BADADDR)
	{
		//Message(form("seeking at %08x from 0x%x\n", curroff,offset));
		offset = FindBinary(offset, SEARCH_UP, "7D 88 02 A6"); // mflr r12
		if(offset != BADADDR)
		{
			Message(form("creating function at 0x%x\n", offset));
			MakeFunction(offset, BADADDR);
//			offset = offset+12;	
		}
	}
	setupFinalFunctions();
}

static main()
{
	SetPrcsr("PPC");
	SetCharPrm(INF_COMPILER, COMP_MS);
	SetCharPrm(INF_MODEL, 0x53); // should be calling conv cdecl, memory model "code near,data near" - use GetCharPrm(INF_MODEL) with right settings to find out val if this is wrong
	SetShortPrm(INF_AF2, ~AF2_FTAIL&GetShortPrm(INF_AF2)); // turns off creating function chunk tails
    if (!LoadTil("xbox360.til"))
    {
        Message("Failed loading xbox360.til\n");
        return 1;
    }
#ifdef __EA64__
	segRelocateHvBasePages();
#endif
	addStructs();
	nameBasics();
	SetupSyscallTable(); // _SyscallTable 
	SetupExp2Table(); // HvExp2
	nameBuilders();
	setupGeneric();
	SetupPointerBranches(); // HvTbl
	nameVectors(0, "");
	SetupRegSaves();
#ifdef __EA64__
	segHvSetupSegPointers();
#endif
	FinalPass();
	AutoMark2(0, MAXADDR, AU_USED); // reanalyze program

	SetShortPrm(INF_AF2, ~AF2_FTAIL&GetShortPrm(INF_AF2)); // turns on creating function chunk tails
	Message("done!\n\n");
}
