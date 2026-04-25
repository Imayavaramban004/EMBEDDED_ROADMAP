/**
 * @file    stm32f4xx.ld
 * @brief   Linker script for STM32F4xx (512K Flash / 128K SRAM / 16K Backup SRAM)
 *
 * Memory map (STM32F411xE / F446xE — adjust LENGTH values for your specific part):
 *
 *   0x00000000  ──  Aliased to Flash or SRAM depending on BOOT pins
 *   0x08000000  ──  FLASH  (512K) — code, rodata, initialisation data image
 *   0x20000000  ──  SRAM   (128K) — stack, heap, .data, .bss
 *   0x40024000  ──  BAKSRAM (16K) — battery-backed SRAM (RTC domain)
 *   0xE0000000  ──  Cortex-M4 core peripherals (SysTick, NVIC, …)
 *
 * Section layout inside Flash:
 *
 *   [ .isr_vector | .text | .rodata | .ARM.extab | .ARM.exidx | .data(LMA) ]
 *
 * Section layout inside SRAM (VMA):
 *
 *   [ .data(VMA) | .bss | ._user_heap_stack ]
 */

/* ── Entry point ─────────────────────────────────────────────────────── */
ENTRY(Reset_Handler)

/* ── Minimum sizes for sanity-check at link time ─────────────────────── */
_Min_Heap_Size  = 0x400;   /* 1 KB  — set to 0 if malloc is not used      */
_Min_Stack_Size = 0x800;   /* 2 KB  — increase for deeper call stacks      */

/* ── Memory regions ──────────────────────────────────────────────────── */
MEMORY
{
    FLASH   (RX)  : ORIGIN = 0x08000000, LENGTH = 512K  /* Adjust for your part  */
    SRAM    (RWX) : ORIGIN = 0x20000000, LENGTH = 128K  /* SRAM1 on F4xx         */
    BAKSRAM (RW)  : ORIGIN = 0x40024000, LENGTH = 16K   /* Battery-backed SRAM   */
}

/* ── Sections ────────────────────────────────────────────────────────── */
SECTIONS
{
    /* ------------------------------------------------------------------ *
     * .isr_vector — must be at Flash origin (0x08000000).                *
     * The Cortex-M core fetches the initial SP and Reset_Handler address  *
     * from offsets 0 and 4 of this table on every reset.                 *
     * ------------------------------------------------------------------ */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))    /* KEEP prevents --gc-sections removal    */
        . = ALIGN(4);
    } > FLASH

    /* ------------------------------------------------------------------ *
     * .text — executable code and read-only data                         *
     * ------------------------------------------------------------------ */
    .text :
    {
        . = ALIGN(4);
        _stext = .;             /* Start of code — used for XIP checks    */

        *(.text)                /* Code from all translation units        */
        *(.text*)               /* Catches .text.funcname (LTO / sections)*/

        /* C++ constructors / destructors called by __libc_init_array     */
        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;             /* End of code                            */
    } > FLASH

    /* ------------------------------------------------------------------ *
     * .rodata — string literals, const globals, jump tables              *
     * ------------------------------------------------------------------ */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    /* ------------------------------------------------------------------ *
     * ARM exception-handling tables                                       *
     * Required for C++ exceptions and for stack unwinding in debuggers.   *
     * Kept even in C-only projects — some RTOS and libc internals use it. *
     * ------------------------------------------------------------------ */
    .ARM.extab :
    {
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > FLASH

    .ARM.exidx :
    {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
        . = ALIGN(4);
    } > FLASH

    /* ------------------------------------------------------------------ *
     * C++ static constructor / destructor function pointer arrays.        *
     * __libc_init_array() (called from Reset_Handler) walks these tables  *
     * to call every global constructor before main().                     *
     * ------------------------------------------------------------------ */
    .preinit_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
        . = ALIGN(4);
    } > FLASH

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
        . = ALIGN(4);
    } > FLASH

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
        . = ALIGN(4);
    } > FLASH

    /* ------------------------------------------------------------------ *
     * .data — initialised global and static variables                     *
     *                                                                      *
     * LMA (Load Memory Address) = Flash  → stored in the binary image     *
     * VMA (Virtual Memory Address) = SRAM → where the CPU accesses it     *
     *                                                                      *
     * Reset_Handler copies [_sdata … _edata) from _lma_data_start in      *
     * Flash to _sdata in SRAM before calling main().                      *
     * ------------------------------------------------------------------ */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;             /* VMA start — copy destination in SRAM   */
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;             /* VMA end                                */
    } > SRAM AT > FLASH

    /*
     * LMA of .data in Flash — passed to the startup copy loop.
     * LOADADDR() returns the LMA of the most recently defined section.
     */
    _lma_data_start = LOADADDR(.data);

    /* ------------------------------------------------------------------ *
     * .bss — zero-initialised globals and statics                         *
     *                                                                      *
     * No Flash image is stored for .bss.  Reset_Handler must zero-fill    *
     * [_sbss … _ebss) before calling main().  The AT > FLASH directive   *
     * is intentionally absent — .bss occupies SRAM only.                 *
     * ------------------------------------------------------------------ */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)               /* Tentative definitions (unoptimised C)  */
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    /* ------------------------------------------------------------------ *
     * .noinit — variables that must survive a soft reset (watchdog,       *
     * power-on-reset flags, crash dump buffers).                          *
     * Neither copied from Flash nor zeroed by startup code.               *
     * Place variables here with __attribute__((section(".noinit")))       *
     * ------------------------------------------------------------------ */
    .noinit (NOLOAD) :
    {
        . = ALIGN(4);
        _snoinit = .;
        *(.noinit)
        *(.noinit*)
        . = ALIGN(4);
        _enoinit = .;
    } > SRAM

    /* ------------------------------------------------------------------ *
     * Battery-backed SRAM — variables that survive a full power cycle     *
     * (requires VBAT connected).  Explicitly placed with:                 *
     *   __attribute__((section(".baksram")))                              *
     * ------------------------------------------------------------------ */
    .baksram (NOLOAD) :
    {
        . = ALIGN(4);
        _sbaksram = .;
        *(.baksram)
        *(.baksram*)
        . = ALIGN(4);
        _ebaksram = .;
    } > BAKSRAM

    /* ------------------------------------------------------------------ *
     * Heap and stack — placed at the top of SRAM.                         *
     *                                                                      *
     * Stack grows downward from the top of SRAM.                          *
     * Heap grows upward from _end.                                        *
     *                                                                      *
     * The ASSERT below catches overflows at link time.  Runtime overflow  *
     * (stack colliding with heap or .bss) requires an MPU or stack        *
     * canary — the linker cannot catch that.                              *
     * ------------------------------------------------------------------ */
    ._user_heap_stack :
    {
        . = ALIGN(8);           /* 8-byte alignment required by AAPCS     */
        _heap_start = .;
        . = . + _Min_Heap_Size;
        . = ALIGN(8);
        _heap_end = .;

        _stack_end = .;         /* Lowest valid stack address (grows down) */
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
        _stack_top = .;         /* Initial SP loaded by Cortex-M on reset  */
    } > SRAM

    /* ── Convenience aliases ─────────────────────────────────────────── */
    PROVIDE(_end      = _ebss);         /* newlib sbrk() heap start       */
    PROVIDE(end       = _ebss);         /* POSIX sbrk() alias             */
    PROVIDE(_sstack   = _stack_end);    /* Lowest stack address           */
    PROVIDE(_estack   = _stack_top);    /* Initial SP (top of SRAM)       */

    /* ── Discard C++ personality routines if not using exceptions ─────── */
    /DISCARD/ :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }

    /* ── Link-time overflow guard ────────────────────────────────────── */
    ASSERT(_stack_top <= ORIGIN(SRAM) + LENGTH(SRAM),
           "ERROR: Stack + heap overflow SRAM — reduce _Min_Stack_Size or _Min_Heap_Size")

    ASSERT(SIZEOF(.isr_vector) >= 0x188,
           "WARNING: ISR vector table appears too small for STM32F4xx (98 vectors * 4 bytes)")
}