#;============================================================================
#; CB_B patches for 13180 corona BL
#;============================================================================

.include "../macros.S"

	.globl _start
_start:

#;============================================================================
#;  patch header version
#;============================================================================
	MAKEPATCH 0x0
0:
	.long 0x434223E4
9:

#;============================================================================
#;  messes with SPR_CTRL and return register
#;============================================================================
	MAKEPATCH 0xE20 
0:
	clrlwi	r3, r3, 16
	addis	r3, r3, 0x400
	mtlr	r3
	MAKEBRANCH 0xE50
9:

#;============================================================================
#;  force branch to 51B0 - skip integrity checks(b 0x168)
#;============================================================================
	MAKEPATCH 0x5038
0:
	MAKEBRANCH	0x512C
9:

#;============================================================================
#;  patch secopt 7, skip smc hash HmacShaVerify
#;============================================================================
	MAKEPATCH 0x51B0
0:
	nop
9:

#;============================================================================
#;  disable check on CD header
#;============================================================================
	MAKEPATCH 0x57DC
0:
	MAKEBRANCH 0x57E4
9:

#;============================================================================
#;  disable SHA_VERIFY_4BL pseudo-memdiff (additional put nop at 590C and 5928 and 59D8)
#;============================================================================
	MAKEPATCH 0x58EC
0:
	li		r3, -1
9:

.set XeCryptCopyQwVec,		0x5C20	# XeCryptCopyQwVec(VOID *dest, VOID *src, DWORD len) ; len is in number of 64bit reads
.set qwSbFlashAddrOff,		0x258
.set dwSysUpdateAddr,		0x64
.set dwSysUpdateSlotSize,	0x70
.set fusesSize,				0xC
#;============================================================================
#;  patch fuse load
#;  r4 is destination, uses r3, r7, r8, r9, r10, r11; max 11 instructions
#; r11 is qwSocMmioAddr
#;============================================================================
	MAKEPATCH 0x5A60
0:
	mr		%r3, %r4
	li		%r5, fusesSize
	addis	%r11, %r2, 0
	ld		%r11, qwSbFlashAddrOff(%r11)
	lwz		%r10, dwSysUpdateAddr(%r11)
	add		%r4, %r11, %r10
	lwz		%r10, dwSysUpdateSlotSize(%r11)
	add		%r4, %r4, %r10
	MAKEBRANCH	XeCryptCopyQwVec
9:

#;============================================================================
#;  patch fuse load
#;  patch2 5388 r10 is the dest, 8 instructions, r3 r4 r5 OK
#;============================================================================
	MAKEPATCH 0x52CC
0:
	ld		%r3, qwSbFlashAddrOff(%r26)
	lwz		%r4, dwSysUpdateAddr(%r3)
	add		%r4, %r3, %r4
	lwz		%r5, dwSysUpdateSlotSize(%r3)
	mr		%r3, %r10
	add		%r4, %r4, %r5
	li		%r5, fusesSize
	MAKEBRANCHL	XeCryptCopyQwVec
9:

#;============================================================================
#;  fix boot flags, default cba sets 0x800, mfg sets 0x801
#;============================================================================
	# MAKEPATCH 0x420
# 0:
	# li		%r4, 0x601
# 9:

#;============================================================================
	.long 0xffffffff
	.end
#;============================================================================
