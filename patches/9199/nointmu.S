.include "../macros.S"
	.globl _start
_start:

#;============================================================================
#; SfcxMuMountStorage, disable nand mu
#; 0x80088D98 lis     %r10, 0x3000
#;============================================================================
	.set PatchAddr, 0x00088D98
	.long PatchAddr
	.long (9f - 0f) / 4
0:
	lis     %r10, 0x6000
9:

.end
