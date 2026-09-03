.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:

#============================================================================
# disable wired lan driver entry                                             
#============================================================================
	KMAKEPATCH NoWiredLan # 0x800D2A7C
0:
	nop
9:

.end
