/* Barebones ARM/Raspberry Pi Standalone (kernel) linker script */
/* http://wiki.osdev.org/Raspberry_Pi_Bare_Bones		*/

/* Entry point */
ENTRY(_start)
 
SECTIONS
{
	/* Sets current address to 0x8000 */
	. = 0x8000;
	/* Sets start and text_start to the current address (0x8000) */
	__start = .;


	/************************ Text Section ********************/
	__text_start = .;
	.text :
	{
		/* Put boot section first */
		KEEP(*(.text.boot))
		/* Then put .text */
		*(.text)
	}
	/* align to page size */
	. = ALIGN(4096); 
	__text_end = .;

	/****************** Read-Only Data Section ********************/
	__rodata_start = .;
	.rodata :
	{
		/* Include all rodata sections */
		*(.rodata)
	}
	. = ALIGN(4096); /* align to page size */
	__rodata_end = .;


	/****************** Data Section ********************/
	__data_start = .;
	.data :
	{
		/* Include all data sections */
		*(.data)
	}
	. = ALIGN(4096); /* align to page size */
	__data_end = .;
 
	/****************** BSS Section ********************/
	__bss_start = .;
	.bss :
	{
		bss = .;
		*(.bss)
	}
	. = ALIGN(4096); /* align to page size */
	__bss_end = .;

	/****************** End of executable **************/
	__end = .;
}
