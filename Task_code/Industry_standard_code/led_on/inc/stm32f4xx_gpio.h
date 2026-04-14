/**
 * @file    stm32f4xx_gpio.h
 * @brief   GPIO HAL driver for STM32F4xx series
 * @version 1.0.0
 *
 * Provides a clean, validated API over raw memory-mapped registers.
 * All functions are reentrant and safe to call from ISR context
 * provided the caller does not share pin state.
 */
 //Initialization of header guard
#ifndef STM32F4XX_GPIO_H
#define STM32F4XX_GPIO_H
#include<stdint.h>
#include<stddef.h>

/* -----------------------------------------------------------------------
 * Peripheral base addresses
 * --------------------------------------------------------------------- */
#define RCC_BASE      0x40023800UL
#define GPIOA_BASE    0x40020000UL
#define GPIOB_BASE    0x40020400UL
#define GPIOC_BASE    0x40020800UL

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
 * Register offsets (relative to GPIOx_BASE)
 * --------------------------------------------------------------------- */
#define GPIO_MODER_OFFSET   0x00UL
#define GPIO_OTYPER_OFFSET  0x04UL
#define GPIO_OSPEEDR_OFFSET 0x08UL
#define GPIO_PUPDR_OFFSET   0x0CUL
#define GPIO_IDR_OFFSET     0x10UL
#define GPIO_ODR_OFFSET     0x14UL
#define GPIO_BSRR_OFFSET    0x18UL  /* Atomic set/reset — preferred over ODR */
/* -----------------------------------------------------------------------
 * Register access macros
 * All accesses are volatile to prevent optimisation across I/O boundaries.
 * --------------------------------------------------------------------- */ 
#define REG32(Base, Offset) (*(volatile uint32_t *)((Base) + (Offset)))
#define RCC_AHB1ENR     REG32(RCC_BASE,   RCC_AHB1ENR_OFFSET)
#define GPIOA_MODER     REG32(GPIOA_BASE, GPIO_MODER_OFFSET)
#define GPIOA_OTYPER    REG32(GPIOA_BASE, GPIO_OTYPER_OFFSET)
#define GPIOA_OSPEEDR   REG32(GPIOA_BASE, GPIO_OSPEEDR_OFFSET)
#define GPIOA_PUPDR     REG32(GPIOA_BASE, GPIO_PUPDR_OFFSET)
#define GPIOA_IDR       REG32(GPIOA_BASE, GPIO_IDR_OFFSET)
#define GPIOA_ODR       REG32(GPIOA_BASE, GPIO_ODR_OFFSET)
#define GPIOA_BSRR      REG32(GPIOA_BASE, GPIO_BSRR_OFFSET)
/* -----------------------------------------------------------------------
 * Bit-field constants
 * --------------------------------------------------------------------- */
 /** GPIO mode register — 2 bits per pin */
#define GPIO_MODER_INPUT    0x00U
#define GPIO_MODER_OUTPUT   0x01U
#define GPIO_MODER_AF       0x02U
#define GPIO_MODER_ANALOG   0x03U
#define GPIO_MODER_MASK     0x03U

/** Output type register — 1 bit per pin */
#define GPIO_OTYPER_PP      0x00U  /* Push-pull  */
#define GPIO_OTYPER_OD      0x01U  /* Open-drain */

/** Output speed register — 2 bits per pin */
#define GPIO_OSPEEDR_LOW    0x00U
#define GPIO_OSPEEDR_MEDIUM 0x01U
#define GPIO_OSPEEDR_HIGH   0x02U
#define GPIO_OSPEEDR_VHIGH  0x03U
#define GPIO_OSPEEDR_MASK   0x03U

/** Pull-up/pull-down register — 2 bits per pin */
#define GPIO_PUPDR_NONE     0x00U
#define GPIO_PUPDR_PULLUP   0x01U
#define GPIO_PUPDR_PULLDOWN 0x02U
#define GPIO_PUPDR_MASK     0x03U

/* BSRR register bit definitions (32 bits total) */
#define GPIO_BSRR_RESET_OFFSET 16U
#define GPIO_BSRR_SET_OFFSET   0U


/** RCC AHB1 clock enable bits */
#define RCC_AHB1ENR_GPIOAEN (1UL << 0)
#define RCC_AHB1ENR_GPIOBEN (1UL << 1)
#define RCC_AHB1ENR_GPIOCEN (1UL << 2)
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
    GPIO_OK             = 0,
    GPIO_ERR_NULL_PTR   = -1,  /**< NULL pointer passed as argument      */
    GPIO_ERR_INVALID_PIN = -2, /**< Pin number out of 0–15 range          */
    GPIO_ERR_INVALID_MODE = -3 /**< Unsupported mode/speed/pull value     */
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
    volatile uint32_t *port_base; /**< e.g. GPIOA_BASE cast to pointer  */
    uint32_t           rcc_bit;   /**< Corresponding RCC_AHB1ENR bit     */
    uint8_t            pin;       /**< 0–15                              */
    uint8_t            mode;      /**< GPIO_MODER_*                      */
    uint8_t            otype;     /**< GPIO_OTYPER_*                     */
    uint8_t            speed;     /**< GPIO_OSPEEDR_*                    */
    uint8_t            pull;      /**< GPIO_PUPDR_*                      */
    uint8_t            init_state;/**< GPIO_PIN_SET / GPIO_PIN_RESET     */
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