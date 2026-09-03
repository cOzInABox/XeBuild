ENTRY(_start)
SECTIONS
{
	. = 0x800000001C000000;
	.text : { *(.text) }
	.data : { *(.data) }
	.sdata : { *(.sdata) }
	.rodata : { *(.rodata)}

	. = 0x800000001C080000;

	bss_start = .;
	.bss : { *(.bss) }
	.sbss : { *(.sbss) }
	bss_end = .;
}
