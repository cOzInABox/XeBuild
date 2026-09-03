.ifdef GLITCH
	.ifdef FATGLITCH
		.print "****BUILDING FOR FAT GLITCH"
	.else
		.print "****BUILDING FOR SLIM GLITCH"
	.endif
.else
	.ifdef VFUSE
		.print "****BUILDING FOR JTAG"
	.endif
.endif
.ifdef NOKV_HASH
	.print "****KV HASH CHECK DISABLED"
.endif
.ifdef _PEEK_POKE
	.print "****PEEK POKE ENABLED"
.endif
.ifdef NO_NAND_MU
	.print "****DISABLE NAND MU"
.endif

.ifdef VFUSE
	.print "****Using virtual fuses****"
.endif

.set KBASE, 0x80000000
.set KIMGBASE, 0x80040000
.set TLB_RPN, 0x3b5
.set SPR_HRMOR, 0x139
.set SPR_PIR, 0x3FF
.set BldrFlags, 0x6
.set xex2key, 0x00000054
.set xex2devkey, 0x000000F0

#;Arg0 is address
.macro MAKENOP PAddress 
.if(\PAddress)
	#;Patch structure
	.set 	PatchAddr, \PAddress
	.long	PatchAddr
	.long	1			#;Length of patch in dwords
	.long	0x60000000
.endif
.endm

#;Arg0 is address
.macro MAKELIR31 PAddress 
.if(\PAddress)
	#;Patch structure
	.set 	PatchAddr, \PAddress
	.long	PatchAddr
	.long	1			#;Length of patch in dwords
	.long 	0x38600001
.endif
.endm

#;Arg0 is address
.macro MAKELIR30 PAddress 
.if(\PAddress)
	#;Patch structure
	.set 	PatchAddr, \PAddress
	.long	PatchAddr
	.long	1			#;Length of patch in dwords
	.long 	0x38600000
.endif
.endm

#;

#;Arg0 is address, Arg1 is label used for handler
.macro MAKEPATCH PAddress 
.if(\PAddress)
	#;Patch structure
	.set 	PatchAddr, \PAddress
	.long	PatchAddr
	.long (9f - 0f) / 4  					#;Length of patch in dwords
.endif
.endm

#;Arg0 is address, Arg1 is label used for handler
.macro KMAKEPATCH PKAddress 
.if(\PKAddress)
	#;Patch structure
	.set 	PatchAddr, (\PKAddress-KBASE)
	.set 	RealAddr, (\PKAddress)
	.long	PatchAddr
	.long (9f - 0f) / 4  					#;Length of patch in dwords
.endif
.endm

#;Arg0 is destination address
.macro MAKEBRANCH DAddress
.if(\DAddress)
		#;Patch structure
 	b	\DAddress - ((.-0b)+PatchAddr)
.endif
.endm

#;Arg0 is destination address
.macro MAKEBRANCH1F
.if(\DAddress)
		#;Patch structure
 	b	((.-0b)+PatchAddr+4)
.endif
.endm

#;Arg0 is destination address
.macro MAKEBRANCHL1F
.if(\DAddress)
		#;Patch structure
 	bl	((.-0b)+PatchAddr+4)
.endif
.endm

#;Arg0 is destination address
.macro MAKEBRANCHL DAddress 
.if(\DAddress)
		#;Patch structure
 	bl	\DAddress - ((.-0b)+PatchAddr)
.endif
.endm

#;Arg0 is destination address
.macro KMAKEBRANCH DAddress
.if(\DAddress)
		#;Patch structure
 	b	\DAddress - ((.-0b)+RealAddr)
.endif
.endm

#;Arg0 is destination address
.macro KMAKEBRANCHL DAddress 
.if(\DAddress)
		#;Patch structure
 	bl	\DAddress - ((.-0b)+RealAddr)
.endif
.endm

#;Arg0 is hook address, Arg1 is destination
.macro KMAKEHOOKL PKAddress PKDest
.if(\PKAddress)
	#;Patch structure
	.set 	PatchAddr, (\PKAddress-KBASE)
	.set 	RealAddr, (\PKAddress)
	.long	PatchAddr
	.long (9f - 0f) / 4  					#;Length of patch in dwords
0:
	MAKEBRANCHL (\PKDest-KBASE)
9:
.endif
.endm
