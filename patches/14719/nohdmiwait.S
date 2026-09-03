.include "../macros.S"
	.globl _start
_start:

#;============================================================================
#; no timeout waiting for HDMI connection...                                  
#;============================================================================
	KMAKEPATCH 0x8014AB00
	.set hdmiReturnAddr, 0x8014AB4C
0:
	KMAKEBRANCH hdmiReturnAddr
9:

.end
