#include <idc.idc>
#include <x360_imports.idc>
#include "generic_functions.idc"
#include "segments.idc"

static loadSectionFile(address, fname)
{
	auto fp, len;
	fp = fopen(fname, "rb");
	if(fp != 0)
	{
		len = filelength(fp);
		SetupSection(address, address+len, "DATA", 4, fname, 1);
		Message(form("loading segment %s at 0x%x for 0x%x\n", fname, address, len));
		loadfile(fp, 0, address, len);
		fclose(fp);
		return 1;
	}
	else
		Message(form("could not load %s\n", fname));
	return 0;
}

static setupExternSections()
{
	if(loadSectionFile(0x8000020000000000, "8000020000000000.bin") == 1)
		segSetupSoCROM(0x8000020000000000);
	if(loadSectionFile(0x8000020000010000, "8000020000010000.bin") == 1)
		segSetupSoCSRAM(0x8000020000010000);
	if(loadSectionFile(0x8000020000020000, "8000020000020000.bin") == 1)
		segSetupSoCRegs(0x8000020000020000);
}

static main()
{
	LoadTil("xbox360.til");
	Message(form("Loading external segments...\n"));
	setupExternSections();
	AutoMark2(0, MAXADDR, AU_USED); // reanalyze program
	Message(form("Done!\n"));
}
