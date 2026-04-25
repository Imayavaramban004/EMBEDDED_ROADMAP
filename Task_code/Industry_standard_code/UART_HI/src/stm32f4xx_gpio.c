/**
 * @file  stm32f4xx_gpio.c
 * @brief GPIO HAL driver implementation for STM32F4xx series
 *
 * Implements all functions declared in stm32f4xx_gpio.h:
 *   GPIO_Init       — clock, OTYPER, OSPEEDR, PUPDR, AFR, MODER
 *   GPIO_WritePin   — atomic BSRR write
 *   GPIO_ReadPin    — IDR sample
 *   GPIO_TogglePin  — IDR read + BSRR write
 *   GPIO_DeInit     — reset to analog / no-pull / low-speed
 *
 * Reference
 * ---------
 *   RM0090 Rev 19  §8  General-purpose I/Os
 */
#include "stm32f4xx_gpio.h"
#include <stdint.h>   /* uintptr_t — C99, guaranteed by <stdint.h>           */

/* -----------------------------------------------------------------------
 * Internal helpers — not exposed in the header
 * --------------------------------------------------------------------- */

/**
 * @brief  Validate all fields in a GPIO_Config_t.
 * @return GPIO_OK or an appropriate error code.
 */
static GPIO_Status_t gpio_validate_config(const GPIO_Config_t *cfg)
{
    if (cfg == NULL || cfg->port_base == NULL) {
        return GPIO_ERR_NULL_PTR;
    }
    if (cfg->pin > GPIO_PIN_MAX) {
        return GPIO_ERR_INVALID_PIN;
    }
    if (cfg->mode  > GPIO_MODER_ANALOG  ||
        cfg->otype > GPIO_OTYPER_OD     ||
        cfg->speed > GPIO_OSPEEDR_VHIGH ||
        cfg->pull  > GPIO_PUPDR_PULLDOWN) {
        return GPIO_ERR_INVALID_MODE;
    }
    if (cfg->mode == GPIO_MODER_AF && cfg->af > GPIO_AF15) {
        return GPIO_ERR_INVALID_MODE;  /* AF value out of 0–15 range          */
    }
    return GPIO_OK;
}

/**
 * @brief  Write a 2-bit field at position (pin * 2) inside @p reg.
 *
 * Clears the field first, then ORs the new value in — read-modify-write
 * sequence applied to the pointed-to volatile register.
 *
 * @param  reg    Pointer to the 32-bit peripheral register.
 * @param  pin    Pin number (0–15); determines the shift amount.
 * @param  mask   Bit mask for one field, e.g. GPIO_MODER_MASK (0x03).
 * @param  value  New 2-bit value to write into the field.
 */
static void gpio_set_2bit_field(volatile uint32_t *reg,
                                uint8_t            pin,
                                uint32_t           mask,
                                uint32_t           value)
{
    uint32_t shift = (uint32_t)pin * 2U;
    *reg = (*reg & ~(mask << shift)) | ((value & mask) << shift);
}

/**
 * @brief  Write a 4-bit alternate-function value into the correct AFR word.
 *
 * Pins 0–7 use AFR[0] (AFRL); pins 8–15 use AFR[1] (AFRH).
 * The field position within the register word is (pin % 8) * 4.
 *
 * @param  port_base  GPIO port base address.
 * @param  pin        Pin number (0–15).
 * @param  af         Alternate function number (GPIO_AF0–GPIO_AF15).
 */
static void gpio_set_af(volatile uint32_t *port_base,
                        uint8_t            pin,
                        uint8_t            af)
{
    uint32_t afr_offset = (pin < 8U) ? GPIO_AFRL_OFFSET : GPIO_AFRH_OFFSET;
    uint32_t local_pin  = (uint32_t)(pin % 8U);
    uint32_t shift      = local_pin * 4U;

    volatile uint32_t *afr =
        (volatile uint32_t *)((uintptr_t)port_base + afr_offset);

    *afr = (*afr & ~((uint32_t)GPIO_AFR_MASK << shift))
         | (((uint32_t)af & GPIO_AFR_MASK) << shift);
}

/* -----------------------------------------------------------------------
 * Public API implementation
 * --------------------------------------------------------------------- */

/**
 * @brief  Initialise a GPIO pin.
 *
 * Step order (prevents glitches on output and AF pins):
 *   1. Enable peripheral clock via RCC AHB1ENR.
 *   2. Pre-drive ODR via BSRR (output mode only) before committing MODER.
 *   3. Configure OTYPER   (push-pull / open-drain).
 *   4. Configure OSPEEDR  (low / medium / high / very-high).
 *   5. Configure PUPDR    (none / pull-up / pull-down).
 *   6. Configure AFR      (only when mode == GPIO_MODER_AF).
 *   7. Configure MODER    (input / output / AF / analog) — last.
 */
GPIO_Status_t GPIO_Init(const GPIO_Config_t *cfg)
{
    GPIO_Status_t status = gpio_validate_config(cfg);
    if (status != GPIO_OK) {
        return status;
    }

    /* 1. Enable peripheral clock (idempotent OR-in) */
    RCC_AHB1ENR |= cfg->rcc_bit;
    (void)RCC_AHB1ENR; /* read-back to flush the AHB bus pipeline */

    /* 2. Pre-drive output level before enabling output mode
     *    Prevents a brief glitch as MODER transitions from reset (input)
     *    to output.  BSRR is atomic — no critical section needed.       */
    if (cfg->mode == GPIO_MODER_OUTPUT) {
        volatile uint32_t *bsrr =
            (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_BSRR_OFFSET);

        if (cfg->init_state == GPIO_PIN_SET) {
            *bsrr = (1UL << cfg->pin);                              /* Set   */
        } else {
            *bsrr = (1UL << (cfg->pin + GPIO_BSRR_RESET_OFFSET));  /* Reset */
        }
    }

    /* 3. Output type: push-pull or open-drain (1 bit per pin) */
    volatile uint32_t *otyper =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_OTYPER_OFFSET);
    if (cfg->otype == GPIO_OTYPER_OD) {
        *otyper |=  (1UL << cfg->pin);
    } else {
        *otyper &= ~(1UL << cfg->pin);
    }

    /* 4. Output speed (2 bits per pin) */
    volatile uint32_t *ospeedr =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_OSPEEDR_OFFSET);
    gpio_set_2bit_field(ospeedr, cfg->pin, GPIO_OSPEEDR_MASK, cfg->speed);

    /* 5. Pull-up / pull-down (2 bits per pin) */
    volatile uint32_t *pupdr =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_PUPDR_OFFSET);
    gpio_set_2bit_field(pupdr, cfg->pin, GPIO_PUPDR_MASK, cfg->pull);

    /* 6. Alternate function (4 bits per pin in AFRL / AFRH) */
    if (cfg->mode == GPIO_MODER_AF) {
        gpio_set_af(cfg->port_base, cfg->pin, cfg->af);
    }

    /* 7. Mode — last, to commit the pin to its final function */
    volatile uint32_t *moder =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_MODER_OFFSET);
    gpio_set_2bit_field(moder, cfg->pin, GPIO_MODER_MASK, cfg->mode);

    return GPIO_OK;
}

/**
 * @brief  Write a logical state to an output pin (atomic BSRR write).
 *
 * Uses the Bit Set/Reset Register — no read-modify-write, ISR-safe.
 * Upper 16 bits reset the pin; lower 16 bits set it.
 */
GPIO_Status_t GPIO_WritePin(const GPIO_Config_t *cfg, GPIO_PinState_t state)
{
    GPIO_Status_t status = gpio_validate_config(cfg);
    if (status != GPIO_OK) {
        return status;
    }

    volatile uint32_t *bsrr =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_BSRR_OFFSET);

    if (state == GPIO_PIN_SET) {
        *bsrr = (1UL << cfg->pin);
    } else {
        *bsrr = (1UL << (cfg->pin + GPIO_BSRR_RESET_OFFSET));
    }

    return GPIO_OK;
}

/**
 * @brief  Read the current logical state of a pin via IDR.
 *
 * Samples the Input Data Register one bit at the pin's position.
 */
GPIO_Status_t GPIO_ReadPin(const GPIO_Config_t *cfg, GPIO_PinState_t *state)
{
    if (state == NULL) {
        return GPIO_ERR_NULL_PTR;
    }

    GPIO_Status_t status = gpio_validate_config(cfg);
    if (status != GPIO_OK) {
        return status;
    }

    volatile uint32_t *idr =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_IDR_OFFSET);

    *state = ((*idr >> cfg->pin) & 1U) ? GPIO_PIN_SET : GPIO_PIN_RESET;

    return GPIO_OK;
}

/**
 * @brief  Toggle an output pin.
 *
 * Reads the current state from IDR, then writes the inverse atomically
 * via BSRR.  The IDR read and BSRR write are not atomic as a pair, but
 * for simple output toggling this is acceptable.
 */
GPIO_Status_t GPIO_TogglePin(const GPIO_Config_t *cfg)
{
    GPIO_Status_t status = gpio_validate_config(cfg);
    if (status != GPIO_OK) {
        return status;
    }

    volatile uint32_t *idr  =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_IDR_OFFSET);
    volatile uint32_t *bsrr =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_BSRR_OFFSET);

    /* Read current state from IDR, then atomically invert via BSRR */
    if ((*idr >> cfg->pin) & 1U) {
        *bsrr = (1UL << (cfg->pin + GPIO_BSRR_RESET_OFFSET)); /* Was set → reset */
    } else {
        *bsrr = (1UL << cfg->pin);                             /* Was reset → set */
    }

    return GPIO_OK;
}

/**
 * @brief  De-initialise a pin back to its hardware reset state.
 *
 * Restores: analog mode, push-pull, low speed, no pull-up/pull-down.
 * Does NOT gate the peripheral clock — other pins on the same port
 * may still be active.
 */
GPIO_Status_t GPIO_DeInit(const GPIO_Config_t *cfg)
{
    GPIO_Status_t status = gpio_validate_config(cfg);
    if (status != GPIO_OK) {
        return status;
    }

    volatile uint32_t *moder   =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_MODER_OFFSET);
    volatile uint32_t *otyper  =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_OTYPER_OFFSET);
    volatile uint32_t *ospeedr =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_OSPEEDR_OFFSET);
    volatile uint32_t *pupdr   =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_PUPDR_OFFSET);

    /* Analog mode (reset default for most pins) */
    gpio_set_2bit_field(moder,   cfg->pin, GPIO_MODER_MASK,   GPIO_MODER_ANALOG);
    /* Low speed (reset default) */
    gpio_set_2bit_field(ospeedr, cfg->pin, GPIO_OSPEEDR_MASK, GPIO_OSPEEDR_LOW);
    /* No pull-up / pull-down */
    gpio_set_2bit_field(pupdr,   cfg->pin, GPIO_PUPDR_MASK,   GPIO_PUPDR_NONE);
    /* Push-pull (reset default) */
    *otyper &= ~(1UL << cfg->pin);

    return GPIO_OK;
}
