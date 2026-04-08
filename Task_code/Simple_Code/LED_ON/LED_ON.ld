ENTRY(Reset_handler)

MEMORY
{
	FLASH(rx) : ORIGIN = 0x08000000, LENGTH = 512K
	SRAM(rwx) : ORIGIN = 0x20000000, LENGTH = 128K
	BAKSRAM(rwx) : ORIGIN = 0x40024000, LENGTH = 16K
}
SECTIONS
{
	.text :
	{
		*(.isr_vector)
		*(.text)
		*(.rodata)
		_etext = .;
	} > FLASH AT >FLASH
	.data :
	{
		_sdata = .;
		*(.data)
		_edata = .;
	}>SRAM AT> FLASH
	.bss :
	{
		_sbss = .;
		*(.bss)
		*(COMMON)
		_ebss = .;
	} > SRAM

}