#define SPR_PIR		0x3ff
#define SPR_HID1	0x3f1
#define SPR_HID4	0x3f4
#define SPR_HID6	0x3f9
#define SPR_LPCR	0x13e
#define SPR_HRMOR	0x139
#define SPR_HSPRG0	0x130
#define SPR_HSPRG1	0x131

.globl _start
.extern start
.globl putch

_start:
	lis     %sp, -0x8000
	rldicr  %sp, %sp, 32,31			// %sp = ((%sp << 32) | (%sp >> 32)) & 0xFFFFFFFF00000000
	oris    %sp, %sp, 0x1E00
	mfspr   %r30, SPR_PIR
	rldicr  %r4, %r30, 16,47		// %r4 = ((%r30 << 16) | (%r30 >> 48)) & 0xFFFFFFFFFFFF0000
	subf    %sp, %r4, %sp
	addi    %sp, %sp, -0x80
	// lis     %r3, -0x8000
	// ori     %r3, %r3, 0x200 		// 0x80000200
	// rldicr  %r3, %r3, 32,31		// 0x80000200.00000000 // %r3 = ((%r3 << 32) | (%r3 >> 32)) & 0xFFFFFFFF00000000
	// oris    %r3, %r3, 5			// 0x80000200.00050000
	mfspr   %r3, SPR_HID1
	li      %r4, 1
	rldimi  %r3, %r4, 44,19			// %r3 = (%r3 & ~0x100000000000) | (((%r4 << 44) | (%r4 >> 20)) & 0x100000000000)
	isync
	mtspr   SPR_HID1, %r3
	isync
	mfspr   %r3, SPR_HID6
	li      %r4, 8
	rldimi  %r3, %r4, 44,16			// %r3 = (%r3 & ~0xF00000000000) | (((%r4 << 44) | (%r4 >> 20)) & 0xF00000000000)
	isync
	mtspr   SPR_HID6, %r3
	isync
	mfspr   %r3, SPR_LPCR
	li      %r4, 1
	rldimi  %r3, %r4, 10,53			// %r3 = (%r3 & ~0x400) | (((%r4 << 10) | (%r4 >> 54)) & 0x400)
	isync
	mtspr   SPR_LPCR, %r3
	isync
	cmpldi  %r30, 0
	bne     hold_all_but_core_0
	lis     %r13, 0x200
	mtmsrd  %r13, 1
	lis 	%r3, 0x8000
	rldicr  %r3, %r3, 32,31
	oris 	%r3, %r3, start@h
	ori 	%r3, %r3, start@l
	ld 		%rtoc, 8(%r3)
	bl      start					// returns 1 to start xell, 0 to reboot

	cmplwi  %r3, 0
	bne     startXell

	lis     %r31, 0x8000
	rldicr  %r31, %r31, 32,31
	ori     %r31, %r31, 0x100		// %r31 = 0x80000000.00000100
	li      %r3, 0
	isync
	mtspr   SPR_HRMOR, %r3
	mtspr   SPR_HSPRG0, %r3
	mtspr   SPR_HSPRG1, %r3
	isync
	mtlr    %r31
	blr

hold_all_but_core_0:
	lis 	%r4, 0x8000
	rldicr 	%r4, %r4, 32,31
	oris 	%r4, %r4, processors_online@h
	ori 	%r4, %r4, processors_online@l
	mfspr   %r3, SPR_PIR
	slwi    %r3, %r3, 2
	add     %r4, %r4, %r3
	li      %r5, 1
	stw     %r5, 0(%r4)
	lis 	%r28, 0x8000
	rldicr 	%r28, %r28, 32,31
	oris 	%r28, %r28, secondary_hold_addr@h
	ori 	%r28, %r28, secondary_hold_addr@l

/*
	// 80000000.00199992 - original hold address
	lis 	%r29, 0x8000
	rldicr 	%r29, %r29, 32,31
	oris    %r29, %r29, 0x20 		// 80000000_00200000
	li      %r30, 1
	slwi    %r3, %r30, 3    		// r3 = 8
	subf    %r29, %r3, %r29 		// ones compliment of r3+r29+1 80000000.001FFFF8

	// 80000000.01003078 - just after fuses which survive reboot
	lis     %r29, 0x8000
	rldicr  %r29, %r29, 32,31		// %r29 = ((%r29 << 32) | (%r29 >> 32)) & 0xFFFFFFFF00000000
	oris    %r29, %r29, 0x0100
	ori     %r29, %r29, 0x3078
*/
	lis 	%r29, 0x8000
	rldicr 	%r29, %r29, 32,31
	oris 	%r29, %r29, primary_hold_addr@h
	ori 	%r29, %r29, primary_hold_addr@l
	ld		%r29, 0(%r29)
	
	li      %r0, 0
	std     %r0, 0(%r29)
idlecorewait:
	ld      %r3, 0(%r29)
	cmpldi  %r3, 0
	bne     corecont
	lwz     %r3, 0(%r28)
	cmplwi  %r3, 0
	beq     idlecorewait
	lis     %r3, 1
	mtctr   %r3

fastsleep:	// give some time for core0 to catch up?
	nop
	bdnz    fastsleep

startXell:
	lis     %r31, 0x8000
	rldicr  %r31, %r31, 32,31
	oris    %r31, %r31, 0x1C04 		// 80000000.1c040000 Xell Entry
	mtlr    %r31
	blr

corecont:
	mfspr   %r3, SPR_HID6
	li      %r4, 1
	rldimi  %r3, %r4, 48,15			// %r3 = (%r3 & ~0x1000000000000) | (((%r4 << 48) | (%r4 >> 16)) & 0x1000000000000)
	li      %r4, 8
	rldimi  %r3, %r4, 44,16			// %r3 = (%r3 & ~0xF00000000000) | (((%r4 << 44) | (%r4 >> 20)) & 0xF00000000000)
	li      %r4, 0xE
	rldimi  %r3, %r4, 34,26			// %r3 = (%r3 & ~0x3C00000000) | (((%r4 << 34) | (%r4 >> 30)) & 0x3C00000000)
	isync
	mtspr   SPR_HID6, %r3
	sync
	isync
	mfspr   %r3, SPR_LPCR
	li      %r4, 1
	rldimi  %r3, %r4, 1,62			// %r3 = (%r3 & ~2) | (((%r4 << 1) | (%r4 >> 63)) & 2)
	isync
	mtspr   SPR_LPCR, %r3
	sync
	isync
	li      %r3, 0x3FF
	rldicr  %r3, %r3, 32,31			// %r3 = ((%r3 << 32) | (%r3 >> 32)) & 0xFFFFFFFF00000000
	tlbiel  %r3, 1
	sync
	mfspr   %r3, SPR_HID4
	li      %r4, 0x3F
	rldimi  %r3, %r4, 40,18			// %r3 = (%r3 & ~0x3F0000000000) | (((%r4 << 40) | (%r4 >> 24)) & 0x3F0000000000)
	isync
	mtspr   SPR_HID4, %r3
	sync
	isync
	li      %r3, 0
	mtspr   SPR_HSPRG0, %r3
	mtspr   SPR_HSPRG1, %r3
	mfspr   %r3, SPR_HID1
	li      %r4, 1
	rldimi  %r3, %r4, 63,0			// %r3 = (%r3 & ~0x8000000000000000) | (((%r4 << 63) | (%r4 >> 1)) & 0x8000000000000000)
	li      %r4, 7
	rldimi  %r3, %r4, 58,3			// %r3 = (%r3 & ~0x1C00000000000000) | (((%r4 << 58) | (%r4 >> 6)) & 0x1C00000000000000)
	li      %r4, 3
	rldimi  %r3, %r4, 52,10			// %r3 = (%r3 & ~0x30000000000000) | (((%r4 << 52) | (%r4 >> 12)) & 0x30000000000000)
	li      %r4, 1
	rldimi  %r3, %r4, 38,25			// %r3 = (%r3 & ~0x4000000000) | (((%r4 << 38) | (%r4 >> 26)) & 0x4000000000)
	li      %r4, 0x100
	rldicr  %r4, %r4, 32,31			// %r4 = ((%r4 << 32) | (%r4 >> 32)) & 0xFFFFFFFF00000000
	mfmsr   %r5
	li      %r6, 0x30
	andc    %r5, %r5, %r6
	mtsrr1  %r5
	li      %r6, 0x100
	mtsrr0  %r6
	mtspr   SPR_HID1, %r3
	mtspr   SPR_HRMOR, %r4
	rfid

putch:
	lis     %r4, 0x8000 
	ori     %r4, %r4, 0x200 
	rldicr  %r4, %r4, 32,31
	oris    %r4, %r4, 0xEA00
WaitLoop:
	lwz     %r5, 0x1018(%r4)
	rlwinm. %r5, %r5, 0,6,6
	beq     WaitLoop
	slwi    %r3, %r3, 24
	stw     %r3, 0x1014(%r4)
	blr

.end
