#;============================================================================
#; CB_B patches for 13182 corona BL
#;============================================================================

.include "../macros.S"

	.globl _start
_start:
.set ORG_SIZE, 0x9080
.set POST_20_SRC, 0x3e8
.set POST_21_SRC, 0x6234 #;
.set POST_22_SRC, 0x6560

#;============================================================================
#;  patch header version to 9199 - not needed
#;============================================================================
	.long 0x0
	.long 1
0:
	.long 0x434223E4
9:

#;============================================================================
#;  yeah we don't really need to mess with the thread control register
#;============================================================================
	MAKEPATCH 0xE20 
0:
	clrlwi	r3, r3, 16
	addis	r3, r3, 0x400
	mtlr	r3
	MAKEBRANCH 0xE50
9:

# =================================================================================
#;  skips check on patch slots
# =================================================================================
	MAKEPATCH 0x62D0
0:
	MAKEBRANCH 0x63C4 #; 61 instructions
	# b		(0x59BC-0x59A0)
	# b		(0x59BC-0x59A4)
	# b		(0x59BC-0x59A8)
	# b		(0x59BC-0x59AC)
	# b		(0x59BC-0x59B0)
	# b		(0x59BC-0x59B4)
	# b		(0x59BC-0x59B8)
9:

#;============================================================================
#;  force zeropair/skip smc hash check
#;============================================================================
	MAKEPATCH 0x6448
0:
	nop
9:

#;============================================================================
#;  disable check on CD header
#;============================================================================
	MAKEPATCH 0x6A78
0:
	nop
9:

#;============================================================================
#;  disable CD rc4 decryption
#;============================================================================
	# MAKEPATCH 0x6B34
# 0:
	# nop
# 9:

#;============================================================================
#;  disable SHA_VERIFY_4BL pseudo-memdiff
#;============================================================================
	MAKEPATCH 0x6B84
0:
	li		r3, -1
9:

#;============================================================================
#;  POST code
#;============================================================================
	MAKEPATCH 0x6D48
	.set POST_20, PatchAddr
	.set POST_21, (PatchAddr + (1f - 0f))
	.set POST_22, (PatchAddr + (2f - 0f))
	# .set GetFlashAddr, (PatchAddr + (3f - 0f))
0:
	.long 0x7C0004AC	# sync
	li		r3, 0x20
	bl		set_POST
	MAKEBRANCH (POST_20_SRC+4)
1:
	li		r3, 0x21
	bl		set_POST
	addi	r4, r1, 0xA0
	MAKEBRANCH (POST_21_SRC+4)
2:
	li		r11, 0
	li		r3, 0x22
	bl		set_POST
	MAKEBRANCH (POST_22_SRC+4)
set_POST:
	li		r4, 0x200
	oris	r4, r4, 0x8000
	sldi	r4, r4, 32
	oris	r4, r4, 6
	sldi	r3, r3, 56
	std		r3, 0x1010(r4)
	blr
# 3:		#;0x80000200C8000000
	# lis		r4, 0x8000
	# ori		r4, r4, 0x200
	# sldi	r4, r4, 32
	# oris	r4, r4, 0xC800
	# blr
	# .long 0xDEADBEEF

	# .long 0x8E98E2BC, 0x605E5B68, 0x899324B1, 0x69279A5B
	# .long 0xA97F680B, 0x74744E01, 0x2095207A, 0xAC490328
	# .long 0x7679B0DB, 0x9421DA0C, 0x40B439DB, 0xD5899C0F
	# .long 0x72A746C3, 0x513CF488, 0x6DB2BDB2, 0x9AEFFEE1
	# .long 0xE00CBC3F, 0x7BA252CC, 0xAC4257F8, 0xCA0FBEB6
	# .long 0x8E98E2BC

9:

#;============================================================================
#;  insert POST out 0x20
#;============================================================================
	MAKEPATCH POST_20_SRC
0:
	MAKEBRANCH POST_20
9:

#;============================================================================
#;  insert POST out 0x21
#;============================================================================
	MAKEPATCH POST_21_SRC
0:
	MAKEBRANCH POST_21
9:

#;============================================================================
#;  insert POST out 0x22
#;============================================================================
	MAKEPATCH POST_22_SRC
0:
	MAKEBRANCH POST_22
9:

.set XeCryptCopyQwVec,		0x6EB0	# XeCryptCopyQwVec(VOID *dest, VOID *src, DWORD len) ; len is in number of 64bit reads
.set qwSbFlashAddrOff,		0x258
.set dwSysUpdateAddr,		0x64
.set dwSysUpdateSlotSize,	0x70
.set fusesSize,				0xC
#;============================================================================
#;  patch fuse load
#;  r10 is the dest, 8 instructions, r3 r4 r5 OK
#;============================================================================
	# MAKEPATCH 0x6564
# 0:
	# ld		r3, qwSbFlashAddrOff(r26)
	# lwz		r4, dwSysUpdateAddr(r3)
	# lwz		r5, dwSysUpdateSlotSize(r3)
	# add		r3, r3, r4
	# add		r4, r3, r5
	# mr		r3, r10
	# li		r5, 0xC
	# MAKEBRANCHL	XeCryptCopyQwVec
# 9:

#;============================================================================
#;  patch fuse load
#;  r4 is destination, use r3-r7; max 12 instructions
#;============================================================================
	# MAKEPATCH 0x6CF0
# 0:
	# mr		r3, r10
	# lis		r4, 0x8000
	# ori		r4, r4, 0x200
	# sldi	r4, r4, 32
	# oris	r4, r4, 0xC80C #; C80C0000
	# ori		r4, r4, 0x0 #; C80C0000
	# li		r5, 0xC
	# lwz		r4, dwSysUpdateAddr(r6)
	# lwz		r5, dwSysUpdateSlotSize(r6)
	# add		r6, r6, r4
	# add		r4, r6, r5
	# MAKEBRANCH	XeCryptCopyQwVec
	# blr
# 9:
	# ld		r6, qwSbFlashAddrOff(r29)
	# lwz		r4, dwSysUpdateAddr(r6)
	# lwz		r5, dwSysUpdateSlotSize(r6)
	# add		r6, r6, r4
	# add		r4, r6, r5
	# mr		r3, r10
	# li		r5, 0xC
	# MAKEBRANCH	XeCryptCopyQwVec

	# mr		r3, r4					#; dest
	# addis	r11, r2, 0
	# addi	r7, r11, 0
	# ld		r7, qwSbFlashAddrOff(r7)
	# lwz		r4, dwSysUpdateAddr(r7)
	# lwz		r5, dwSysUpdateSlotSize(r7)
	# add		r7, r7, r4 				#; flashAddr+UpdateAddr
	# add		r4, r7, r5 				#; source (flashAddr+UpdateAddr)+UpdateSlotSize (2nd patch slot source)
	# li		r5, 0xC					#; size
	# MAKEBRANCH	XeCryptCopyQwVec

# 6CF0 readFuses #; 12 instructions plus all next
# 6D48 blr #; 70 instructions plus next, init running state only call (69B0 bl initRunningState), disable that and can disable (6998 bl readFuses)
# 6E60 blr #; 20 instructions update running state (called from everywhere)
#;============================================================================
#;  disable read fuses for seeding timebase randomization function
#;============================================================================
	MAKEPATCH 0x69B0
0:
	nop
9:

#;============================================================================
#;  disable timebase randomization init
#;============================================================================
	MAKEPATCH 0x6998
0:
	nop
9:

#;============================================================================
#;  disable timebase randomization update (called from everywhere)
#;============================================================================
	MAKEPATCH 0x6E60
0:
	blr
9:

#;============================================================================
	.long 0xffffffff
	.end
#;============================================================================
