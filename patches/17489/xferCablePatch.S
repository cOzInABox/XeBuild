.include "../macros.S"
.include "patches_offsets.S"
	.globl _start
_start:
#;============================================================================
# Device USB\VID_05E3&PID_0718  black "slim 360 hdd adapter" from china
# Device USB\VID_11B0&PID_6518  USB 3.0 enclosure
# Device USB\VID_1F75&PID_0621	mustache generic usb enclosure
# transfer cable	152D 2339		152d JMicron Technology Corp.	2339 JM20339 SATA Bridge
# 0x152D2339 is the VID/PID of the transfer cable
# .text:800E5090         lis       r10, 0x152D # 0x152D2339
# .text:800E5094         ori       r10, r10, 0x2339 # 0x152D2339
#;============================================================================
	KMAKEPATCH 0x800E5090
	# .set NewVID, 0x05E3
	# .set NewPID, 0x0718
	# .set NewPIDVID, 0x1F750621 # mustache generic usb enclosure
	# .set NewPIDVID, 0x05E30718 # my own black "slim 360 hdd adapter" from china
	.set NewPIDVID, 0x11B06518 # USB 3.0 enclosure
	
0:
	lis       r10, NewPIDVID@h
	ori       r10, r10, NewPIDVID@l
	# lis       r10, NewVID
	# ori       r10, r10, NewPID
9:




.end
