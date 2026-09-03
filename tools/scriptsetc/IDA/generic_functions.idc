#include <idc.idc>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// borrowed from xorloser's IDC
static SetupSection(startAddr, endAddr, segClass, perms, name, base)
{
    SetSelector(base, 0);
#ifdef __EA64__
    AddSegEx(startAddr, endAddr, base, 2, 3, 2, ADDSEG_NOSREG); // 64bit, saRelPage, scPub, ADDSEG_NOSREG
#else
    AddSegEx(startAddr, endAddr, base, 1, 3, 2, ADDSEG_NOSREG); // 32bit, saRelPage, scPub, ADDSEG_NOSREG
#endif
    SegClass(startAddr, segClass);
    SegRename(startAddr, name);
    SetSegmentAttr(startAddr, SEGATTR_PERM, perms); // 4=read, 2=write, 1=execute
    SetSegmentAttr(startAddr, SEGATTR_FLAGS, 0x10); // SFL_LOADER
    SegDefReg(startAddr, "%r26", 0);
    SegDefReg(startAddr, "%r27", 0);
    SegDefReg(startAddr, "%r28", 0);
    SegDefReg(startAddr, "%r29", 0);
    SegDefReg(startAddr, "%r30", 0);
    SegDefReg(startAddr, "%r31", 0);
}

// simple way to mask out the negative bit
static getAddrMask()
{
#ifdef __EA64__
	return 0x7FFFFFFFFFFFFFFF;
#else
	return 0x7FFFFFFF;
#endif
}

// import a struct from TIL and name it at a given address (currently no failbacks!!)
static makeStructNamed(addr, sname, name)
{
	auto id, size, ret;
	Til2Idb(-1, sname);
	id = GetStrucIdByName(sname);
	if(id != -1)
	{
		size = GetStrucSize(id);
		MakeUnknown(addr, size, 2);
		ret = MakeStructEx(addr, -1, sname);
		MakeName(addr, name);
		MakeComm(addr, sname); // MakeComm MakeRptCmt
	}
	else
		Message(form("****** Error: makeStructNamed, no struct named '%s' is available! (off: 0x%x)\n", sname, addr));
	return ret;
}

// make a byte array of a specific length and name it at a given address
static makeNamedByteArray(addr, len, name)
{
	MakeUnknown(addr, len, 2);
	MakeByte(addr);
	MakeArray(addr, len);
	MakeName(addr, name);
}

// make a string of a specific length and name it at a given address
static makeNamedString(addr, len, name)
{
	MakeUnknown(addr, len, 2);
	MakeStr(addr, addr+len);
	MakeName(addr, name);
}

// make a byte and name it at a given address
static makeNamedByte(offset, name)
{
	MakeUnknown(offset, 1, 2);
	MakeByte(offset);
	MakeName(offset, name);
}

// make a word and name it at a given address
static makeNamedWord(offset, name)
{
	MakeUnknown(offset, 2, 2);
	MakeWord(offset);
	MakeName(offset, name);
}

// make a dword and name it at a given address
static makeNamedDword(offset, name)
{
	MakeUnknown(offset, 4, 2);
	MakeDword(offset);
	MakeName(offset, name);
}

// make a qword and name it at a given address
static makeNamedQword(offset, name)
{
	MakeUnknown(offset, 8, 2);
	MakeQword(offset);
	MakeName(offset, name);
}

// name a function at a given address
static makeNamedFunction(address, name)
{
	MakeName(address, name);
	MakeFunction(address, BADADDR);
}

// quick way to remove all function tails
static RemoveAllChunks(address)
{
	auto a, b;
	a = NextFuncFchunk(address, address);
	b=0;
	while(a != BADADDR)
	{
		RemoveFchunk(address, a);
		a = NextFuncFchunk(address, address);
		b = b +1;
	}
	Message(form("function at 0x%08X, removed %d chunks\n",address,b));
}

// given binary bytes this will attempt to scan all data in idb for the first occurance and convert it to a string
static nameBinaryString(startAddr, endAddr, bytes, len, name)
{
	auto offset, end;
	if(endAddr == 0)
		end = MAXADDR;
	else
		end = endAddr;
	offset = FindBinary(startAddr, SEARCH_DOWN, bytes);
	if(offset != BADADDR)
	{
		if(offset <= (end-len))
		{
		MakeStr(offset, offset+len);
		MakeName(offset, name);
		Message(form("Named binary string %s at offset: 0x%08x\n", name, offset));
		offset = offset + len;
		}
		// else
			// Message(form("Found binaryString data %s outside of range 0x%x-0x%x\n", name, startAddr, end));	
	}
	else
		Message(form("Did not find binaryString data %s\n", name));	
	return offset;
}

// given binary bytes this will attempt to scan all data in idb for occurences, then convert them to strings
static nameBinaryStringScan(startAddr, endAddr, bytes, len, name)
{
	auto offset, rname, cnt, mask;
	mask = getAddrMask();
	offset = startAddr;
	rname = form("%s", name);
	cnt = 0;
	while(offset != BADADDR)
	{
		if((offset&mask) < (endAddr-len))
		{
			if(cnt != 0)
				rname = form("%s_%d", name, cnt);
			offset = nameBinaryString(offset, endAddr, bytes, len, rname);
			cnt++;
		}
		else return offset;
	}
	return offset;
}

// given binary bytes this will attempt to scan between start and end addresses in idb for occurences, then convert them to binary named arrays
static nameBinary(startAddr, endAddr, bytes, len, name)
{
	auto offset, end, mask;
	mask = getAddrMask();
	if(endAddr == 0)
		end = MAXADDR;
	else
		end = endAddr;
	offset = FindBinary(startAddr, SEARCH_DOWN, bytes);
	if((offset != BADADDR)&&((offset&mask) <= (end-len)))
	{
		// if((offset&mask) <= (end-len))
		// {
			MakeByte(offset);
			MakeArray(offset, len);
			MakeName(offset, name);
			Message(form("Named binary array %s at offset: 0x%08x\n", name, offset));
			offset = offset + len;
		// }
		// else
			// Message(form("Found binary data %s outside of range 0x%x-0x%x\n", name, startAddr, end));	
	}
	else
	{
		Message(form("Did not find binary data %s\n", name));
		offset = BADADDR;
	}
	return offset;
}

// given binary bytes this will attempt to scan all data in idb for occurences, then convert them to strings
static nameBinaryScan(startAddr, endAddr, bytes, len, name)
{
	auto offset, rname, cnt;
	offset = startAddr;
	rname = form("%s", name);
	cnt = 0;
	while((offset != BADADDR)&&(offset < (endAddr-len)))
	{
		if(cnt != 0)
			rname = form("%s_%d", name, cnt);
		offset = nameBinary(offset, endAddr, bytes, len, rname);
		cnt++;
	}
	return offset;
}

static nameVectors(base, nameadd)
{
	auto name;
	name = form("%s%s", "_v_THERMAL_MANAGEMENT", nameadd);
	makeNamedFunction(base+0x00001800, name);
	
	name = form("%s%s", "_v_VMX_ASSIST", nameadd);
	makeNamedFunction(base+0x00001700, name);
	name = form("%s%s", "_v_MAINTENANCE", nameadd);
	makeNamedFunction(base+0x00001600, name);
	name = form("%s%s", "_v_VPU_UNAVAILABLE", nameadd);
	makeNamedFunction(base+0x00000F20, name);
	name = form("%s%s", "_v_FPU_Assist", nameadd);
	makeNamedFunction(base+0x00000E00, name);
	name = form("%s%s", "_v_TRACE", nameadd);
	makeNamedFunction(base+0x00000D00, name);
	name = form("%s%s", "_v_SYSTEM_CALL", nameadd);
	makeNamedFunction(base+0x00000C00, name);
	name = form("%s%s", "_v_Reserved_A00", nameadd);
	makeNamedFunction(base+0x00000A00, name);
	name = form("%s%s", "_v_HYPERVISOR_DECREMENTER", nameadd);
	makeNamedFunction(base+0x00000980, name);
	name = form("%s%s", "_v_DECREMENTER", nameadd);
	makeNamedFunction(base+0x00000900, name);
	name = form("%s%s", "_v_FPU_UNAVAILABLE", nameadd);
	makeNamedFunction(base+0x00000800, name);
	name = form("%s%s", "_v_PROGRAM", nameadd);
	makeNamedFunction(base+0x00000700, name);
	name = form("%s%s", "_v_ALIGNMENT", nameadd);
	makeNamedFunction(base+0x00000600, name);
	name = form("%s%s", "_v_EXTERNAL", nameadd);
	makeNamedFunction(base+0x00000500, name);
	name = form("%s%s", "_v_INSTRUCTION_SEGMENT", nameadd);
	makeNamedFunction(base+0x00000480, name);
	name = form("%s%s", "_v_INSTRUCTION_STORAGE", nameadd);
	makeNamedFunction(base+0x00000400, name);
	name = form("%s%s", "_v_DATA_SEGMENT", nameadd);
	makeNamedFunction(base+0x00000380, name);
	name = form("%s%s", "_v_DATA_STORAGE", nameadd);
	makeNamedFunction(base+0x00000300, name);
	name = form("%s%s", "_v_MACHINE_CHECK", nameadd);
	makeNamedFunction(base+0x00000200, name);
	name = form("%s%s", "_v_RESET", nameadd);
	makeNamedFunction(base+0x00000100, name);
}

static SetupRegSaves()
{
	auto currAddr, i, j;
	
	// find all saves of gp regs
	for(currAddr=0; currAddr != BADADDR; currAddr=currAddr+4)
	{
		// find "std %r14, -0x98(%sp)" followed by "std %r15, -0x90(%sp)"
		currAddr = FindBinary(currAddr, SEARCH_DOWN, "F9 C1 FF 68 F9 E1 FF 70");
		if(currAddr == BADADDR)
			break;
		for(i=14; i<=31; i++)
		{
			MakeUnknown(currAddr, 8, 0); // DOUNK_SIMPLE
			MakeCode(currAddr);
			if(i != 31)
				MakeFunction(currAddr, currAddr + 4);
			else
				MakeFunction(currAddr, currAddr + 0x0C);
			if(MakeNameEx(currAddr, form("__Save_R12_%d_thru_31", i), SN_NOCHECK|SN_NOWARN|SN_NOLIST) != 1)
			{
				j = 0;
				while(MakeNameEx(currAddr, form("__Save_R12_%d_thru_31_%d", i, j), SN_NOCHECK|SN_NOWARN|SN_NOLIST) != 1)
				{
					j = j + 1;
				}
			}
			currAddr = currAddr + 4;
		}
	}
	
	// find all loads of gp regs
	for(currAddr=0; currAddr != BADADDR; currAddr=currAddr+4)
	{
		// find "ld  %r14, var_98(%sp)" followed by "ld  %r15, var_90(%sp)"
		currAddr = FindBinary(currAddr, SEARCH_DOWN, "E9 C1 FF 68 E9 E1 FF 70");
		if(currAddr == BADADDR)
			break;
		for(i=14; i<=31; i++)
		{
			MakeUnknown(currAddr, 8, 0); // DOUNK_SIMPLE
			MakeCode(currAddr);
			if(i != 31)
				MakeFunction(currAddr, currAddr + 4);
			else
				MakeFunction(currAddr, currAddr + 0x10);
			if(MakeNameEx(currAddr, form("__Rest_R12_lr_%d_thru_31", i), SN_NOCHECK|SN_NOWARN|SN_NOLIST) != 1)
			{
				j = 0;
				while(MakeNameEx(currAddr, form("__Rest_R12_lr_%d_thru_31_%d", i, j), SN_NOCHECK|SN_NOWARN|SN_NOLIST) != 1)
				{
					j = j + 1;
				}
			}
			currAddr = currAddr + 4;
		}
	}
}
