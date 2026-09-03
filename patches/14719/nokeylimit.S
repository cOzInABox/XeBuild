.include "../macros.S"
	.globl _start
_start:

#;============================================================================
#; don't limit what keys can be gotton from hv                                
#;============================================================================
	MAKEPATCH 0x00007A34
	.set destAddr, 0x00007A54
0:
	MAKEBRANCH destAddr
9:

.end
