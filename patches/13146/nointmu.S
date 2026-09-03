.include "../macros.S"
	.globl _start
_start:

#;============================================================================
#; SfcxMuMountStorage, disable nand mu
#; lis     %r10, 0x3000
#;============================================================================
	KMAKEPATCH 0x8008CDA8
0:
	lis     %r10, 0x6000
9:

.end
