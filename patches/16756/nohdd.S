.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:

#;============================================================================
#; SataDiskInitialize, disable internal hard disk
#; bl SataChannelDetectDevice; cmplwi r3, 0; beq return
#;============================================================================
	KMAKEPATCH SataChannelDetectDevice
0:
	li     %r3, 0
9:

.end
