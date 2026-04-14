/**
 * @file  stm32f4xx_gpio.c
 * @brief GPIO HAL driver implementation
 */
#include<stm32f4xx_gpio.h>

/* -----------------------------------------------------------------------
 * Internal helpers — not exposed in the header
 * --------------------------------------------------------------------- */

/**
 * @brief  Validate all fields in a GPIO_Config_t.
 * @return GPIO_OK or an appropriate error code.
 */
 static GPIO_Status_t gpio_validate_config(const GPIO_Config_t *cfg){
    if(cfg==NULL || cfg->port_base==NULL){
        return GPIO_ERR_NULL_PTR;
    }
    else if(cfg->pin>GPIO_PIN_MAX){
        return GPIO_ERR_INVALID_PIN;
    }
     if (cfg->mode  > GPIO_MODER_ANALOG  ||
        cfg->otype > GPIO_OTYPER_OD     ||
        cfg->speed > GPIO_OSPEEDR_VHIGH ||
        cfg->pull  > GPIO_PUPDR_PULLDOWN) {
        return GPIO_ERR_INVALID_MODE;
    }
    return GPIO_OK;
 }
 /**
 * @brief  Apply a 2-bit field to a 32-bit register at (pin * 2).
 *         Clears the field first, then ORs the new value in.
 */
 static void gpio_set_2bit_field(volatile uint32_t *reg, uint8_t pin, uint32_t mask, uint32_t value){
    uint32_t shift= (uint32_t)pin*2U;
    *reg = (*reg & ~(mask << shift)) | ((value & mask) << shift);
 }
/* -----------------------------------------------------------------------
 * Public API implementation
 * --------------------------------------------------------------------- */
GPIO_Status_t GPIO_Init(const GPIO_Config_t *cfg){
    GPIO_Status_t status= gpio_validate_config(cfg);
    if(status!=GPIO_OK){
        return status;
    }
     /* 1. Enable peripheral clock (idempotent OR-in) */
    RCC_AHB1ENR |= cfg->rcc_bit;
     /*
     * 2. Set initial output level BEFORE enabling output mode.
     *    This prevents a brief glitch on the pin as MODER transitions
     *    from reset (input) to output.  Use BSRR for atomic write.
     */
    if (cfg->mode == GPIO_MODER_OUTPUT) {
        volatile uint32_t *bsrr =
            (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_BSRR_OFFSET);

        if (cfg->init_state == GPIO_PIN_SET) {
            *bsrr = (1UL << cfg->pin);                           /* Set   */
        } else {
            *bsrr = (1UL << (cfg->pin + GPIO_BSRR_RESET_OFFSET)); /* Reset */
        }
    }
    /* 3. Output type: push-pull or open-drain (1 bit per pin) */
    volatile uint32_t *otyper =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_OTYPER_OFFSET);
    if (cfg->otype == GPIO_OTYPER_OD) {
        *otyper |= (1UL << cfg->pin);
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

    /* 6. Mode — last, to commit the pin to its final function */
    volatile uint32_t *moder =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_MODER_OFFSET);
    gpio_set_2bit_field(moder, cfg->pin, GPIO_MODER_MASK, cfg->mode);

    return GPIO_OK;
}
GPIO_Status_t GPIO_WritePin(const GPIO_Config_t *cfg, GPIO_PinState_t state)
{
    GPIO_Status_t status = gpio_validate_config(cfg);
    if (status != GPIO_OK) {
        return status;
    }

    volatile uint32_t *bsrr =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_BSRR_OFFSET);

    /*
     * BSRR is a write-only, atomic register.
     * Upper 16 bits reset the pin; lower 16 bits set it.
     * A single 32-bit store cannot partially glitch — no critical section needed.
     */
    if (state == GPIO_PIN_SET) {
        *bsrr = (1UL << cfg->pin);
    } else {
        *bsrr = (1UL << (cfg->pin + GPIO_BSRR_RESET_OFFSET));
    }

    return GPIO_OK;
}
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

GPIO_Status_t GPIO_DeInit(const GPIO_Config_t *cfg)
{
    GPIO_Status_t status = gpio_validate_config(cfg);
    if (status != GPIO_OK) {
        return status;
    }

    /* Reset to hardware default: analog mode, no pull, low speed, push-pull */
    volatile uint32_t *moder   =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_MODER_OFFSET);
    volatile uint32_t *otyper  =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_OTYPER_OFFSET);
    volatile uint32_t *ospeedr =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_OSPEEDR_OFFSET);
    volatile uint32_t *pupdr   =
        (volatile uint32_t *)((uintptr_t)cfg->port_base + GPIO_PUPDR_OFFSET);

    gpio_set_2bit_field(moder,   cfg->pin, GPIO_MODER_MASK,   GPIO_MODER_ANALOG);
    gpio_set_2bit_field(ospeedr, cfg->pin, GPIO_OSPEEDR_MASK, GPIO_OSPEEDR_LOW);
    gpio_set_2bit_field(pupdr,   cfg->pin, GPIO_PUPDR_MASK,   GPIO_PUPDR_NONE);
    *otyper &= ~(1UL << cfg->pin);

    return GPIO_OK;
}