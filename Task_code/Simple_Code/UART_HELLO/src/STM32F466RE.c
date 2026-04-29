//STM32F466RE.c
#include<stdint.h>
#include<STM32F466RE.h>
/* -----------------------------------------------------------------------
 * Internal helpers — not exposed in the header
 * --------------------------------------------------------------------- */
static GPIO_Status_t gpio_status_validate(const GPIO_Config_t *cfg){
	if(cfg==NULL || *cfg->*port_base == NULL){
		return GPIO_ERR_NULL_PTR;
	}
	else if(cfg->pin > GPIO_PINS_MAX){
		return GPIO_ERR_INVALID_PIN;
	}
	if(cfg->mode > GPIO_MODER_ANALOG || cfg->otype > GPIO_OTYPER_OD || cfg->speed > GPIO_OSPEEDR_VHIGH || cfg->pull > GPIO_PUPDR_PULLDOWN){
		return GPIO_ERR_INVALID_MODE;
	}
	return GPIO_OK;
} 
/**
 * @brief  Apply a 2-bit field to a 32-bit register at (pin * 2).
 *         Clears the field first, then ORs the new value in.
 */
static void gpio_set_2bit(volatile uint32_t *reg, uint8_t pin, uint32_t mask, uint32_t value){
	uint32_t shift= (uint32_t)pin*2U;
	*reg = (*reg & ~(mask << shift)) | ((value & mask) << shift);
}
/* -----------------------------------------------------------------------
 * Internal helpers — not exposed in the header
 * --------------------------------------------------------------------- */

/**
 * @brief  Validate all fields in a UART_Config_t.
 * @param  cfg Pointer to UART configuration structure
 * @return UART_Status_t:
 *         - UART_OK on valid configuration
 *         - UART_ERROR if any parameter is invalid
 */
static UART_Status_t uart_validate_config(const UART_Config_t *cfg)
{
    /* Null pointer check */
    if (cfg == NULL)
    {
        return UART_ERR_NULL_PTR;
    }

    /* Baudrate validation */
    if (cfg->baudrate == 0U)
    {
        return UART_ERR_BAUDRATE;
    }

    /* Word length validation */
    if (cfg->word_length > UART_WORDLENGTH_9B)
    {
        return UART_ERR_WORDLENGTH;
    }

    /* Stop bits validation */
    if (cfg->stop_bits > UART_STOPBITS_1_5)
    {
        return UART_ERR_STOPBIT;
    }

    /* Parity validation */
    if (cfg->parity > UART_PARITY_ODD)
    {
        return UART_ERR_PARITY;
    }

    /* Mode validation */
    if (cfg->mode > UART_MODE_TX_RX)
    {
        return UART_ERR_MODE;
    }

    /* Hardware flow control validation */
    if (cfg->hw_flow_ctrl > UART_HWCONTROL_RTS_CTS)
    {
        return UART_ERR_FLOWCTRL;
    }

    /* Oversampling validation */
    if ((cfg->oversampling != 8U) && (cfg->oversampling != 16U))
    {
        return UART_ERR_OVERSAMPLING;
    }

    return UART_OK;
}
/* -----------------------------------------------------------------------
 * Public API implementation
 * --------------------------------------------------------------------- */
GPIO_Status_t GPIO_Init(const GPIO_Config_t *cfg){
    GPIO_Status_t status= gpio_validate_config(cfg);
    if(status!=GPIO_OK){
        return status;
    }
    




}


