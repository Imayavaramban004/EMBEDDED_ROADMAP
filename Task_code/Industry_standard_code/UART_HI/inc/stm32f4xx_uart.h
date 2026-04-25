/**
 * @file    stm32f4xx_uart.h
 * @brief   UART / USART HAL driver for STM32F4xx series
 * @version 1.0.0
 *
 * Provides a clean, validated API over raw memory-mapped registers.
 * All functions are reentrant and safe to call from ISR context
 * provided the caller does not share UART state.
 *
 * Peripherals supported
 * ---------------------
 *   USART1  APB2  0x4001 1000  (max clock 84 MHz)
 *   USART2  APB1  0x4000 4400  (max clock 42 MHz)
 *   USART6  APB2  0x4001 1400  (max clock 84 MHz)
 *
 * Reference
 * ---------
 *   RM0090 Rev 19  §30  USART
 */
 //Initialization of header guard
#ifndef STM32F4XX_UART_H
#define STM32F4XX_UART_H

#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx_rcc.h"   /* REG32, RCC_BASE, clock-enable bits           */
#include "stm32f4xx_gpio.h"  /* GPIO_Config_t, GPIO_AF7, GPIO_AF8            */

/* -----------------------------------------------------------------------
 * USART peripheral base addresses  (RM0090 §2.3 Memory Map)
 * --------------------------------------------------------------------- */
#define USART1_BASE     0x40011000UL   /* USART1 — APB2  (max 84 MHz)        */
#define USART2_BASE     0x40004400UL   /* USART2 — APB1  (max 42 MHz)        */
#define USART3_BASE     0x40004800UL   /* USART3 — APB1  (max 42 MHz)        */
#define UART4_BASE      0x40004C00UL   /* UART4  — APB1  (max 42 MHz)        */
#define UART5_BASE      0x40005000UL   /* UART5  — APB1  (max 42 MHz)        */
#define USART6_BASE     0x40011400UL   /* USART6 — APB2  (max 84 MHz)        */

/* -----------------------------------------------------------------------
 * USART register offsets (relative to USARTx_BASE)  — RM0090 §30.6
 * --------------------------------------------------------------------- */
#define USART_SR_OFFSET     0x00UL   /* Status register                              */
#define USART_DR_OFFSET     0x04UL   /* Data register  (TX write / RX read)          */
#define USART_BRR_OFFSET    0x08UL   /* Baud rate register                           */
#define USART_CR1_OFFSET    0x0CUL   /* Control register 1                           */
#define USART_CR2_OFFSET    0x10UL   /* Control register 2                           */
#define USART_CR3_OFFSET    0x14UL   /* Control register 3                           */
#define USART_GTPR_OFFSET   0x18UL   /* Guard time and prescaler register            */

/* -----------------------------------------------------------------------
 * USART1 named-register shortcuts
 * --------------------------------------------------------------------- */
#define USART1_SR       REG32(USART1_BASE, USART_SR_OFFSET)
#define USART1_DR       REG32(USART1_BASE, USART_DR_OFFSET)
#define USART1_BRR      REG32(USART1_BASE, USART_BRR_OFFSET)
#define USART1_CR1      REG32(USART1_BASE, USART_CR1_OFFSET)
#define USART1_CR2      REG32(USART1_BASE, USART_CR2_OFFSET)
#define USART1_CR3      REG32(USART1_BASE, USART_CR3_OFFSET)

/* -----------------------------------------------------------------------
 * USART2 named-register shortcuts
 * --------------------------------------------------------------------- */
#define USART2_SR       REG32(USART2_BASE, USART_SR_OFFSET)
#define USART2_DR       REG32(USART2_BASE, USART_DR_OFFSET)
#define USART2_BRR      REG32(USART2_BASE, USART_BRR_OFFSET)
#define USART2_CR1      REG32(USART2_BASE, USART_CR1_OFFSET)
#define USART2_CR2      REG32(USART2_BASE, USART_CR2_OFFSET)
#define USART2_CR3      REG32(USART2_BASE, USART_CR3_OFFSET)

/* -----------------------------------------------------------------------
 * USART6 named-register shortcuts
 * --------------------------------------------------------------------- */
#define USART6_SR       REG32(USART6_BASE, USART_SR_OFFSET)
#define USART6_DR       REG32(USART6_BASE, USART_DR_OFFSET)
#define USART6_BRR      REG32(USART6_BASE, USART_BRR_OFFSET)
#define USART6_CR1      REG32(USART6_BASE, USART_CR1_OFFSET)
#define USART6_CR2      REG32(USART6_BASE, USART_CR2_OFFSET)
#define USART6_CR3      REG32(USART6_BASE, USART_CR3_OFFSET)

/* -----------------------------------------------------------------------
 * Bit-field constants — USART_SR  (RM0090 §30.6.1)
 * --------------------------------------------------------------------- */
/** Status register — read-only flags */
#define USART_SR_PE     (1UL << 0)   /* Parity error                                */
#define USART_SR_FE     (1UL << 1)   /* Framing error                               */
#define USART_SR_NE     (1UL << 2)   /* Noise detected flag                         */
#define USART_SR_ORE    (1UL << 3)   /* Overrun error                               */
#define USART_SR_IDLE   (1UL << 4)   /* IDLE line detected                          */
#define USART_SR_RXNE   (1UL << 5)   /* Read data register not empty — RX ready     */
#define USART_SR_TC     (1UL << 6)   /* Transmission complete                       */
#define USART_SR_TXE    (1UL << 7)   /* Transmit data register empty — TX ready     */
#define USART_SR_LBD    (1UL << 8)   /* LIN break detection flag                    */
#define USART_SR_CTS    (1UL << 9)   /* CTS flag                                    */

/* -----------------------------------------------------------------------
 * Bit-field constants — USART_BRR  (RM0090 §30.3.4)
 * --------------------------------------------------------------------- */
/** BRR — mantissa [15:4] and fraction [3:0] */
#define USART_BRR_DIV_FRACTION_MASK  0x00FUL   /* Fractional part of USARTDIV        */
#define USART_BRR_DIV_MANTISSA_MASK  0xFF0UL   /* Integer  part of USARTDIV          */
#define USART_BRR_DIV_MANTISSA_SHIFT 4U

/**
 * @brief  Compute BRR value for 16x oversampling (OVER8=0).
 *
 * Formula: BRR = fCK / baud  (integer division, no rounding needed for
 * standard baud rates when fCK is a multiple of baud).
 *
 * Example: USART1_BRR = USART_BRR_VALUE(84000000UL, 115200UL);
 */
#define USART_BRR_VALUE(fck, baud)   ((uint32_t)((fck) / (baud)))

/* -----------------------------------------------------------------------
 * Bit-field constants — USART_CR1  (RM0090 §30.6.4)
 * --------------------------------------------------------------------- */
/** Control register 1 — enable / word / parity / interrupt bits */
#define USART_CR1_SBK       (1UL << 0)   /* Send break character                   */
#define USART_CR1_RWU       (1UL << 1)   /* Receiver wakeup mute mode              */
#define USART_CR1_RE        (1UL << 2)   /* Receiver enable                        */
#define USART_CR1_TE        (1UL << 3)   /* Transmitter enable                     */
#define USART_CR1_IDLEIE    (1UL << 4)   /* IDLE interrupt enable                  */
#define USART_CR1_RXNEIE    (1UL << 5)   /* RXNE interrupt enable                  */
#define USART_CR1_TCIE      (1UL << 6)   /* Transmission complete interrupt enable */
#define USART_CR1_TXEIE     (1UL << 7)   /* TXE interrupt enable                   */
#define USART_CR1_PEIE      (1UL << 8)   /* Parity error interrupt enable          */
#define USART_CR1_PS        (1UL << 9)   /* Parity selection: 0=even, 1=odd        */
#define USART_CR1_PCE       (1UL << 10)  /* Parity control enable                  */
#define USART_CR1_WAKE      (1UL << 11)  /* Wakeup method (0=idle, 1=address mark) */
#define USART_CR1_M         (1UL << 12)  /* Word length:  0=8-bit, 1=9-bit         */
#define USART_CR1_UE        (1UL << 13)  /* USART enable — set last                */
#define USART_CR1_OVER8     (1UL << 15)  /* Oversampling: 0=16x (default), 1=8x   */

/* -----------------------------------------------------------------------
 * Bit-field constants — USART_CR2  (RM0090 §30.6.5)
 * --------------------------------------------------------------------- */
/** Control register 2 — stop bits and synchronous clock */
#define USART_CR2_ADD_MASK      0x0FUL          /* Node address [3:0]              */
#define USART_CR2_LBDL          (1UL << 5)      /* LIN break detection length      */
#define USART_CR2_LBDIE         (1UL << 6)      /* LIN break detect interrupt en.  */
#define USART_CR2_LBCL          (1UL << 8)      /* Last bit clock pulse            */
#define USART_CR2_CPHA          (1UL << 9)      /* Clock phase                     */
#define USART_CR2_CPOL          (1UL << 10)     /* Clock polarity                  */
#define USART_CR2_CLKEN         (1UL << 11)     /* CK pin clock enable             */
#define USART_CR2_STOP_MASK     (0x3UL << 12)   /* Stop bits mask [13:12]          */
#define USART_CR2_STOP_1        (0x0UL << 12)   /* 1   stop bit  — most common     */
#define USART_CR2_STOP_0_5      (0x1UL << 12)   /* 0.5 stop bits — smartcard only  */
#define USART_CR2_STOP_2        (0x2UL << 12)   /* 2   stop bits                   */
#define USART_CR2_STOP_1_5      (0x3UL << 12)   /* 1.5 stop bits — smartcard only  */
#define USART_CR2_LINEN         (1UL << 14)     /* LIN mode enable                 */

/* -----------------------------------------------------------------------
 * Bit-field constants — USART_CR3  (RM0090 §30.6.6)
 * --------------------------------------------------------------------- */
/** Control register 3 — flow control / DMA / error interrupt */
#define USART_CR3_EIE       (1UL << 0)   /* Error interrupt enable (FE/NE/ORE)     */
#define USART_CR3_IREN      (1UL << 1)   /* IrDA mode enable                       */
#define USART_CR3_IRLP      (1UL << 2)   /* IrDA low-power mode                    */
#define USART_CR3_HDSEL     (1UL << 3)   /* Half-duplex selection                  */
#define USART_CR3_NACK      (1UL << 4)   /* Smartcard NACK enable                  */
#define USART_CR3_SCEN      (1UL << 5)   /* Smartcard mode enable                  */
#define USART_CR3_DMAR      (1UL << 6)   /* DMA enable receiver                    */
#define USART_CR3_DMAT      (1UL << 7)   /* DMA enable transmitter                 */
#define USART_CR3_RTSE      (1UL << 8)   /* RTS flow control enable                */
#define USART_CR3_CTSE      (1UL << 9)   /* CTS flow control enable                */
#define USART_CR3_CTSIE     (1UL << 10)  /* CTS interrupt enable                   */
#define USART_CR3_ONEBIT    (1UL << 11)  /* One sample bit method enable           */

/* -----------------------------------------------------------------------
 * Valid configuration parameter values
 * --------------------------------------------------------------------- */
#define UART_WORDLEN_8BIT    0U   /* 8 data bits — standard                         */
#define UART_WORDLEN_9BIT    1U   /* 9 data bits — used when parity is enabled       */

#define UART_STOPBITS_1      0U   /* 1   stop bit  (maps to USART_CR2_STOP_1)       */
#define UART_STOPBITS_2      1U   /* 2   stop bits (maps to USART_CR2_STOP_2)       */

#define UART_PARITY_NONE     0U   /* No parity bit                                   */
#define UART_PARITY_EVEN     1U   /* Even parity (CR1_PCE=1, CR1_PS=0)              */
#define UART_PARITY_ODD      2U   /* Odd  parity (CR1_PCE=1, CR1_PS=1)              */

#define UART_MODE_TX         0U   /* Transmit only                                   */
#define UART_MODE_RX         1U   /* Receive only                                    */
#define UART_MODE_TX_RX      2U   /* Full-duplex (TX + RX)                          */

#define UART_HW_FLOW_NONE    0U   /* No hardware flow control                        */
#define UART_HW_FLOW_RTS     1U   /* RTS enabled                                     */
#define UART_HW_FLOW_CTS     2U   /* CTS enabled                                     */
#define UART_HW_FLOW_RTS_CTS 3U   /* RTS + CTS — full hardware handshake            */

/* -----------------------------------------------------------------------
 * Public types
 * --------------------------------------------------------------------- */

/** Return status for all driver functions */
typedef enum {
    UART_OK                 =  0,   /**< Operation succeeded                        */
    UART_ERR_NULL_PTR       = -1,   /**< NULL pointer passed as argument            */
    UART_ERR_INVALID_BAUD   = -2,   /**< Baud rate is zero or out of valid range    */
    UART_ERR_INVALID_WORD   = -3,   /**< Word length not 8 or 9                     */
    UART_ERR_INVALID_STOP   = -4,   /**< Stop-bit value not 1 or 2                  */
    UART_ERR_INVALID_PARITY = -5,   /**< Parity value not NONE / EVEN / ODD         */
    UART_ERR_INVALID_MODE   = -6,   /**< Mode value not TX / RX / TX_RX             */
    UART_ERR_TX_TIMEOUT     = -7,   /**< TXE or TC flag never set (blocking TX)     */
    UART_ERR_RX_TIMEOUT     = -8,   /**< RXNE flag never set     (blocking RX)      */
    UART_ERR_OVERRUN        = -9,   /**< Overrun error detected in SR               */
    UART_ERR_FRAMING        = -10   /**< Framing error detected  in SR              */
} UART_Status_t;

/**
 * @brief  Complete UART configuration descriptor.
 *
 * Pass this struct to UART_Init().  Every field must be set by the caller;
 * there are no silent defaults so misconfigured structs fail validation.
 */
typedef struct {
    volatile uint32_t *uart_base;    /**< e.g. (volatile uint32_t *)USART1_BASE    */
    volatile uint32_t *rcc_apb_reg;  /**< Pointer to RCC APB enable register       */
    uint32_t           rcc_en_bit;   /**< e.g. RCC_APB2ENR_USART1EN               */
    uint32_t           pclk_hz;      /**< Peripheral bus clock in Hz (fCK)         */
    uint32_t           baud_rate;    /**< Baud rate in bps, e.g. 115200            */
    uint8_t            word_length;  /**< UART_WORDLEN_8BIT / UART_WORDLEN_9BIT    */
    uint8_t            stop_bits;    /**< UART_STOPBITS_1  / UART_STOPBITS_2       */
    uint8_t            parity;       /**< UART_PARITY_NONE / _EVEN / _ODD          */
    uint8_t            mode;         /**< UART_MODE_TX / _RX / _TX_RX              */
    uint8_t            hw_flow_ctrl; /**< UART_HW_FLOW_NONE / _RTS / _CTS / _RTS_CTS */
} UART_Config_t;

/* -----------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------- */

/**
 * @brief  Initialise a USART / UART peripheral.
 *
 * Enables the peripheral clock, programs BRR for the requested baud rate,
 * configures CR1/CR2/CR3, then enables the UART (UE bit set last to avoid
 * partial configuration reaching the line).
 *
 * @note   GPIO pins must be configured to AF mode before calling this
 *         function.  Use GPIO_Init() with mode=GPIO_MODER_AF and the
 *         appropriate AF number (GPIO_AF7 for USART1–3, GPIO_AF8 for
 *         UART4/5/USART6).
 *
 * @param  cfg  Pointer to a fully populated UART_Config_t struct.
 * @return UART_OK on success, negative UART_Status_t on error.
 */
UART_Status_t UART_Init(const UART_Config_t *cfg);

/**
 * @brief  Transmit a single byte (blocking — polls TXE flag).
 *
 * Waits until the transmit data register is empty, then writes the byte.
 * Returns immediately after the write; TC is NOT awaited.
 *
 * @param  cfg   Pointer to the config used in UART_Init().
 * @param  data  Byte to send.
 * @return UART_OK on success, UART_ERR_TX_TIMEOUT if TXE never set.
 */
UART_Status_t UART_TransmitByte(const UART_Config_t *cfg, uint8_t data);

/**
 * @brief  Transmit a buffer of bytes (blocking — polls TXE per byte).
 *
 * Calls UART_TransmitByte() in sequence for each byte in the buffer.
 * Stops and returns the error status if any byte transmission fails.
 *
 * @param  cfg    Pointer to the config used in UART_Init().
 * @param  buf    Pointer to the data buffer.
 * @param  len    Number of bytes to transmit.
 * @return UART_OK on success, negative UART_Status_t on first error.
 */
UART_Status_t UART_Transmit(const UART_Config_t *cfg,
                             const uint8_t       *buf,
                             uint32_t             len);

/**
 * @brief  Receive a single byte (blocking — polls RXNE flag).
 *
 * Waits until the receive data register is not empty, reads the byte,
 * then checks SR for overrun / framing errors.
 *
 * @param  cfg   Pointer to the config used in UART_Init().
 * @param  data  Output: populated with the received byte.
 * @return UART_OK on success, negative UART_Status_t on error.
 */
UART_Status_t UART_ReceiveByte(const UART_Config_t *cfg, uint8_t *data);

/**
 * @brief  Receive a buffer of bytes (blocking — polls RXNE per byte).
 *
 * Calls UART_ReceiveByte() in sequence until @p len bytes have been
 * received.  Stops and returns the error status on any receive fault.
 *
 * @param  cfg   Pointer to the config used in UART_Init().
 * @param  buf   Pointer to the destination buffer (must hold @p len bytes).
 * @param  len   Number of bytes to receive.
 * @return UART_OK on success, negative UART_Status_t on first error.
 */
UART_Status_t UART_Receive(const UART_Config_t *cfg,
                            uint8_t             *buf,
                            uint32_t             len);

/**
 * @brief  Transmit a null-terminated string (blocking).
 *
 * Convenience wrapper around UART_TransmitByte().  Stops at the first
 * '\0'; the null terminator itself is not transmitted.
 *
 * @param  cfg  Pointer to the config used in UART_Init().
 * @param  str  Pointer to the null-terminated string to send.
 * @return UART_OK on success, negative UART_Status_t on first error.
 */
UART_Status_t UART_TransmitString(const UART_Config_t *cfg,
                                   const char          *str);

/**
 * @brief  De-initialise a UART peripheral back to its reset state.
 *
 * Clears CR1 (disables UE, TE, RE), clears BRR, and gates the peripheral
 * clock via the relevant RCC APB enable register.
 *
 * @param  cfg  Pointer to the config used in UART_Init().
 * @return UART_OK on success, negative UART_Status_t on error.
 */
UART_Status_t UART_DeInit(const UART_Config_t *cfg);

#endif /* STM32F4XX_UART_H */
