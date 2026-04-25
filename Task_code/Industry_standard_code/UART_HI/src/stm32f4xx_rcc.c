/**
 * @file  stm32f4xx_rcc.c
 * @brief RCC and SysTick HAL driver implementation
 *
 * Implements helper functions for:
 *   - Peripheral clock gating  (AHB1 / APB1 / APB2)
 *   - System clock source query
 *   - SysTick initialisation and blocking delay
 *
 * Reference
 * ---------
 *   RM0090 Rev 19  §6    RCC
 *   PM0214 Rev 10  §4.4  SysTick
 */
#include "stm32f4xx_rcc.h"

/* -----------------------------------------------------------------------
 * Internal state
 * --------------------------------------------------------------------- */

/** Millisecond software tick counter, incremented in SysTick_Handler. */
static volatile uint32_t s_tick_ms = 0U;

/* -----------------------------------------------------------------------
 * Internal helpers — not exposed in the header
 * --------------------------------------------------------------------- */

/** Maximum iterations to wait for clock switch convergence (~5 ms @ 16 MHz). */
#define RCC_CLKSRC_TIMEOUT  80000U

/**
 * @brief  Spin until the SWS (system clock switch status) field matches
 *         the requested source, or until the timeout expires.
 *
 * @param  sws_expected  Expected RCC_CFGR_SWS_* value after masking.
 * @return 1 if clock switch confirmed, 0 if timed out.
 */
static uint32_t rcc_wait_for_clock_source(uint32_t sws_expected)
{
    uint32_t timeout = RCC_CLKSRC_TIMEOUT;
    while ((RCC_CFGR & RCC_CFGR_SWS_MASK) != sws_expected) {
        if (--timeout == 0U) {
            return 0U; /* Convergence failure — oscillator may not be stable */
        }
    }
    return 1U;
}

/* -----------------------------------------------------------------------
 * Public API implementation — RCC
 * --------------------------------------------------------------------- */

/**
 * @brief  Enable the clock for a peripheral on the AHB1 bus.
 *
 * The OR-in is idempotent: calling this function multiple times for the
 * same bit is safe.  Two dummy reads after the write flush the AHB bus
 * pipeline before the peripheral is accessed.
 *
 * @param  ahb1_bit  One of the RCC_AHB1ENR_GPIOxEN / DMAx_EN constants.
 */
void RCC_AHB1ClockEnable(uint32_t ahb1_bit)
{
    RCC_AHB1ENR |= ahb1_bit;
    (void)RCC_AHB1ENR; /* read-back to flush AHB pipeline */
    (void)RCC_AHB1ENR;
}

/**
 * @brief  Disable the clock for a peripheral on the AHB1 bus.
 *
 * @param  ahb1_bit  One of the RCC_AHB1ENR_GPIOxEN / DMAx_EN constants.
 */
void RCC_AHB1ClockDisable(uint32_t ahb1_bit)
{
    RCC_AHB1ENR &= ~ahb1_bit;
}

/**
 * @brief  Enable the clock for a peripheral on the APB1 bus.
 *
 * @param  apb1_bit  One of the RCC_APB1ENR_USARTxEN constants.
 */
void RCC_APB1ClockEnable(uint32_t apb1_bit)
{
    RCC_APB1ENR |= apb1_bit;
    (void)RCC_APB1ENR; /* read-back to flush APB pipeline */
    (void)RCC_APB1ENR;
}

/**
 * @brief  Disable the clock for a peripheral on the APB1 bus.
 *
 * @param  apb1_bit  One of the RCC_APB1ENR_USARTxEN constants.
 */
void RCC_APB1ClockDisable(uint32_t apb1_bit)
{
    RCC_APB1ENR &= ~apb1_bit;
}

/**
 * @brief  Enable the clock for a peripheral on the APB2 bus.
 *
 * @param  apb2_bit  One of the RCC_APB2ENR_USARTxEN constants.
 */
void RCC_APB2ClockEnable(uint32_t apb2_bit)
{
    RCC_APB2ENR |= apb2_bit;
    (void)RCC_APB2ENR; /* read-back to flush APB pipeline */
    (void)RCC_APB2ENR;
}

/**
 * @brief  Disable the clock for a peripheral on the APB2 bus.
 *
 * @param  apb2_bit  One of the RCC_APB2ENR_USARTxEN constants.
 */
void RCC_APB2ClockDisable(uint32_t apb2_bit)
{
    RCC_APB2ENR &= ~apb2_bit;
}

/**
 * @brief  Select the system clock source and wait until the hardware
 *         confirms the switch via the SWS field.
 *
 * @param  sw  RCC_CFGR_SW_HSI / RCC_CFGR_SW_HSE / RCC_CFGR_SW_PLL.
 */
uint32_t RCC_SetSysClockSource(uint32_t sw)
{
    uint32_t reg = RCC_CFGR;
    reg &= ~RCC_CFGR_SW_MASK;
    reg |= (sw & RCC_CFGR_SW_MASK);
    RCC_CFGR = reg;
    return rcc_wait_for_clock_source(sw << 2U); /* SWS field is at [3:2]    */
}

/**
 * @brief  Return the current system clock source from the SWS field.
 *
 * @return RCC_CFGR_SWS_HSI / SWS_HSE / SWS_PLL (masked, shifted to [1:0]).
 */
uint32_t RCC_GetSysClockSource(void)
{
    return (RCC_CFGR & RCC_CFGR_SWS_MASK) >> 2U;
}

/* -----------------------------------------------------------------------
 * Public API implementation — SysTick
 * --------------------------------------------------------------------- */

/**
 * @brief  Initialise the SysTick timer for millisecond ticks.
 *
 * Configures LOAD to fire every 1 ms based on @p cpu_hz, resets VAL,
 * selects the processor clock, and enables both the counter and the
 * exception so SysTick_Handler() increments s_tick_ms.
 *
 * @param  cpu_hz  Core clock frequency in Hz (e.g. 16000000UL for HSI).
 */
void SysTick_Init(uint32_t cpu_hz)
{
    uint32_t reload = (cpu_hz / 1000U) - 1U; /* Ticks per 1 ms */

    SYSTICK_CTRL  = 0U;              /* Disable while configuring            */
    SYSTICK_LOAD  = reload & SYSTICK_LOAD_RELOAD_MASK;
    SYSTICK_VAL   = 0U;              /* Clear current value                  */

    /* Processor clock | exception enabled | counter enabled */
    SYSTICK_CTRL  = SYSTICK_CTRL_CLKSOURCE
                  | SYSTICK_CTRL_TICKINT
                  | SYSTICK_CTRL_ENABLE;
}

/**
 * @brief  Block for at least @p ms milliseconds.
 *
 * Reads s_tick_ms which is incremented by SysTick_Handler().
 * Accurate to ±1 ms (may be 1 ms early if called just before a tick).
 *
 * @param  ms  Number of milliseconds to wait.
 */
void SysTick_DelayMs(uint32_t ms)
{
    /*
     * Take a single snapshot of the volatile tick counter so the compiler
     * cannot re-read it mid-expression and introduce a race condition.
     * The subtraction wraps safely with unsigned arithmetic.
     */
    uint32_t start = s_tick_ms;
    uint32_t elapsed;
    do {
        elapsed = s_tick_ms - start; /* volatile read isolated to one point  */
    } while (elapsed < ms);
}

/**
 * @brief  Return elapsed milliseconds since SysTick_Init().
 *
 * @return 32-bit tick counter (wraps every ~49 days at 1 ms resolution).
 */
uint32_t SysTick_GetTick(void)
{
    return s_tick_ms;
}

/**
 * @brief  SysTick exception handler — increment the millisecond counter.
 *
 * Must be placed in the vector table (startup file) under the name
 * SysTick_Handler.  Named with weak linkage so the application can
 * override it if a full RTOS tick hook is needed.
 */
__attribute__((weak)) void SysTick_Handler(void)
{
    s_tick_ms++;
}
