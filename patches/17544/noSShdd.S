.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:

#============================================================================
# fix what we messed up before                                            
#============================================================================
	KMAKEPATCH SataDiskAuthenticateDevice
0:
	.long SataDiskAuthenticateDeviceInst1
	.long SataDiskAuthenticateDeviceInst2
9:

#============================================================================
# invert logic of li        r3, 1
#============================================================================
	KMAKEPATCH SataDiskAuthenticateDeviceInv1
0:
	li		r3, 0
9:

#============================================================================
# invert logic of li        r3, 0
#============================================================================
	KMAKEPATCH SataDiskAuthenticateDeviceInv2
0:
	li		r3, 1
9:

.end
