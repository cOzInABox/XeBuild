#define FLASH_BASE 	0xC8090000
#define CODE_BASE	0x1C000000
#define PHY_OFFSET 	0x00130360
#define EXEC_SIZE	-1		// (2936/4)	# 0x2DE
#define V(x) 		((x) + 0x80000000 + PHY_OFFSET)
#define P(x) 		((x) + PHY_OFFSET)

// we are at 130360 here.

_start:
.globl _start
. = 0
	.long 0x80131db0, V(ex_code), 0x00000000, 0x02000000		// stack limit, stack pointer, ??, ??
. = 0x10
	.long 0x12000000, 0x80130374, 0x80130374, 0x8013037c		// 10
. = 0x20
	.long 0x8013037c, 0x80125020, 0x00000001, 0x00000000		// 20 ??, KeIdleProcess
. = 0x30
	.long 0x00000005, 0x00000000, 0x00000000, 0xfdffd7ff		// 30

		// our registers are largely prefilled form the stack - see below.
ex_code:
	li 		%r3, 0x10
	rldicr 	%r3, %r3, 56, 7
	std 	%r3, 0(%r7)	// post 0x10

	mtlr 	%r6
	li 		%r4, EXEC_SIZE										// 0x10000 copy size (65536*4 = 256k) - freeboot.bin is only 0xB78 (2936) bytes (0x2DE reps)
	mtctr   %r4

		// Copy from Flash, src = %r5, dst = %r6
flash_loadloop:
	lwz     %r8, 0(%r5)											// Memcopy
	stw     %r8, 0(%r6)
	dcbst   %r0, %r6											// Flush cache to ram
	icbi	%r0, %r6
	sync	0
	isync
	addi    %r6, %r6, 4
	addi    %r5, %r5, 4
	bdnz    flash_loadloop

	li 		%r3, 0x11
	rldicr 	%r3, %r3, 56, 7
	std 	%r3, 0(%r7) // post 0x11

	blr

		// Context restore at 800701c0 loads r0..r12 from here
. = ex_code + 0x50
	.long 0x20000000, 0x00000046+2, 0x00000000, V(ex_code) 		// 50 r0, r1
	.long 0x00000000, 0x00000000, 0x00000000, 0x000000e0		// 60 r2, r3
	.long 0x80000000, P(ex_code), 0x80000200, FLASH_BASE		// 70 r4, r5
	.long 0x80000000, CODE_BASE , 0x80000200, 0x00061010		// 80 r6, r7
	.long 0x80000200, 0xea000000, 0x00000000, 0x04000000		// 90 r8, r9
	.long 0x00000000, 0x01000000, 0x00000000, 0x00000000		// A0 r10, r11
	.long 0x00000000, 0x00000000, 0x00000000, 0x00000000		// B0 r12, r13
	.long 0x00000000, 0x00000000, 0x00000000, 0x00000000		// C0 r14, r15
	.long 0x00000000, 0x00000000, 0x00000000, 0x00000000		// D0 r16, r17
. = ex_code + 0xE0
	.long 0x00000000, 0x80070190, 0x00000000, 0x00000000 		// E0 NIP (next instruction pointer) to context restore
		// some space here :p gcc fills with 0x0
. = ex_code + 0x1a0
	.long 0x00000000, 0x00000000, 0x80070228, 0x80070228 		// 1a0 NIP (next instruction pointer), LR, points to SC in kernel
	.long 0x00000000, 0x00009030 								// 1b0 MSR

. = 0x200
