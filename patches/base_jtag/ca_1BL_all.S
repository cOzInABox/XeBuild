#;—————————————————————————————————————————————————————————————————————————————
#;  1BL ROM patches (all fat machines)                                         
#;—————————————————————————————————————————————————————————————————————————————
.include "../macros.S" #; provides macros like MAKEPATCH and defines like SPR_HRMOR
.set CBFixOffset, 0xD5060

	.globl _start
_start:

#;============================================================================
#; patch entry point for dot output on uart                                   
#;============================================================================
	MAKEPATCH 0x00000104
0:
	bl	UartDot - ((.-0b)+PatchAddr)
9:

#;============================================================================
#; change address to that of the in ram/copied 1BL                            
#;============================================================================
	MAKEPATCH 0x000030B0
0:
	li		%r4, 0
9:

#;============================================================================
#; branch to code to preserve/apply patches and virtual fuses                 
#;============================================================================
	MAKEPATCH 0x0000339C
	.set CopyPatchesDone, (PatchAddr+4)
0:
	b	CopyPatches - ((.-0b)+PatchAddr)
9:

#;============================================================================
#; alter CB base address to non-exploitable CB version                        
#;============================================================================
	MAKEPATCH 0x000042ac
	.set FixCBAddrDone, (PatchAddr+4)
0:
	b	FixCBAddr - ((.-0b)+PatchAddr)
9:

#;============================================================================
#; utility functions                                                          
#;============================================================================
	MAKEPATCH 0x00007E88
	.set UartDot, PatchAddr
	.set FixCBAddr, (PatchAddr + (1f - 0f))
	.set CopyPatches, (PatchAddr + (2f - 0f))
0: #; UartDot
	li		%r3, 0x2E #; '.'
	lis		%r4, 0x8000
	ori		%r4, %r4, 0x200
	rldicr	%r4, %r4, 32,31
	oris	%r4, %r4, 0xEA00 		#; 0x80000200_EA000000
	slwi	%r3, %r3, 24
	stw		%r3, 0x1014(%r4)		#; 0x80000200_EA001014
waitloop:
	lwz		%r3, 0x1018(%r4) 		#; 0x80000200_EA001018
	rlwinm.	%r3, %r3, 0,6,6
	beq		waitloop
	li		%r3, 0
	blr
#;——————————————————————————————————————————————————————————
1: #; FixCBAddr                                             
#;——————————————————————————————————————————————————————————
	lis		%r25, CBFixOffset@h
	ori		%r25, %r25, CBFixOffset@l
	b		FixCBAddrDone - ((.-0b)+PatchAddr)
#;——————————————————————————————————————————————————————————
2: #; CopyPatches                                           
#;——————————————————————————————————————————————————————————
	lis		%r3, 0x8000
	rldicr	%r3, %r3, 32,31
	oris	%r3, %r3, 0x100			#; 0x80000000_00010000
	lis		%r4, 0x8000
	ori		%r4, %r4, 0x200
	rldicr	%r4, %r4, 32,31
	oris	%r4, %r4, 1				#; 0x80000200_00010000
	addi	%r3, %r3, -4
	addi	%r4, %r4, -4
copyPatchLoop:
	lwzu	%r5, 4(%r3)
	cmpwi	%r5, -1
	beq		copyFuses
	add		%r5, %r4, %r5
	lwzu	%r6, 4(%r3)
	mtctr	%r6
copyloop:
	lwzu	%r6, 4(%r3)
	stwu	%r6, 4(%r5)
	bdnz	copyloop
	b		copyPatchLoop
copyFuses:
	lis		%r3, 0x8000
	rldicr	%r3, %r3, 32,31
	oris	%r3, %r3, 0x100
	ori		%r3, %r3, 0x3000 		#; 0x80000000_01003000
	lis		%r4, 0x8000
	ori		%r4, %r4, 0x200
	rldicr	%r4, %r4, 32,31
	oris	%r4, %r4, 1
	ori		%r4, %r4, 0x9B00 		#; 0x80000200_00019B00
	li		%r5, 0
copyFuseloop:
	ld		%r6, 0(%r3)
	stdx	%r6, %r4, %r5
	addi	%r3, %r3, 8
	addi	%r5, %r5, 8
	cmpldi	%r5, 0x60
	blt		copyFuseloop
	lis		%r3, 0x8000
	ori		%r3, %r3, 0x200
	rldicr	%r3, %r3, 32,31 		#; 0x80000200_00000000
	isync
	mtspr	SPR_HRMOR, %r3
	isync
	li		%r0, 0
	MAKEBRANCH CopyPatchesDone
9:

#;============================================================================
	.long 0xFFFFFFFF
	.end
#;============================================================================
