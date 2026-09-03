# ============================================================================
#   Patches for Hypervisor and Kernel 9199
# ============================================================================
		# 1: b destination-((1b-0b)+source)

		.globl _start

.include "../macros.S"

_start:

# ============================================================================
#   Send Dot Character over Serial as Progress Indicator (ikari)
# ============================================================================
		.set PatchAddr, 0x000019a0
		.set PrintDotDone, (PatchAddr + (9f - 0f))

		.long PatchAddr                 # target address
		.long (9f - 0f) / 4             # number of patches

0:
		ba      PrintDot                # patches
9:

# ============================================================================
#   Change Threads' Hold Address (ikari)
# ============================================================================

		.set PatchAddr, 0x00001c0c
		.set FixHoldAddrDone, (PatchAddr + (9f - 0f))

		.long PatchAddr
		.long (9f - 0f) / 4

0:
		ba      FixHoldAddr
9:

# ============================================================================
#   Setup Fuse Row Pointer to point at Virtual Fuses (ikari)
# ============================================================================

		.long 0x00002074
		.long (9f - 0f) / 4

0:
		bla     FixFusesAddr
9:

		.long 0x00006bdc
		.long (9f - 0f) / 4

0:
		bla     FixFusesAddr
9:

		.long 0x00006d4c
		.long (9f - 0f) / 4

0:
		bla     FixFusesAddr
9:

		.long 0x00006e6c
		.long (9f - 0f) / 4

0:
		bla     FixFusesAddr
9:

		.long 0x000070dc
		.long (9f - 0f) / 4

0:
		bla     FixFusesAddr
9:

		.long 0x0000722c
		.long (9f - 0f) / 4

0:
		bla     FixFusesAddr
9:

		.long 0x000072ec
		.long (9f - 0f) / 4

0:
		bla     FixFusesAddr
9:

# ============================================================================
#   Change Increment and Range Check of Fuse Row Pointer (ikari)
# ============================================================================

		.long 0x0000208C
		.long (9f - 0f) / 4

0:
		addi	%r11, %r11, 1
		cmplwi	cr6, %r11, 0xC
9:

		.long 0x00006c10
		.long (9f - 0f) / 4

0:
		addi    %r11, %r11, 0x0001
		cmplwi  cr6, %r11, 0x000c
9:

		.long 0x00006d80
		.long (9f - 0f) / 4

0:
		addi    %r11, %r11, 0x0001
		cmplwi  cr6, %r11, 0x000c
9:

		.long 0x00006ea0
		.long (9f - 0f) / 4

0:
		addi    %r11, %r11, 0x0001
		cmplwi  cr6, %r11, 0x000c
9:

		.long 0x00007110
		.long (9f - 0f) / 4

0:
		addi    %r11, %r11, 0x0001
		cmplwi  cr6, %r11, 0x000c
9:

		.long 0x00007260
		.long (9f - 0f) / 4

0:
		addi    %r11, %r11, 0x0001
		cmplwi  cr6, %r11, 0x000c
9:

		.long 0x0000731c
		.long (9f - 0f) / 4

0:
		addi    %r11, %r11, 0x0001
9:

		.long 0x00007324
		.long (9f - 0f) / 4

0:
		cmpwi   cr6, %r11, 0x000c
9:

# ============================================================================
# pbkey xex2key ensure all zeroes
# ============================================================================

		.long 0x000000F0
		.long (9f - 0f) / 4

0:
		.long 0x00000000
		.long 0x00000000
		.long 0x00000000
		.long 0x00000000
9:

# ============================================================================
#  Syscall Dispatcher - Increment max Syscall Value to add one (c0z)
# ============================================================================

		.long 0x00001420
		.long (9f - 0f) / 4

0:
		cmplwi  %r0, 0x7c
9:

# ============================================================================
#   hv tlb_rpn set command location (cOz)
# ============================================================================

		.set PatchAddr, 0x0000114c
		.set PatchTlbRpnDone, (PatchAddr + (9f - 0f))

		.long PatchAddr
		.long (9f - 0f) / 4

0:
		ba      PatchTlbRpn
9:

# ============================================================================
#   Patch TLB RPN to Remove Protections (cOz)
#
#     - allows code excution anywhere in 0x80000000->0x9FFFFFFF range
#     - allows system thread manipulation of code/read only data without
#       disabling encryption
#     - 0x0007 clears bits NoExec:DataPage:Readonly of RPN
# ============================================================================

		.set PatchAddr, 0x00001570
		.set PatchTlbRpn, PatchAddr

		.long PatchAddr
		.long (9f - 0f) / 4

0:
		li      %r4, 0x0007             # removes all page protection
		andc    %sp, %sp, %r4
		mtspr   TLB_RPN, %sp            # instruction replaced to get here
		ba      PatchTlbRpnDone
9:

# ============================================================================
# HvxLoadImageData - ignores return of SHA memdiff (for devkit xex)
# ============================================================================
		.long 0x000043B4
		.long (9f - 0f) / 4
0:
		.long 0x60000000 # nop
		.long 0x60000000 # nop
9:

# ============================================================================
#   Utility Functions (c0z and ikari)
# ============================================================================

		.set PatchAddr, 0x000f568
		.set PrintDot, PatchAddr
		.set FixFusesAddr, (PatchAddr + (1f - 0f))
		.set FixHoldAddr, (PatchAddr + (2f - 0f))
		.set PrintChar, (PatchAddr + (3f - 0f))
		.set TogglePublicKey, (PatchAddr + (5f - 0f))

		.set sc_zero_key, 0x72627472 #; rbtr
		.set sc_zero_org, 0x00002030 #; original function for syscall 0
		.set sc_zero_tent, 0x00001C98 #; address of syscall 0 function entry
	.set HvpMemCpy,				0x0000ED20 #  memcpy(void *destination, const void *source, int len)

		.long PatchAddr
		.long (9f - 0f) / 4

#
# PrintDot
#
0:
.ifdef NOKV_HASH
		lhz     %r3, BldrFlags(0)
		li		%r4, 0x20
		andc	%r3, %r3, %r4
		sth     %r3, BldrFlags(0)
.endif
		li      %r3, '.'
		bla     PrintChar

		li      %r3, '\n'
		bla     PrintChar

		li      %r3, 0x0200
		ba      PrintDotDone

#
# FixFusesAddr
#
1:
		lis     %r3, 0x0002
		addi    %r3, %r3, -0x6500
		blr

#
# FixHoldAddr
#
2:
		lis     %r4, 0x8000
		rldicr  %r4, %r4, 32, 31
		oris    %r4, %r4, 0x0020
		ba      FixHoldAddrDone

#
# PrintChar
#
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

#
# TogglePublicKey, ToggleTlbProtection
#
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

#——————————————————————————————————————————————————————————
#   patch syscall 0 to jump to hook                        
#——————————————————————————————————————————————————————————

	.long sc_zero_tent
	.long (9f - 0f) / 4

0:
	.long TogglePublicKey
9:

# ============================================================================
#   Allow Unsigned XEX Execution (ikari)
# ============================================================================
	MAKEPATCH 0x000034D0
	.set HvCryptAesKey, 0xF3D0
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
	MAKEPATCH 0x000037B0
0:
	li		%r3, 0
9:

# ============================================================================
#   HvxCreateImageMapping - Remove XEX Region Check (cOz)
# ============================================================================

		.long 0x00003a48
		.long (9f - 0f) / 4

0:
		nop
9:

# ============================================================================
#   HvxSecuritySetDetected (Redline99)
# ============================================================================

		.long 0x00008db8
		.long (9f - 0f) / 4

0:
		li      %r3, 0
		blr
9:

# ============================================================================
#   HvxSecurityGetDetected (Redline99)
# ============================================================================

		.long 0x00008e80
		.long (9f - 0f) / 4

0:
		li      %r3, 0
		blr
9:

# ============================================================================
#   HvxSecuritySetActivated (Redline99)
# ============================================================================

		.long 0x00008ec8
		.long (9f - 0f) / 4

0:
		li      %r3, 0
		blr
9:

# ============================================================================
#   HvxSecurityGetActivated (Redline99)
# ============================================================================

		.long 0x00008f70
		.long (9f - 0f) / 4

0:
		li      %r3, 0
		blr
9:

# ============================================================================
#   XeKeysSecuritySetStat (Redline99) HvxSecuritySetStat
# ============================================================================

		.long 0x00008fb8
		.long (9f - 0f) / 4

0:
		blr
9:

#;============================================================================
#; don't limit what keys can be gotton from hv         search 0x1150010       
#;============================================================================
	MAKEPATCH 0x0000A078
	.set destAddr, 0x0000A0A8
0:
	MAKEBRANCH destAddr
9:

# ============================================================================
#   Run GOD Content
# ============================================================================

		.long 0x0000a260
		.long (9f - 0f) / 4

0:
		li      %r3, 1
		blr
9:

#;============================================================================
#; HvxExpansionInstall - pass mfg sig check                                   
#;============================================================================
	MAKEPATCH 0x0000EAA8
0:
	li		%r3, 1
9:


# ============================================================================
#   Disable Media Flags Check 0x800785e4
# ============================================================================

		.long 0x000785e4
		.long (9f - 0f) / 4

0:
		b       (0b + 0x00000010)
9:

# ============================================================================
#   XexpLoadXexHeaders - disable min version check 0x800785E8
# ============================================================================

		.long 0x000785E8
		.long (9f - 0f) / 4

0:
		.long 0x60000000 # nop
		.long 0x60000000 # nop
		.long 0x60000000 # nop
9:

# ============================================================================
#   Disable PIRS/LIVE Signature Check (ikari)
# ============================================================================

		.long 0x000f6118
		.long (9f - 0f) / 4

0:
		li      %r3, 1
		blr
9:

# ============================================================================
#   _XeKeysRevokeIsValid 0x800f70c0
# ============================================================================

		.long 0x000f70c0
		.long (9f - 0f) / 4

0:
		li      %r3, 1
		blr
9:

# ============================================================================
#   _XeKeysRevokeIsRevoked (for dynamicUpdate) 0x800F7160
# ============================================================================

		.long 0x000f7160
		.long (9f - 0f) / 4

0:
		li      %r3, 1
		blr
9:

# ============================================================================
#   XeKeysRevokeIsRevoked 0x800f7230
# ============================================================================

		.long 0x000f7230
		.long (9f - 0f) / 4

0:
		li      %r3, 0
		blr
9:

# ============================================================================
# XeKeysRevokeConvertError - converts revoke all errors into OK 0x800F72A8
# ============================================================================

		.long 0x000F72A8
		.long (9f - 0f) / 4

0:
		li %r3, 0
		blr
9:

# ============================================================================
#   needed for LIVE/PIRS execution
# ============================================================================

		.long 0x000fc470
		.long (9f - 0f) / 4

0:
		li      %r3, 1
		blr
9:

# ============================================================================
#   Disable Harddisk Authentication (Redline99)
# ============================================================================

		.long 0x00128378
		.long (9f - 0f) / 4

0:
		li      %r3, 1
		blr
9:

#;============================================================================
#; start launch.xex if present                                                
#;============================================================================
	KMAKEPATCH 0x8012d000
	.set HookDest, PatchAddr
	.set XexLoadImage, (0x800793d8-KBASE) 	#; XexLoadImage
	.set XamLoadedHook, (0x80061154-KBASE) 	#; Phase1Initialization
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
#; XexpLoadXexHeaders - 0xC0000059 revision mismatch                          
#;============================================================================
	MAKEPATCH 0x000791C8
0:
	li		%r3, 0
9:

# ============================================================================
# fix sfcxmu corruption issues on jasper bigblock - use 008 or higher for retail
# SfcxInspectLargeDataBlock 8008E060 lbz     %r23, 7(%r30) # r23 = kernelresv blocks
# ============================================================================
	.set PatchAddr, 0x0008E060
	.long PatchAddr
	.long (9f - 0f) / 4
0:
		li		%r23, 0x10
9:

#============================================================================
# XexpLoadFile - skip 0xC00000CC (BAD_NETWORK_NAME) error                    
# bl        RtlImageXexHeaderString                                          
#============================================================================
	KMAKEPATCH 0x800786f0
0:
	li     %r3, 0
9:

#;============================================================================
#; XexpLoadFile - skip 0xC000008B (resource not found) error                  
#; lwz     %r11, 0x4C(%r25) # 0x40000000 bound path flag                      
#;============================================================================
	.set PatchAddr, 0x00078744
	.long PatchAddr
	.long (9f - 0f) / 4
0:
	li     %r11, 0
9:

#;============================================================================
#; XexpLoadFile - skip 0xC000008B (resource not found) error                  
#; lwz     %r11, 0x4C(%r25) # 0x20000000 bound device flag                    
#;============================================================================
	.set PatchAddr, 0x00078794
	.long PatchAddr
	.long (9f - 0f) / 4
0:
	li     %r11, 0
9:

#;============================================================================
#; mid XeKeysConsoleSignatureVerification - skip sig check                    
#;============================================================================
	KMAKEPATCH 0x800F7D50
	.set csig_return, 0x800F7D78
0:
	li 		%r3, 1
	KMAKEBRANCH csig_return
9:

# ============================================================================
		.long 0xffffffff
		.end

# ============================================================================
