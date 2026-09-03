#CB_patches = [[9188,
#[0x4d10,0x7BEB0620,0x48000168], force branch to 4E78 skip integrity checks(b 0x168)
#[0x4df4,0x409a0010,0x60000000], ConsoleSequence check
#[0x4e40,0x409A0038,0x48000038], bConsoleType != 1 check
#[0x4ecc,0x409a0010,0x60000000], HammingWeight check
#[0x4f08,0x409a0010,0x60000000], force zeropair/skip smc hash
#[0x5618,0x480018e1,0x60000000], disable CD rc4 decryption
#[0x5678,0x480000b9,0x60000000], disable SHA_VERIFY_4BL error dieloop
#[9188,[[0x4f08,0x409a0010,0x60000000],[0x5618,0x480018e1,0x60000000],[0x5678,0x480000b9,0x60000000], [0x4d10, 0x7BEB0620, 0x48000168]]],

#;============================================================================
#; CB_B patches for 9188 trinity BL
#;============================================================================

.include "../macros.S"

	.globl _start
_start:

#;============================================================================
#;  patch secopt 6, don't revoke on fuseline 2
#;============================================================================
	MAKEPATCH 0x4DF4
0:
	nop
9:

#;============================================================================
#;  remove smc size limitation
#;============================================================================
	MAKEPATCH 0x4F50
0:
	nop
	nop
	nop
9:

#;============================================================================
#;  patch 4bl sha verify memdiff
#;============================================================================
	MAKEPATCH 0x5660
0:
	li		%r3, 0
9:

.set XeCryptCopyQwVec,		0x5750	# XeCryptCopyQwVec(VOID *dest, VOID *src, DWORD len) ; len is in number of 64bit reads
.set qwSbFlashAddrOff,		0x258
.set dwSysUpdateAddr,		0x64
.set dwSysUpdateSlotSize,	0x70
.set fusesSize,				0xC
#;============================================================================
#;  patch fuse load
#;  patch1 4C4C r10 is the dest, 8 instructions, r3 r4 r5 OK
#;============================================================================
	MAKEPATCH 0x4C4C
0:
	ld		%r3, qwSbFlashAddrOff(%r29)
	lwz		%r4, dwSysUpdateAddr(%r3)
	lwz		%r5, dwSysUpdateSlotSize(%r3)
	add		%r3, %r3, %r4
	add		%r4, %r3, %r5
	mr		%r3, %r10
#;	addi	%r4, 0x10
	li		%r5, 0xC
	MAKEBRANCHL	XeCryptCopyQwVec
9:

#;============================================================================
#;  patch fuse load
#;  patch2 5048 r10 is the dest, 8 instructions, r3 r4 r5 OK
#;============================================================================
	MAKEPATCH 0x5048
0:
	ld		%r3, qwSbFlashAddrOff(%r26)
	lwz		%r4, dwSysUpdateAddr(%r3)
	lwz		%r5, dwSysUpdateSlotSize(%r3)
	add		%r3, %r3, %r4
	add		%r4, %r3, %r5
	mr		%r3, %r10
#;	addi	%r4, 0x10
	li		%r5, 0xC
	MAKEBRANCHL	XeCryptCopyQwVec
9:

#;============================================================================
#;  fix boot flags, default cba sets 0x800, mfg sets 0x801
#;============================================================================
	MAKEPATCH 0x410
0:
	li		%r4, 0x601
9:


#;============================================================================
	.long 0xffffffff
	.end
#;============================================================================
