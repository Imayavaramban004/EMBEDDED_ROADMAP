/**
 * @file   startupstm32f4.c
 * @brief  Startup / reset handler for STM32F4xx (Cortex-M4)
 *
 * Responsibilities
 * ----------------
 *  1. Provide the interrupt vector table placed at address 0x0800_0000
 *     (.isr_vector section, mapped by the linker script).
 *  2. Implement Reset_Handler:
 *       a. Copy initialised data (.data) from flash LMA → SRAM VMA.
 *       b. Zero-fill uninitialised data (.bss).
 *       c. Call SystemInit() for clock/FPU configuration (weak no-op here).
 *       d. Call main().
 *  3. Supply a weak Default_Handler that all unused IRQs alias to.
 *
 * Naming convention
 * -----------------
 *  Handler names follow the CMSIS/ST HAL convention: <Peripheral>_IRQHandler
 *  (e.g. TIM2_IRQHandler).  This lets application code override any handler
 *  by simply defining a function with the same name — no linker flag needed.
 *
 * @note   Compile with -O0 or -Og.  The Reset_Handler is marked
 *         __attribute__((optimize("O0"))) so the copy/zero loops are never
 *         optimised away even in release builds.
 *
 * References
 *  - PM0214  (Cortex-M4 Programming Manual), Table 2-4  — vector table layout
 *  - RM0090  (STM32F446 Reference Manual), §10           — interrupt map
 *  - ARM IHI0042F (Cortex-M ABI)                        — Handler naming
 */

#include <stdint.h>   /* uint32_t, uint8_t                                  */
#include <stddef.h>   /* size_t                                              */

/* =========================================================================
 * Stack configuration
 * The stack pointer initial value is the first entry of the vector table.
 * We derive it from linker-exported symbols so it stays consistent with
 * the linker script even if SRAM size changes.
 * ========================================================================= */

/** Top-of-stack symbol exported by the linker script (end of SRAM). */
extern uint32_t _estack; /* defined in stm32f4.ld                          */

/* =========================================================================
 * Linker-script symbols for C runtime initialisation
 *
 *  Flash layout (Load Memory Address):
 *    _stext  → start of .text (= start of flash image)
 *    _etext  → end  of .text / start of .data initialisation values
 *
 *  SRAM layout (Virtual Memory Address):
 *    _sdata  → start of .data in SRAM
 *    _edata  → end   of .data in SRAM
 *    _sbss   → start of .bss  in SRAM
 *    _ebss   → end   of .bss  in SRAM
 * ========================================================================= */
extern uint32_t _etext;   /**< End of .text in flash (= LMA of .data)      */
extern uint32_t _sdata;   /**< Start of .data in SRAM (VMA)                */
extern uint32_t _edata;   /**< End   of .data in SRAM (VMA)                */
extern uint32_t _sbss;    /**< Start of .bss  in SRAM                      */
extern uint32_t _ebss;    /**< End   of .bss  in SRAM                      */

/* =========================================================================
 * Forward declarations
 * ========================================================================= */

/** Application entry point (defined in main.c). */
int  main(void);

/**
 * @brief  Optional system initialisation hook (clock, FPU, MPU).
 *
 * Provided here as a weak no-op.  Override in system_stm32f4xx.c or
 * any other translation unit to configure clocks before main() runs.
 */
void SystemInit(void) __attribute__((weak));

/** Reset handler — first code executed after power-on or reset. */
void Reset_Handler(void);

/* =========================================================================
 * Default / weak handler
 *
 * Every IRQ that has no application-defined handler falls through to this
 * infinite loop.  Mark it __attribute__((noreturn)) so the compiler knows
 * it never returns, enabling better code-gen for callers.
 *
 * In production replace the loop with a fault log + watchdog kick, or an
 * NVIC_SystemReset().
 * ========================================================================= */
void Default_Handler(void) __attribute__((noreturn));

/* -------------------------------------------------------------------------
 * WEAK_HANDLER — convenience macro used for every IRQ alias.
 *
 * Why three attributes together?
 *   weak       — lets the application override by defining the same name.
 *   alias      — makes the symbol an alias for Default_Handler at link time.
 *   noreturn   — required because Default_Handler itself is __attribute__
 *                ((noreturn)).  GCC [-Wmissing-attributes] warns when an
 *                alias is "less restrictive" than its target, i.e. the
 *                target has noreturn but the alias does not.  Adding
 *                noreturn here silences all those warnings *and* lets the
 *                compiler elide dead code after any call through the alias.
 * ------------------------------------------------------------------------- */
#define WEAK_HANDLER(name) \
    void name(void) __attribute__((weak, alias("Default_Handler"), noreturn))

/* -------------------------------------------------------------------------
 * Cortex-M4 system exceptions  (weak aliases → Default_Handler)
 * Names match CMSIS so application overrides require no extra glue.
 * ------------------------------------------------------------------------- */
WEAK_HANDLER(NMI_Handler);
WEAK_HANDLER(HardFault_Handler);
WEAK_HANDLER(MemManage_Handler);
WEAK_HANDLER(BusFault_Handler);
WEAK_HANDLER(UsageFault_Handler);
WEAK_HANDLER(SVC_Handler);
WEAK_HANDLER(DebugMon_Handler);
WEAK_HANDLER(PendSV_Handler);
WEAK_HANDLER(SysTick_Handler);


/* -------------------------------------------------------------------------
 * External / peripheral IRQ handlers  — IRQ0 … IRQ96  (STM32F446xx)
 * All default to the catch-all Default_Handler via weak alias.
 * An application ISR simply defines e.g.:
 *     void TIM2_IRQHandler(void) { ... }
 * and the linker replaces the weak alias automatically.
 * ------------------------------------------------------------------------- */
/* --- IRQ  0–10 : Watchdog, Power, RTC, Flash, RCC, EXTI 0-4 --- */
WEAK_HANDLER(WWDG_IRQHandler);
WEAK_HANDLER(PVD_IRQHandler);
WEAK_HANDLER(TAMP_STAMP_IRQHandler);
WEAK_HANDLER(RTC_WKUP_IRQHandler);
WEAK_HANDLER(FLASH_IRQHandler);
WEAK_HANDLER(RCC_IRQHandler);
WEAK_HANDLER(EXTI0_IRQHandler);
WEAK_HANDLER(EXTI1_IRQHandler);
WEAK_HANDLER(EXTI2_IRQHandler);
WEAK_HANDLER(EXTI3_IRQHandler);
WEAK_HANDLER(EXTI4_IRQHandler);

/* --- IRQ 11–17 : DMA1 streams 0-6 --- */
WEAK_HANDLER(DMA1_Stream0_IRQHandler);
WEAK_HANDLER(DMA1_Stream1_IRQHandler);
WEAK_HANDLER(DMA1_Stream2_IRQHandler);
WEAK_HANDLER(DMA1_Stream3_IRQHandler);
WEAK_HANDLER(DMA1_Stream4_IRQHandler);
WEAK_HANDLER(DMA1_Stream5_IRQHandler);
WEAK_HANDLER(DMA1_Stream6_IRQHandler);

/* --- IRQ 18 : ADC --- */
WEAK_HANDLER(ADC_IRQHandler);

/* --- IRQ 19–22 : CAN1 --- */
WEAK_HANDLER(CAN1_TX_IRQHandler);
WEAK_HANDLER(CAN1_RX0_IRQHandler);
WEAK_HANDLER(CAN1_RX1_IRQHandler);
WEAK_HANDLER(CAN1_SCE_IRQHandler);

/* --- IRQ 23 : EXTI 9-5 --- */
WEAK_HANDLER(EXTI9_5_IRQHandler);

/* --- IRQ 24–27 : TIM1 --- */
WEAK_HANDLER(TIM1_BRK_TIM9_IRQHandler);
WEAK_HANDLER(TIM1_UP_TIM10_IRQHandler);
WEAK_HANDLER(TIM1_TRG_COM_TIM11_IRQHandler);
WEAK_HANDLER(TIM1_CC_IRQHandler);

/* --- IRQ 28–30 : TIM2/3/4 --- */
WEAK_HANDLER(TIM2_IRQHandler);
WEAK_HANDLER(TIM3_IRQHandler);
WEAK_HANDLER(TIM4_IRQHandler);

/* --- IRQ 31–34 : I2C1/2 --- */
WEAK_HANDLER(I2C1_EV_IRQHandler);
WEAK_HANDLER(I2C1_ER_IRQHandler);
WEAK_HANDLER(I2C2_EV_IRQHandler);
WEAK_HANDLER(I2C2_ER_IRQHandler);

/* --- IRQ 35–36 : SPI1/2 --- */
WEAK_HANDLER(SPI1_IRQHandler);
WEAK_HANDLER(SPI2_IRQHandler);

/* --- IRQ 37–39 : USART1/2/3 --- */
WEAK_HANDLER(USART1_IRQHandler);
WEAK_HANDLER(USART2_IRQHandler);
WEAK_HANDLER(USART3_IRQHandler);

/* --- IRQ 40–42 : EXTI15-10, RTC Alarm, OTG_FS wakeup --- */
WEAK_HANDLER(EXTI15_10_IRQHandler);
WEAK_HANDLER(RTC_Alarm_IRQHandler);
WEAK_HANDLER(OTG_FS_WKUP_IRQHandler);

/* --- IRQ 43–46 : TIM8 --- */
WEAK_HANDLER(TIM8_BRK_TIM12_IRQHandler);
WEAK_HANDLER(TIM8_UP_TIM13_IRQHandler);
WEAK_HANDLER(TIM8_TRG_COM_TIM14_IRQHandler);
WEAK_HANDLER(TIM8_CC_IRQHandler);

/* --- IRQ 47 : DMA1 stream 7 --- */
WEAK_HANDLER(DMA1_Stream7_IRQHandler);

/* --- IRQ 48–49 : FSMC, SDIO --- */
WEAK_HANDLER(FSMC_IRQHandler);
WEAK_HANDLER(SDIO_IRQHandler);

/* --- IRQ 50–51 : TIM5, SPI3 --- */
WEAK_HANDLER(TIM5_IRQHandler);
WEAK_HANDLER(SPI3_IRQHandler);

/* --- IRQ 52–53 : UART4/5 --- */
WEAK_HANDLER(UART4_IRQHandler);
WEAK_HANDLER(UART5_IRQHandler);

/* --- IRQ 54–55 : TIM6_DAC, TIM7 --- */
WEAK_HANDLER(TIM6_DAC_IRQHandler);
WEAK_HANDLER(TIM7_IRQHandler);

/* --- IRQ 56–60 : DMA2 streams 0-4 --- */
WEAK_HANDLER(DMA2_Stream0_IRQHandler);
WEAK_HANDLER(DMA2_Stream1_IRQHandler);
WEAK_HANDLER(DMA2_Stream2_IRQHandler);
WEAK_HANDLER(DMA2_Stream3_IRQHandler);
WEAK_HANDLER(DMA2_Stream4_IRQHandler);

/* --- IRQ 61–62 : Reserved (ETH, ETH_WKUP on other STM32F4 variants) --- */

/* --- IRQ 63–66 : CAN2 --- */
WEAK_HANDLER(CAN2_TX_IRQHandler);
WEAK_HANDLER(CAN2_RX0_IRQHandler);
WEAK_HANDLER(CAN2_RX1_IRQHandler);
WEAK_HANDLER(CAN2_SCE_IRQHandler);

/* --- IRQ 67 : OTG_FS --- */
WEAK_HANDLER(OTG_FS_IRQHandler);

/* --- IRQ 68–70 : DMA2 streams 5-7 --- */
WEAK_HANDLER(DMA2_Stream5_IRQHandler);
WEAK_HANDLER(DMA2_Stream6_IRQHandler);
WEAK_HANDLER(DMA2_Stream7_IRQHandler);

/* --- IRQ 71 : USART6 --- */
WEAK_HANDLER(USART6_IRQHandler);

/* --- IRQ 72–73 : I2C3 --- */
WEAK_HANDLER(I2C3_EV_IRQHandler);
WEAK_HANDLER(I2C3_ER_IRQHandler);

/* --- IRQ 74–77 : OTG_HS --- */
WEAK_HANDLER(OTG_HS_EP1_OUT_IRQHandler);
WEAK_HANDLER(OTG_HS_EP1_IN_IRQHandler);
WEAK_HANDLER(OTG_HS_WKUP_IRQHandler);
WEAK_HANDLER(OTG_HS_IRQHandler);

/* --- IRQ 78 : DCMI --- */
WEAK_HANDLER(DCMI_IRQHandler);

/* --- IRQ 79–80 : Reserved --- */

/* --- IRQ 81 : FPU --- */
WEAK_HANDLER(FPU_IRQHandler);

/* --- IRQ 82–83 : Reserved --- */

/* --- IRQ 84 : SPI4 --- */
WEAK_HANDLER(SPI4_IRQHandler);

/* --- IRQ 85–86 : Reserved --- */

/* --- IRQ 87 : SAI1 --- */
WEAK_HANDLER(SAI1_IRQHandler);

/* --- IRQ 88–90 : Reserved --- */

/* --- IRQ 91–96 : SAI2, QuadSPI, HDMI-CEC, SPDIF-RX, FMPI2C1 --- */
WEAK_HANDLER(SAI2_IRQHandler);
WEAK_HANDLER(QuadSPI_IRQHandler);
WEAK_HANDLER(HDMI_CEC_IRQHandler);
WEAK_HANDLER(SPDIF_RX_IRQHandler);
WEAK_HANDLER(FMPI2C1_EV_IRQHandler);
WEAK_HANDLER(FMPI2C1_ER_IRQHandler);

/* =========================================================================
 * Interrupt Vector Table
 *
 * Placed in section ".isr_vector" — the linker script maps this to the
 * very start of flash (0x0800_0000).  Declared `const` so the table lives
 * in flash, not SRAM, saving 400+ bytes of precious RAM.
 *
 * Layout: [0] Initial SP, [1] Reset, [2..15] Core exceptions,
 *         [16..112] External IRQs  (PM0214 Table 2-4)
 * ========================================================================= */
typedef void (*const IRQHandler_t)(void); /**< Function-pointer type for handlers */

/** @cond — suppress Doxygen for the raw table */
__attribute__((section(".isr_vector"), used))
IRQHandler_t vector_table[] =
{
    /* Stack pointer initial value — must be first */
    (IRQHandler_t)&_estack,

    /* --- Cortex-M4 core exceptions (offsets 0x04 – 0x3C) --- */
    Reset_Handler,           /* 1  Reset                                    */
    NMI_Handler,             /* 2  Non-maskable interrupt                   */
    HardFault_Handler,       /* 3  Hard fault                               */
    MemManage_Handler,       /* 4  Memory management fault (MPU)            */
    BusFault_Handler,        /* 5  Prefetch / data bus fault                */
    UsageFault_Handler,      /* 6  Undefined instruction / unaligned access */
    0, 0, 0, 0,              /* 7–10  Reserved                              */
    SVC_Handler,             /* 11 Supervisor call (SVC / SWI)              */
    DebugMon_Handler,        /* 12 Debug monitor                            */
    0,                       /* 13 Reserved                                 */
    PendSV_Handler,          /* 14 Pendable service request                 */
    SysTick_Handler,         /* 15 System tick timer                        */

    /* --- External / peripheral IRQs (offsets 0x40 +) --- */
    WWDG_IRQHandler,             /*  0  Watchdog                            */
    PVD_IRQHandler,              /*  1  PVD via EXTI line 16                */
    TAMP_STAMP_IRQHandler,       /*  2  Tamper + Timestamp                  */
    RTC_WKUP_IRQHandler,         /*  3  RTC wakeup via EXTI line 22         */
    FLASH_IRQHandler,            /*  4  Flash global                        */
    RCC_IRQHandler,              /*  5  RCC global                          */
    EXTI0_IRQHandler,            /*  6  EXTI line 0                         */
    EXTI1_IRQHandler,            /*  7  EXTI line 1                         */
    EXTI2_IRQHandler,            /*  8  EXTI line 2                         */
    EXTI3_IRQHandler,            /*  9  EXTI line 3                         */
    EXTI4_IRQHandler,            /* 10  EXTI line 4                         */
    DMA1_Stream0_IRQHandler,     /* 11  DMA1 stream 0                       */
    DMA1_Stream1_IRQHandler,     /* 12  DMA1 stream 1                       */
    DMA1_Stream2_IRQHandler,     /* 13  DMA1 stream 2                       */
    DMA1_Stream3_IRQHandler,     /* 14  DMA1 stream 3                       */
    DMA1_Stream4_IRQHandler,     /* 15  DMA1 stream 4                       */
    DMA1_Stream5_IRQHandler,     /* 16  DMA1 stream 5                       */
    DMA1_Stream6_IRQHandler,     /* 17  DMA1 stream 6                       */
    ADC_IRQHandler,              /* 18  ADC1/2/3 global                     */
    CAN1_TX_IRQHandler,          /* 19  CAN1 TX                             */
    CAN1_RX0_IRQHandler,         /* 20  CAN1 RX0                            */
    CAN1_RX1_IRQHandler,         /* 21  CAN1 RX1                            */
    CAN1_SCE_IRQHandler,         /* 22  CAN1 SCE                            */
    EXTI9_5_IRQHandler,          /* 23  EXTI lines 9-5                      */
    TIM1_BRK_TIM9_IRQHandler,    /* 24  TIM1 break + TIM9 global            */
    TIM1_UP_TIM10_IRQHandler,    /* 25  TIM1 update + TIM10 global          */
    TIM1_TRG_COM_TIM11_IRQHandler,/* 26  TIM1 trigger/commutation + TIM11   */
    TIM1_CC_IRQHandler,          /* 27  TIM1 capture/compare                */
    TIM2_IRQHandler,             /* 28  TIM2 global                         */
    TIM3_IRQHandler,             /* 29  TIM3 global                         */
    TIM4_IRQHandler,             /* 30  TIM4 global                         */
    I2C1_EV_IRQHandler,          /* 31  I2C1 event                          */
    I2C1_ER_IRQHandler,          /* 32  I2C1 error                          */
    I2C2_EV_IRQHandler,          /* 33  I2C2 event                          */
    I2C2_ER_IRQHandler,          /* 34  I2C2 error                          */
    SPI1_IRQHandler,             /* 35  SPI1 global                         */
    SPI2_IRQHandler,             /* 36  SPI2 global                         */
    USART1_IRQHandler,           /* 37  USART1 global                       */
    USART2_IRQHandler,           /* 38  USART2 global                       */
    USART3_IRQHandler,           /* 39  USART3 global                       */
    EXTI15_10_IRQHandler,        /* 40  EXTI lines 15-10                    */
    RTC_Alarm_IRQHandler,        /* 41  RTC alarm A+B via EXTI line 17      */
    OTG_FS_WKUP_IRQHandler,      /* 42  USB OTG FS wakeup via EXTI line 18  */
    TIM8_BRK_TIM12_IRQHandler,   /* 43  TIM8 break + TIM12 global           */
    TIM8_UP_TIM13_IRQHandler,    /* 44  TIM8 update + TIM13 global          */
    TIM8_TRG_COM_TIM14_IRQHandler,/* 45  TIM8 trigger/commutation + TIM14   */
    TIM8_CC_IRQHandler,          /* 46  TIM8 capture/compare                */
    DMA1_Stream7_IRQHandler,     /* 47  DMA1 stream 7                       */
    FSMC_IRQHandler,             /* 48  FSMC global                         */
    SDIO_IRQHandler,             /* 49  SDIO global                         */
    TIM5_IRQHandler,             /* 50  TIM5 global                         */
    SPI3_IRQHandler,             /* 51  SPI3 global                         */
    UART4_IRQHandler,            /* 52  UART4 global                        */
    UART5_IRQHandler,            /* 53  UART5 global                        */
    TIM6_DAC_IRQHandler,         /* 54  TIM6 global + DAC1/2 underrun error */
    TIM7_IRQHandler,             /* 55  TIM7 global                         */
    DMA2_Stream0_IRQHandler,     /* 56  DMA2 stream 0                       */
    DMA2_Stream1_IRQHandler,     /* 57  DMA2 stream 1                       */
    DMA2_Stream2_IRQHandler,     /* 58  DMA2 stream 2                       */
    DMA2_Stream3_IRQHandler,     /* 59  DMA2 stream 3                       */
    DMA2_Stream4_IRQHandler,     /* 60  DMA2 stream 4                       */
    0,                           /* 61  Reserved (ETH on other variants)    */
    0,                           /* 62  Reserved (ETH_WKUP on other variants)*/
    CAN2_TX_IRQHandler,          /* 63  CAN2 TX                             */
    CAN2_RX0_IRQHandler,         /* 64  CAN2 RX0                            */
    CAN2_RX1_IRQHandler,         /* 65  CAN2 RX1                            */
    CAN2_SCE_IRQHandler,         /* 66  CAN2 SCE                            */
    OTG_FS_IRQHandler,           /* 67  USB OTG FS global                   */
    DMA2_Stream5_IRQHandler,     /* 68  DMA2 stream 5                       */
    DMA2_Stream6_IRQHandler,     /* 69  DMA2 stream 6                       */
    DMA2_Stream7_IRQHandler,     /* 70  DMA2 stream 7                       */
    USART6_IRQHandler,           /* 71  USART6 global                       */
    I2C3_EV_IRQHandler,          /* 72  I2C3 event                          */
    I2C3_ER_IRQHandler,          /* 73  I2C3 error                          */
    OTG_HS_EP1_OUT_IRQHandler,   /* 74  USB OTG HS EP1 OUT global           */
    OTG_HS_EP1_IN_IRQHandler,    /* 75  USB OTG HS EP1 IN global            */
    OTG_HS_WKUP_IRQHandler,      /* 76  USB OTG HS wakeup via EXTI line 20  */
    OTG_HS_IRQHandler,           /* 77  USB OTG HS global                   */
    DCMI_IRQHandler,             /* 78  DCMI global                         */
    0,                           /* 79  Reserved                            */
    0,                           /* 80  Reserved                            */
    FPU_IRQHandler,              /* 81  FPU global                          */
    0,                           /* 82  Reserved                            */
    0,                           /* 83  Reserved                            */
    SPI4_IRQHandler,             /* 84  SPI4 global                         */
    0,                           /* 85  Reserved                            */
    0,                           /* 86  Reserved                            */
    SAI1_IRQHandler,             /* 87  SAI1 global                         */
    0,                           /* 88  Reserved                            */
    0,                           /* 89  Reserved                            */
    0,                           /* 90  Reserved                            */
    SAI2_IRQHandler,             /* 91  SAI2 global                         */
    QuadSPI_IRQHandler,          /* 92  QuadSPI global                      */
    HDMI_CEC_IRQHandler,         /* 93  HDMI-CEC global                     */
    SPDIF_RX_IRQHandler,         /* 94  SPDIF-RX global                     */
    FMPI2C1_EV_IRQHandler,       /* 95  FMPI2C1 event                       */
    FMPI2C1_ER_IRQHandler,       /* 96  FMPI2C1 error                       */
};
/** @endcond */

/* =========================================================================
 * SystemInit — weak no-op
 *
 * Override this in system_stm32f4xx.c to configure PLL, HSE, FPU, MPU
 * before the C runtime is fully up.  Called from Reset_Handler before main.
 * ========================================================================= */
void SystemInit(void)
{
    /* Default: run on internal HSI at 16 MHz, no FPU, no MPU.
     * Replace with full clock + FPU setup for production firmware.    */
}

/* =========================================================================
 * Reset_Handler
 *
 * Executed immediately after every reset (power-on, pin reset, software
 * reset, watchdog reset).  Must not call any library function before the
 * BSS and data sections are initialised.
 *
 * Marked optimize("O0") to guarantee the copy loops are never elided,
 * even when the enclosing translation unit is compiled with -O2/-O3.
 * ========================================================================= */
__attribute__((optimize("O0")))
void Reset_Handler(void)
{
    /* ------------------------------------------------------------------
     * Step 1 — Copy initialised data (.data) from flash to SRAM.
     *
     *  Source (LMA): _etext … _etext + sizeof(.data)  — immediately
     *                after the .text section in flash.
     *  Destination (VMA): _sdata … _edata              — in SRAM.
     *
     * Byte-granularity copy via uint8_t pointers keeps the loop correct
     * even for sections that are not word-aligned.
     * ------------------------------------------------------------------ */
    uint32_t data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint8_t *src        = (uint8_t *)&_etext;   /* flash LMA of .data    */
    uint8_t *dest       = (uint8_t *)&_sdata;   /* SRAM  VMA of .data    */

    for (uint32_t i = 0U; i < data_size; i++) {
        dest[i] = src[i];
    }

    /* ------------------------------------------------------------------
     * Step 2 — Zero-initialise the .bss section.
     *
     * The C standard requires all objects with static storage duration
     * that have no explicit initialiser to be zero at program start.
     * ------------------------------------------------------------------ */
    uint32_t bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    uint8_t *bss       = (uint8_t *)&_sbss;

    for (uint32_t i = 0U; i < bss_size; i++) {
        bss[i] = 0U;
    }

    /* ------------------------------------------------------------------
     * Step 3 — Hardware / clock initialisation hook.
     *
     * SystemInit() is a weak no-op here; override it to configure
     * clocks, FPU, and MPU before reaching main().
     * ------------------------------------------------------------------ */
    SystemInit();

    /* ------------------------------------------------------------------
     * Step 4 — Call application entry point.
     *
     * On bare-metal, main() should never return.  If it does (e.g. in a
     * test harness), spin in Default_Handler instead of crashing into
     * undefined memory.
     * ------------------------------------------------------------------ */
    (void)main();

    /* Unreachable on production firmware — trap if main() ever returns. */
    Default_Handler();
}

/* =========================================================================
 * Default_Handler
 *
 * Catch-all for any interrupt that has no application-defined handler.
 * In production: log the active IRQ from SCB->ICSR[8:0], record it in
 * a non-volatile fault register, then either reset or halt.
 * ========================================================================= */
void Default_Handler(void)
{
    /* Optional debug aid: set a breakpoint here in your debugger.
     * The IPSR register tells you which IRQ fired:
     *   volatile uint32_t ipsr = __get_IPSR();
     */
    for (;;) {
        /* Spin forever — watchdog will bite if enabled. */
    }
}
