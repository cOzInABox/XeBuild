.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:

#============================================================================
# disable wired lan driver entry                                             
#============================================================================
	# KMAKEPATCH AddonNoWiredLanPatch+4
# 0:
	# nop
# 9:

#============================================================================
# disables NicRegisterDevice
	# KMAKEPATCH 0x800D2E34
# 0:
	# blr
# 9:
# disables NicUnregisterDevice - causes shutdown problems
	# KMAKEPATCH 0x8010488C
# 0:
	# blr
# 9:
#============================================================================

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
