.include "../macros.S"
.include "patches_offsets.S"

#————————————————————————————————————————————————————————————————————————————E
#   Patches for Hypervisor and Kernel 17150                                   
#————————————————————————————————————————————————————————————————————————————E
	# b destination-((.-0b)+source)

	.globl _start
_start:

#============================================================================
#   Send Dot Character            li      r3, 0x200                          
#============================================================================
	MAKEPATCH 0x00001880
	.set PrintDotDest, 0x000002D8 # build_68_01f50000
0:
	bla      PrintDot                # patches
9:

.ifndef GLITCH
	#============================================================================
	#   Change Thread Hold Address      addis   r4, r3, -3                       
	#============================================================================
		MAKEPATCH 0x00000C60
		.set FixHoldAddrDone, (PatchAddr + (9f - 0f))
	0:
		ba      FixHoldAddr
	9:
.endif # GLITCH

.ifdef VFUSE
	.ifdef GLITCH

		#============================================================================
		#   force glitch mfg to use 0x0 cpu key for cbb as the mfg flag is removed   
		#============================================================================
		MAKEPATCH 0x000071EC
		0:
			nop
		9:
		
		.endif # GLITCH

	#============================================================================
	#   Setup Fuse Row Pointer to point at Virtual Fuses                         
	#   Change Increment and Range Check of Fuse Row Pointer                     
	#============================================================================
		MAKEPATCH 0x00001D10
	0:
		bla     FixFusesAddr
	9:
	#--------------------------------------
		MAKEPATCH 0x00001D24  # addi    r11, r11, 0x40
	0:
		addi	r11, r11, 1
		cmplwi	cr6, r11, 0xC
	9:
		# ——————————————————————————————————————————————————————————
			# MAKEPATCH 0x00008170 # will need update for processor version >= 0x710900
		# 0:
			# bla     FixFusesAddr
		# 9:
	#——————————————————————————————————————————————————————————
		MAKEPATCH 0x00009778
	0:
		bla     FixFusesAddr
	9:
	#--------------------------------------
		MAKEPATCH 0x000097A8  # addi    r11, r11, 0x40
	0:
		addi    r11, r11, 0x0001
		cmplwi  cr6, r11, 0x000c
	9:
	#——————————————————————————————————————————————————————————
		MAKEPATCH 0x00009918
	0:
		bla     FixFusesAddr
	9:
	#--------------------------------------
		MAKEPATCH 0x00009948  # addi    r11, r11, 0x40
	0:
		addi    r11, r11, 0x0001
		cmplwi  cr6, r11, 0x000c
	9:
	#——————————————————————————————————————————————————————————
		MAKEPATCH 0x00009A68
	0:
		bla     FixFusesAddr
	9:
	#--------------------------------------
		MAKEPATCH 0x00009A98  # addi    r11, r11, 0x40
	0:
		addi    r11, r11, 0x0001
		cmplwi  cr6, r11, 0x000c
	9:
	#——————————————————————————————————————————————————————————
		MAKEPATCH 0x00009D08
	0:
		bla     FixFusesAddr
	9:
	#--------------------------------------
		MAKEPATCH 0x00009D38   # addi    r11, r11, 0x40
	0:
		addi    r11, r11, 0x0001
		cmplwi  cr6, r11, 0x000c
	9:
	#——————————————————————————————————————————————————————————
		MAKEPATCH 0x0000A078
	0:
		bla     FixFusesAddr
	9:
	#--------------------------------------
		MAKEPATCH 0x0000A0A8  # addi    r11, r11, 0x40
	0:
		addi    r11, r11, 0x0001
		cmplwi  cr6, r11, 0x000c
	9:
	#——————————————————————————————————————————————————————————
		MAKEPATCH 0x0000A138	# will need update for processor version >= 0x710900
	0:
		bla     FixFusesAddr
	9:
	#--------------------------------------
		MAKEPATCH 0x0000A168  # addi    r11, r11, 0x40
	0:
		addi    r11, r11, 0x0001
		sldi	r9, r9, 3 		# rldicr  r9, r9, 3,60		# no change here
		cmpwi   cr6, r11, 0x000c
	9:
	#——————————————————————————————————————————————————————————
		MAKEPATCH 0x0000A470
	0:
		bla     FixFusesAddr
	9:
	#--------------------------------------
		MAKEPATCH 0x0000A49C  # addi    r11, r11, 0x40
	0:
		addi    r11, r11, 0x0001
		sldi	r9, r9, 3 		# rldicr  r9, r9, 3,60		# no change here
		cmpwi   cr6, r11, 0x000c
	9:
.endif # VFUSE

#============================================================================
# pbkey xex2key ensure all zeroes                                            
#============================================================================
	MAKEPATCH xex2devkey
0:
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
9:

#============================================================================
#   hv tlb_rpn set command location       mtspr   SPR_TLB_RPN, sp           
#============================================================================
	MAKEPATCH 0x000011BC
	.set PatchTlbRpnDone, (PatchAddr + (9f - 0f))
0:
	ba      PatchTlbRpn
9:

#============================================================================
#   Patch TLB RPN to Remove Protections                                      
#                                                                            
#     - allows code excution anywhere in 0x80000000->0x9FFFFFFF range        
#     - allows system thread manipulation of code/read only data without     
#       disabling encryption                                                 
#     - 0x0007 clears bits NoExec:DataPage:Readonly of RPN                   
#============================================================================
	MAKEPATCH 0x0000154C
	.set PatchTlbRpn, PatchAddr
0:
	li      r4, 0x0007             # removes all page protection
	andc    sp, sp, r4
	mtspr   TLB_RPN, sp            # instruction replaced to get here
	ba      PatchTlbRpnDone
9:

#============================================================================
#   disable LDV check                                                        
#============================================================================
	MAKEPATCH 0x00003120
0:
	nop
9:

#============================================================================
# disable HvxBlowFuses                                                       
#============================================================================
	MAKEPATCH 0x0000A560
0:
	li		r3, 1
	blr
9:

#============================================================================
# HvxLoadImageData - ignores return of SHA memdiff (for devkit xex)          
#============================================================================
	MAKEPATCH 0x0002A30C
0:
	.long 0x60000000 # nop cmpwi   cr6, r3, 0
	.long 0x60000000 # nop bne     cr6, loc_
9:

#============================================================================
#HvxResolveImports - removes min version check in hv  (0xC0000059)           
#00004980 blt     cr6, version_mismatch                                      
#0000498C blt     cr6, version_mismatch                                      
#============================================================================
	MAKEPATCH 0x0002AA80
0:
	.long 0x60000000 # nop
9:
#--------------------------------------
	MAKEPATCH 0x0002AA8C
0:
	.long 0x60000000 # nop
9:

# .set DEBUG_OUT, 1
.ifdef DEBUG_OUT
	.print "****DEBUG OUT"
	#============================================================================
	#   puts link register to UART                         
	#============================================================================
		MAKEPATCH 0x00071EC
	0:
		ba      PrintDot2                # patches
	9:
.endif

#============================================================================
#   Utility Functions                                                        
#============================================================================
	MAKEPATCH 0x0000B4F8
	.set PrintDot, PatchAddr
.ifdef DEBUG_OUT
	.set PrintDot2, (PatchAddr + (4f - 0f))
.endif

.ifdef VFUSE
	.set FixFusesAddr, (PatchAddr + (1f - 0f))
	# .set VfuseAddr, (PatchAddr + (8f - 0f))
	# .set VfuseAddr, 0xFFA0 # (PatchAddr + (9f - 0f))
	.ifndef GLITCH
		.set FixHoldAddr, (PatchAddr + (2f - 0f))
		.set nandFuses, 0x00095000 # hard coded jtag NAND offset
	.endif
.endif
.ifdef _PEEK_POKE
	.set PatchHvPte, (PatchAddr + (6f - 0f))
	.set PatchHvVals, ((PatchAddr + (7f - 0f))-4)
.endif
	.set PrintChar, (PatchAddr + (3f - 0f))
	.set TogglePublicKey, (PatchAddr + (5f - 0f))

	.set dwSysUpdateAddr,		0x64
	.set dwSysUpdateSlotSize,	0x70
	.set fusesSize,				0xC
	.set sc_zero_key, 			0x72627472 # rbtr
	.set sc_zero_org, 			0x00001CC8 # original function for syscall 0 HvxGetVersions
	.set sc_zero_tent, 			0x00015E60 # address of syscall 0 function entry
	.set HvpGetFlashBaseAddress,0x0000068C # builds 0x80000200_C8000000 in r3
	.set HvpGetSocMMIORegs, 	0x000006A0 # adds 0x80000200_00000000 to r3
	.set XeCryptCopyQwVec, 		0x00000484 # XeCryptCopyQwVec(void* dest, void* src, int iter) ; len is in number of 64bit reads, uses r3-r7
	.set HvpMemCpy,				0x0000A880 #  memcpy(void *destination, const void *source, int len)

#——————————————————————————————————————————————————————————
# PrintDot                                                 
#——————————————————————————————————————————————————————————
0:
	mflr	r8
.ifdef FATGLITCH
	li		r3, -0x2FE2  		# remove mfg mode C01E|1000 or C01E|2000 0xD01E(slot1) 0xE01E(slot2)
	sth     r3, BldrFlags(0)
.else
	lhz     r3, BldrFlags(0)
	li		r4, 0x21
	andc	r3, r3, r4
	sth     r3, BldrFlags(0)
.endif

.ifdef VFUSE
	#——————————————————————————————————————————————————————————
	# copy VFUSE to hv space                                  
	#——————————————————————————————————————————————————————————
		bla		HvpGetFlashBaseAddress
	.ifdef GLITCH # for mfg glitch, copy directly from flash
		lwz		r4, dwSysUpdateAddr(r3)
		lwz		r5, dwSysUpdateSlotSize(r3)
		add		r3, r3, r4
		add		r4, r3, r5
	.else # for jtag copy from flash
		oris	r4, r3, nandFuses@h
		ori		r4, r4, nandFuses@l # source
	.endif # GLITCH
		lis		r3, 1
		subi	r3, r3, 0x60
		# lis		r3, VfuseAddr@h		# destination
		# ori		r3, r3, VfuseAddr@l
		li		r5, fusesSize
		bla		XeCryptCopyQwVec
.endif # VFUSE

	li      r3, '!'
	bla     PrintChar

	li      r3, '\n'
	bla     PrintChar

	mtlr	r8
	ba      PrintDotDest

.ifdef VFUSE
	#——————————————————————————————————————————————————————————
	# FixFusesAddr                                             
	#——————————————————————————————————————————————————————————
	1:
		lis		r3, 1
		subi	r3, r3, 0x60
		# lis		r3, VfuseAddr@h
		# ori		r3, r3, VfuseAddr@l
		blr

	.ifndef GLITCH
		#——————————————————————————————————————————————————————————
		# FixHoldAddr                                              
		#——————————————————————————————————————————————————————————
		2:
			lis     r4, 0x8000
			rldicr  r4, r4, 32, 31
			oris    r4, r4, 0x100
			ori     r4, r4, 0x3080 	# 0x80000000_01003080 - had to patch freeboot.bin
			ba      FixHoldAddrDone
	.endif #GLITCH
.endif # VFUSE

#——————————————————————————————————————————————————————————
# PrintChar                                                
#——————————————————————————————————————————————————————————
3:
	lis     r4, 0x8000
	ori     r4, r4, 0x0200
	rldicr  r4, r4, 32, 31
	oris    r4, r4, 0xea00
	slwi    r3, r3, 24
	stw     r3, 0x1014(r4)
uartwaitloop:
	lwz     r3, 0x1018(r4)
	rlwinm. r3, r3, 0, 6, 6
	beq     uartwaitloop
	blr

.ifdef DEBUG_OUT
	#——————————————————————————————————————————————————————————
	# PrintDot2                                                
	#——————————————————————————————————————————————————————————
	4:
		mflr	r5
		li		r3, ':'
		bl		3b
		li		r6, 0
	loopydbg1:
		rldicl	r5, r5, 4, 0				# 64bit
		rlwinm	r3, r5, 0, 28, 31 		# 64bit
		#rlwinm	r5, r5, 4, 0, 31		# 32bit
		#rlwinm	r3, r5, 0, 28, 31 # 28	# 32bit
		bl		hex2asciidbg
		addi	r6, r6, 1
		cmpwi	r6, 16						# 64bit
		#cmpwi		r6, 8						# 32bit
		blt		loopydbg1
		
		li		r3, '\n' # linefeed
		bl		3b
	deathdbg:
		b		deathdbg
	hex2asciidbg:
		addi	r3, r3, 0x30
		cmpwi	r3, 0x3A
		bge		addonedbg
		b		3b
	addonedbg:
		addi	r3, r3, 0x7
		b		3b
.endif

#——————————————————————————————————————————————————————————
# TogglePublicKey, ToggleTlbProtection, LoadBinHv          
# r3 = key sc_zero_key                                     
# r4 = function                                            
# r5 = dest/entry (loadbin only)                           
# r6 = src (loadbin only)                                  
# r7 = lenInU32 (loadbin only)                             
#——————————————————————————————————————————————————————————
5:
	lis		r11, sc_zero_key@h
	ori		r11, r11, sc_zero_key@l
	cmplw	cr6, r3, r11
	beq		cr6, key_found
	ba		sc_zero_org
key_found:
	cmplwi  cr6, r4, 4			# xell loader
	bgt		cr6, peek_poke		# peek and poke api
	beq		cr6, bin_launch
	li      r5, PatchTlbRpn	# offset to tlb mod code
	lis     r6, 0x3880			# creating a li inst
protectOffTest:
	cmplwi  cr6, r4, 2			# 2 = protection off
	bne		cr6, protectOnTest
	ori     r6, r6, 0x0007	# 0x38800007 = li r4, 0x07
	b       applykey
protectOnTest:
	cmplwi  cr6, r4, 3			# 3 = protection on 0x38800000 = li r4, 0
	bne 	cr6, keyret
applykey:
	li      r0, 0
	stw     r6, 0(r5)
	dcbst   r0, r5
	icbi    r0, r5
	sync
	isync
keyret:
	li		r3, 1				# just a dummy return
	blr
bin_launch:
	mflr	r12  # stow on stack
	std		r12, -0x8(sp)
	stdu    sp, -0x10(sp)
	mtlr 	r5
	mtctr   r7
	# Copy src = r6, dst = r7
lcopyloop:
	lwz     r4, 0(r6)
	stw     r4, 0(r5)
	dcbst   r0, r5	
	icbi	r0, r5
	sync	0
	isync
	addi    r5, r5, 4
	addi    r6, r6, 4
	bdnz    lcopyloop
	blr
	addi	sp, sp, 0x10
	ld		r12, -0x8(sp)
	mtlr	r12	#loaded from stack
	blr
.ifndef _PEEK_POKE
#——————————————————————————————————————————————————————————
# peek(DWORD key, DWORD type, DWORD SourceAddress, DWORD DestAddress, DWORD lenInBytes) -> memcpy(void *destination, const void *source, int len)
#——————————————————————————————————————————————————————————
peek_poke:		# peek and poke api
	cmplwi  cr6, r4, 5			# peek poke
	bne		cr6, pp_ver
	mr		r3, r6
	mr		r4, r5
	mr		r5, r7
	ba 		HvpMemCpy
pp_ver:
	li		r3, 2
	blr
.else
#——————————————————————————————————————————————————————————
# peek_poke(DWORD key, DWORD type, DWORD SourceAddress, DWORD DestAddress, DWORD Size)
#——————————————————————————————————————————————————————————
peek_poke:		# peek and poke api
	# Setup our stack
	mflr	r12
	std		r30, -0x18(sp)
	std		r31, -0x10(sp)
	stw		r12, -0x8(sp)
	stwu	sp, -0x100(sp)

	# Save our type before we overwrite, fix disparity from original code
	subi	r30, r4, 5
	
	# Check to see if its Peek
	cmplwi	cr6, r30, 0
	beq		cr6, PeekBYTE
	cmplwi	cr6, r30, 1
	beq		cr6, PeekWORD
	cmplwi	cr6, r30, 2
	beq		cr6, PeekDWORD
	cmplwi	cr6, r30, 3
	beq		cr6, PeekQWORD
	cmplwi	cr6, r30, 10
	beq		cr6, PeekSPR
	cmplwi	cr6, r30, 11
	beq		cr6, PokeSPR

	# Check to see if its Poke
	cmplwi	cr6, r30, 5
	beq		cr6, PokeBYTE
	cmplwi	cr6, r30, 6
	beq		cr6, PokeWORD
	cmplwi	cr6, r30, 7
	beq		cr6, PokeDWORD
	cmplwi	cr6, r30, 8
	beq		cr6, PokeQWORD

	# Its a invalid type if its >= 11
	cmplwi	cr6, r30, 12
	bge		cr6, Done

	# Its either peek or poke data so lets get our dest address
	# Get function "HvpPhysicalToReal"
	lwz		r31, 0x4C(0)
	add		r31, rtoc, r31
	lwz		r31, 0x08(r31)
	mtctr	r31

	# Call HvpPhysicalToReal
	mr		r3, r6
	li		r4, 1
	bctrl
	mr		r6, r3
	mtctr	r7

	cmplwi	cr6, r30, 4
	beq		cr6, PeekBytes
	cmplwi	cr6, r30, 9
	beq		cr6, PokeBytes

	# We should never get here...
	b		Done

PeekSPR:
	bl 		PatchPeekSPR
PatchPeekSPR:
	mflr	r31
	mr		r6, r5
	lis		r4, 0x7C60
	rlwimi  r6, r5, 10, 17, 21
	ori		r4, r4, 0x2A6
	rlwimi  r4, r6, 6, 11, 20
	addi	r31, r31, (ReadSPR - PatchPeekSPR)
	stw		r4, 0(r31)
	dcbst	r0, r31
	icbi	r0, r31
	sync	0
	isync
ReadSPR:
	mfspr	r3, 313
	b		CleanUp

PokeSPR:
	bl 		PatchPokeSPR
PatchPokeSPR:
	mflr	r31
	mr		r3, r6
	mr		r6, r5
	lis		r4, 0x7C60
	rlwimi  r6, r5, 10, 17, 21
	ori		r4, r4, 0x3A6
	rlwimi  r4, r6, 6, 11, 20
	addi	r31, r31, (WriteSPR - PatchPokeSPR)
	stw		r4, 0(r31)
	dcbst	r0, r31
	icbi	r0, r31
	sync	0
	isync

WriteSPR:
	mtspr	313, r3
	b		CleanUp

	# This code is for peeking data
PeekBYTE:
	lbz		r3, 0(r5)
	b		CleanUp
PeekWORD:
	lhz		r3, 0(r5)
	b		CleanUp
PeekDWORD:
	lwz		r3, 0(r5)
	b		CleanUp
PeekQWORD:
	ld		r3, 0(r5)
	b		CleanUp
PeekBytes:
	lbz		r31, 0(r5)
	stb		r31, 0(r6)
	addi	r5, r5, 1
	addi	r6, r6, 1
	bdnz	PeekBytes
	b		Done

	# This code is for poking data
PokeBYTE:
	stb		r6, 0(r5)
	b		PokeFlush
PokeWORD:
	sth		r6, 0(r5)
	b		PokeFlush
PokeDWORD:
	stw		r6, 0(r5)
	b		PokeFlush
PokeQWORD:
	std		r6, 0(r5)
	b		PokeFlush
PokeBytes:
	lbz		r31, 0(r6)
	stb		r31, 0(r5)
	addi	r5, r5, 1
	addi	r6, r6, 1
	bdnz	PokeBytes
PokeFlush:
	dcbst	r0, r5
	icbi	r0, r5
	sync	0
	isync

Done:
	li		r3, 0

CleanUp:
	# Tear down our stack and restore registers
	addi	sp, sp, 0x100
	ld		r30, -0x18(sp)
	ld		r31, -0x10(sp)
	lwz		r12, -0x8(sp)
	mtlr	r12
	blr
6: # PatchHvPte patch PTE - r3 r4 r5 r8 r9
	stw		r3, 4(r11) # instruction replaced to get here
	li		r3, 0x68
	sldi	r3, r3, 32
	oris	r3, r3, 0x1F5 # 6801F50000
	addi	r3, r3, -4
	lis		r4, PatchHvVals@h
	ori		r4, r4, PatchHvVals@l
	li		r5, 4
	mtctr	r5
PtePatchLoop:
	lwzu	r5, 4(r4)
	stwu	r5, 4(r3)
	bdnz	PtePatchLoop
	blr
	
7: # PatchHvVals
	.long 0x4000001C, 0x4080004C, 0x4100008C, 0x418000CC # 4000001C4080004C4100008C418000CC
.endif # PEEK_POKE
# .ifdef VFUSE
# 8:
	# .long 0x11111111, 0x11111111, 0x11111111, 0x11111111
	# .long 0x11111111, 0x11111111, 0x11111111, 0x11111111
	# .long 0x11111111, 0x11111111, 0x11111111, 0x11111111
	# .long 0x11111111, 0x11111111, 0x11111111, 0x11111111
	# .long 0x11111111, 0x11111111, 0x11111111, 0x11111111
	# .long 0x11111111, 0x11111111, 0x11111111, 0x11111111
# .endif
9:

.ifdef _PEEK_POKE
	#——————————————————————————————————————————————————————————
	# patch tlb to map hv to kernel space                      
	#——————————————————————————————————————————————————————————
		MAKEPATCH 0x00001A04
	0:
		bla 	PatchHvPte
	9:
.endif # PEEK_POKE

#——————————————————————————————————————————————————————————
# patch syscall 0 to jump to hook                          
#——————————————————————————————————————————————————————————
	MAKEPATCH sc_zero_tent
0:
	.long TogglePublicKey
9:

#============================================================================
# HvxSecuritySetDetected                                                    
#============================================================================
	MAKEPATCH 0x00006BB0
0:
	li      r3, 0
	blr
9:

#============================================================================
# HvxSecurityGetDetected                                                     
#============================================================================
	MAKEPATCH 0x00006C48
0:
	li      r3, 0
	blr
9:

#============================================================================
# HvxSecuritySetActivated                                                    
#============================================================================
	MAKEPATCH 0x00006C98
0:
	li      r3, 0
	blr
9:

#============================================================================
# HvxSecurityGetActivated                                                    
#============================================================================
	MAKEPATCH 0x00006D08
0:
	li      r3, 0
	blr
9:

#============================================================================
# XeKeysSecuritySetStat    HvxSecuritySetStat                                
#============================================================================
	MAKEPATCH 0x00006D58
0:
	li      r3, 0
	blr
9:

#============================================================================
# don't limit what keys can be gotten from hv         search 0x1150010       
#============================================================================
	MAKEPATCH 0x0000813C
	.set destAddr, 0x0000816C # this is the second branch addy not the first!!
0:
	MAKEBRANCH destAddr
9:

#============================================================================
#   disable CBA sig validation                                               
#============================================================================
	MAKEPATCH 0x000070BC
0:
	li		r3, 1
9:

#============================================================================
#   disable CBB hash memcmp                                                  
#============================================================================
	MAKEPATCH 0x00007268
0:
	li		r3, 0
9:

#============================================================================
#   disable machine check on bConsoleType                                    
#============================================================================
	MAKEPATCH 0x000072B4
0:
	nop
9:
#============================================================================
#   disable machine check on bConsoleSequence                                
#============================================================================
	MAKEPATCH 0x000072C4
0:
	nop
9:

#============================================================================
#   disable machine check on wConsoleSequenceAllow, force console to detect as retail                          
#============================================================================
	MAKEPATCH 0x000072EC
0:
	nop
	li		r11, 1
9:

#============================================================================
# locked memcmp Run GOD Content                                              
#============================================================================
	MAKEPATCH 0x00024D58
0:
	li      r3, 1
	blr
9:

#;============================================================================
#;   264F0 bla HvpPkcs1Verify - disable fcrt signature check                  
#;============================================================================
	MAKEPATCH 0x000264F0
0:
	li		r3, 1
9:

#============================================================================
# Patch signature check and load as retail if invalid (Ant)                  
#============================================================================
	MAKEPATCH 0x00029B08
	.set HvCryptAesKey, 0x200F8
0:
	# Check if AES Ctx is NULL
	cmpldi  cr6, r28, 0		# first instruction after sig check
	beq     cr6, xex_ret_zero

	# Check to see if signature was valid
	cmpwi   cr6, r3, 0
	bne     cr6, xex_proc_key

	# Devkit signature invalid so load as retail
	li      r4, xex2devkey
	b       xex_setkey
	nop
xex_proc_key:
	cmplwi	cr6, r29, 0
	addi	r4, r31, 0x440
	bne		cr6, xex_setkey
	li		r4, xex2key   # pbKey
xex_setkey:
	mr		r3, r28       # pAesState
	MAKEBRANCHL HvCryptAesKey
xex_ret_zero:
	li		r31, 0			# original instruction, make sure this lines up
9:

#============================================================================
#   HvxImageTransformImageKey remove type check                              
#============================================================================
	MAKEPATCH 0x0002B770
0:
	nop
9:

#============================================================================
# needed when patching out revoke Check from xex image header in xam     
# HvxCreateImageMapping 0xC8003004                                           
#============================================================================
	MAKEPATCH 0x0002C088
0:
	li		r3, 0
9:

#============================================================================
# HvxCreateImageMapping - Remove XEX Region Check 0xC0050001                 
#============================================================================
	MAKEPATCH 0x0002C378
0:
	nop
9:

#============================================================================
# HvxExpansionInstall - if sig check fails pass it anyway, but don't use cpukey crypto
#============================================================================
	MAKEPATCH 0x0003089C
0:
    bne       cr6, signedExp        # beq       cr6, return_C8007003
    li        r29, 0                # lwz       r11, 8(r31)
signedExp:
    nop                            # cmplw     cr6, r11, r28
    nop                            # bgt       cr6, return_C8007003
9:

#============================================================================
# HvxExpansionInstall do_expansion_install check                             
#============================================================================
	MAKEPATCH 0x000304E8
0:
	nop
9:

#============================================================================
# HvxExpansionInstall do_expansion_install check                             
#============================================================================
	MAKEPATCH 0x000304FC
0:
	nop
9:

#===========================================================================================================
#===========================================================================================================
# 								START KERNEL PATCHES														 
#===========================================================================================================
#===========================================================================================================

#============================================================================
# XexpConvertError - converts revoke all errors into OK                      
#============================================================================
	KMAKEPATCH XexpConvertError
0:
	li		r3, 0
	blr
9:

#============================================================================
# XexpLoadXexHeaders/XexpVerifyMediaType - Disable Media Flags Check         
#============================================================================
	KMAKEPATCH XexpLoadXexHeadersPatch
0:
	li		r3, 1
9:

#============================================================================
# XexpLoadFile - skip 0xC00000CC (BAD_NETWORK_NAME) error                    
# bl        RtlImageXexHeaderString                                          
#============================================================================
	KMAKEPATCH BadNetworkNamePatch
0:
	li     r3, 0
9:

#============================================================================
# XexpLoadFile - skip 0xC000008B (resource not found) error                  
# lwz     r11, 0x4C(r25) # 0x40000000 bound path flag                      
#============================================================================
	KMAKEPATCH ResourceNotFound1Patch
0:
	li     r11, 0
9:

#============================================================================
# XexpLoadFile - skip 0xC000008B (resource not found) error                  
# lwz     r11, 0x4C(r25) # 0x20000000 bound device flag                    
#============================================================================
	KMAKEPATCH ResourceNotFound2Patch
0:
	li     r11, 0
9:

#============================================================================
# XexpVerifyMinimumVersion - 0xC0000059 revision mismatch                    
#============================================================================
	KMAKEPATCH XexpVerifyMinimumVersion
0:
	li		r3, 0
	blr
9:

#============================================================================
# fix sfcxmu corruption issues on jasper bigblock - use 008 or higher for retail
# SfcxInspectLargeDataBlock 80091C28 lbz r23, 7(r30) # r23 = kernelresv blocks
#============================================================================
	KMAKEPATCH SfcxFixBBSizePatch
0:
	li		r23, 0x10
9:

#============================================================================
# SataCdRomAuthenticationExInitialize - disables AuthEx/AP25 on supp. drives 
#============================================================================
	KMAKEPATCH SataCdRomAuthenticationExInitializePatch
0:
	cmplwi  cr6, r11, 0xFF # originally checking XEIKA drive version !< 1
9:

#============================================================================
# prevent E66 with XGD3 on some drives                                       
#============================================================================
	KMAKEPATCH E66xgd3
0:
	li		r3, 0
	nop
	nop
	nop
	nop
9:

#============================================================================
# * XeKeysVerifyRSASignature Disable PIRS/LIVE Signature Check               
#============================================================================
	KMAKEPATCH XeKeysVerifyRSASignature
0:
	li      r3, 1
	blr
9:

#============================================================================
# XeKeysSecurityConvertError										         
#============================================================================
	KMAKEPATCH XeKeysSecurityConvertError
0:
	li 		r3, 0
	blr
9:

#============================================================================
# XeKeysDvdAuthExConvertError										         
#============================================================================
	KMAKEPATCH XeKeysDvdAuthExConvertError
0:
	li 		r3, 0
	blr
9:

#============================================================================
# _XeKeysRevokeIsValid                                                       
#============================================================================
	KMAKEPATCH _XeKeysRevokeIsValid
0:
	li      r3, 1
	blr
9:

#============================================================================
#  XeKeysRevokeIsRevoked                                                     
#============================================================================
	KMAKEPATCH XeKeysRevokeIsRevoked
0:
	li      r3, 0
	blr
9:

#============================================================================
# _XeKeysRevokeIsRevoked (for dynamicUpdate)first call in XeKeysRevokeIsRevoked
#============================================================================
	KMAKEPATCH XeKeysRevokeIsRevokedp # MANUAL
0:
	li      r3, 0
	blr
9:

#============================================================================
# * XeKeysRevokeIsDeviceRevoked - usb device revocation (natal needs)        
#============================================================================
	KMAKEPATCH XeKeysRevokeIsDeviceRevoked
0:
	li 		r3, 0
	blr
9:

#============================================================================
# XeKeysRevokeConvertError - converts revoke all errors into OK              
#============================================================================
	KMAKEPATCH XeKeysRevokeConvertError
0:
	li 		r3, 0
	blr
9:

#============================================================================
# XeKeysConsoleSignatureVerification - skip sig check, make all appear local 
#============================================================================
	KMAKEPATCH XeKeysConsoleSignatureVerification
0:
	cmplwi	cr6, r5, 0
	li 		r3, 1
	beq		cr6, 1f
	stw		r3, 0(r5)
1:
	blr
9:

#============================================================================
# * XeCryptBnQwBeSigVerify needed for LIVE/PIRS execution                    
#============================================================================
	KMAKEPATCH XeCryptBnQwBeSigVerify
0:
	li      r3, 1
	blr
9:

#;============================================================================
# MassConfigureTransferCable Disable Harddisk Authentication
#;============================================================================
	KMAKEPATCH xferMassConfigureTransferCable1
0:
	KMAKEBRANCH xferMassConfigureTransferCable1dest
9:

#;============================================================================
# MassConfigureTransferCable Disable HDD size from SS
#;============================================================================
	KMAKEPATCH xferMassConfigureTransferCable2
0:
	lwz       r26, 0x98(r28)
	nop
	nop
9:

#============================================================================
# SataDiskAuthenticateDevice Disable Harddisk Authentication                 
#============================================================================
	KMAKEPATCH SataDiskAuthenticateDevice
0:
	li      r3, 1
	blr
9:

#============================================================================
#  		Kernel patches for launch.xex and devkit key switching            
#============================================================================

#============================================================================
# start launch.xex if present                                                
#============================================================================
	.set XamLoadedHook, (XamLoadedHookPatch-KBASE) 	# Phase1Initialization # MANUAL
	# KMAKEPATCH (SataDiskAuthenticateDevice+8) # approx 0x12C space available, at 0x84 now = A8 (42) remaining (before NicAttachDest)
	KMAKEPATCH (XeKeysConsoleSignatureVerification+0x20) # approx 0x180 space available
	.set HookDest, PatchAddr
	.set XexLoadImagePhy, (XexLoadImage-KBASE) 	# * XexLoadImage
	.set LoadExecDest, (RealAddr+(1f-0f))
	.set XeKeysPropDest, (RealAddr+(2f-0f))
	.set HookFileName, (RealAddr+(3f-0f))
	.set LoadExecWaitVar, (RealAddr+(4f-0f))
0:
	bge     cr6, loadmod            # once xam loads successfully, try and start another module from hdd
	blr                             # if xam fails to load, then let the normal process of recording argon
									# error and showing E7x screen commence
loadmod:
	lis     r3, HookFileName@h
	lis     r5, 0x0                # minver
	li      r4, 0x0
	ori     r4, r4, 8             # mod flag
	ori    	r3, r3, HookFileName@l    # address of path to xex dll
	li      r6, 0
	MAKEBRANCHL XexLoadImagePhy
	li		r3, 0					# just in case!
	lis     r4, LoadExecWaitVar@h
	ori    	r4, r4, LoadExecWaitVar@l
	isync
	stw		r3, 0(r4)
	MAKEBRANCH (XamLoadedHook+0x10)
1: # LoadExecDest
	addi 	r5, sp, 0x54 	# instruction replaced to get here
#	b		startWait		# enable this if another wait is added
startWait:
	# b 		doPowerWait
	lis     r7, LoadExecWaitVar@h
	ori    	r7, r7, LoadExecWaitVar@l
powerWait:
	lwz		r8, 0(r7)
	isync
	cmplwi	cr6, r8, 0
	beq		cr6, exitFinal
doPowerWait:
	db16cyc
	b		powerWait
exitFinal:
	blr
2: # XeKeysPropDest, do not use r3/r4
	cmplwi 	cr6, r3, 0x14
	bne		cr6, exitFinal2
	lis     r7, LoadExecWaitVar@h
	ori    	r7, r7, LoadExecWaitVar@l
powerWait2:
	lwz		r8, 0(r7)
	isync
	cmplwi	cr6, r8, 0
	beq		cr6, exitFinal2
	db16cyc
	b		powerWait2
exitFinal2:
	KMAKEBRANCH 	HvxKeysGetKeyProperties_2fPatch
3: # HookFileName
	#.string "\\Device\\Harddisk0\\Partition1\\launch.xex"
	.string "\\Device\\Flash\\launch.xex"
	.byte 0
	.align 2 
4: # LoadExecWaitVar
	.long	0x12345678
9:# uses less than 0xD4 bytes of 0x130 bytes

#============================================================================
# the instruction following xam loading in phase1init (mr r31, r3)           
#============================================================================
	MAKEPATCH XamLoadedHook	# NOTE this one is not kpatch
0:
	MAKEBRANCHL HookDest
9:

#============================================================================
# XexLoadExecutable  addi r5, sp, 0x54                                       
# give launch.xex a chance to load, prevent other exe from loading           
#============================================================================
	KMAKEPATCH XexLoadExecutablePatch
0:
	KMAKEBRANCHL LoadExecDest
9:

#============================================================================
# XeKeysGetKeyProperties                                                      
#============================================================================
	KMAKEPATCH XeKeysGetKeyProperties
0:
	KMAKEBRANCH	XeKeysPropDest
9:

#============================================================================
	.long 0xffffffff
	.end
#============================================================================

