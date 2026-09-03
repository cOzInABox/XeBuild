.include "../macros.S"

#;—————————————————————————————————————————————————————————————————————————————
#;   3BL / CD 8453 (All)                                                       
#;—————————————————————————————————————————————————————————————————————————————
		.globl _start
_start:

.ifdef XENON
	.print "****XENON ADDRESS SET"
	.set CEAddr, 0x00016EC0			#; address of CE in flash (xenon)
.endif

.ifdef ZEPHYR
	.print "****ZEPHYR ADDRESS SET"
	.set CEAddr, 0x00015170			#; address of CE in flash (zephyr)  FIXME
.endif

.ifdef FALCON
	.print "****FALCON ADDRESS SET"
	.set CEAddr, 0x00016500			#; address of CE in flash (falcon)
.endif

.ifdef JASPER
	.print "****JASPER ADDRESS SET"
	.set CEAddr, 0x00016C40			#; address of CE in flash (jasper)  FIXME
.endif

.ifndef CEAddr
	.print "******************CE ADDR NOT SET FOR THIS CONSOLE TYPE *********************"
.endif

.set CDOrigSz, 0x00005780		#; original size at 0xC of this BL

#; ============================================================================
#;   patch size in header (0x5780)                                             
#; ============================================================================
	MAKEPATCH 0x0000000C
0:
	.long (UtilitySize+CDOrigSz-8)
	#;.long 0x5860
9:

#; ============================================================================
#;   Output dot on uart (li r3, 0x200)                                         
#; ============================================================================
	MAKEPATCH 0x00000260
0:
	bl		UartDotOut-PatchAddr
9:

#; ============================================================================
#;   fix CE address (mr r3, r31)                                               
#; ============================================================================
	MAKEPATCH 0x000003E0
0:
	bl		FixCeAddr-PatchAddr
9:

#; ============================================================================
#;   copy patches (rfid)                                                       
#; ============================================================================
	MAKEPATCH 0x000005A4
0:
	b		CopyPatches-PatchAddr
9:

#; ============================================================================
#;   fix fuse address                                                          
#; ============================================================================
	MAKEPATCH 0x00000940
0:
	addi	%r11, %r11, 1
9:

#; ============================================================================
#;   fix fuse address                                                          
#; ============================================================================
	MAKEPATCH 0x00000948
0:
	cmpwi	cr6, %r11, 0xC
	addi	%r9, %r9, 0x3360
9:

#; ============================================================================
#;   Utility                                                                   
#; ============================================================================
	MAKEPATCH 0x00005778
	.set UartDotOut, PatchAddr
	.set FixCeAddr, (PatchAddr + (1f - 0f))
	.set CopyPatches, (PatchAddr + (2f - 0f))
	.set UtilitySize, (9f-0f)

#;——————————————————————————————————————————————————————————
#; UartDotOut                                               
#;——————————————————————————————————————————————————————————
0: 
	li		%r3, 0x2E 			#; '.'
	lis		%r4, -0x8000
	ori		%r4, %r4, 0x200		#; 0x80000200
	rldicr	%r4, %r4, 32,31
	oris	%r4, %r4, 0xEA00 	#; 0x80000200_EA000000
	slwi	%r3, %r3, 24
	stw		%r3, 0x1014(%r4) 	#; 0x80000200_EA001014
waitloop:
	lwz		%r3, 0x1018(%r4) 	#; 0x80000200_EA001018
	rlwinm.	%r3, %r3, 0,6,6
	beq		waitloop
	li		%r3, 0x200			#; original instruction
	blr

#;——————————————————————————————————————————————————————————
#; FixCeAddr                                                
#;——————————————————————————————————————————————————————————
1:
	lis     %r3, CEAddr@h
	ori     %r3, %r3, CEAddr@l
	blr

#;——————————————————————————————————————————————————————————
#; CopyPatches                                              
#;——————————————————————————————————————————————————————————
2:
	mfmsr	%r7
	li		%r8, 0x10
	andc	%r8, %r7, %r8
	lis		%r3, 0x8000
	rldicr	%r3, %r3, 32,31
	oris	%r3, %r3, 0x100
	ori		%r3, %r3, 0x2000
	addi	%r3, %r3, -4
iterLoop:
	mtmsrd	%r8, 0
	isync
	lwzu	%r4, 4(%r3)
	cmpwi	%r4, -1
	beq		exit
	addi	%r4, %r4, -4
	lwzu	%r6, 4(%r3)
	mtctr	%r6
patchLoop:
	mtmsrd	%r8, 0
	isync
	lwzu	%r6, 4(%r3)
	mtmsrd	%r7, 0
	isync
	stwu	%r6, 4(%r4)
	dcbst	%r0, %r4
	icbi	%r0, %r4
	bdnz	patchLoop
	b		iterLoop
exit:
	sync
	isync
	rfid
	.long 0
	.long 0
9:

#; ============================================================================
	.long 0xFFFFFFFF
	.end
#; ============================================================================
