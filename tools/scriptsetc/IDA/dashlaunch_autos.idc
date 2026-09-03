#include <idc.idc>

// names a function based on bytes found, has no output
static nameFunctionFindBytesQuiet(xbname, name, bytes, corr)
{
	auto ea, start;
	ea = FindBinary(0, SEARCH_DOWN, bytes);
	if(ea != BADADDR)
	{	
		start = GetFunctionAttr(ea,FUNCATTR_START);
		if(start != BADADDR)
		{
			MakeName(start, name);
		}
		ea = FindBinary(ea+4, SEARCH_DOWN, bytes);
		if(ea != BADADDR)
		{
			Message(form("# WARNING second instance found at 0x%x\n", name, ea));	
		}	
	}
	else
		Message(form("Did not find binary function %s\n", name));	
}

// names a function based on bytes found, outputs offset of bytes + corr with xbname
static nameFunctionFindBytes(xbname, name, bytes, corr)
{
	auto ea, start, ret;
	ret = BADADDR;
	ea = FindBinary(0, SEARCH_DOWN, bytes);
	if(ea != BADADDR)
	{	
		start = GetFunctionAttr(ea,FUNCATTR_START);
		if(start != BADADDR)
		{
			MakeName(start, name);
		}
		Message(form("%s @ 0x%X\n", xbname, ea+corr));
		ret = ea+corr;
		ea = FindBinary(ea+4, SEARCH_DOWN, bytes);
		if(ea != BADADDR)
		{
			Message(form("# WARNING second instance found at 0x%x\n", name, ea));	
		}	
	}
	else
		Message(form("Did not find binary function %s\n", name));
	return ret;
}

// shows the start of a function + corr, based on binary search and DWORD inst found at offset from binary search offset
// names the function name
static nameFunctionFindStartVerify(xbname, name, bytes, offset, inst, corr)
{
	auto ea, start;
	ea = FindBinary(0, SEARCH_DOWN, bytes);
	if(ea != BADADDR)
	{	
		//Message(form("%s init at 0x%x dword at offset 0x%x is 0x%x \n", xbname, ea, offset, Dword(ea+offset)));	
		if(Dword(ea+offset) == inst)
		{
			start = GetFunctionAttr(ea,FUNCATTR_START);
			if(start != BADADDR)
			{
				MakeName(start, name);
			}
			Message(form("%s @ 0x%X\n", xbname, start+corr));	
		}
		else
			Message(form("# WARNING did not find %s\n", xbname));	
		
		ea = FindBinary(ea+4, SEARCH_DOWN, bytes);
		if(ea != BADADDR)
		{
			if(Dword(ea+offset) == inst)
				Message(form("# WARNING second instance found at 0x%x\n", xbname, ea));	
		}	
	}
	else
		Message(form("Did not find binary function %s\n", xbname));	
}

static nameFunction(xbname, name, bytes)
{
	auto ea, start;
	ea = FindBinary(0, SEARCH_DOWN, bytes);
	if(ea != BADADDR)
	{	
		start = GetFunctionAttr(ea,FUNCATTR_START);
		if(start != BADADDR)
		{
			Message(form("%s @ 0x%X\n", xbname, start));	
			MakeName(start, name);
		}
		else
		{
			Message(form("# WARNING FUNCTION START NOT FOUND!!\n"));	
		}
		ea = FindBinary(ea+4, SEARCH_DOWN, bytes);
		if(ea != BADADDR)
		{
			Message(form("# WARNING second instance found at 0x%x\n", name, ea));	
		}	
	}
	else
		Message(form("# Did not find binary function %s\n", name));	
}

static showNamedFunction(name)
{
	auto addr;
	addr = LocByName(name);
	if(addr != BADADDR)
	{	
		Message(form(".set %s, 0x%x\n", name, addr));
	}
	else
		Message(form("# Did not find binary function %s\n", name));	
}

static checkShowNamedFunction(name, offset, inst)
{
	auto ea;
	ea = LocByName(name);
	if(ea != BADADDR)
	{	
		if(Dword(ea+offset) == inst)
			Message(form(".set %sPatch, 0x%x\n", name, ea+offset));
		else
			Message(form("# expected 0x%x at offset 0x%x in function %s\n", inst, offset, name));	
	}
	else
		Message(form("# Did not find binary function %s\n", name));	
}

static showGenericPatchOffset(name, bytes, offset, inst, corr)
{
	auto ea, start, ret;
	ret = BADADDR;
	ea = FindBinary(0, SEARCH_DOWN, bytes);
	if(ea != BADADDR)
	{	
		if(Dword(ea+offset) == inst)
		{
			Message(form("%s @ 0x%X\n", name, ea+offset+corr));
			ret = ea+offset+corr;
		}
		else
			Message(form("# expected 0x%x at offset 0x%x corr 0x%x in function %s\n", inst, offset, corr, name));	

		ea = FindBinary(ea+4, SEARCH_DOWN, bytes);
		if(ea != BADADDR)
		{
			if(Dword(ea+offset) == inst)
				Message(form("# WARNING second instance found at 0x%x\n", ea));	
		}
		
	}
	else
		Message(form("Did not find binary function %s\n", name));
	return ret;
}

static findNextInst(start, xbname, bytes)
{
	auto ea;
	if(start != BADADDR)
	{
		ea = FindBinary(start, SEARCH_DOWN, bytes);
		if(ea != BADADDR)
		{
			Message(form("%s @ 0x%X\n", xbname, ea));
		}
		else
			Message(form("# WARNING search for %s bytes failed!\n", xbname));
	}
	else
		Message(form("# WARNING cound not search for %s bytes, BADADDR supplied\n", xbname));
}

static showXtweakXrefs()
{
	auto ea, ref1, ref2, ref3, start;
	ea = LocByName("XamGetXTweakManager");
	if(ea == BADADDR)
		ea = LocByName("xam_DummiedFunction");
	if(ea != BADADDR)
	{
		//Message(form("# INFO XamGetXTweakManager @ 0x%X\n", ea));
		ref1 = RfirstB(ea);
		ref2 = RnextB(ea, ref1);   
		ref3 = RnextB(ea, ref2);
		Message(form("XamWaitForNSAL @ 0x%X\n", ref1));
		Message(form("XamRequestToken @ 0x%X\n", ref3));
		Message(form("LookupAppliesTo @ 0x%X\n", ref2));
		if(ref2 != BADADDR)
		{
			start = GetFunctionAttr(ref2,FUNCATTR_START);
			if(start != BADADDR)
			{
				MakeName(start, "?LookupAppliesTo@TokenRequest@@SAJPAEAAKAAV?$XNetUri@$0EAA@$0BA@$0BAA@$0CHA@$0EA@$0EA@@@@Z");
			}
		}
	}
	else
		Message(form("# WARNING cound not find XamGetXTweakManager!!\n"));
}

static showHudNew()
{
	auto ea, ref1, ref2;
	ea = LocByName("XamAppLoad");
	if(ea != BADADDR)
	{
		//Message(form("# INFO XamGetXTweakManager @ 0x%X\n", ea));
		ref1 = RfirstB(ea);
		ref2 = RnextB(ea, ref1);   
		ref1 = RnextB(ea, ref2);   
		ref2 = RnextB(ea, ref1);   
		ref1 = RnextB(ea, ref2);
		if(ref1 != BADADDR)
		{
			if(Dword(ref1-(11*4)) == 0x38600001)
			{
				Message(form("HudDisableNew @ 0x%X\n", ref1-(11*4)));
			}
			else
			{
				Message(form("#### BAD #### HudDisableNew @ 0x%X\n", ref1));
			}
		}
		else
			Message(form("#### BAD #### could not find HudDisableNew!\n"));

	}
	else
		Message(form("# WARNING cound not find XamAppLoad!!\n"));
}

// static nameBranchFunction(bAddr, name)
// {
	// auto inst, dest, ea, curr;
	// if(bAddr != BADADDR)
	// {
		// inst = Dword(bAddr);
		// //if(inst&0xFC000000)
		// dest = inst&0x3FFFFFC;
		// curr = bAddr&~0x80000000;
		// if(inst&0x2000000) // backward branch
			// dest = dest|0xFC000000;
		// ea = ((bAddr+dest)&0x80000000);
		// Message(form("branch at 0x%x jumps to 0x%x (inst %08x)\n", bAddr, ea, inst));
	// }
// }

static main()
{
	auto temp;
	Message("starting script\n");
	nameFunctionFindStartVerify("LoaderPrepe", "?TitleLoaderPrepareLoadExecutableFile@CXamTitleLoader@@AAAJPAD00KKKKK@Z", "3D 60 C0 05 61 6B 00 04", 5*4, 0x616B026E, 0);
	showGenericPatchOffset("PingLimit", "81 6B 00 10 7D 6A 58 50 2B 0B 00 1E", 2*4, 0x2B0B001E, 0);
	nameFunction("LicenseChk", "?ContentEvaluateLicense@XContent@@YAJPBU_XCONTENT_HEADER@@PAXPAKPAH@Z", "3F 80 80 07 7C 9B 23 78 7C B9 2B 78 7C DA 33 78 3B C0 00 00 63 9C 00 05 3B A0 00 00 3B E3 02 2C");
	showHudNew();
	//showGenericPatchOffset("HudDisable", "38 E0 00 00 38 C0 00 00 38 A0 00 00 7F 24 CB 78 4B", -7*4, 0x38600001, 0);
	nameFunctionFindBytes("XamRevokePatch", "?DetermineTitleParamsFromXexHeader@CXamTitleLoader@@AAAJPAU_IMAGE_XEX_HEADER@@KPAU_XAM_LOADER_TITLE_LAUNCH_PARAMS@1@@Z", "81 7D 00 04 61 6B 00 02 91 7D 00 04 81 63 00 0C 3D 40 53 51 61 4A 07 D6", 1*4);
	nameFunctionFindBytes("contLP", "?EvaluateContent@XContent@@YAJ_KKPBU_XCONTENT_HEADER@@PBU_XCONTENT_METADATA@@PAXHPAKPBD@Z", "3D 40 4C 49 81 65 00 00 61 4A 56 45 7F", 8*4);
	showGenericPatchOffset("contDID", "21 69 00 00 38 E0 00 00 7D 6B 59 10 38 C0 00 00 7D 6B F0 38 38 A1 00 54 91 61 00 54 38 80 00 02", 8*4, 0x7F83E378, -2*4);
	temp = nameFunctionFindBytes("XamKinectHealth", "?PreloadRunTasksForTitle@CXamTitleLoader@@AAAJPAU_XAM_LOADER_TITLE_LAUNCH_PARAMS@1@@Z", "7C 9F 23 78 55 7C FF FF 7F 19 C3 78 55 5A DF FE", 14*4);
	nameFunctionFindBytes("NoNewUpdate", "XampSystemUpdateIsPresent", "3D 60 00 00 3B 40 00 01 61 6B 97 36 7D 79 58 2E 7F 0B E0 40", 6*4);
		// nameBranchFunction(temp, "?SetLaunchParamsForKinectPreGameApp@CXamTitleLoader@@AAAXPAU_XAM_LOADER_TITLE_LAUNCH_PARAMS@1@@Z");
	temp = showGenericPatchOffset("XampXAuthStartup", "3B 60 00 00 3F A0 80 15 7F 7C DB 78 63 BD 84 06 7F 83 E3 78", 0, 0x3B600000, 0);
	findNextInst(temp, "XAuthStartupDest", "39 60 00 01");
	showXtweakXrefs();
	nameFunctionFindBytes("XAuthValidateURL", "XAuthValidateURL", "39 20 00 00 39 00 00 00 7F 67 DB 78 7F 86 E3 78 7F A5 EB 78 7F C4 F3 78 7F E3 FB 78 4B", 7*4);

	//just naming things...
	//nameFunctionFindBytesQuiet("XamKinectHealthOld", "?TitleLoaderPrepareRunPostStartTasks@CXamTitleLoader@@AAAXXZ", "81 7F 02 74 2B 0B 00 00 41 9A 00 0C 39 60 00 00 91 7F 02 74 38 60 00 00", -1*4);
	Message("script done\n");
}
