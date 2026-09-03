.include "../macros.S"

#;—————————————————————————————————————————————————————————————————————————————
#;   Patches for Hypervisor and Kernel 13146                                   
#;—————————————————————————————————————————————————————————————————————————————
	#; b destination-((.-0b)+source)

	.globl _start
_start:

#;============================================================================
#;   Send Dot Character            li      %r3, 0x200                         
#;============================================================================
	MAKEPATCH 0x00001880
	.set PrintDotDone, (PatchAddr + (9f - 0f))
0:
	ba      PrintDot                #; patches
9:

#;============================================================================
#;   Change Thread Hold Address      addis   %r4, %r3, -3                     
#;============================================================================
	MAKEPATCH 0x00000C60
	.set FixHoldAddrDone, (PatchAddr + (9f - 0f))
0:
	ba      FixHoldAddr
9:

#;============================================================================
#;   Setup Fuse Row Pointer to point at Virtual Fuses                         
#;   Change Increment and Range Check of Fuse Row Pointer                     
#;============================================================================
	MAKEPATCH 0x00001D0C 
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00001D24  #; addi    %r11, %r11, 0x40
0:
	addi	%r11, %r11, 1
	cmplwi	cr6, %r11, 0xC
9:
#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x0000894C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00008980  #; addi    %r11, %r11, 0x40
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:
#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x00008ABC
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00008AF0  #; addi    %r11, %r11, 0x40
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:
#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x00008BDC
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00008C10  #; addi    %r11, %r11, 0x40
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:
#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x00008E4C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00008E80   #; addi    %r11, %r11, 0x40
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:
#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x00008F9C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00008FD0  #; addi    %r11, %r11, 0x40
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:
#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x0000905C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x0000908C  #; addi    %r11, %r11, 0x40
0:
	addi    %r11, %r11, 0x0001
	rldicr  %r9, %r9, 3,60		#; no change here
	cmpwi   cr6, %r11, 0x000c
9:
#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x000092FC
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x0000932C  #; addi    %r11, %r11, 0x40
0:
	addi    %r11, %r11, 0x0001
	rldicr  %r9, %r9, 3,60		#; no change here
	cmpwi   cr6, %r11, 0x000c
9:

#;============================================================================
#; pbkey xex2key ensure all zeroes                                            
#;============================================================================
	MAKEPATCH xex2devkey
0:
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
9:

#;============================================================================
#;   hv tlb_rpn set command location       mtspr   SPR_TLB_RPN, %sp           
#;============================================================================
	MAKEPATCH 0x000011BC
	.set PatchTlbRpnDone, (PatchAddr + (9f - 0f))
0:
	ba      PatchTlbRpn
9:

#;============================================================================
#;   Patch TLB RPN to Remove Protections                                      
#;                                                                            
#;     - allows code excution anywhere in 0x80000000->0x9FFFFFFF range        
#;     - allows system thread manipulation of code/read only data without     
#;       disabling encryption                                                 
#;     - 0x0007 clears bits NoExec:DataPage:Readonly of RPN                   
#;============================================================================
	MAKEPATCH 0x0000154C
	.set PatchTlbRpn, PatchAddr
0:
	li      %r4, 0x0007             #; removes all page protection
	andc    %sp, %sp, %r4
	mtspr   TLB_RPN, %sp            #; instruction replaced to get here
	ba      PatchTlbRpnDone
9:

#;============================================================================
#; HvxLoadImageData - ignores return of SHA memdiff (for devkit xex)          
#;============================================================================
	MAKEPATCH 0x0002A094
0:
	.long 0x60000000 #; nop cmpwi   cr6, %r3, 0
	.long 0x60000000 #; nop bne     cr6, loc_
9:

#;============================================================================
#;HvxResolveImports - removes min version check in hv  (0xC0000059)           
#;00004980 blt     cr6, version_mismatch                                      
#;0000498C blt     cr6, version_mismatch                                      
#;============================================================================
	MAKEPATCH 0x0002A808
0:
	.long 0x60000000 # nop
9:
#;--------------------------------------
	MAKEPATCH 0x0002A814
0:
	.long 0x60000000 # nop
9:

#;.ifdef NOKV_HASH_NEW
#;============================================================================
#; HvpIsKeyVaultSignatureValid
#;============================================================================
#;	MAKEPATCH 0x00006980
#;0:
#;	li 		%r3, 1
#;	blr
#;9:

#;	MAKEPATCH 0x00006B3C
#;0:
#;	nop
#;9:
#;.endif

#;============================================================================
#;   Utility Functions                                                        
#;============================================================================
	MAKEPATCH 0x0000A344
	.set PrintDot, PatchAddr
	.set FixFusesAddr, (PatchAddr + (1f - 0f))
	.set FixHoldAddr, (PatchAddr + (2f - 0f))
	.set PrintChar, (PatchAddr + (3f - 0f))
	.set TogglePublicKey, (PatchAddr + (5f - 0f))

	.set sc_zero_key, 0x72627472 #; rbtr
	.set sc_zero_org, 0x00001CC8 #; original function for syscall 0
	.set sc_zero_tent, 0x00015700 #; address of syscall 0 function entry
	.set HvpMemCpy,				0x000096F0 #  memcpy(void *destination, const void *source, int len)

#;——————————————————————————————————————————————————————————
#; PrintDot                                                 
#;——————————————————————————————————————————————————————————
0:
.ifdef NOKV_HASH
	lhz     %r3, BldrFlags(0)
	li		%r4, 0x20
	andc	%r3, %r3, %r4
	sth     %r3, BldrFlags(0)
.endif
	li      %r3, '!'
	bla     PrintChar

	li      %r3, '\n'
	bla     PrintChar

	li      %r3, 0x0200
	ba      PrintDotDone

#;——————————————————————————————————————————————————————————
#; FixFusesAddr                                             
#;——————————————————————————————————————————————————————————
1:
	lis     %r3, 0x0002
	addi    %r3, %r3, -0x6500
	blr

#;——————————————————————————————————————————————————————————
#; FixHoldAddr                                              
#;——————————————————————————————————————————————————————————
2:
	lis     %r4, 0x8000
	rldicr  %r4, %r4, 32, 31
	oris    %r4, %r4, 0x100
	ori     %r4, %r4, 0x3080 	#; 0x80000000_01003080 - had to patch freeboot.bin
	ba      FixHoldAddrDone

#;——————————————————————————————————————————————————————————
#; PrintChar                                                
#;——————————————————————————————————————————————————————————
3:
	lis     %r4, 0x8000
	ori     %r4, %r4, 0x0200
	rldicr  %r4, %r4, 32, 31
	oris    %r4, %r4, 0xea00
	slwi    %r3, %r3, 24
	stw     %r3, 0x1014(%r4)
uartwaitloop:
	lwz     %r3, 0x1018(%r4)
	rlwinm. %r3, %r3, 0, 6, 6
	beq     uartwaitloop
	blr

#;——————————————————————————————————————————————————————————
#; TogglePublicKey, ToggleTlbProtection, LoadBinHv          
#; r3 = key sc_zero_key                                     
#; r4 = function                                            
#; r5 = dest/entry (loadbin only)                           
#; r6 = src (loadbin only)                                  
#; r7 = lenInU32 (loadbin only)                             
#;——————————————————————————————————————————————————————————
5:
	lis		%r11, sc_zero_key@h
	ori		%r11, %r11, sc_zero_key@l
	cmplw	cr6, %r3, %r11
	beq		cr6, key_found
	ba		sc_zero_org
key_found:
	cmplwi  cr6, %r4, 4			#; xell loader
	bgt		cr6, peek_poke		#; peek and poke api
	beq		cr6, bin_launch
	
	li      %r5, PatchTlbRpn	#; offset to tlb mod code
	lis     %r6, 0x3880			#; creating a li inst
protectOffTest:
	cmplwi  cr6, %r4, 2			#; 2 = protection off
	bne		cr6, protectOnTest
	ori     %r6, %r6, 0x0007	#; 0x38800007 = li %r4, 0x07
	b       applykey
protectOnTest:
	cmplwi  cr6, %r4, 3			#; 3 = protection on 0x38800000 = li %r4, 0
	bne 	cr6, keyret
applykey:
	li      %r0, 0
	stw     %r6, 0(%r5)
	dcbst   %r0, %r5
	icbi    %r0, %r5
	sync
	isync
keyret:
	li		%r3, 1				#; just a dummy return
	blr
bin_launch:
	mflr	%r12  #; stow on stack
	std		%r12, -0x8(%sp)
	stdu    %sp, -0x10(%sp)
	mtlr 	%r5
	mtctr   %r7
	#; Copy src = %r6, dst = %r7
lcopyloop:
	lwz     %r4, 0(%r6)
	stw     %r4, 0(%r5)
	dcbst   %r0, %r5	
	icbi	%r0, %r5
	sync	0
	isync
	addi    %r5, %r5, 4
	addi    %r6, %r6, 4
	bdnz    lcopyloop
	blr
	addi	%sp, %sp, 0x10
	ld		%r12, -0x8(%sp)
	mtlr	%r12	#;loaded from stack
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
#;——————————————————————————————————————————————————————————
#; peek_poke(DWORD key, DWORD type, DWORD SourceAddress, DWORD DestAddress, DWORD Size)
#;——————————————————————————————————————————————————————————
peek_poke:		#; peek and poke api
	#; Setup our stack
	mflr	%r12
	std		%r30, -0x18(%sp)
	std		%r31, -0x10(%sp)
	stw		%r12, -0x8(%sp)
	stwu	%sp, -0x100(%sp)

	#; Save our type before we overwrite, fix disparity from original code
	subi	%r30, %r4, 5
	
	#; Check to see if its Peek
	cmplwi	cr6, %r30, 0
	beq		cr6, PeekBYTE
	cmplwi	cr6, %r30, 1
	beq		cr6, PeekWORD
	cmplwi	cr6, %r30, 2
	beq		cr6, PeekDWORD
	cmplwi	cr6, %r30, 3
	beq		cr6, PeekQWORD
	cmplwi	cr6, %r30, 10
	beq		cr6, PeekSPR
	cmplwi	cr6, %r30, 11
	beq		cr6, PokeSPR

	#; Check to see if its Poke
	cmplwi	cr6, %r30, 5
	beq		cr6, PokeBYTE
	cmplwi	cr6, %r30, 6
	beq		cr6, PokeWORD
	cmplwi	cr6, %r30, 7
	beq		cr6, PokeDWORD
	cmplwi	cr6, %r30, 8
	beq		cr6, PokeQWORD

	#; Its a invalid type if its >= 11
	cmplwi	cr6, %r30, 12
	bge		cr6, Done

	#; Its either peek or poke data so lets get our dest address
	#; Get function "HvpPhysicalToReal"
	lwz		%r31, 0x4C(0)
	add		%r31, %rtoc, %r31
	lwz		%r31, 0x08(%r31)
	mtctr	%r31

	#; Call HvpPhysicalToReal
	mr		%r3, %r6
	li		%r4, 1
	bctrl
	mr		%r6, %r3
	mtctr	%r7

	cmplwi	cr6, %r30, 4
	beq		cr6, PeekBytes
	cmplwi	cr6, %r30, 9
	beq		cr6, PokeBytes

	#; We should never get here...
	b		Done

PeekSPR:
	bl 		PatchPeekSPR
PatchPeekSPR:
	mflr	%r31
	mr		%r6, %r5
	lis		%r4, 0x7C60
	rlwimi  %r6, %r5, 10, 17, 21
	ori		%r4, %r4, 0x2A6
	rlwimi  %r4, %r6, 6, 11, 20
	addi	%r31, %r31, (ReadSPR - PatchPeekSPR)
	stw		%r4, 0(%r31)
	dcbst	%r0, %r31
	icbi	%r0, %r31
	sync	0
	isync
ReadSPR:
	mfspr	%r3, 313
	b		CleanUp

PokeSPR:
	bl 		PatchPokeSPR
PatchPokeSPR:
	mflr	%r31
	mr		%r3, %r6
	mr		%r6, %r5
	lis		%r4, 0x7C60
	rlwimi  %r6, %r5, 10, 17, 21
	ori		%r4, %r4, 0x3A6
	rlwimi  %r4, %r6, 6, 11, 20
	addi	%r31, %r31, (WriteSPR - PatchPokeSPR)
	stw		%r4, 0(%r31)
	dcbst	%r0, %r31
	icbi	%r0, %r31
	sync	0
	isync

WriteSPR:
	mtspr	313, %r3
	b		CleanUp

	#; This code is for peeking data
PeekBYTE:
	lbz		%r3, 0(%r5)
	b		CleanUp
PeekWORD:
	lhz		%r3, 0(%r5)
	b		CleanUp
PeekDWORD:
	lwz		%r3, 0(%r5)
	b		CleanUp
PeekQWORD:
	ld		%r3, 0(%r5)
	b		CleanUp
PeekBytes:
	lbz		%r31, 0(%r5)
	stb		%r31, 0(%r6)
	addi	%r5, %r5, 1
	addi	%r6, %r6, 1
	bdnz	PeekBytes
	b		Done

	#; This code is for poking data
PokeBYTE:
	stb		%r6, 0(%r5)
	b		PokeFlush
PokeWORD:
	sth		%r6, 0(%r5)
	b		PokeFlush
PokeDWORD:
	stw		%r6, 0(%r5)
	b		PokeFlush
PokeQWORD:
	std		%r6, 0(%r5)
	b		PokeFlush
PokeBytes:
	lbz		%r31, 0(%r6)
	stb		%r31, 0(%r5)
	addi	%r5, %r5, 1
	addi	%r6, %r6, 1
	bdnz	PokeBytes
PokeFlush:
	dcbst	%r0, %r5
	icbi	%r0, %r5
	sync	0
	isync

Done:
	li		%r3, 0

CleanUp:
	#; Tear down our stack and restore registers
	addi	%sp, %sp, 0x100
	ld		%r30, -0x18(%sp)
	ld		%r31, -0x10(%sp)
	lwz		%r12, -0x8(%sp)
	mtlr	%r12
	blr
.endif #; PEEK_POKE
9:

#;——————————————————————————————————————————————————————————
#; patch syscall 0 to jump to hook                          
#;——————————————————————————————————————————————————————————
	MAKEPATCH sc_zero_tent
0:
	.long TogglePublicKey
9:

#;============================================================================
#; Patch signature check and load as retail if invalid (Ant)
#;============================================================================
	MAKEPATCH 0x00029890
	.set HvCryptAesKey, 0x200F8
0:
	#; Check if AES Ctx is NULL
	cmpldi  cr6, %r28, 0		#; first instruction after sig check
	beq     cr6, xex_ret_zero

	#; Check to see if signature was valid
	cmpwi   cr6, %r3, 0
	bne     cr6, xex_proc_key

	#; Devkit signature invalid so load as retail
	li      %r4, xex2devkey
	b       xex_setkey
	nop
xex_proc_key:
	cmplwi	cr6, %r29, 0
	addi	%r4, %r31, 0x440
	bne		cr6, xex_setkey
	li		%r4, xex2key   # pbKey
xex_setkey:
	mr		%r3, %r28       # pAesState
	MAKEBRANCHL HvCryptAesKey
xex_ret_zero:
	li		%r31, 0			#; original instruction, make sure this lines up
9:

#;============================================================================
#; needed when patching out Revocation Check from xex image header in xam     
#; HvxCreateImageMapping 0xC8003004                                           
#;============================================================================
	MAKEPATCH 0x0002BD68
0:
	li		%r3, 0
9:

#;============================================================================
#; HvxCreateImageMapping - Remove XEX Region Check                            
#;============================================================================
	MAKEPATCH 0x0002C000
0:
	nop
9:

#;============================================================================
#; HvxSecuritySetDetected                                                     
#;============================================================================
	MAKEPATCH 0x000060E8
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#; HvxSecurityGetDetected                                                     
#;============================================================================
	MAKEPATCH 0x00006178
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#; HvxSecuritySetActivated                                                    
#;============================================================================
	MAKEPATCH 0x000061C0
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#; HvxSecurityGetActivated                                                    
#;============================================================================
	MAKEPATCH 0x00006228
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#; XeKeysSecuritySetStat                                                      
#;============================================================================
	MAKEPATCH 0x00006270
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#; don't limit what keys can be gotton from hv         search 0x1150010       
#;============================================================================
	MAKEPATCH 0x00007520
	.set destAddr, 0x00007550
0:
	MAKEBRANCH destAddr
9:


#;============================================================================
#; locked memcmp Run GOD Content                                              
#;============================================================================
	MAKEPATCH 0x00024B98
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#; HvxExpansionInstall - if sig check fails pass it anyway, but don't use cpukey crypto
#;============================================================================
	MAKEPATCH 0x0003089C
0:
	bne		cr6, signedExp		#; beq       cr6, return_C8007003
	li		%r29, 0				#; lwz       r11, 8(r31)
signedExp:
	nop							#; cmplw     cr6, r11, r28
	nop							#; bgt       cr6, return_C8007003
9:

#;============================================================================
#; HvxExpansionInstall do_expansion_install check                             
#;============================================================================
	MAKEPATCH 0x000304E8
0:
	nop
9:

#;============================================================================
#; HvxExpansionInstall do_expansion_install check                             
#;============================================================================
	MAKEPATCH 0x000304FC
0:
	nop
9:


#;===========================================================================================================
#;===========================================================================================================
#; 								START KERNEL PATCHES														 
#;===========================================================================================================
#;===========================================================================================================


#;============================================================================
#; XexpConvertError - converts revoke all errors into OK              
#;============================================================================
	KMAKEPATCH 0x8007A9D0
0:
	li		%r3, 0
	blr
9:

#;============================================================================
#; XexpLoadXexHeaders - disable min version check, Disable Media Flags Check  
#;============================================================================
	KMAKEPATCH 0x8007B570
0:
	b       (0b + 0x00000010)
	#;.long 0x60000000 #; nop
	#;.long 0x60000000 #; nop
	#;.long 0x60000000 #; nop
9:

#============================================================================
# XexpLoadFile - skip 0xC00000CC (BAD_NETWORK_NAME) error                    
# bl        RtlImageXexHeaderString                                          
#============================================================================
	KMAKEPATCH 0x8007b678
0:
	li     %r3, 0
9:

#;============================================================================
#; XexpLoadFile - skip 0xC000008B (resource not found) error                  
#; lwz     %r11, 0x4C(%r25) # 0x40000000 bound path flag                      
#;============================================================================
	KMAKEPATCH 0x8007B6E0
0:
	li     %r11, 0
9:

#;============================================================================
#; XexpLoadFile - skip 0xC000008B (resource not found) error                  
#; lwz     %r11, 0x4C(%r25) # 0x20000000 bound device flag                    
#;============================================================================
	KMAKEPATCH 0x8007B730
0:
	li     %r11, 0
9:

#;============================================================================
#; XexpLoadXexHeaders - 0xC0000059 revision mismatch                          
#;============================================================================
	KMAKEPATCH 0x8007C5A0
0:
	li		%r3, 0
9:

#;============================================================================
#; fix sfcxmu corruption issues on jasper bigblock - use 008 or higher for retail
#; SfcxInspectLargeDataBlock 80091C28 lbz %r23, 7(%r30) #; r23 = kernelresv blocks
#;============================================================================
	KMAKEPATCH 0x80093938
0:
	li		%r23, 0x10
9:

#;============================================================================
#; SataCdRomAuthenticationExInitialize - disables AuthEx/AP25 on supp. drives 
#;============================================================================
	KMAKEPATCH 0x80097EA0
0:
	cmplwi  cr6, %r11, 0xFF #; originally checking XEIKA drive version !< 1
9:

#;============================================================================
#; XeKeysVerifyRSASignature Disable PIRS/LIVE Signature Check                 
#;============================================================================
	KMAKEPATCH 0x801037A0
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#; XeKeysSecurityConvertError										          
#;============================================================================
	KMAKEPATCH 0x80104218
0:
	li %r3, 0
	blr
9:

#;============================================================================
#; XeKeysDvdAuthExConvertError										          
#;============================================================================
	KMAKEPATCH 0x801044F8
0:
	li %r3, 0
	blr
9:

#;============================================================================
#; _XeKeysRevokeIsValid                                                       
#;============================================================================
	KMAKEPATCH 0x801049D8
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#; _XeKeysRevokeIsRevoked (for dynamicUpdate)      **1**                      
#;============================================================================
	KMAKEPATCH 0x80104B90
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#;  XeKeysRevokeIsRevoked                                                     
#;============================================================================
	KMAKEPATCH 0x80104BE0
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#; XeKeysRevokeIsDeviceRevoked - usb device revocation (natal needs)          
#;============================================================================
	KMAKEPATCH 0x80104D20
0:
	li %r3, 0
	blr
9:

#;============================================================================
#; XeKeysRevokeConvertError - converts revoke all errors into OK              
#;============================================================================
	KMAKEPATCH 0x80104EA0
0:
	li %r3, 0
	blr
9:

#;============================================================================
#; XeCryptBnQwBeSigVerify needed for LIVE/PIRS execution                      
#;============================================================================
	KMAKEPATCH 0x8010A240
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#; mid XeKeysConsoleSignatureVerification - skip sig check                    
#;============================================================================
	KMAKEPATCH 0x80105958
	.set csig_return, 0x80105980
0:
	li 		%r3, 1
	KMAKEBRANCH csig_return
9:

#;============================================================================
#; SataDiskAuthenticateDevice Disable Harddisk Authentication                 
#;============================================================================
	KMAKEPATCH 0x80145E88
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#;  		Kernel patches for launch.xex and devkit key switching            
#;============================================================================

#;============================================================================
#; start launch.xex if present                                                
#;============================================================================
	KMAKEPATCH 0x80149C00
	.set HookDest, PatchAddr
	.set XexLoadImage, (0x8007C7B0-KBASE) 	#; XexLoadImage
	.set XamLoadedHook, (0x800611A4-KBASE) 	#; Phase1Initialization
	.set HookFileName, (RealAddr+(1f-0f))
0:
	mr      %r31, %r3               #; instruction replaced to get here,
									#; the load result of xam moved from %r3
	cmpwi   cr6, %r31, 0
	blt     cr6, xamfailed
	beq     cr6, loadmod            #; once xam loads successfully,
									#; try and start another module from hdd

xamfailed:
	blr                             #; if xam fails to load, then let the
									#; normal process of recording argon
									#; error and showing E7x screen commence
loadmod:
	lis     %r3, HookFileName@h
	ori    	%r3, %r3, HookFileName@l    #; address of path to xex dll
	li      %r4, 0x0
	ori     %r4, %r4, 8             #; mod flag
	li      %r5, 0					#; minver
	li      %r6, 0					#; phandle
	MAKEBRANCHL XexLoadImage
	li		%r31, 0					#; just in case!
	MAKEBRANCH (XamLoadedHook+4)
1:
	#;.string "\\Device\\Harddisk0\\Partition1\\launch.xex"
	.string "\\Device\\Flash\\launch.xex"
	.byte 0
	.align 2
9:

#;============================================================================
#; the instruction following xam loading in phase1init (mr %r31, %r3)         
#;============================================================================
	MAKEPATCH XamLoadedHook	#; NOTE this one is not kpatch
0:
	MAKEBRANCHL HookDest
9:

#;============================================================================
	.long 0xffffffff
	.end
#;============================================================================

