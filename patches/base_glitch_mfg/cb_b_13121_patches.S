#;============================================================================
#; CB_B patches for 13121 corona BL
#;============================================================================

.include "../macros.S"

	.globl _start
_start:

#;============================================================================
#;  force branch to 51B0 - skip integrity checks(b 0x168)
#;============================================================================
	# MAKEPATCH 0x5048
# 0:
	# b 0x168
# 9:

#;============================================================================
#;  patch secopt 7, skip smc hash HmacShaVerify
#;============================================================================
	# MAKEPATCH 0x5240
# 0:
	# nop
# 9:

#;============================================================================
#;  disable check on CD header
#;============================================================================
	# MAKEPATCH 0x58ac
# 0:
	# nop
# 9:

# =================================================================================
# remove 4BL hash check
# (bl XeCryptMemDiff -> li r3, 0
# =================================================================================
#; GOLDEN!!!
	MAKEPATCH 0x59A0		# 3,4 cold 4,1,3,3,1,3 hot
0:
	b		(0x59BC-0x59A0)
	b		(0x59BC-0x59A4)
	b		(0x59BC-0x59A8)
	b		(0x59BC-0x59AC)
	b		(0x59BC-0x59B0)
	b		(0x59BC-0x59B4)
	b		(0x59BC-0x59B8)
9:

#; GOLDEN!!!
 # MAKEPATCH 0x59B8		# 3,2,3 cold 1,1,1,3 hot
# 0:
	# b		(0x59BC-PatchAddr)
# 9:



	# MAKEPATCH 0x59A0 # 4 everytime
# 0:
	# b		(0x59BC-PatchAddr)
# 9:

 # MAKEPATCH 0x59A4		# 3,6 cold 1,1 hot
# 0:
	# b		(0x59BC-PatchAddr)
# 9:

 # MAKEPATCH 0x59A8		# 5 cold
# 0:
	# b		(0x59BC-PatchAddr)
# 9:

 # MAKEPATCH 0x59AC		#  2, 5 cold 1,1,2,5 hot
# 0:
	# b		(0x59BC-PatchAddr)
# 9:

 # MAKEPATCH 0x59B0		# 5 cold 1,1,2 hot
# 0:
	# b		(0x59BC-PatchAddr)
# 9:

 # MAKEPATCH 0x59B4		# 6 cold 2, hot
# 0:
	# b		(0x59BC-PatchAddr)
# 9:

 # MAKEPATCH 0x59B8		# 3,2,3 cold 1,1,1,3 hot
# 0:
	# b		(0x59BC-PatchAddr)
# 9:


	# MAKEPATCH 0x59A0
# 0:
	# li 		%r3, 0
# 9:

	# MAKEPATCH 0x59B8
# 0:
	# nop
# 9:

#;============================================================================
#;  fix boot flags, default cba sets 0x800, mfg sets 0x801
#;============================================================================
	# MAKEPATCH 0x420
# 0:
	# li		%r4, 0x601
# 9:

.set XeCryptCopyQwVec,		0x5A90	# XeCryptCopyQwVec(VOID *dest, VOID *src, DWORD len) ; len is in number of 64bit reads
.set qwSbFlashAddrOff,		0x258
.set dwSysUpdateAddr,		0x64
.set dwSysUpdateSlotSize,	0x70
.set fusesSize,				0xC
#;============================================================================
#;  patch fuse load
#;  patch1 4F84 r10 is the dest, 8 instructions, r3 r4 r5 OK
#;============================================================================
	MAKEPATCH 0x4F84
0:
	ld		%r3, qwSbFlashAddrOff(%r29)
	lwz		%r4, dwSysUpdateAddr(%r3)
	lwz		%r5, dwSysUpdateSlotSize(%r3)
	add		%r3, %r3, %r4
	add		%r4, %r3, %r5
	mr		%r3, %r10
	li		%r5, 0xC
	MAKEBRANCHL	XeCryptCopyQwVec
9:

#;============================================================================
#;  patch fuse load
#;  patch2 5388 r10 is the dest, 8 instructions, r3 r4 r5 OK
#;============================================================================
	MAKEPATCH 0x5388
0:
	ld		%r3, qwSbFlashAddrOff(%r26)
	lwz		%r4, dwSysUpdateAddr(%r3)
	lwz		%r5, dwSysUpdateSlotSize(%r3)
	add		%r3, %r3, %r4
	add		%r4, %r3, %r5
	mr		%r3, %r10
	li		%r5, 0xC
	MAKEBRANCHL	XeCryptCopyQwVec
9:

#;============================================================================
#;  patch fuse load
#;  patch1 4F84 r10 is the dest, 8 instructions, r3 r4 r5 OK
#;============================================================================
	# MAKEPATCH 0x4F84
# 0:
	# ld		%r3, qwSbFlashAddrOff(%r29)
	# lwz		%r4, dwSysUpdateAddr(%r3)
	# add		%r4, %r3, %r4
	# lwz		%r5, dwSysUpdateSlotSize(%r3)
	# add		%r4, %r4, %r5
	# li		%r5, fusesSize
	# mr		%r3, %r10
	# MAKEBRANCHL	XeCryptCopyQwVec
# 9:

#;============================================================================
#;  patch fuse load
#;  patch2 5388 r10 is the dest, 8 instructions, r3 r4 r5 OK
#;============================================================================
	# MAKEPATCH 0x5388
# 0:
	# ld		%r3, qwSbFlashAddrOff(%r26)
	# lwz		%r4, dwSysUpdateAddr(%r3)
	# add		%r4, %r3, %r4
	# lwz		%r5, dwSysUpdateSlotSize(%r3)
	# add		%r4, %r4, %r5
	# li		%r5, fusesSize
	# mr		%r3, %r10
	# MAKEBRANCHL	XeCryptCopyQwVec
# 9:


	# MAKEPATCH 0x7B40
# 0:
	# .long 0x8E98E2BC, 0x605E5B68, 0x899324B1, 0x69279A5B
	# .long 0xA97F680B, 0x74744E01, 0x2095207A, 0xAC490328

	# 3 on cold, fast on warm
	# .long 0x8E98E2BC, 0x605E5B68, 0x899324B1, 0x69279A5B
	# .long 0xA97F680B, 0x74744E01, 0x2095207A, 0xAC490328
	# .long 0x7679B0DB, 0x9421DA0C, 0x40B439DB, 0xD5899C0F
	# .long 0x72A746C3, 0x513CF488, 0x6DB2BDB2, 0x9AEFFEE1
	# .long 0xE00CBC3F, 0x7BA252CC, 0xAC4257F8, 0xCA0FBEB6
	# .long 0x8E98E2BC
	
	# 4 on cold
	# .long 0x8E98E2BC, 0x605E5B68, 0x899324B1, 0x69279A5B
	# .long 0xA97F680B, 0x74744E01, 0x2095207A, 0xAC490328
	# .long 0x7679B0DB, 0x9421DA0C, 0x40B439DB, 0xD5899C0F
	# .long 0x72A746C3, 0x513CF488, 0x6DB2BDB2, 0x9AEFFEE1
	# .long 0xE00CBC3F, 0x7BA252CC

	# 4 on cold, no 601 pathch
	# .long 0x8E98E2BC, 0x605E5B68, 0x899324B1, 0x69279A5B
	# .long 0xA97F680B, 0x74744E01, 0x2095207A, 0xAC490328
	# .long 0x7679B0DB, 0x9421DA0C, 0x40B439DB, 0xD5899C0F
	# .long 0x72A746C3, 0x513CF488, 0x6DB2BDB2, 0x9AEFFEE1
	# .long 0xE00CBC3F

	# .long 0x8E98E2BC, 0x605E5B68, 0x899324B1, 0x69279A5B
	# .long 0xA97F680B, 0x74744E01, 0x2095207A, 0xAC490328
	# .long 0x7679B0DB, 0x9421DA0C, 0x40B439DB, 0xD5899C0F


# 9:


#;============================================================================
	.long 0xffffffff
	.end
#;============================================================================
