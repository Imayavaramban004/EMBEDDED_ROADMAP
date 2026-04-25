/**
 * @file    stm32f4xx_rcc.h
 * @brief   RCC and SysTick register definitions for STM32F4xx series
 * @version 1.0.0
 *
 * Provides base addresses, register offsets, the REG32 access macro,
 * named register shortcuts, and all clock-enable bit definitions for
 * the Reset & Clock Control (RCC) and the Cortex-M4 SysTick timer.
 *
 * Included by
 * -----------
 *   stm32f4xx_gpio.h  — needs AHB1ENR GPIO enable bits
 *   stm32f4xx_uart.h  — needs APB1ENR / APB2ENR UART enable bits
 *
 * Reference
 * ---------
 *   RM0090 Rev 19  §6   RCC
 *   PM0214 Rev 10  §4.4 SysTick
 */
 //Initialization of header guard
#ifndef STM32F4XX_RCC_H
#define STM32F4XX_RCC_H

#include <stdint.h>

/* -----------------------------------------------------------------------
 * Peripheral base addresses  (RM0090 §2.3 Memory Map)
 * --------------------------------------------------------------------- */
#define RCC_BASE        0x40023800UL   /* Reset & Clock Control — AHB1       */
#define SYSTICK_BASE    0xE000E010UL   /* SysTick — Cortex-M4 PPB            */

/* -----------------------------------------------------------------------
 * Register access macro
 * All accesses are volatile to prevent optimisation across I/O boundaries.
 * --------------------------------------------------------------------- */
#define REG32(Base, Offset)  (*(volatile uint32_t *)((Base) + (Offset)))

/* -----------------------------------------------------------------------
 * RCC register offsets (relative to RCC_BASE)  — Table 21, RM0090
 * --------------------------------------------------------------------- */
#define RCC_CR_OFFSET           0x00UL   /* Clock control register                   */
#define RCC_PLLCFGR_OFFSET      0x04UL   /* PLL configuration register               */
#define RCC_CFGR_OFFSET         0x08UL   /* Clock configuration register             */
#define RCC_CIR_OFFSET          0x0CUL   /* Clock interrupt register                 */
#define RCC_AHB1RSTR_OFFSET     0x10UL   /* AHB1 peripheral reset register           */
#define RCC_AHB2RSTR_OFFSET     0x14UL   /* AHB2 peripheral reset register           */
#define RCC_AHB3RSTR_OFFSET     0x18UL   /* AHB3 peripheral reset register           */
/* 0x1C: Reserved                                                                     */
#define RCC_APB1RSTR_OFFSET     0x20UL   /* APB1 peripheral reset register           */
#define RCC_APB2RSTR_OFFSET     0x24UL   /* APB2 peripheral reset register           */
/* 0x28: Reserved                                                                     */
/* 0x2C: Reserved                                                                     */
#define RCC_AHB1ENR_OFFSET      0x30UL   /* AHB1 peripheral clock enable register    */
#define RCC_AHB2ENR_OFFSET      0x34UL   /* AHB2 peripheral clock enable register    */
#define RCC_AHB3ENR_OFFSET      0x38UL   /* AHB3 peripheral clock enable register    */
/* 0x3C: Reserved                                                                     */
#define RCC_APB1ENR_OFFSET      0x40UL   /* APB1 peripheral clock enable register    */
#define RCC_APB2ENR_OFFSET      0x44UL   /* APB2 peripheral clock enable register    */
/* 0x48: Reserved                                                                     */
/* 0x4C: Reserved                                                                     */
#define RCC_AHB1LPENR_OFFSET    0x50UL   /* AHB1 clock enable in low-power mode      */
#define RCC_AHB2LPENR_OFFSET    0x54UL   /* AHB2 clock enable in low-power mode      */
#define RCC_AHB3LPENR_OFFSET    0x58UL   /* AHB3 clock enable in low-power mode      */
/* 0x5C: Reserved                                                                     */
#define RCC_APB1LPENR_OFFSET    0x60UL   /* APB1 clock enable in low-power mode      */
#define RCC_APB2LPENR_OFFSET    0x64UL   /* APB2 clock enable in low-power mode      */
/* 0x68: Reserved                                                                     */
/* 0x6C: Reserved                                                                     */
#define RCC_BDCR_OFFSET         0x70UL   /* Backup domain control register           */
#define RCC_CSR_OFFSET          0x74UL   /* Clock control & status register          */
/* 0x78: Reserved                                                                     */
/* 0x7C: Reserved                                                                     */
#define RCC_SSCGR_OFFSET        0x80UL   /* Spread spectrum clock generation register */
#define RCC_PLLI2SCFGR_OFFSET   0x84UL   /* PLLI2S configuration register            */
#define RCC_PLLSAICFGR_OFFSET   0x88UL   /* PLLSAI configuration register            */
#define RCC_DCKCFGR_OFFSET      0x8CUL   /* Dedicated clocks configuration register  */
#define RCC_CKGATENR_OFFSET     0x90UL   /* Clocks gated enable register             */
#define RCC_DCKCFGR2_OFFSET     0x94UL   /* Dedicated clocks configuration register 2*/

/* -----------------------------------------------------------------------
 * RCC named-register shortcuts
 * --------------------------------------------------------------------- */
#define RCC_CR          REG32(RCC_BASE, RCC_CR_OFFSET)
#define RCC_CFGR        REG32(RCC_BASE, RCC_CFGR_OFFSET)
#define RCC_AHB1ENR     REG32(RCC_BASE, RCC_AHB1ENR_OFFSET)
#define RCC_AHB2ENR     REG32(RCC_BASE, RCC_AHB2ENR_OFFSET)
#define RCC_APB1ENR     REG32(RCC_BASE, RCC_APB1ENR_OFFSET)
#define RCC_APB2ENR     REG32(RCC_BASE, RCC_APB2ENR_OFFSET)
#define RCC_AHB1RSTR    REG32(RCC_BASE, RCC_AHB1RSTR_OFFSET)
#define RCC_APB1RSTR    REG32(RCC_BASE, RCC_APB1RSTR_OFFSET)
#define RCC_APB2RSTR    REG32(RCC_BASE, RCC_APB2RSTR_OFFSET)

/* -----------------------------------------------------------------------
 * RCC_CR bit definitions  (RM0090 §6.3.1)
 * --------------------------------------------------------------------- */
#define RCC_CR_HSION        (1UL << 0)   /* Internal HSI oscillator enable           */
#define RCC_CR_HSIRDY       (1UL << 1)   /* Internal HSI oscillator ready            */
#define RCC_CR_HSEON        (1UL << 16)  /* External HSE oscillator enable           */
#define RCC_CR_HSERDY       (1UL << 17)  /* External HSE oscillator ready            */
#define RCC_CR_HSEBYP       (1UL << 18)  /* HSE clock bypass                         */
#define RCC_CR_CSSON        (1UL << 19)  /* Clock security system enable             */
#define RCC_CR_PLLON        (1UL << 24)  /* Main PLL enable                          */
#define RCC_CR_PLLRDY       (1UL << 25)  /* Main PLL clock ready                     */
#define RCC_CR_PLLI2SON     (1UL << 26)  /* PLLI2S enable                            */
#define RCC_CR_PLLI2SRDY    (1UL << 27)  /* PLLI2S clock ready                       */

/* -----------------------------------------------------------------------
 * RCC_CFGR bit definitions  (RM0090 §6.3.3)
 * --------------------------------------------------------------------- */
#define RCC_CFGR_SW_HSI     (0x0UL)       /* HSI selected as system clock            */
#define RCC_CFGR_SW_HSE     (0x1UL)       /* HSE selected as system clock            */
#define RCC_CFGR_SW_PLL     (0x2UL)       /* PLL selected as system clock            */
#define RCC_CFGR_SW_MASK    (0x3UL)       /* System clock switch mask                */
#define RCC_CFGR_SWS_MASK   (0x3UL << 2)  /* System clock switch status mask         */
#define RCC_CFGR_SWS_HSI    (0x0UL << 2)  /* HSI used as sys clock (SWS readback)    */
#define RCC_CFGR_SWS_HSE    (0x1UL << 2)  /* HSE used as sys clock (SWS readback)    */
#define RCC_CFGR_SWS_PLL    (0x2UL << 2)  /* PLL used as sys clock (SWS readback)    */

/* -----------------------------------------------------------------------
 * RCC_AHB1ENR bit definitions — GPIO clock enables  (RM0090 §6.3.10)
 * --------------------------------------------------------------------- */
#define RCC_AHB1ENR_GPIOAEN     (1UL << 0)   /* GPIOA clock enable                  */
#define RCC_AHB1ENR_GPIOBEN     (1UL << 1)   /* GPIOB clock enable                  */
#define RCC_AHB1ENR_GPIOCEN     (1UL << 2)   /* GPIOC clock enable                  */
#define RCC_AHB1ENR_GPIODEN     (1UL << 3)   /* GPIOD clock enable                  */
#define RCC_AHB1ENR_GPIOEEN     (1UL << 4)   /* GPIOE clock enable                  */
#define RCC_AHB1ENR_DMA1EN      (1UL << 21)  /* DMA1 clock enable                   */
#define RCC_AHB1ENR_DMA2EN      (1UL << 22)  /* DMA2 clock enable                   */

/* -----------------------------------------------------------------------
 * RCC_APB1ENR bit definitions — USART2/3 and UART4/5  (RM0090 §6.3.14)
 * --------------------------------------------------------------------- */
#define RCC_APB1ENR_USART2EN    (1UL << 17)  /* USART2 clock enable                 */
#define RCC_APB1ENR_USART3EN    (1UL << 18)  /* USART3 clock enable                 */
#define RCC_APB1ENR_UART4EN     (1UL << 19)  /* UART4  clock enable                 */
#define RCC_APB1ENR_UART5EN     (1UL << 20)  /* UART5  clock enable                 */

/* -----------------------------------------------------------------------
 * RCC_APB2ENR bit definitions — USART1 / USART6  (RM0090 §6.3.15)
 * --------------------------------------------------------------------- */
#define RCC_APB2ENR_USART1EN    (1UL << 4)   /* USART1 clock enable                 */
#define RCC_APB2ENR_USART6EN    (1UL << 5)   /* USART6 clock enable                 */

/* -----------------------------------------------------------------------
 * SysTick register offsets (relative to SYSTICK_BASE)  — PM0214 §4.4
 * --------------------------------------------------------------------- */
#define SYSTICK_CTRL_OFFSET     0x00UL   /* Control and status register              */
#define SYSTICK_LOAD_OFFSET     0x04UL   /* Reload value register                    */
#define SYSTICK_VAL_OFFSET      0x08UL   /* Current value register                   */
#define SYSTICK_CALIB_OFFSET    0x0CUL   /* Calibration value register (read-only)   */

/* -----------------------------------------------------------------------
 * SysTick named-register shortcuts
 * --------------------------------------------------------------------- */
#define SYSTICK_CTRL    REG32(SYSTICK_BASE, SYSTICK_CTRL_OFFSET)
#define SYSTICK_LOAD    REG32(SYSTICK_BASE, SYSTICK_LOAD_OFFSET)
#define SYSTICK_VAL     REG32(SYSTICK_BASE, SYSTICK_VAL_OFFSET)
#define SYSTICK_CALIB   REG32(SYSTICK_BASE, SYSTICK_CALIB_OFFSET)

/* -----------------------------------------------------------------------
 * SysTick_CTRL bit definitions  (PM0214 §4.4.1)
 * --------------------------------------------------------------------- */
#define SYSTICK_CTRL_ENABLE     (1UL << 0)   /* Counter enable                       */
#define SYSTICK_CTRL_TICKINT    (1UL << 1)   /* SysTick exception request enable     */
#define SYSTICK_CTRL_CLKSOURCE  (1UL << 2)   /* 1=Processor clk, 0=External ref clk */
#define SYSTICK_CTRL_COUNTFLAG  (1UL << 16)  /* Returns 1 if timer counted to 0      */

/* -----------------------------------------------------------------------
 * SysTick_LOAD bit definitions  (PM0214 §4.4.2)
 * --------------------------------------------------------------------- */
#define SYSTICK_LOAD_RELOAD_MASK    0x00FFFFFFUL  /* 24-bit reload value mask         */

/* -----------------------------------------------------------------------
 * SysTick_CALIB bit definitions  (PM0214 §4.4.4)
 * --------------------------------------------------------------------- */
#define SYSTICK_CALIB_TENMS_MASK    0x00FFFFFFUL  /* 10-ms calibration value          */
#define SYSTICK_CALIB_SKEW          (1UL << 30)   /* Calibration value is not exact   */
#define SYSTICK_CALIB_NOREF         (1UL << 31)   /* No external reference clock      */

/* -----------------------------------------------------------------------
 * Public API — RCC clock gating
 * --------------------------------------------------------------------- */
void RCC_AHB1ClockEnable(uint32_t ahb1_bit);
void RCC_AHB1ClockDisable(uint32_t ahb1_bit);
void RCC_APB1ClockEnable(uint32_t apb1_bit);
void RCC_APB1ClockDisable(uint32_t apb1_bit);
void RCC_APB2ClockEnable(uint32_t apb2_bit);
void RCC_APB2ClockDisable(uint32_t apb2_bit);
uint32_t RCC_SetSysClockSource(uint32_t sw); /* returns 1=OK, 0=timeout      */
uint32_t RCC_GetSysClockSource(void);

/* -----------------------------------------------------------------------
 * Public API — SysTick
 * --------------------------------------------------------------------- */
void     SysTick_Init(uint32_t cpu_hz);
void     SysTick_DelayMs(uint32_t ms);
uint32_t SysTick_GetTick(void);
void     SysTick_Handler(void);

#endif /* STM32F4XX_RCC_H */
