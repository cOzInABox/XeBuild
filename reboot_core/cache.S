#define CACHE_SIZE 0x80

.globl dcache_flush
dcache_flush:
	lis %r5, 0xFFFF
	ori %r5, %r5, 0xFFF1
	and %r5, %r5, %r3
	subf %r3, %r5, %r3
	add %r4, %r4, %r3
1:
	dcbst %r0, %r5
	addic %r5, %r5, CACHE_SIZE
	subic. %r4, %r4, CACHE_SIZE
	bge 1b
	isync
	blr

.globl dcache_inv
dcache_inv:
	lis %r5, 0xFFFF
	ori %r5, %r5, 0xFFF1
	and %r5, %r5, %r3
	subf %r3, %r5, %r3
	add %r4, %r4, %r3
1:
	dcbf %r0, %r5
	addic %r5, %r5, CACHE_SIZE
	subic. %r4, %r4, CACHE_SIZE
	bge 1b
	isync
	blr

.globl flush_code
flush_code:
	lis %r5, 0xFFFF
	ori %r5, %r5, 0xFFF1
	and %r5, %r5, %r3
	subf %r3, %r5, %r3
	add %r4, %r4, %r3
1:
	dcbst %r0, %r5
	sync
	icbi %r0, %r5
	addic %r5, %r5, CACHE_SIZE
	subic. %r4, %r4, CACHE_SIZE
	bge 1b
	sync
	isync
	blr
