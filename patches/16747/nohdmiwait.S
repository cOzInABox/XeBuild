.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:

#;============================================================================
#; no timeout waiting for HDMI connection...        HalpReadCurrentAVPack     
#;============================================================================
	KMAKEPATCH AddonNoHdmiWaitPatch
0:
	KMAKEBRANCH AddonNoHdmiWaitDestPatch
9:

.end
