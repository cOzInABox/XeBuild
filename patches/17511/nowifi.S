.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:

# disable Nomnil detection in NomnilDetectDeviceAndGetDriver
	KMAKEPATCH NomnilDetectDevicePatch
0:
	li	r3, 0
	blr
9:

# disable Omni detection in OmniDetectDevice
	KMAKEPATCH OmniDetectDevicePatch
0:
	li	r3, 0
	blr
9:

.end
