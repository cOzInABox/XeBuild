.include "../macros.S"

#;—————————————————————————————————————————————————————————————————————————————
#;   Patches for Hypervisor and Kernel 12611                                   
#;—————————————————————————————————————————————————————————————————————————————
	#; b destination-((.-0b)+source)

	.globl _start
	.set TLB_RPN, 0x3b5

_start:

#;============================================================================
#;   Send Dot Character over Serial as Progress Indicator                     
#;============================================================================
	MAKEPATCH 0x000019a0
	.set PrintDotDone, (PatchAddr + (9f - 0f))
0:
	ba      PrintDot                #; patches
9:

#;============================================================================
#;   Change Thread Hold Address                                               
#;============================================================================
	MAKEPATCH 0x000008D0
	.set FixHoldAddrDone, (PatchAddr + (9f - 0f))
0:
	ba      FixHoldAddr
9:

#;============================================================================
#;   Setup Fuse Row Pointer to point at Virtual Fuses                         
#;   Change Increment and Range Check of Fuse Row Pointer                     
#;============================================================================

	MAKEPATCH 0x00001F64
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00001F7C
0:
	addi	%r11, %r11, 1
	cmplwi	cr6, %r11, 0xC
9:

#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x00006E7C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00006EB0
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:

#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x00006FEC
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00007020
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:

#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x0000710C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00007140
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:

#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x0000737C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x000073B0
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:

#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x000074CC
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x00007500
0:
	addi    %r11, %r11, 0x0001
	cmplwi  cr6, %r11, 0x000c
9:

#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x0000758C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x000075BC
0:
	addi    %r11, %r11, 0x0001
	rldicr  %r9, %r9, 3,60		#; no change here
	cmpwi   cr6, %r11, 0x000c
9:

#;——————————————————————————————————————————————————————————
#;	NEW??                                                   
#;——————————————————————————————————————————————————————————
	MAKEPATCH 0x0000782C
0:
	bla     FixFusesAddr
9:
#;--------------------------------------
	MAKEPATCH 0x0000785C
0:
	addi    %r11, %r11, 0x0001
	rldicr  %r9, %r9, 3,60		#; no change here
	cmpwi   cr6, %r11, 0x000c
9:

#;============================================================================
#; pbkey xex2key ensure all zeroes                                            
#;============================================================================
	MAKEPATCH 0x000000F0
0:
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
9:

#;============================================================================
#;   hv tlb_rpn set command location                                          
#;============================================================================
	MAKEPATCH 0x0000114c
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
	MAKEPATCH 0x00001570
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
	MAKEPATCH 0x000041FC
0:
	.long 0x60000000 #; nop
	.long 0x60000000 #; nop
9:

#;============================================================================
#;HvxResolveImports - removes min version check in hv  (0xC0000059)           
#;00004980 blt     cr6, version_mismatch                                      
#;0000498C blt     cr6, version_mismatch                                      
#;============================================================================
		MAKEPATCH 0x00004978
0:
		.long 0x60000000 # nop
9:
#;--------------------------------------
		MAKEPATCH 0x00004984
0:
		.long 0x60000000 # nop
9:

#;============================================================================
#;   Utility Functions                                                        
#;============================================================================
	MAKEPATCH 0x000F8E8
	.set PrintDot, PatchAddr
	.set FixFusesAddr, (PatchAddr + (1f - 0f))
	.set FixHoldAddr, (PatchAddr + (2f - 0f))
	.set PrintChar, (PatchAddr + (3f - 0f))
	.set TogglePublicKey, (PatchAddr + (5f - 0f))

	.set sc_zero_key, 0x72627472 #; rbtr
	.set sc_zero_org, 0x00001F20 #; original function for syscall 0
	.set sc_zero_tent, 0x00001BA0 #; address of syscall 0 function entry
	.set HvpMemCpy,				0x0000F020 #  memcpy(void *destination, const void *source, int len)

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
4:
	lwz     %r3, 0x1018(%r4)
	rlwinm. %r3, %r3, 0, 6, 6
	beq     4b
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
9:
#;——————————————————————————————————————————————————————————
#;   patch syscall 0 to jump to hook                        
#;——————————————————————————————————————————————————————————
	MAKEPATCH sc_zero_tent
0:
	.long TogglePublicKey
9:

#;============================================================================
#;   Allow Unsigned XEX Execution                                             
#;============================================================================
	MAKEPATCH 0x00003300
	.set HvCryptAesKey, 0xF750
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
	MAKEPATCH 0x000035E0
0:
	li		%r3, 0
9:

#;============================================================================
#;   HvxCreateImageMapping - Remove XEX Region Check                          
#;============================================================================
	MAKEPATCH 0x00003878
0:
	nop
9:

#;============================================================================
#;   HvxSecuritySetDetected                                                   
#;============================================================================
	MAKEPATCH 0x00009058
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#;   HvxSecurityGetDetected                                                   
#;============================================================================
	MAKEPATCH 0x000090E8
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#;   HvxSecuritySetActivated                                                  
#;============================================================================
	MAKEPATCH 0x00009130
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#;   HvxSecurityGetActivated                                                  
#;============================================================================
	MAKEPATCH 0x00009198
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#;   XeKeysSecuritySetStat                                                    
#;============================================================================
	MAKEPATCH 0x000091E0
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#; don't limit what keys can be gotton from hv         search 0x1150010       
#;============================================================================
	MAKEPATCH 0x0000A308
	.set destAddr, 0x0000A338
0:
	MAKEBRANCH destAddr
9:


#;============================================================================
#;   Run GOD Content                                                          
#;============================================================================
	MAKEPATCH 0x0000A4F0
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#; HvxExpansionInstall - pass mfg sig check                                   
#;============================================================================
	MAKEPATCH 0x0001ADB0
0:
	li		%r3, 1
9:


#;===========================================================================================================
#;===========================================================================================================
#; 								START KERNEL PATCHES														 
#;===========================================================================================================
#;===========================================================================================================


#;============================================================================
#;  XexpConvertError - converts revoke all errors into OK 8007A670            
#;  this one is called on every executeable xex load...                       
#;============================================================================
	MAKEPATCH 0x0007A660
0:
	li %r3, 0
	blr
9:

#;============================================================================
#;   XexpLoadXexHeaders - disable min version check, Disable Media Flags Check
#;============================================================================
	MAKEPATCH 0x0007B240
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
	KMAKEPATCH 0x8007b348
0:
	li     %r3, 0
9:

#;============================================================================
#; XexpLoadFile - skip 0xC000008B (resource not found) error                  
#; lwz     %r11, 0x4C(%r25) # 0x40000000 bound path flag                      
#;============================================================================
	MAKEPATCH 0x0007B3B0
0:
	li     %r11, 0
9:

#;============================================================================
#; XexpLoadFile - skip 0xC000008B (resource not found) error                  
#; lwz     %r11, 0x4C(%r25) # 0x20000000 bound device flag                    
#;============================================================================
	MAKEPATCH 0x0007B400
0:
	li     %r11, 0
9:

#;============================================================================
#; XexpLoadXexHeaders - 0xC0000059 revision mismatch                          
#;============================================================================
	MAKEPATCH 0x0007C270
0:
	li		%r3, 0
9:

#;============================================================================
#; fix sfcxmu corruption issues on jasper bigblock - use 008 or higher for retail
#; SfcxInspectLargeDataBlock 80091C28 lbz %r23, 7(%r30) #; r23 = kernelresv blocks
#;============================================================================
	MAKEPATCH 0x00091BE0
0:
	li		%r23, 0x10
9:

#;============================================================================
#; 80095640 SataCdRomAuthenticationExInitialize - disables AuthEx/AP25 on supp. drives
#;============================================================================
	MAKEPATCH 0x000955F0
0:
	cmplwi  cr6, %r11, 0xFF #; originally checking XEIKA drive version !< 1
9:

#;============================================================================
#;  800FF038 XeKeysVerifyRSASignature Disable PIRS/LIVE Signature Check       
#;============================================================================
	MAKEPATCH 0x000FF020
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#; 800FF880 XeKeysSecurityConvertError										  
#;============================================================================
	MAKEPATCH 0x000FF868
0:
	li %r3, 0
	blr
9:

#;============================================================================
#; 800FFB60 XeKeysDvdAuthExConvertError										  
#;============================================================================
	MAKEPATCH 0x000FFB48
0:
	li %r3, 0
	blr
9:

#;============================================================================
#;  80100028 _XeKeysRevokeIsValid                                             
#;============================================================================
	MAKEPATCH 0x00100010
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#;   801001E0 _XeKeysRevokeIsRevoked (for dynamicUpdate)       **1**          
#;============================================================================
	MAKEPATCH 0x001001C8
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#;  80100230 XeKeysRevokeIsRevoked                                            
#;============================================================================
	MAKEPATCH 0x00100218
0:
	li      %r3, 0
	blr
9:

#;============================================================================
#;  80100370 XeKeysRevokeIsDeviceRevoked - usb device revocation (natal needs)
#;============================================================================
	MAKEPATCH 0x00100358
0:
	li %r3, 0
	blr
9:

#;============================================================================
#;  801004E8 XeKeysRevokeConvertError - converts revoke all errors into OK    
#;============================================================================
	MAKEPATCH 0x001004D0
0:
	li %r3, 0
	blr
9:

#;============================================================================
#;  80105680 XeCryptBnQwBeSigVerify needed for LIVE/PIRS execution            
#;============================================================================
	MAKEPATCH 0x00105660
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#; mid XeKeysConsoleSignatureVerification - skip sig check                    
#;============================================================================
	KMAKEPATCH 0x80100F58
	.set csig_return, 0x80100F80
0:
	li 		%r3, 1
	KMAKEBRANCH csig_return
9:

#;============================================================================
#;  80140EE0 SataDiskAuthenticateDevice Disable Harddisk Authentication       
#;============================================================================
	MAKEPATCH 0x00140E60
0:
	li      %r3, 1
	blr
9:

#;============================================================================
#;  		Kernel patches for launch.xex and devkit key switching            
#;============================================================================

#;============================================================================
#; b from XexpLoadImage - check for devkit key switching                      
#;============================================================================
	KMAKEPATCH 0x80144C00
	.set HookDest, PatchAddr
	.set XexLoadImage, (0x8007C480-KBASE) 	#; XexLoadImage
	.set XamLoadedHook, (0x80061198-KBASE) 	#; Phase1Initialization
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
	lis     %r5, 0x0                #; minver
	li      %r4, 0x0
	ori     %r4, %r4, 8             #; mod flag
	li      %r6, 0
	MAKEBRANCHL 	XexLoadImage
	li		%r31, 0					#; just in case!
 	MAKEBRANCH 		(XamLoadedHook+4)
1:
	#;.string "\\Device\\Harddisk0\\Partition1\\launch.xex"
	.string "\\Device\\Flash\\launch.xex"
	.byte 0
	.align 2
9:

#;============================================================================
#; the instruction following xam loading in phase1init (mr %r31, %r3)         
#;============================================================================
	#; .set XamLoadedHook, 0x00061198 #; 80061198 
	MAKEPATCH XamLoadedHook
0:
	MAKEBRANCHL HookDest
9:


#;============================================================================
	.long 0xffffffff
	.end
#;============================================================================
