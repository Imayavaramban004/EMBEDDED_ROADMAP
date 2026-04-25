/**
 * @file    stm32f4xx_gpio.h
 * @brief   GPIO HAL driver for STM32F4xx series
 * @version 1.0.0
 *
 * Provides a clean, validated API over raw memory-mapped registers.
 * All functions are reentrant and safe to call from ISR context
 * provided the caller does not share pin state.
 *
 * Reference
 * ---------
 *   RM0090 Rev 19  §8  General-purpose I/Os
 */
 //Initialization of header guard
#ifndef STM32F4XX_GPIO_H
#define STM32F4XX_GPIO_H

#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx_rcc.h"   /* REG32, RCC_BASE, RCC_AHB1ENR_GPIOxEN        */

/* -----------------------------------------------------------------------
 * GPIO peripheral base addresses  (RM0090 §2.3 Memory Map)
 * --------------------------------------------------------------------- */
#define GPIOA_BASE    0x40020000UL   /* GPIO port A — AHB1                   */
#define GPIOB_BASE    0x40020400UL   /* GPIO port B — AHB1                   */
#define GPIOC_BASE    0x40020800UL   /* GPIO port C — AHB1                   */
#define GPIOD_BASE    0x40020C00UL   /* GPIO port D — AHB1                   */
#define GPIOE_BASE    0x40021000UL   /* GPIO port E — AHB1                   */

/* -----------------------------------------------------------------------
 * GPIO register offsets (relative to GPIOx_BASE)  — RM0090 §8.4
 * --------------------------------------------------------------------- */
#define GPIO_MODER_OFFSET   0x00UL   /* Mode register          — 2 bits/pin  */
#define GPIO_OTYPER_OFFSET  0x04UL   /* Output type register   — 1 bit/pin   */
#define GPIO_OSPEEDR_OFFSET 0x08UL   /* Output speed register  — 2 bits/pin  */
#define GPIO_PUPDR_OFFSET   0x0CUL   /* Pull-up/pull-down reg  — 2 bits/pin  */
#define GPIO_IDR_OFFSET     0x10UL   /* Input data register    — 1 bit/pin   */
#define GPIO_ODR_OFFSET     0x14UL   /* Output data register   — 1 bit/pin   */
#define GPIO_BSRR_OFFSET    0x18UL   /* Bit set/reset register — atomic WO   */
#define GPIO_LCKR_OFFSET    0x1CUL   /* Configuration lock register          */
#define GPIO_AFRL_OFFSET    0x20UL   /* Alternate function low  (pins 0–7)   */
#define GPIO_AFRH_OFFSET    0x24UL   /* Alternate function high (pins 8–15)  */

/* -----------------------------------------------------------------------
 * GPIOA named-register shortcuts
 * --------------------------------------------------------------------- */
#define GPIOA_MODER     REG32(GPIOA_BASE, GPIO_MODER_OFFSET)
#define GPIOA_OTYPER    REG32(GPIOA_BASE, GPIO_OTYPER_OFFSET)
#define GPIOA_OSPEEDR   REG32(GPIOA_BASE, GPIO_OSPEEDR_OFFSET)
#define GPIOA_PUPDR     REG32(GPIOA_BASE, GPIO_PUPDR_OFFSET)
#define GPIOA_IDR       REG32(GPIOA_BASE, GPIO_IDR_OFFSET)
#define GPIOA_ODR       REG32(GPIOA_BASE, GPIO_ODR_OFFSET)
#define GPIOA_BSRR      REG32(GPIOA_BASE, GPIO_BSRR_OFFSET)
#define GPIOA_AFRL      REG32(GPIOA_BASE, GPIO_AFRL_OFFSET)
#define GPIOA_AFRH      REG32(GPIOA_BASE, GPIO_AFRH_OFFSET)

/* -----------------------------------------------------------------------
 * GPIOB named-register shortcuts
 * --------------------------------------------------------------------- */
#define GPIOB_MODER     REG32(GPIOB_BASE, GPIO_MODER_OFFSET)
#define GPIOB_OTYPER    REG32(GPIOB_BASE, GPIO_OTYPER_OFFSET)
#define GPIOB_OSPEEDR   REG32(GPIOB_BASE, GPIO_OSPEEDR_OFFSET)
#define GPIOB_PUPDR     REG32(GPIOB_BASE, GPIO_PUPDR_OFFSET)
#define GPIOB_IDR       REG32(GPIOB_BASE, GPIO_IDR_OFFSET)
#define GPIOB_ODR       REG32(GPIOB_BASE, GPIO_ODR_OFFSET)
#define GPIOB_BSRR      REG32(GPIOB_BASE, GPIO_BSRR_OFFSET)
#define GPIOB_AFRL      REG32(GPIOB_BASE, GPIO_AFRL_OFFSET)
#define GPIOB_AFRH      REG32(GPIOB_BASE, GPIO_AFRH_OFFSET)

/* -----------------------------------------------------------------------
 * Bit-field constants — GPIO_MODER  (2 bits per pin)
 * --------------------------------------------------------------------- */
/** GPIO mode register values */
#define GPIO_MODER_INPUT    0x00U   /* Input mode (reset state)                     */
#define GPIO_MODER_OUTPUT   0x01U   /* General purpose output mode                  */
#define GPIO_MODER_AF       0x02U   /* Alternate function mode                       */
#define GPIO_MODER_ANALOG   0x03U   /* Analog mode                                  */
#define GPIO_MODER_MASK     0x03U   /* Mask for one pin's 2-bit field               */

/* -----------------------------------------------------------------------
 * Bit-field constants — GPIO_OTYPER  (1 bit per pin)
 * --------------------------------------------------------------------- */
/** Output type register values */
#define GPIO_OTYPER_PP      0x00U   /* Push-pull  output (reset state)              */
#define GPIO_OTYPER_OD      0x01U   /* Open-drain output                            */

/* -----------------------------------------------------------------------
 * Bit-field constants — GPIO_OSPEEDR  (2 bits per pin)
 * --------------------------------------------------------------------- */
/** Output speed register values */
#define GPIO_OSPEEDR_LOW    0x00U   /* Low speed                                    */
#define GPIO_OSPEEDR_MEDIUM 0x01U   /* Medium speed                                 */
#define GPIO_OSPEEDR_HIGH   0x02U   /* High speed                                   */
#define GPIO_OSPEEDR_VHIGH  0x03U   /* Very high speed                              */
#define GPIO_OSPEEDR_MASK   0x03U   /* Mask for one pin's 2-bit field               */

/* -----------------------------------------------------------------------
 * Bit-field constants — GPIO_PUPDR  (2 bits per pin)
 * --------------------------------------------------------------------- */
/** Pull-up / pull-down register values */
#define GPIO_PUPDR_NONE     0x00U   /* No pull-up, no pull-down (reset state)       */
#define GPIO_PUPDR_PULLUP   0x01U   /* Pull-up                                      */
#define GPIO_PUPDR_PULLDOWN 0x02U   /* Pull-down                                    */
#define GPIO_PUPDR_MASK     0x03U   /* Mask for one pin's 2-bit field               */

/* -----------------------------------------------------------------------
 * Bit-field constants — GPIO_BSRR
 * --------------------------------------------------------------------- */
/* Set   pin n : GPIOx_BSRR = (1UL << n)        — upper 16 bits reset     */
/* Reset pin n : GPIOx_BSRR = (1UL << (n + 16)) — lower 16 bits set       */
#define GPIO_BSRR_SET_OFFSET    0U    /* Bit offset for set   half (BS[15:0])        */
#define GPIO_BSRR_RESET_OFFSET  16U   /* Bit offset for reset half (BR[31:16])       */

/* -----------------------------------------------------------------------
 * Alternate function register constants  (4 bits per pin in AFR[])
 * --------------------------------------------------------------------- */
/** Alternate function numbers — RM0090 Table 9 */
#define GPIO_AF0            0x00U   /* System: MCO, SWD, JTAG, RTC                  */
#define GPIO_AF1            0x01U   /* TIM1 / TIM2                                  */
#define GPIO_AF2            0x02U   /* TIM3 / TIM4 / TIM5                           */
#define GPIO_AF3            0x03U   /* TIM8 / TIM9 / TIM10 / TIM11                  */
#define GPIO_AF4            0x04U   /* I2C1 / I2C2 / I2C3                           */
#define GPIO_AF5            0x05U   /* SPI1 / SPI2 / SPI3 / SPI4                    */
#define GPIO_AF6            0x06U   /* SPI3 / SAI1                                  */
#define GPIO_AF7            0x07U   /* USART1 / USART2 / USART3                     */
#define GPIO_AF8            0x08U   /* UART4 / UART5 / USART6                       */
#define GPIO_AF9            0x09U   /* CAN1 / CAN2 / TIM12 / TIM13 / TIM14          */
#define GPIO_AF10           0x0AU   /* OTG_FS / OTG_HS                              */
#define GPIO_AF11           0x0BU   /* ETH                                           */
#define GPIO_AF12           0x0CU   /* FSMC / SDIO / OTG_HS                         */
#define GPIO_AF13           0x0DU   /* DCMI                                          */
#define GPIO_AF15           0x0FU   /* EVENTOUT                                      */
#define GPIO_AFR_MASK       0x0FU   /* Mask for one pin's 4-bit AFR field           */

/* -----------------------------------------------------------------------
 * Valid pin numbers
 * --------------------------------------------------------------------- */
#define GPIO_PIN_MIN  0U
#define GPIO_PIN_MAX  15U

/* -----------------------------------------------------------------------
 * Public types
 * --------------------------------------------------------------------- */

/** Return status for all driver functions */
typedef enum {
    GPIO_OK               =  0,   /**< Operation succeeded                          */
    GPIO_ERR_NULL_PTR     = -1,   /**< NULL pointer passed as argument              */
    GPIO_ERR_INVALID_PIN  = -2,   /**< Pin number out of 0–15 range                 */
    GPIO_ERR_INVALID_MODE = -3    /**< Unsupported mode / speed / pull value        */
} GPIO_Status_t;

/** Logical pin state */
typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET   = 1
} GPIO_PinState_t;

/**
 * @brief  Complete pin configuration descriptor.
 *
 * Pass this struct to GPIO_Init().  Every field must be set by the caller;
 * there are no silent defaults so misconfigured structs fail validation.
 */
typedef struct {
    volatile uint32_t *port_base; /**< e.g. (volatile uint32_t *)GPIOA_BASE        */
    uint32_t           rcc_bit;   /**< Corresponding RCC_AHB1ENR_GPIOxEN bit       */
    uint8_t            pin;       /**< 0–15                                         */
    uint8_t            mode;      /**< GPIO_MODER_INPUT / OUTPUT / AF / ANALOG      */
    uint8_t            otype;     /**< GPIO_OTYPER_PP / OD                          */
    uint8_t            speed;     /**< GPIO_OSPEEDR_LOW / MEDIUM / HIGH / VHIGH     */
    uint8_t            pull;      /**< GPIO_PUPDR_NONE / PULLUP / PULLDOWN          */
    uint8_t            af;        /**< GPIO_AF0–GPIO_AF15  (used when mode=AF)      */
    uint8_t            init_state;/**< GPIO_PIN_SET / GPIO_PIN_RESET                */
} GPIO_Config_t;

/* -----------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------- */

/**
 * @brief  Initialise a GPIO pin.
 *
 * Enables the peripheral clock, applies all configuration in the correct
 * sequence (ODR before MODER to avoid glitches on output pins), and
 * validates every parameter before touching hardware.
 *
 * @param  cfg  Pointer to a fully populated GPIO_Config_t struct.
 * @return GPIO_OK on success, negative GPIO_Status_t on error.
 */
GPIO_Status_t GPIO_Init(const GPIO_Config_t *cfg);

/**
 * @brief  Write a logical state to an output pin (atomic BSRR write).
 *
 * Uses the Bit Set/Reset Register — no read-modify-write, ISR-safe.
 *
 * @param  cfg    Pointer to the same config used in GPIO_Init().
 * @param  state  GPIO_PIN_SET or GPIO_PIN_RESET.
 * @return GPIO_OK on success, negative GPIO_Status_t on error.
 */
GPIO_Status_t GPIO_WritePin(const GPIO_Config_t *cfg, GPIO_PinState_t state);

/**
 * @brief  Read the current logical state of a pin (via IDR).
 *
 * @param  cfg    Pointer to the pin configuration.
 * @param  state  Output: populated with GPIO_PIN_SET or GPIO_PIN_RESET.
 * @return GPIO_OK on success, negative GPIO_Status_t on error.
 */
GPIO_Status_t GPIO_ReadPin(const GPIO_Config_t *cfg, GPIO_PinState_t *state);

/**
 * @brief  Toggle an output pin (atomic via BSRR — reads IDR, writes BSRR).
 *
 * @param  cfg  Pointer to the pin configuration.
 * @return GPIO_OK on success, negative GPIO_Status_t on error.
 */
GPIO_Status_t GPIO_TogglePin(const GPIO_Config_t *cfg);

/**
 * @brief  De-initialise a pin back to its reset state (analog, no pull).
 *
 * @param  cfg  Pointer to the pin configuration.
 * @return GPIO_OK on success, negative GPIO_Status_t on error.
 */
GPIO_Status_t GPIO_DeInit(const GPIO_Config_t *cfg);

#endif /* STM32F4XX_GPIO_H */
