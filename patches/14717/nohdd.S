.include "../macros.S"
	.globl _start
_start:

#;============================================================================
#; SataDiskInitialize, disable internal hard disk
#; bl SataChannelDetectDevice; cmplwi r3, 0; beq return
#;============================================================================
	KMAKEPATCH 0x8014C7D8
0:
	li     %r3, 0
9:

.end
