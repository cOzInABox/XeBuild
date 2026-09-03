#include <types.h>
#include <cache.h>

#define PWR_REAS_PWRBTN		0x11 // power button pushed
#define PWR_REAS_EJECT		0x12 // eject button pushed
#define PWR_REAS_ALARM		0x15 // guess ~ should be the wake alarm ~
#define PWR_REAS_REMOPWR	0x20 // power button on 3rd party remote/ xbox universal remote
#define PWR_REAS_REMOX		0x22 // xbox universal media remote X button
#define PWR_REAS_WINBTN		0x24 // windows button pushed IR remote
#define PWR_REAS_RESET		0x30 // HalReturnToFirmware(1 or 2 or 3) = hard reset by smc
#define	PWR_REAS_KIOSK		0x41 // console powered on by kiosk pin
#define PWR_REAS_WIRELESS	0x55 // wireless controller middle button/start button pushed to power on controller and console
#define PWR_REAS_WIRED		0x5A // wired controller guide button pushed (attached to back usb port)

// 0x80000000.001FFFF8ULL - original hold address
// 0x80000000.01003078ULL - just after fuses which survive reboot
#define DEFAULT_PHOLD_ADDR	0x8000000001003078ULL
#define DEFAULT_PWRRSN		PWR_REAS_EJECT
#define DEFAULT_ALT_PWRRSN	PWR_REAS_KIOSK
#define SET_CYGNOS_UART		0x1
#define SET_NODVD			0x2
#define SET_USEPWRRSN		0x4

#define SMC_CMD_TRAYSTS		0xA
#define SMC_CMD_PWRRSN		0x1

// PCI BASE
#define PCI_BAR		(0x80000200EA000000ULL)

// c8xxxxxx: memory mapped NAND flash (read only, 1:1 mapping, no ECC bytes)
// 0x80000200C8000000       Scfx Flash 
#define SMC_XMIT_STS 	(*(volatile uint32_t*)(PCI_BAR+0x1084))
#define SMC_XMIT_BUF 	(*(volatile uint32_t*)(PCI_BAR+0x1080))
#define SMC_RCV_STS 	(*(volatile uint32_t*)(PCI_BAR+0x1094))
#define SMC_RCV_BUF 	(*(volatile uint32_t*)(PCI_BAR+0x1090))
#define UART_CNTRL		(*(volatile uint32_t*)(PCI_BAR+0x101C))
// #define SMC_XMIT_STS 	(*(volatile uint32_t*)0x80000200EA001084ULL)
// #define SMC_XMIT_BUF 	(*(volatile uint32_t*)0x80000200EA001080ULL)
// #define SMC_RCV_STS 	(*(volatile uint32_t*)0x80000200EA001094ULL)
// #define SMC_RCV_BUF 	(*(volatile uint32_t*)0x80000200EA001090ULL)
// #define UART_CNTRL		(*(volatile uint32_t*)0x80000200EA00101CULL)

#define XELL_NAND_OFF  (void*)(0x80000200C8095060ULL) // 256kib
#define XELL_MEM_DST   (void*)(0x800000001C040000ULL)

#define SOC_OFF (void*)(0x8000020000000000ULL) // 32kib
#define SOC_DST (void*)(0x8000000000000000ULL)

#define MAX_PATCH_SIZE = 0x4000
#define PATCH_NAND_OFF (void*)(0x80000200C8091000ULL)
#define PATCH_MEM_DST  (void*)(0x8000000000000000ULL)
#define PATCH_REM_DST  (void*)(0x8000000001000000ULL)

#define FUSE_NAND_OFF (void*)(0x80000200C8095000ULL) // 0x60 bytes
#define FUSE_MEM_DST  (void*)(0x8000000001003000ULL)

#define OPTS_NAND_OFF (void*)(0x80000200C8000048ULL)
// offset in array
#define OPTS_DUAL		4
#define OPTS_OPTIONS	5 
#define OPTS_PWRR2		6
#define OPTS_PWRR1		7

extern void putch(unsigned char c);
extern void setuart(void);

extern char _start[];
extern char bss_start[], bss_end[];

// some pre-alloc'd data areas for catch and release core fishing
volatile int processors_online[6] = {1,0,0,0,0,0};
volatile int secondary_hold_addr[1] = {0};
unsigned char options[8] = {0, 0, 0, 0x1, 0, (SET_CYGNOS_UART|SET_USEPWRRSN), DEFAULT_ALT_PWRRSN, DEFAULT_PWRRSN};
// u32 options[4] = {DEFAULT_OPTIONS, DEFAULT_PWRRSN, 0x0, DEFAULT_ALT_PWRRSN};
u64 primary_hold_addr[1] = {DEFAULT_PHOLD_ADDR};

// interrupt vector addresses
const unsigned long exc[]={0x100, 0x200, 0x300, 0x380, 0x400, 0x480, 0x500, 0x600, 0x700, 0x800, 0x900, 0x980, 0xC00, 0xD00, 0xF00, 0xF20, 0x1300, 0x1600, 0x1700, 0x1800};

// gratuitously swiped from vsprintf
static inline void _memset(void * s, int c, size_t count)
{
	char* xs = (char*)s;

	while(count--)
		*xs++ = c;
}

static inline void _memcpy(void * dest, const void *src, size_t count)
{
	char* tmp = (char*)dest;
	char* s = (char*)src;

	while (count--)
		*tmp++ = *s++;
}

static inline void _memcpy32(void * dest, const void *src, size_t count)
{
	uint32_t* tmp = (uint32_t*)dest;
	uint32_t* s = (uint32_t*)src;

	while (count--)
		*tmp++ = *s++;
}

/*
const char chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
static void charToPut(char c)
{
	putch(chars[((c>>4)&0xF)]);
	putch(chars[(c&0xF)]);
}
static void wordToPut(uint32_t c)
{
	charToPut((c>>24)&0xFF);
	charToPut((c>>16)&0xFF);
	charToPut((c>>8)&0xFF);
	charToPut(c&0xFF);
	putch('\n');
}
*/

static void putstring(const char *c)
{
	int i = 0;
	while (c[i] != 0)
	{
		putch(c[i]);
		i++;
	}
}

static inline int get_online_processors(void)
{
	int i;
	int res = 0;
	for (i = 0; i < 6; ++i)
		if (processors_online[i])
			res |= 1 << i;
	return res;
}

static inline void place_jump(void *addr, void *_target)
{
	unsigned long target = (unsigned long)_target;
	dcache_flush(addr - 0x80, 0x100);
	*(volatile uint32_t*)(addr - 0x18 + 0) = 0x3c600000 | ((target >> 48) & 0xFFFF);
	*(volatile uint32_t*)(addr - 0x18 + 4) = 0x786307c6;
	*(volatile uint32_t*)(addr - 0x18 + 8) = 0x64630000 | ((target >> 16) & 0xFFFF);
	*(volatile uint32_t*)(addr - 0x18 + 0xc) = 0x60630000 | (target & 0xFFFF);
	*(volatile uint32_t*)(addr - 0x18 + 0x10) = 0x7c6803a6;
	*(volatile uint32_t*)(addr - 0x18 + 0x14) = 0x4e800021;
	flush_code(addr - 0x18, 0x18);
	*(volatile uint32_t*)(addr + 0) = 0x4bffffe8;
	flush_code(addr, 0x80);
}

static void smc_send(unsigned char *msg)
{
	uint32_t* msgl = (uint32_t*)msg;
	int i;
	while (!(SMC_XMIT_STS >> 26 & 1));
	SMC_XMIT_STS = 0x4000000;
	for(i=0; i<4; i++)
		SMC_XMIT_BUF = msgl[i];
	SMC_XMIT_STS = 0;
}

static int smc_receive(unsigned char *msg)
{
	if (SMC_RCV_STS & 0x4000000)
	{
		uint32_t *msgl = (uint32_t*)msg;
		int i;
		SMC_RCV_STS = 4;
		for (i = 0; i < 4; ++i)
			*msgl++ = SMC_RCV_BUF;
		SMC_RCV_STS = 0;
		return 0;
	}
	return -1;
}

static unsigned char getSmc(char command)
{
	unsigned char smsg[16];
	unsigned char rmsg[16];
	_memset(smsg, 0x0, 16);
	_memset(rmsg, 0x0, 16);
	smsg[0] = command; // get tray status
	smc_send(smsg);

	while(rmsg[0] != command)
	{
		_memset(rmsg, 0x0, 16);
		smc_receive(rmsg);
	}
	//charToPut(rmsg[1]);
	return (rmsg[1]&0xFF);
}
static inline uint32_t bswap_32(uint32_t t)
{
	return ((t & 0xFF) << 24) | ((t & 0xFF00) << 8) | ((t & 0xFF0000) >> 8) | ((t & 0xFF000000) >> 24);
}

static void resetUsb()
{
	// resetting EHCI
	// disable interrupts 
	*(volatile uint32_t*)(PCI_BAR+0x3028) = 0x00000000;
	*(volatile uint32_t*)(PCI_BAR+0x5028) = 0x00000000;
	// set reset, set interrupt thresh
	*(volatile uint32_t*)(PCI_BAR+0x3020) = 0x02000000; // bswap_32(0x00000002);
	*(volatile uint32_t*)(PCI_BAR+0x5020) = 0x02000000; // bswap_32(0x00000002);
	*(volatile uint32_t*)(PCI_BAR+0x3020) = 0x00000800; // bswap_32(0x00080000);
	*(volatile uint32_t*)(PCI_BAR+0x5020) = 0x00000800; // bswap_32(0x00080000);
	// set configure flag to default to OHCI controller
	*(volatile uint32_t*)(PCI_BAR+0x3060) = 0x00000000;
	*(volatile uint32_t*)(PCI_BAR+0x5060) = 0x00000000;
	// set frame
	*(volatile uint32_t*)(PCI_BAR+0x302C) = 0x00000000;
	*(volatile uint32_t*)(PCI_BAR+0x502C) = 0x00000000;
	// clear status
	*(volatile uint32_t*)(PCI_BAR+0x3024) = 0xFFFF0000; // bswap_32(0x0000FFFF);
	*(volatile uint32_t*)(PCI_BAR+0x5024) = 0xFFFF0000; // bswap_32(0x0000FFFF);
}

static inline void patchEngine(void * dest, const void *src, void* remDest)
{
	uint32_t count=0, addr = 0, i;
	uint32_t* tmp;
	uint32_t* s = (uint32_t*)src;

	// apply 1bl patches
	addr = *s++;
	//wordToPut(addr);
	while (addr != 0xFFFFFFFF)
	{
		count = *s++;
		tmp = (uint32_t*)(dest+addr);
		//wordToPut((uint32_t)tmp);
		for(i = 0; i < count; i++)
		{
			*tmp++ = *s++;
		}
		addr = *s++;
	}
	count = 0;
	tmp = (uint32_t*)remDest;

	// shoot the remainder to the next slots patch location
	while(count < 3)
	{
		addr = *s++;
		*tmp++ = addr;
		if(addr == 0xFFFFFFFF)
		{
			count++;
			tmp = (uint32_t*)(remDest+(count*0x1000));
			//wordToPut((uint32_t)tmp);
		}
	}
	//putch('\n');
	//wordToPut(addr);
	//wordToPut((uint32_t)tmp);
}

int start(void)
{
	int i;
	unsigned char boot = 0;
	BOOL bootxell = FALSE;
	BOOL dualboot = FALSE;

	/* clear BSS, we're already late */
	unsigned char *p = (unsigned char*)(bss_start);
	_memset(p, 0, bss_end - bss_start);

	// replace all interrupt vectors
	for (i=0; i<20; ++i)
	{
		place_jump((void*)exc[i], _start); //HRMOR + 
	}

	// tell the cores to continue so they will hit a hv interrupt
	*(uint64_t*)(0x8000020000052010ULL) = 0x3e0078;
	
	// catch the cores when they do hit the interrupt
	while (get_online_processors() != 0x3F)
	{
		//charToPut(get_online_processors());
	}

	_memcpy32(options, OPTS_NAND_OFF, 2);
	if(options[OPTS_PWRR1] == 0)
		options[OPTS_PWRR1] = PWR_REAS_EJECT;

	/* set UART to 38400, 8, N, 1 for cygnos*/
	if((options[OPTS_OPTIONS]&SET_CYGNOS_UART) != 0)
	{
		UART_CNTRL = 0xae010000;
	}
	//putch('\n');
	//charToPut(get_online_processors());
	
	// get smc boot reason / tray status
	if((options[OPTS_OPTIONS]&SET_USEPWRRSN) != 0)
	{
		boot = getSmc(SMC_CMD_PWRRSN);
		if(boot == options[OPTS_PWRR1])
			bootxell = 1;
		else if(options[OPTS_PWRR2] != 0)
		{
			if(options[OPTS_PWRR2] == boot)
				bootxell = 1;		
		}
		if(options[OPTS_DUAL] != 0)
		{
			if(boot == options[OPTS_DUAL])
			{
				putstring("!SWITCH");
				dualboot = TRUE;
			}
		}
	}
	else
	{
		boot = getSmc(SMC_CMD_TRAYSTS);
		// tray out = 0x60, tray not closed and not out = 0x61, tray closed = 0x62
		if((options[OPTS_OPTIONS]&SET_NODVD) != 0)
		{
			bootxell = (boot == 0x60); // if tray is open, boot xell
		}
		else
		{
			bootxell = (boot != 0x62); // if tray is not closed, boot xell
		}
	}
	
	//charToPut(boot);
	putstring("\n  __               ____   ___   ___ _____");
	putstring("\n / _|_ __ ___  ___| __ ) / _ \\ / _ \\_   _|");
	putstring("\n| |_| '__/ _ \\/ _ \\  _ \\| | | | | | || |");
	putstring("\n|  _| | |  __/  __/ |_) | |_| | |_| || |");
	putstring("\n|_| |_|  \\___|\\___|____/ \\___/ \\___/ |_|");

	putstring("\n             [v0.10 - inspired by ikari]");
	putstring("\nbooting ");


	if(bootxell)
	{
		putstring("xell....\n");
		resetUsb();
		_memcpy32(XELL_MEM_DST, XELL_NAND_OFF, (0x40000/4));
		secondary_hold_addr[0] = 1;
		return 1;
	}
	else // boot firmware
	{
		if(dualboot == 0)
			putstring("firmware v.XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		else
			putstring("alt firmware");
			
		// copy 1bl
		_memcpy(SOC_DST, SOC_OFF, 0x8000);

		// apply 1bl patches and move remaining patches to proper location
		patchEngine(PATCH_MEM_DST, PATCH_NAND_OFF, PATCH_REM_DST);

		// copy fuses
		_memcpy(FUSE_MEM_DST, FUSE_NAND_OFF, 0x60);
		return 0;
	}
	return 0;
}
/*
	0x90000,	// FS_FREEBOOTBIN
	0x91000,	// FS_PATCHBIN
	0x95000,	// FS_FUSESBIN
	0x95060,	// FS_XELL
*/

