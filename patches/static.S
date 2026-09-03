.set SPR_XER, 0x1 			# Fixed-Point Exception Register
.set SPR_LR, 0x8 			# Link Register
.set SPR_CTR, 0x9 			# Count Register
.set SPR_DSISR, 0x12 		# Data Storage Interrupt Status Register
.set SPR_DAR, 0x13 			# Data Address Register
.set SPR_DEC, 0x16 			# Decrementer Register
.set SPR_SDR1, 0x19 		# Storage Description Register 1
.set SPR_SRR0, 0x1a 		# Machine Status Save/Restore Register 0
.set SPR_SRR1, 0x1b 		# Machine Status Save/Restore Register 1
.set SPR_ACCR, 0x1d 		# Address Compare Control Register
.set SPR_CTRL_R, 0x88 		# Control Register (Read Only)
.set SPR_CTRL_W, 0x98 		# Control Register (Write Only)
.set SPR_VRSAVE, 0x100 		# VXU Register Save
.set SPR_SPRG3_R, 0x103 	# Software Use Special Purpose Register 3 (Read Only)
.set SPR_TBL_R, 0x10c 		# Time Base Register Lower (Read Only)
.set SPR_TBU_R, 0x10d 		# Time Base Register Upper (Read Only) 
.set SPR_SPRG0, 0x110 		# Special Purpose General Register 0
.set SPR_SPRG1, 0x111 		# Special Purpose General Register 1
.set SPR_SPRG2, 0x112 		# Special Purpose General Register 2
.set SPR_SPRG3, 0x113 		# Special Purpose General Register 3
.set SPR_TBL_W, 0x11c 		# Time Base Register Lower (Write Only)
.set SPR_TBU_W, 0x11d 		# Time Base Register Upper (Write Only) 
.set SPR_PVR, 0x11f 		# Processor Version Register (Read Only)
.set SPR_HSPRG0, 0x130 		# Hypervisor Software Use Special Purpose Register 0
.set SPR_HSPRG1, 0x131 		# Hypervisor Software Use Special Purpose Register 1
.set SPR_HDEC, 0x136 		# Hypervisor Decrementer Register
.set SPR_HIOR, 0x137 		# Hardware Interrupt Offset Register
.set SPR_RMOR, 0x138 		# Real Mode Offset Register
.set SPR_HRMOR, 0x139 		# Hypervisor Real Mode Offset Register
.set SPR_HSRR0, 0x13a 		# Hypervisor Machine Status Save/Restore Register 0
.set SPR_HSRR1, 0x13b 		# Hypervisor Machine Status Save/Restore Register 1
.set SPR_LPCR, 0x13e 		# Logical Partition Control Register
.set SPR_LPIDR, 0x13f 		# Logical Partition Identity Register
.set SPR_TSRL, 0x380 		# Thread Status Register Local
.set SPR_TSRR, 0x381 		# Thread Status Register Remote (Read Only)
.set SPR_TSCR, 0x399 		# Thread Switch Control Register
.set SPR_TTR, 0x39a 		# Thread Switch Timeout Register
.set SPR_TLB_HINT, 0x3b2 	# Translation Lookaside Buffer Index Hint Register
.set SPR_TLB_INDEX, 0x3b3	# Translation Lookaside Buffer Index Register (Read Only)
.set SPR_TLB_VPN, 0x3b4 	# Translation Lookaside Buffer Virtual-Page Number Register
.set SPR_TLB_RPN, 0x3b5 	# Translation Lookaside Buffer Real-Page Number Register
.set SPR_TLB_RMT, 0x3b7 	# Translation Lookaside Buffer RMT (replacement management table) Register
.set SPR_DRSR0, 0x3b8 		# Data Range Start Register 0
.set SPR_DRMR0, 0x3b9 		# Data Range Mask Register 0
.set SPR_DCIDR0, 0x3ba		# Data Class ID Register 0
.set SPR_DRSR1, 0x3bb 		# Data Range Start Register 1
.set SPR_DRMR1, 0x3bc 		# Data Range Mask Register 1
.set SPR_DCIDR1, 0x3bd 		# Data Class ID Register 1
.set SPR_IRSR0, 0x3d0 		# Instruction Range Start Register 0
.set SPR_IRMR0, 0x3d1 		# Instruction Range Mask Register 0
.set SPR_ICIDR0, 0x3d2 		# Instruction Class ID Register 0
.set SPR_IRSR1, 0x3d3 		# Instruction Range Start Register 1
.set SPR_IRMR1, 0x3d4 		# Instruction Range Mask Register 1
.set SPR_ICIDR1, 0x3d5 		# Instruction Class ID Register 0
.set SPR_HID0, 0x3f0 		# Hardware Implementation Register 0
.set SPR_HID1, 0x3f1 		# Hardware Implementation Register 1
.set SPR_IABR, 0x3f2 		# Instruction Address Breakpoint Register
.set SPR_HID4, 0x3f4 		# Hardware Implementation Register 4
.set SPR_DABR, 0x3f5 		# Data Address Breakpoint Register
.set SPR_HID5, 0x3f6 		# Hardware Implementation Register 5
.set SPR_DABRX, 0x3f7 		# Data Address Breakpoint Register Extension
.set SPR_TDABRX, 0x3f8 		# Trigger Data Address Breakpoint Register Extension
.set SPR_HID6, 0x3f9 		# Hardware Implementation Register 6
.set SPR_HID7, 0x3fa 		# Hardware Implementation Register 7
.set SPR_TIABR, 0x3fb 		# Trigger Instruction Address Breakpoint Register
.set SPR_BKMK, 0x3fc 		# BookMark (used by core0 with bookmark trace enable)
.set SPR_TDABR, 0x3fd 		# Trigger Data Address Breakpoint Register
.set SPR_PIR, 0x3ff 		# Processor Identification Register
