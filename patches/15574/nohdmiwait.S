.include "../macros.S"
	.globl _start
_start:

#;============================================================================
#; no timeout waiting for HDMI connection...      HalpReadCurrentAVPack       
#;============================================================================
	KMAKEPATCH 0x801553F0
	.set hdmiReturnAddr, 0x8015543C
0:
	KMAKEBRANCH hdmiReturnAddr
9:

.end
