.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:
#;============================================================================
#; bl XeKeysFcrtLoad                                                          
#;============================================================================
	KMAKEPATCH AddonNoFcrtPhase1Patch
0:
	nop
9:

#;============================================================================
#;   disable kv policy for dvd drives  (fcrt.bin) ALTERNATE NEEDS TEST        
#;============================================================================
	MAKEPATCH NoFcrtKvPatch
0:
	cmplwi	cr6, %r10, 0xF
	nop
	li		%r31, 0
9:



.end
