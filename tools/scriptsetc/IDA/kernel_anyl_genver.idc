#include <idc.idc>
#include <x360_imports.idc>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// borrowed from xorloser's IDC
static SetupSection(startAddr, endAddr, segClass, perms, name, base)
{
    SetSelector(base, 0);
    SegCreate(startAddr, endAddr, base, 1, 3, 2);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static ByteSwap16(Value)
{
	auto fval;
	fval = (((Value>>8)&0xff) | ((Value<<8)&0xff00));
	return fval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static ByteSwap32(Value)
{
	auto fval;
	fval = (((Value>>24)&0xff) | ((Value>>8)&0xff00) | ((Value<<8)&0xff0000) | ((Value<<24)&0xff000000));
	return fval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static PatchSwap16(Address)
{
	auto wVar;
	wVar = Word(Address);
	wVar = ByteSwap16(wVar);
	PatchWord(Address, wVar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static PatchSwap32(Address)
{
	auto dwVar;
	dwVar = Dword(Address);
	dwVar = ByteSwap32(dwVar);
	PatchDword(Address, dwVar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static fixupSwap16(Addr, len)
{
	auto k;
	k = 0;
	while(k<len)
	{
		PatchSwap16(Addr);
		Addr = Addr + 2;
		k = k +1;
	}
	return Addr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static fixupSwap32(Addr, len)
{
	auto k;
	k = 0;
	while(k<len)
	{
		PatchSwap32(Addr);
		Addr = Addr + 4;
		k = k +1;
	}
	return Addr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static addSwapped(Addr, name, len)
{
	if(len == 1)
		MakeByte(Addr);
	else if (len == 2)
	{
		PatchSwap16(Addr);
		MakeWord(Addr);
	}
	else if (len == 4)
	{
		PatchSwap32(Addr);
		MakeDword(Addr);
	}
	MakeName(Addr, name);
	Addr = Addr + len;
	return Addr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static SetupHeaderSegment(peoff, loadAddr)
{
	auto addr, segbase;
	Message(form("peoff %08x\n",peoff));
	addr = Dword((peoff+0x2C)+loadAddr); // gets the number of bytes from image base load addr to end of headers
	Message(form("adr %08x\n",addr));
	addr = loadAddr + ByteSwap32(addr);
	Message(form("headers end at %08x\n",addr));
	SetupSection(loadAddr, addr, "DATA", 4, "Headers", 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkDosMzHeader(loadAddr)
{
	auto id, i, j, pebase;
	
	Message(form("Marking Dos Mz header\n"));
	id = GetStrucIdByName("_DOS_EXE_HEADER");
	// fix endianess of the DOS header and make sure nothing is defined in it yet so the struct will apply
	i = loadAddr;
	j = GetStrucSize(id);
	MakeUnknown(i, j+4, 2); // 		MakeUnknown(0x80040000, 0x40, 2); // works on idc command line?
	j = (j-4)/2;
	fixupSwap16(i,j);

	j = GetStrucSize(id);
	i = loadAddr+j-4;
	// PE header is the only 32 bit member
	PatchSwap32(i);
	// get the offset of PE header and use it to turn the bytes between the offset and PE header into an array, saves some space
	pebase = Dword(i);
	Message(form("pebase %08x\n",pebase));
	SetupHeaderSegment(pebase, loadAddr);
	i=i+4;
	j = (pebase+loadAddr)-i;
	MakeUnknown(i, j, 2);
	MakeByte(i);
	MakeArray(i,j);
	MakeName(i, "DOS_EXE_DATA");
	
	// apply the struct
	j = MakeStructEx(loadAddr,-1, "_DOS_EXE_HEADER");
	if(j != 1)
		Message(form("** error: MakeStructEx _DOS_EXE_HEADER failed\n"));
	else
	{
		Message(form("Marking Dos Mz header Completed\n"));
		MakeName(loadAddr,"DOS_EXE_HEADER");
	}
	return (pebase+loadAddr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static AddDataDir(Addr, name, com, loadAddr)
{
	auto size, id, rva, len;
	id = GetStrucIdByName("_NT_IMAGE_DATA_DIRECTORY");
	size = GetStrucSize(id);
	rva = Dword(Addr);
	len = Dword(Addr+4);
	MakeStructEx(Addr,-1, "_NT_IMAGE_DATA_DIRECTORY");
	MakeName(Addr, name);
	if(rva != 0)
	{
		rva = rva+loadAddr;
		MakeComm(Addr, form("%s - 0x%08x : 0x%x", com, rva, len));
	}
	else
		MakeComm(Addr, com);
	Addr = Addr + size;
	return Addr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkPEHeader(loadAddr, pebase)
{
	auto idPe, idOpt, sz, taddr, optbase;

	idPe = GetStrucIdByName("_NT_PE_FILE_HEADER");
	idOpt = GetStrucIdByName("_NT_IMAGE_OPTIONAL_HEADER");
	sz = GetStrucSize(idPe);
	MakeUnknown(pebase, sz+4, 2);
	optbase = pebase+sz;
	sz = GetStrucSize(idOpt);
	MakeUnknown(optbase, sz+4, 2);
	taddr = pebase;

	// 4 22 444 22 - 2 11 4444 4444 222222 4444 22 444444
	taddr = fixupSwap32(taddr, 1);
	taddr = fixupSwap16(taddr, 2);
	taddr = fixupSwap32(taddr, 3);
	taddr = fixupSwap16(taddr, 3);
	taddr = taddr+2;
	taddr = fixupSwap32(taddr, 8);
	taddr = fixupSwap16(taddr, 6);
	taddr = fixupSwap32(taddr, 4);
	taddr = fixupSwap16(taddr, 2);
	taddr = fixupSwap32(taddr, 7);
	
	Message(form("PE offset: %08x\n", pebase));
	if(MakeStructEx(pebase,-1, "_NT_PE_FILE_HEADER") != 1)
		Message(form("Could not make PE header struct offset: %08x\n", pebase));
	MakeName(pebase, "PE_Header");
	
	Message(form("PE Opt offset: %08x\n", optbase));
	if(MakeStructEx(optbase,-1, "_NT_IMAGE_OPTIONAL_HEADER") != 1)
		Message(form("Could not make PE Optional header struct offset: %08x\n", pebase));
	MakeName(optbase, "PE_Opt_Header");
	
		// data directories
	MakeUnknown(taddr, 128+4, 2); // 16*(4*2) +4
	fixupSwap32(taddr, 32); // 16*2 u32s
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_EXPORT", "Export Directory", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_IMPORT", "Import Directory", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_RESOURCE", "Resource Directory", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_EXCEPTION", "Exception Directory", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_SECURITY", "Security Directory", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_BASERELOC", "Base Relocation Table", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_DEBUG", "Debug Directory", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_ARCHITECTURE", "Architecture Specific Data", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_GLOBALPTR", "RVA of GP", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_TLS", "TLS Directory", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG", "Load Configuration Directory", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT", "Bound Import Directory in headers", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_IAT", "Import Address Table", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT", "Delay Load Import Descriptors", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR", "COM Runtime descriptor", loadAddr);
	taddr = AddDataDir(taddr, "IMAGE_DIRECTORY_UNUSED", "unused", loadAddr);
	return taddr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkEdata(mainbase, segbase) // export directory in little endian
{
	auto taddr, id, sz, i, numf, name, base, foff, temp;
	taddr = segbase;
	taddr = fixupSwap32(taddr, 2);
	taddr = fixupSwap16(taddr, 2);
	taddr = fixupSwap32(taddr, 6);
	taddr = fixupSwap16(taddr, 1);
	id = GetStrucIdByName("_IMAGE_EXPORT_DIRECTORY");
	sz = GetStrucSize(id);
	MakeUnknown(segbase, sz+4, 2);
	if(MakeStructEx(segbase,-1, "_IMAGE_EXPORT_DIRECTORY") != 1)
		Message(form("Could not make _IMAGE_EXPORT_DIRECTORY struct offset: %08x\n", segbase));
	MakeName(segbase, "IMG_Export_Data");
	name = Dword(segbase+12)+mainbase; // ptr to name
	base = Dword(segbase+16); // base ordinal
	numf = Dword(segbase+20); // number of function in table
	foff = Dword(segbase+28)+mainbase; // first function
	fixupSwap32(foff, numf);
	for(i=0; i<numf; i++)
	{
		taddr = foff+(4*i);
		MakeDword(taddr);
		temp = Dword(taddr)+mainbase;
		MakeComm(taddr, form("ord: %d 0x%08x",i+base,temp));
	}
	MakeStr(name, BADADDR);
	MakeName(name, "eModule_Name");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkPdata(mainbase, segbase) // debug directory
{
	auto addr, vaddr, excp, id; // _IMAGE_EXCEPT_DATA
	auto type, len, pro;
	addr = segbase;
	vaddr = Dword(addr);
	id = GetStrucIdByName("_EXC_DATA");
	while(vaddr != 0)
	{
		vaddr = Dword(addr);
		excp = Dword(addr+4);
		if(vaddr != 0)
		{
			MakeUnknown(addr, 12, 2);
			MakeStructEx(addr,-1, "_EXC_DATA");
			type = (excp>>30)&0x3;
			len = (excp>>8)&0x3FFFFF;
			pro = excp&0xFF;
			MakeComm(addr, form("type: 0x%x len: 0x%06x prolog: 0x%x", type, len, pro));
			MakeFunction(vaddr, BADADDR);
			addr = addr+8;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkXedata(mainbase, segbase, codestart, codeend) // Xbox export directory big endian
{
	auto id, sz, baseAddr, count, baseOrd, i, currAddr, stat, str, name, lib;
	lib = form("xboxkrnl");
	id = GetStrucIdByName("_HV_IMAGE_EXPORT_TABLE");
	sz = GetStrucSize(id);
	MakeUnknown(segbase, sz+4, 2);
	MakeStructEx(segbase,-1, "_HV_IMAGE_EXPORT_TABLE");
	baseAddr = Dword(segbase+sz-12)<<16;
	count = Dword(segbase+sz-8);
	baseOrd = Dword(segbase+sz-4);
	currAddr = segbase+sz;
	Message(form("Labeling Functions by Export table\nsegbase: %08x cstart: %08x cend:%08x\n", segbase, codestart, codeend));
	for(i=0; i<count; i++)
	{
		MakeDword(currAddr);
		stat = Dword(currAddr);
		if(stat != 0)
		{
			stat = stat + baseAddr;
			name = GetExportName(lib, baseOrd+i);
			MakeRptCmt(currAddr, form("ord %d 0x%X %s", baseOrd+i, stat, name));
			//Message(form("ord %d 0x%x 0x%X %s\n", baseOrd+i, baseOrd+i, stat, name)); // these are to capture ordinal output in ida
			if((stat < codestart)||(stat > codeend)) // only can be data .... str = GetTrueName (stat);
			{
				MakeDword(stat);
				AddEntryPoint(baseOrd+i, stat, name, 0);
				MakeName(stat, name);
			}
			else
			{ // its a function
				MakeUnkn(stat, 0);
				MakeCode(stat);
				MakeName(stat, name);
				MakeFunction(stat, BADADDR);
				AddEntryPoint(baseOrd+i, stat, name, 1);				
			}
		}
		else
		{
			MakeRptCmt(currAddr, form("ord %d 0x%X Kernel_%03X", baseOrd+i, stat, baseOrd+i));
		}
		currAddr = currAddr+4;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MakeAligns(codestart, codeend)
{
	auto ea, i, nonbyte;
	ea = codestart;
	while((ea != BADADDR) && (ea < codeend))
	{
		ea = FindUnexplored  (ea, SEARCH_DOWN|SEARCH_NEXT);
		nonbyte = 0;
		i = 0;
		while(nonbyte == 0)
		{
			if(Byte(ea+i) != 0x0)
				nonbyte = 1;
			else
				i = i + 1;
		}
		MakeAlign(ea,i,0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkTextFinal(mainbase, codestart, codeend)
{
	auto unk, microcode; // 00 C6 04 00 00 7E 42 4B 00 A0 00 00 00 7E 82 8B = VdpXenosPFPMicrocode
	microcode = FindBinary(codeend, SEARCH_UP, "00 C6 04 00 00 7E 42 4B 00 A0 00 00 00 7E 82 8B");
	if(microcode == BADADDR)
	{
		Message("Vdp microcode not found\n");
		microcode = codeend;
	}
	else
	{
		Message("Vpp microcode found at %08x\n",microcode);
		MakeName(microcode,"VdpXenosPFPMicrocode");
		MakeName(microcode+0x480,"VdpXenosPM4Microcode");
	}
	unk = FindUnexplored(codestart, SEARCH_DOWN|SEARCH_NEXT);
	Message(form("Analyzing unexplored code from 0x%08x to 0x%08x\n", codestart, microcode));
	while (unk < microcode)
	{
		MakeCode(unk);
		unk = FindUnexplored(unk, SEARCH_DOWN|SEARCH_NEXT);
	}
	Message(form("Analyzing unexplored completed!\nMaking align directives in code\n"));
	MakeAligns(codestart, microcode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static AddSectionEnt(Addr, mainbase)
{
	auto size, id, rva, len;
	id = GetStrucIdByName("_NT_IMAGE_SECTION_HEADER");
	size = GetStrucSize(id);
	fixupSwap32((Addr+8), 8);
	MakeUnknown(Addr, size+4, 2);
	MakeStructEx(Addr,-1, "_NT_IMAGE_SECTION_HEADER");
	return Addr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static typeFromChar(segtype) // get ida compatible type when given DOS characteristics
{
	auto temp; // 4=read, 2=write, 1=execute
	// 0x20000000 executeable, 0x40000000 readable, 0x80000000 writeable
	temp = 0;
	if((segtype & 0x20000000) != 0)
		temp = temp|1;
	if((segtype & 0x40000000) != 0)
		temp = temp|4;
	if((segtype & 0x80000000) != 0)
		temp = temp|2;
	return temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkTsections(mainbase, tbase, pebase)
{
	auto numEnt, id, i, addr, off, size, pdata, xedata;
	auto segname, segbase, segsize, segtype, segperm;
	auto textbase, textsize;
	id = GetStrucIdByName("_NT_PE_FILE_HEADER");
	off = GetMemberOffset(id, "NumberOfSections");
	numEnt = Word(pebase+off);
	addr = tbase;
	pdata = 0;
	xedata = 0;
	id = GetStrucIdByName("_NT_IMAGE_SECTION_HEADER");
	size = GetStrucSize(id);
	for(i = 0; i<numEnt; i++)
	{
		AddSectionEnt(addr, mainbase);
		segname = GetString(addr, 8, ASCSTR_TERMCHR);
		MakeName(addr, form("_NT_IMG_SEC_%s",segname));
		segbase = mainbase+Dword(addr+12);
		segsize = Dword(addr+16);
		segtype = Dword(addr+size-4);
		segperm = typeFromChar(segtype);
		Message(form("segment: %s base: 0x%08x size: 0x%08x type: 0x%08x perm: %d\n", segname, segbase, segsize, segtype, segperm));
		if((segtype & 0x00000020) != 0) // 0x20 = code, 0x40 = data, 0x80 = unit data
			SetupSection(segbase, (segbase+segsize), "CODE", segperm, segname, i);
		else
			SetupSection(segbase, (segbase+segsize), "DATA", segperm, segname, i);
		if(strstr(segname, ".edata")==0)
			MarkEdata(mainbase, segbase);
		else if(strstr(segname, ".pdata")==0)
			pdata = segbase;
		else if(strstr(segname, ".xedata")==0)
			xedata = segbase;
		else if(strstr(segname, ".text")==0)
		{
			textbase = segbase;
			textsize = segsize;
		}
	
		addr = addr + size;
	}
	// so that these can be re-ordered
	if((textbase != 0) && (textsize != 0)) // for final analy pass
	{
		if(xedata != 0)
			MarkXedata(mainbase, xedata, textbase, (textbase+textsize));
		else
			Message(form("****WARNING! Xedata segment was not found!!!!****\n"));

		if(pdata != 0)
			MarkPdata(mainbase, pdata);
		else
			Message(form("****WARNING! Pdata segment was not found!!!!****\n"));
		MarkTextFinal(mainbase, textbase, (textbase+textsize));
	}
	else
		Message(form("****WARNING! TEXT segment was not found!!!!****"));
	return textbase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	AddStrucMember(id,"", -1, FF_BYTE|FF_DATA, -1, 1);
//	AddStrucMember(id,"", -1, FF_WORD|FF_DATA, -1, 2);
//	AddStrucMember(id,"", -1, FF_DWRD|FF_DATA, -1, 4);
static MakeStructs()
{
	auto id, id2;
	// DOS MZ struct
	id = AddStrucEx(-1,"_DOS_EXE_HEADER",0); // -1 next highest, name, 0=struct 1=union; returns id
	AddStrucMember(id,"signature", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"bytes_in_last_block", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"blocks_in_file", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"num_relocs", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"header_paragraphs", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"min_extra_paragraphs", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"max_extra_paragraphs", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"stack_segment", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"sp_initial", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"checksum", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"ip_initial", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"cs_initial_rel", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"reloc_table_offset", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"overlay_number", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"reserved", -1, FF_WORD|FF_DATA, -1, 8); // 14b reserved
	AddStrucMember(id,"oemId", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"oemInfo", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"reserved2", -1, FF_WORD|FF_DATA, -1, 20);
	AddStrucMember(id,"PE_Header_Offset", -1, FF_DWRD|FF_DATA, -1, 4);

	// Data Directories
	id2 = AddStrucEx(-1,"_NT_IMAGE_DATA_DIRECTORY",0); // -1 next highest, name, 0=struct 1=union; returns id
	AddStrucMember(id2,"VirtualAddress", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id2,"Size", -1, FF_DWRD|FF_DATA, -1, 4);
	
	// PE header struct
	id = AddStrucEx(-1,"_NT_PE_FILE_HEADER",0); // -1 next highest, name, 0=struct 1=union; returns id
	AddStrucMember(id,"NTSignature", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"Machine", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"NumberOfSections", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"TimeDateStamp", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"PointerToSymbolTable", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"NumberOfSymbols", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfOptionalHeader", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"Characteristics", -1, FF_WORD|FF_DATA, -1, 2);

	// PE extended header struct, as above - if anyone..._IMAGE_OPTIONAL_HEADER
	id = AddStrucEx(-1,"_NT_IMAGE_OPTIONAL_HEADER",0); // -1 next highest, name, 0=struct 1=union; returns id
	AddStrucMember(id,"Magic", -1, FF_WORD|FF_DATA, -1, 2);
 	AddStrucMember(id,"MajorLinkerVersion", -1, FF_BYTE|FF_DATA, -1, 1);
	AddStrucMember(id,"MinorLinkerVersion", -1, FF_BYTE|FF_DATA, -1, 1);
	AddStrucMember(id,"SizeOfCode", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfInitializedData", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfUninitializedData", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"AddressOfEntryPoint", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"BaseOfCode", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"BaseOfData", -1, FF_DWRD|FF_DATA, -1, 4);
    // NT additional fields.
	AddStrucMember(id,"ImageBase", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SectionAlignment", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"FileAlignment", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"MajorOperatingSystemVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"MinorOperatingSystemVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"MajorImageVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"MinorImageVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"MajorSubsystemVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"MinorSubsystemVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"Reserved1", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfImage", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfHeaders", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"CheckSum", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"Subsystem", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"DllCharacteristics", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"SizeOfStackReserve", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfStackCommit", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfHeapReserve", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfHeapCommit", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"LoaderFlags", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"NumberOfRvaAndSizes", -1, FF_DWRD|FF_DATA, -1, 4);
	//AddStrucMember(id,"DataDirectory", -1, FF_STRU|FF_DATA, id2, 8*16);

	// NT image section headers
	id = AddStrucEx(-1,"_NT_IMAGE_SECTION_HEADER",0); // -1 next highest, name, 0=struct 1=union; returns id
 	AddStrucMember(id,"Name", -1, FF_ASCI|FF_DATA, ASCSTR_C, 8);
	AddStrucMember(id,"VirtualSize", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"VirtualAddress", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfRawData", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"PointerToRawData", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"PointerToRelocations", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"PointerToLinenumbers", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"NumberOfRelocations", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"NumberOfLinenumbers", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"Characteristics", -1, FF_DWRD|FF_DATA, -1, 4);
	
	// .edata
	id = AddStrucEx(-1,"_IMAGE_EXPORT_DIRECTORY",0); // -1 next highest, name, 0=struct 1=union; returns id
	AddStrucMember(id,"Characteristics", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"TimeDateStamp", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"MajorVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"MinorVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"Name", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"Base", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"NumberOfFunctions", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"NumberOfNames", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"PAddressOfFunctions", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"PAddressOfNames", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"PAddressOfNameOrdinals", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"unk", -1, FF_DWRD|FF_DATA, -1, 4);

	id = AddStrucEx(-1,"_IMAGE_DEBUG_DIRECTORY",0); // -1 next highest, name, 0=struct 1=union; returns id
	AddStrucMember(id,"Characteristics", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"TimeDateStamp", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"MajorVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"MinorVersion", -1, FF_WORD|FF_DATA, -1, 2);
	AddStrucMember(id,"Type", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"SizeOfData", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"AddressOfRawData", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"PointerToRawData", -1, FF_DWRD|FF_DATA, -1, 4);

	id = AddStrucEx(-1,"_EXC_DATA",0); // -1 next highest, name, 0=struct 1=union; returns id
	//long AddStrucMember(long id,string name,long offset,long flag,long typeid,long nbytes, long target,long tdelta, long reftype);
	AddStrucMember(id,"BeginAddress", -1, FF_0OFF|FF_DWRD|FF_DATA, -1, 4, -1, 0, REF_OFF32);
	AddStrucMember(id,"TypLenProlog", -1, FF_DWRD|FF_DATA, -1, 4);
	
	id = AddStrucEx(-1,"_HV_IMAGE_EXPORT_TABLE",0); // -1 next highest, name, 0=struct 1=union; returns id
	AddStrucMember(id,"Magic", -1, FF_DWRD|FF_DATA, -1, 12);
	AddStrucMember(id,"ModuleNumber", -1, FF_DWRD|FF_DATA, -1, 8);
	AddStrucMember(id,"Version", -1, FF_DWRD|FF_DATA, -1, 12);
	AddStrucMember(id,"ImageBaseAddress", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"Count", -1, FF_DWRD|FF_DATA, -1, 4);
	AddStrucMember(id,"Base", -1, FF_DWRD|FF_DATA, -1, 4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkRegSaves()
{
	auto currAddr, i;
	
	// find all saves of gp regs
	for(currAddr=0; currAddr != BADADDR; currAddr=currAddr+4)
	{
		// find "std %r14, -0x98(%sp)" followed by "std %r15, -0x90(%sp)"
		currAddr = FindBinary(currAddr, SEARCH_DOWN, "F9 C1 FF 68 F9 E1 FF 70");
		if(currAddr == BADADDR)
			break;
		for(i=14; i<=31; i++)
		{
			MakeUnknown(currAddr, 8, 0); // DOUNK_SIMPLE 0 DOUNK_DELNAMES  0x0002
			MakeCode(currAddr);
			if(i != 31)
				MakeFunction(currAddr, currAddr + 4);
			else
				MakeFunction(currAddr, currAddr + 0x0C);
			if(MakeNameEx(currAddr, form("__Save_R12_R%d_thru_R31", i), SN_NOCHECK|SN_NOWARN) != 1)
				MakeNameEx(currAddr, form("__Save_R12_R%d_thru_R31_", i), 0);
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
			if(MakeNameEx(currAddr, form("__Rest_R12_lr_R%d_thru_R31", i), SN_NOCHECK|SN_NOWARN) != 1)
				MakeNameEx(currAddr, form("__Rest_R12_lr_R%d_thru_R31_", i), 0);
			currAddr = currAddr + 4;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static MarkSyscalls(cbase)
{
	auto addr, raddr, tshort, i;
	addr = cbase;
	while(addr != BADADDR)
	{
		addr = FindBinary(addr, SEARCH_DOWN|SEARCH_NEXT, "44 00 00 02 4E 80 00 20"); // sc; blr;
		if(addr != BADADDR)
		{
			raddr = addr-4;
			tshort = Word(addr-2);
			if(MakeNameEx(raddr, form("%s_%x",GetSyscallName(tshort), tshort), SN_NOCHECK|SN_NOWARN) != 1)
			{
				i = 1;
				while(MakeNameEx(raddr, form("%s_%d_%x",GetSyscallName(tshort), i, tshort), SN_NOCHECK|SN_NOWARN) != 1)
					i = i+1;
			}
			Message(form("syscall marked: %s_%x 0x%08x\n",GetSyscallName(tshort), tshort, raddr));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static rebase()
{
	auto addr;
	addr = ByteSwap32(Dword(0x3C));
	addr = ByteSwap32(Dword(addr+0x34)); // gets the base from NTO header
	RebaseProgram(addr, 0);
	Message(form("Rebasing to 0x%08X complete\n", addr));
	return addr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static main()
{
	auto mainbase, pebase, tbase, cbase;
	Message(form("Starting Analysis, please wait...\n"));
	SetPrcsr("PPC"); // sets the processor
	SetCharPrm(INF_COMPILER, COMP_MS); // sets the compiler
	SetCharPrm(INF_MODEL, 0x33); // sets calling conv cdecl, memory model "code near,data near" - used idc cmd line GetCharPerm(INF_MODEL) after setting manually to get this value
	SetShortPrm(INF_AF2, ~AF2_FTAIL&GetShortPrm(INF_AF2)); // turns off creating function chunk tails

	mainbase = rebase();
	MakeStructs();
	pebase = MarkDosMzHeader(mainbase);                                         // Mark up the Dos header
	tbase = MarkPEHeader(mainbase, pebase);
	cbase = MarkTsections(mainbase, tbase, pebase);
	MarkRegSaves();
	MarkSyscalls(cbase);
	Message(form("Analysis completed!\n"));

}
