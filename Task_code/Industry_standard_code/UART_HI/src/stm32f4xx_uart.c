/**
 * @file  stm32f4xx_uart.c
 * @brief UART / USART HAL driver implementation for STM32F4xx series
 *
 * Implements all functions declared in stm32f4xx_uart.h:
 *   UART_Init            — clock, BRR, CR1/CR2/CR3, UE
 *   UART_TransmitByte    — blocking poll on TXE
 *   UART_Transmit        — buffer TX, byte-by-byte
 *   UART_ReceiveByte     — blocking poll on RXNE + error check
 *   UART_Receive         — buffer RX, byte-by-byte
 *   UART_TransmitString  — null-terminated string TX
 *   UART_DeInit          — disable & gate clock
 *
 * Reference
 * ---------
 *   RM0090 Rev 19  §30  USART
 */
#include "stm32f4xx_uart.h"

/* -----------------------------------------------------------------------
 * Timeout constants
 * --------------------------------------------------------------------- */

/**
 * Polling loop iteration limit used by blocking TX / RX functions.
 * Sized for ~115200 baud at 84 MHz; increase for lower baud rates or
 * slower clocks.  Replace with a SysTick-based timeout in production.
 */
#define UART_POLL_TIMEOUT   100000U

/* -----------------------------------------------------------------------
 * Internal helpers — not exposed in the header
 * --------------------------------------------------------------------- */

/**
 * @brief  Validate a UART_Config_t pointer and its compulsory fields.
 * @return UART_OK or the first detected error code.
 */
static UART_Status_t uart_validate_config(const UART_Config_t *cfg)
{
    if (cfg == NULL || cfg->uart_base == NULL || cfg->rcc_apb_reg == NULL) {
        return UART_ERR_NULL_PTR;
    }
    if (cfg->baud_rate == 0U) {
        return UART_ERR_INVALID_BAUD;
    }
    if (cfg->word_length > UART_WORDLEN_9BIT) {
        return UART_ERR_INVALID_WORD;
    }
    if (cfg->stop_bits > UART_STOPBITS_2) {
        return UART_ERR_INVALID_STOP;
    }
    if (cfg->parity > UART_PARITY_ODD) {
        return UART_ERR_INVALID_PARITY;
    }
    if (cfg->mode > UART_MODE_TX_RX) {
        return UART_ERR_INVALID_MODE;
    }
    return UART_OK;
}

/**
 * @brief  Return a volatile pointer to a USART register at @p offset.
 *
 * @param  uart_base  Peripheral base address (from UART_Config_t).
 * @param  offset     One of the USART_*_OFFSET constants.
 */
static inline volatile uint32_t *uart_reg(volatile uint32_t *uart_base,
                                          uint32_t           offset)
{
    return (volatile uint32_t *)((uintptr_t)uart_base + offset);
}

/* -----------------------------------------------------------------------
 * Public API implementation
 * --------------------------------------------------------------------- */

/**
 * @brief  Initialise a USART / UART peripheral.
 *
 * Sequence (RM0090 §30.3.2 — Character transmission):
 *   1. Enable peripheral clock via rcc_apb_reg.
 *   2. Ensure USART is disabled (UE=0) before programming registers.
 *   3. Program BRR for the requested baud rate.
 *   4. Program CR2: stop bits.
 *   5. Program CR3: hardware flow control.
 *   6. Program CR1: word length, parity, TX/RX enable.
 *   7. Enable USART: set UE last.
 */
UART_Status_t UART_Init(const UART_Config_t *cfg)
{
    UART_Status_t status = uart_validate_config(cfg);
    if (status != UART_OK) {
        return status;
    }

    /* 1. Enable peripheral clock */
    *cfg->rcc_apb_reg |= cfg->rcc_en_bit;
    (void)*cfg->rcc_apb_reg; /* read-back to flush APB pipeline */

    /* 2. Disable USART before configuration (clear UE) */
    *uart_reg(cfg->uart_base, USART_CR1_OFFSET) &= ~USART_CR1_UE;

    /* 3. Baud rate — 16x oversampling: BRR = fCK / baud */
    *uart_reg(cfg->uart_base, USART_BRR_OFFSET) =
        USART_BRR_VALUE(cfg->pclk_hz, cfg->baud_rate);

    /* 4. CR2: stop bits — clear STOP field then write */
    uint32_t cr2 = *uart_reg(cfg->uart_base, USART_CR2_OFFSET);
    cr2 &= ~USART_CR2_STOP_MASK;
    if (cfg->stop_bits == UART_STOPBITS_2) {
        cr2 |= USART_CR2_STOP_2;
    }
    /* else UART_STOPBITS_1 → STOP_1 = 0b00 (already cleared) */
    *uart_reg(cfg->uart_base, USART_CR2_OFFSET) = cr2;

    /* 5. CR3: hardware flow control */
    uint32_t cr3 = *uart_reg(cfg->uart_base, USART_CR3_OFFSET);
    cr3 &= ~(USART_CR3_RTSE | USART_CR3_CTSE);
    if (cfg->hw_flow_ctrl == UART_HW_FLOW_RTS ||
        cfg->hw_flow_ctrl == UART_HW_FLOW_RTS_CTS) {
        cr3 |= USART_CR3_RTSE;
    }
    if (cfg->hw_flow_ctrl == UART_HW_FLOW_CTS ||
        cfg->hw_flow_ctrl == UART_HW_FLOW_RTS_CTS) {
        cr3 |= USART_CR3_CTSE;
    }
    *uart_reg(cfg->uart_base, USART_CR3_OFFSET) = cr3;

    /* 6. CR1: word length, parity, TX/RX enable — UE still 0 */
    uint32_t cr1 = 0U;

    /* Word length: CR1_M=0 → 8-bit, CR1_M=1 → 9-bit */
    if (cfg->word_length == UART_WORDLEN_9BIT) {
        cr1 |= USART_CR1_M;
    }

    /* Parity */
    if (cfg->parity == UART_PARITY_EVEN) {
        cr1 |= USART_CR1_PCE;          /* PCE=1, PS=0 → even               */
    } else if (cfg->parity == UART_PARITY_ODD) {
        cr1 |= USART_CR1_PCE | USART_CR1_PS; /* PCE=1, PS=1 → odd          */
    }

    /* TX / RX enable */
    if (cfg->mode == UART_MODE_TX || cfg->mode == UART_MODE_TX_RX) {
        cr1 |= USART_CR1_TE;
    }
    if (cfg->mode == UART_MODE_RX || cfg->mode == UART_MODE_TX_RX) {
        cr1 |= USART_CR1_RE;
    }

    *uart_reg(cfg->uart_base, USART_CR1_OFFSET) = cr1;

    /* 7. Enable USART — UE must be set last */
    *uart_reg(cfg->uart_base, USART_CR1_OFFSET) |= USART_CR1_UE;

    return UART_OK;
}

/**
 * @brief  Transmit a single byte (blocking — polls TXE flag).
 *
 * Waits until SR.TXE=1 (transmit data register empty), then writes
 * the byte to DR.  TC is not awaited — the byte is still shifting out
 * when this function returns.
 */
UART_Status_t UART_TransmitByte(const UART_Config_t *cfg, uint8_t data)
{
    UART_Status_t status = uart_validate_config(cfg);
    if (status != UART_OK) {
        return status;
    }

    volatile uint32_t *sr = uart_reg(cfg->uart_base, USART_SR_OFFSET);
    volatile uint32_t *dr = uart_reg(cfg->uart_base, USART_DR_OFFSET);

    /* Poll TXE until set or timeout */
    uint32_t timeout = UART_POLL_TIMEOUT;
    while (!(*sr & USART_SR_TXE)) {
        if (--timeout == 0U) {
            return UART_ERR_TX_TIMEOUT;
        }
    }

    *dr = (uint32_t)data & 0xFFU;
    return UART_OK;
}

/**
 * @brief  Transmit a buffer of bytes (blocking — polls TXE per byte).
 */
UART_Status_t UART_Transmit(const UART_Config_t *cfg,
                             const uint8_t       *buf,
                             uint32_t             len)
{
    if (buf == NULL) {
        return UART_ERR_NULL_PTR;
    }

    UART_Status_t status;
    for (uint32_t i = 0U; i < len; i++) {
        status = UART_TransmitByte(cfg, buf[i]);
        if (status != UART_OK) {
            return status;
        }
    }
    return UART_OK;
}

/**
 * @brief  Receive a single byte (blocking — polls RXNE flag).
 *
 * Waits until SR.RXNE=1 (receive data register not empty), reads DR,
 * then inspects SR for overrun (ORE) and framing (FE) errors.
 * Reading DR clears RXNE; clearing SR error flags requires an SR read
 * followed by a DR read (done implicitly here).
 */
UART_Status_t UART_ReceiveByte(const UART_Config_t *cfg, uint8_t *data)
{
    if (data == NULL) {
        return UART_ERR_NULL_PTR;
    }

    UART_Status_t status = uart_validate_config(cfg);
    if (status != UART_OK) {
        return status;
    }

    volatile uint32_t *sr = uart_reg(cfg->uart_base, USART_SR_OFFSET);
    volatile uint32_t *dr = uart_reg(cfg->uart_base, USART_DR_OFFSET);

    /* Poll RXNE until set or timeout */
    uint32_t timeout = UART_POLL_TIMEOUT;
    while (!(*sr & USART_SR_RXNE)) {
        if (--timeout == 0U) {
            return UART_ERR_RX_TIMEOUT;
        }
    }

    /* Sample SR for errors before reading DR (DR read clears RXNE & ORE) */
    uint32_t sr_snapshot = *sr;
    *data = (uint8_t)(*dr & 0xFFU);

    /* Check errors after DR read (flags are sticky until DR is read) */
    if (sr_snapshot & USART_SR_ORE) {
        return UART_ERR_OVERRUN;
    }
    if (sr_snapshot & USART_SR_FE) {
        return UART_ERR_FRAMING;
    }

    return UART_OK;
}

/**
 * @brief  Receive a buffer of bytes (blocking — polls RXNE per byte).
 */
UART_Status_t UART_Receive(const UART_Config_t *cfg,
                            uint8_t             *buf,
                            uint32_t             len)
{
    if (buf == NULL) {
        return UART_ERR_NULL_PTR;
    }

    UART_Status_t status;
    for (uint32_t i = 0U; i < len; i++) {
        status = UART_ReceiveByte(cfg, &buf[i]);
        if (status != UART_OK) {
            return status;
        }
    }
    return UART_OK;
}

/**
 * @brief  Transmit a null-terminated string (blocking).
 *
 * Iterates until '\0' is found; the null terminator is not transmitted.
 */
UART_Status_t UART_TransmitString(const UART_Config_t *cfg,
                                   const char          *str)
{
    if (str == NULL) {
        return UART_ERR_NULL_PTR;
    }

    UART_Status_t status;
    while (*str != '\0') {
        status = UART_TransmitByte(cfg, (uint8_t)*str);
        if (status != UART_OK) {
            return status;
        }
        str++;
    }
    return UART_OK;
}

/**
 * @brief  De-initialise a UART peripheral back to its reset state.
 *
 * Clears UE, TE, RE in CR1; zeros BRR; then gates the peripheral clock.
 * GPIO pins are left in AF mode — call GPIO_DeInit() separately if needed.
 */
UART_Status_t UART_DeInit(const UART_Config_t *cfg)
{
    UART_Status_t status = uart_validate_config(cfg);
    if (status != UART_OK) {
        return status;
    }

    /* Disable transmitter and receiver first, then disable USART */
    *uart_reg(cfg->uart_base, USART_CR1_OFFSET) &=
        ~(USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);

    /* Clear baud rate register */
    *uart_reg(cfg->uart_base, USART_BRR_OFFSET) = 0U;

    /* Clear CR2 and CR3 to reset values */
    *uart_reg(cfg->uart_base, USART_CR2_OFFSET) = 0U;
    *uart_reg(cfg->uart_base, USART_CR3_OFFSET) = 0U;

    /* Gate peripheral clock */
    *cfg->rcc_apb_reg &= ~cfg->rcc_en_bit;

    return UART_OK;
}
