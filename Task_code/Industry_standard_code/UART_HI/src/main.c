/**
 * @file  main.c
 * @brief UART4 HAL demonstration — STM32F4xx
 *
 * Hardware connections
 * --------------------
 *   PA0  → UART4 TX  (AF8)     RM0090 Table 9
 *   PA1  → UART4 RX  (AF8)     RM0090 Table 9
 *
 * Register-level flow (all values from RM0090 / datasheet)
 * ---------------------------------------------------------
 *
 * STEP 1 — ENABLE PERIPHERAL CLOCKS
 *   RCC  BASE : 0x4002 3800
 *   APB1ENR   : offset 0x40, reset 0x0000 0000
 *     UART4EN   bit 19 → set 1
 *   AHB1ENR   : offset 0x30, reset 0x0000 0000
 *     GPIOAEN   bit  0 → set 1
 *
 * STEP 2 — CONFIGURE GPIOA (PA0 TX, PA1 RX)
 *   GPIOA BASE : 0x4002 0000
 *   MODER      : offset 0x00, reset 0xA800 0000
 *     PA0 [1:0] = 10 (Alternate Function)
 *     PA1 [3:2] = 10 (Alternate Function)
 *   OTYPER     : offset 0x04, reset 0x0000 0000
 *     PA0 bit 0 = 0  (Push-Pull)
 *     PA1 bit 1 = 0  (Push-Pull)
 *   OSPEEDR    : offset 0x08, reset 0x0C00 0000
 *     PA0 [1:0] = 00 (Low Speed)
 *     PA1 [3:2] = 00 (Low Speed)
 *   PUPDR      : offset 0x0C, reset 0x6400 0000
 *     PA0 [1:0] = 01 (Pull-Up)
 *     PA1 [3:2] = 01 (Pull-Up)
 *   AFRL       : offset 0x20, reset 0x0000 0000
 *     PA0 [3:0]  = 1000 (AF8 → UART4)
 *     PA1 [7:4]  = 1000 (AF8 → UART4)
 *
 * STEP 3 — CONFIGURE UART4 PERIPHERAL
 *   UART4 BASE : 0x4000 4C00
 *   (See UART_Config_t below)
 *
 * STEP 4 — SYSTICK INITIALISATION
 *   STK_CTRL  : 0xE000 E010
 *   STK_LOAD  : 0xE000 E014
 *   STK_VAL   : 0xE000 E018
 *   STK_CALIB : 0xE000 E01C
 *
 * STEP 5 — MAIN FUNCTION
 *   - Init SysTick (16 MHz HSI, 1 ms tick)
 *   - Init PA0 (TX) and PA1 (RX)
 *   - Init UART4 transfer
 *   - Transmit UART message
 *   - 1 second delay, repeat
 *
 * Clock assumptions
 * -----------------
 *   System clock = HSI = 16 MHz (hardware default after reset).
 *   APB1 prescaler = 1 → UART4 fCK = 16 MHz.
 *   Adjust HSI_FREQ_HZ if you enable the PLL.
 */

#include "stm32f4xx_uart.h"   /* pulls in stm32f4xx_gpio.h + stm32f4xx_rcc.h */

/* -----------------------------------------------------------------------
 * Board-level constants
 * --------------------------------------------------------------------- */
#define HSI_FREQ_HZ     16000000UL   /* HSI default after reset             */
#define BAUD_RATE       115200UL

/* -----------------------------------------------------------------------
 * STEP 2 — GPIO pin descriptors (const → placed in flash by linker)
 * --------------------------------------------------------------------- */

/**
 * PA0 — UART4 TX
 *   MODER   : 10  (AF)
 *   OTYPER  : 0   (PP)
 *   OSPEEDR : 00  (Low Speed)
 *   PUPDR   : 01  (Pull-Up)
 *   AFRL    : AF8 (UART4)
 */
static const GPIO_Config_t UART4_TX_PA0 = {
    .port_base  = (volatile uint32_t *)GPIOA_BASE,
    .rcc_bit    = RCC_AHB1ENR_GPIOAEN,   /* AHB1ENR bit 0  — STEP 1        */
    .pin        = 0U,
    .mode       = GPIO_MODER_AF,          /* MODER[1:0]  = 10               */
    .otype      = GPIO_OTYPER_PP,         /* OTYPER bit0 = 0                */
    .speed      = GPIO_OSPEEDR_LOW,       /* OSPEEDR[1:0] = 00              */
    .pull       = GPIO_PUPDR_PULLUP,      /* PUPDR[1:0]  = 01               */
    .af         = GPIO_AF8,               /* AFRL[3:0]   = 1000             */
    .init_state = GPIO_PIN_RESET,         /* Unused in AF mode              */
};

/**
 * PA1 — UART4 RX
 *   MODER   : 10  (AF)
 *   OTYPER  : 0   (PP)
 *   OSPEEDR : 00  (Low Speed)
 *   PUPDR   : 01  (Pull-Up)
 *   AFRL    : AF8 (UART4)
 */
static const GPIO_Config_t UART4_RX_PA1 = {
    .port_base  = (volatile uint32_t *)GPIOA_BASE,
    .rcc_bit    = RCC_AHB1ENR_GPIOAEN,   /* AHB1ENR bit 0  — STEP 1        */
    .pin        = 1U,
    .mode       = GPIO_MODER_AF,          /* MODER[3:2]  = 10               */
    .otype      = GPIO_OTYPER_PP,         /* OTYPER bit1 = 0                */
    .speed      = GPIO_OSPEEDR_LOW,       /* OSPEEDR[3:2] = 00              */
    .pull       = GPIO_PUPDR_PULLUP,      /* PUPDR[3:2]  = 01               */
    .af         = GPIO_AF8,               /* AFRL[7:4]   = 1000             */
    .init_state = GPIO_PIN_RESET,         /* Unused in AF mode              */
};

/* -----------------------------------------------------------------------
 * STEP 3 — UART4 peripheral configuration
 *
 * uart_base    : 0x4000 4C00  (UART4_BASE)
 * rcc_apb_reg  : 0x4002 3840  (RCC_BASE + APB1ENR offset 0x40)
 * rcc_en_bit   : bit 19       (UART4EN in APB1ENR)
 * --------------------------------------------------------------------- */
static const UART_Config_t UART4_Cfg = {
    .uart_base    = (volatile uint32_t *)UART4_BASE,
    .rcc_apb_reg  = (volatile uint32_t *)(RCC_BASE + RCC_APB1ENR_OFFSET),
    .rcc_en_bit   = RCC_APB1ENR_UART4EN,  /* APB1ENR bit 19               */
    .pclk_hz      = HSI_FREQ_HZ,          /* APB1 = SYSCLK / 1 at reset   */
    .baud_rate    = BAUD_RATE,
    .word_length  = UART_WORDLEN_8BIT,
    .stop_bits    = UART_STOPBITS_1,
    .parity       = UART_PARITY_NONE,
    .mode         = UART_MODE_TX_RX,
    .hw_flow_ctrl = UART_HW_FLOW_NONE,
};

/* -----------------------------------------------------------------------
 * Error handler
 * --------------------------------------------------------------------- */
static void Error_Handler(void)
{
    while (1) { /* Trap — replace with watchdog or fault log in production */ }
}

/* -----------------------------------------------------------------------
 * STEP 5 — Main function
 * --------------------------------------------------------------------- */
int main(void)
{
    /* ---- STEP 4: SysTick init — 1 ms tick at 16 MHz HSI ---- */
    SysTick_Init(HSI_FREQ_HZ);
    /*
     * STK_LOAD  ← (16000000 / 1000) - 1 = 15999
     * STK_VAL   ← 0  (reset current count)
     * STK_CTRL  ← CLKSOURCE=1 | TICKINT=1 | ENABLE=1
     */

    /* ---- STEP 2: Configure PA0 (TX) and PA1 (RX) to AF8 ---- */
    if (GPIO_Init(&UART4_TX_PA0) != GPIO_OK) {
        Error_Handler();
    }
    /*
     * AHB1ENR  |= (1 << 0)        — GPIOA clock ON
     * MODER    : PA0[1:0]  = 10   — AF
     * OTYPER   : PA0 bit0  = 0    — PP
     * OSPEEDR  : PA0[1:0]  = 00   — Low Speed
     * PUPDR    : PA0[1:0]  = 01   — Pull-Up
     * AFRL     : PA0[3:0]  = 1000 — AF8
     */

    if (GPIO_Init(&UART4_RX_PA1) != GPIO_OK) {
        Error_Handler();
    }
    /*
     * MODER    : PA1[3:2]  = 10   — AF
     * OTYPER   : PA1 bit1  = 0    — PP
     * OSPEEDR  : PA1[3:2]  = 00   — Low Speed
     * PUPDR    : PA1[3:2]  = 01   — Pull-Up
     * AFRL     : PA1[7:4]  = 1000 — AF8
     */

    /* ---- STEP 3: Init UART4 peripheral ---- */
    if (UART_Init(&UART4_Cfg) != UART_OK) {
        Error_Handler();
    }
    /*
     * APB1ENR  |= (1 << 19)       — UART4 clock ON
     * BRR       = 16000000 / 115200 = 138  (≈ 115200 baud)
     * CR2       : STOP[13:12] = 00 — 1 stop bit
     * CR1       : M=0 (8-bit), PCE=0 (no parity), TE=1, RE=1
     * CR1       : UE=1            — UART4 enabled (set last)
     */

    /* ---- STEP 5: Transmit message + 1 second repeat loop ---- */
    UART_TransmitString(&UART4_Cfg, "\r\n");
    UART_TransmitString(&UART4_Cfg, "====================================\r\n");
    UART_TransmitString(&UART4_Cfg, "  STM32F4xx UART4 Demo\r\n");
    UART_TransmitString(&UART4_Cfg, "  TX:PA0  RX:PA1  AF8  115200 8N1\r\n");
    UART_TransmitString(&UART4_Cfg, "====================================\r\n");

    for (;;) {
        /* Transmit UART message */
        UART_TransmitString(&UART4_Cfg, "Hello from UART4!\r\n");

        /* 1 second delay */
        SysTick_DelayMs(1000U);
    }

    return 0; /* Unreachable on bare-metal; satisfies the compiler */
}
