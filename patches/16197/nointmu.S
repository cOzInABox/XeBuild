.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:

#;============================================================================
#; SfcxMuMountStorage, disable nand mu
#; lis     %r10, 0x3000
#;============================================================================
	KMAKEPATCH AddonNoNandMuPatch
0:
	lis     %r10, 0x6000
9:

#;============================================================================
#; MassDriverEntry, disable internal usb mu
#; lis       r10, 0x4000
#;============================================================================
	KMAKEPATCH AddonNoTrinMuPatch
0:
	lis     %r10, 0x6000
9:

#;============================================================================
#; MmcxMuMountStorage, disable corona memory unit
#;============================================================================
	KMAKEPATCH AddonNoCorMuPatch
0:
	blr
9:


.end
