#ifndef STM32F4XX_H
#define STM32F4XX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* ================== COMPILER ATTRIBUTES ================== */
#ifndef __IO
#define __IO volatile
#endif

#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif

/* ================== BASE ADDRESSES ================== */
#define PERIPH_BASE        (0x40000000UL)
#define AHB1PERIPH_BASE    (PERIPH_BASE + 0x00020000UL)
#define APB1PERIPH_BASE    (PERIPH_BASE + 0x00000000UL)

/* Peripheral Base Addresses */
#define RCC_BASE           (AHB1PERIPH_BASE + 0x3800UL)
#define GPIOA_BASE         (AHB1PERIPH_BASE + 0x0000UL)
#define UART4_BASE         (APB1PERIPH_BASE + 0x4C00UL)

/* SysTick Base */
#define SYSTICK_BASE       (0xE000E010UL)

/* ================== REGISTER ACCESS ================== */
#define REG32(addr)        (*((__IO uint32_t *)(addr)))

/* ================== BIT MANIPULATION ================== */
#define SET_BIT(REG, BIT)      ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)    ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)     ((REG) & (BIT))
#define WRITE_REG(REG, VAL)    ((REG) = (VAL))

/* ================== RCC ================== */
#define RCC_AHB1ENR        REG32(RCC_BASE + 0x30UL)
#define RCC_APB1ENR        REG32(RCC_BASE + 0x40UL)

/* ================== GPIO ================== */
#define GPIOA_MODER        REG32(GPIOA_BASE + 0x00UL)
#define GPIOA_OTYPER       REG32(GPIOA_BASE + 0x04UL)
#define GPIOA_OSPEEDR      REG32(GPIOA_BASE + 0x08UL)
#define GPIOA_PUPDR        REG32(GPIOA_BASE + 0x0CUL)
#define GPIOA_AFRL         REG32(GPIOA_BASE + 0x20UL)
#define GPIOA_AFRH         REG32(GPIOA_BASE + 0x24UL)

/* ================== SYSTICK ================== */
#define STK_CTRL           REG32(SYSTICK_BASE + 0x00UL)
#define STK_LOAD           REG32(SYSTICK_BASE + 0x04UL)
#define STK_VAL            REG32(SYSTICK_BASE + 0x08UL)
#define STK_CALIB          REG32(SYSTICK_BASE + 0x0CUL)

/* ================== GPIO_MAX_MIN_PINS ================== */
#define GPIO_PINS_MAX 15U
#define GPIO_PINS_MIN 0U

/* ================== FIELD POSITION HELPERS ================== */
#define GPIO_PIN_POS(pin)        ((pin) * 2U)
#define GPIO_AF_POS(pin)         ((pin) * 4U)

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
#define GPIO_OTYPER_PP      0x00U
#define GPIO_OTYPER_OD      0x01U

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

/* BSRR register bit definitions */
#define GPIO_BSRR_RESET_OFFSET 16U
#define GPIO_BSRR_SET_OFFSET   0U

/** Alternate Function selection — 4 bits per pin */
#define GPIO_AF0   0x00U
#define GPIO_AF1   0x01U
#define GPIO_AF2   0x02U
#define GPIO_AF3   0x03U
#define GPIO_AF4   0x04U
#define GPIO_AF5   0x05U
#define GPIO_AF6   0x06U
#define GPIO_AF7   0x07U
#define GPIO_AF8   0x08U
#define GPIO_AF9   0x09U
#define GPIO_AF10  0x0AU
#define GPIO_AF11  0x0BU
#define GPIO_AF12  0x0CU
#define GPIO_AF13  0x0DU
#define GPIO_AF14  0x0EU
#define GPIO_AF15  0x0FU

#define GPIO_AF_MASK  0x0FU

/* ================== GPIO TYPES ================== */
typedef enum{
    GPIO_OK = 0,
    GPIO_ERR_NULL_PTR,
    GPIO_ERR_INVALID_PIN,
    GPIO_ERR_INVALID_MODE
} GPIO_Status_t;

typedef enum{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_Pinstate_t;

typedef struct{
    __IO uint32_t *port_base;
    uint32_t rcc_bit;
    uint8_t pin;
    uint8_t mode;
    uint8_t otype;
    uint8_t speed;
    uint8_t pull;
    uint8_t AFRL;
    uint8_t AFRH;
    uint8_t init_state;
} GPIO_Config_t;

/* ================== UART TYPES ================== */
typedef enum {
    UART_OK = 0,
    UART_ERR_NULL_PTR        = -1,
    UART_ERR_BAUDRATE        = -2,
    UART_ERR_WORDLENGTH      = -3,
    UART_ERR_STOPBIT         = -4,
    UART_ERR_PARITY          = -5,
    UART_ERR_MODE            = -6,
    UART_ERR_FLOWCTRL        = -7,
    UART_ERR_OVERSAMPLING    = -8
} UART_Status_t;

typedef struct{
    uint32_t baudrate;
    uint8_t  word_length;
    uint8_t  stop_bits;
    uint8_t  parity;
    uint8_t  mode;
    uint8_t  hw_flow_ctrl;
    uint8_t  oversampling;
} UART_Config_t;

/* ================== UART ENUMS ================== */
typedef enum {
    UART_WORDLENGTH_8B = 0,
    UART_WORDLENGTH_9B
} UART_WordLength_t;

typedef enum {
    UART_STOPBITS_1 = 0,
    UART_STOPBITS_0_5,
    UART_STOPBITS_2,
    UART_STOPBITS_1_5
} UART_StopBits_t;

typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN,
    UART_PARITY_ODD
} UART_Parity_t;

typedef enum {
    UART_MODE_TX = 1,
    UART_MODE_RX = 2,
    UART_MODE_TX_RX = 3
} UART_Mode_t;

typedef enum {
    UART_HWCONTROL_NONE = 0,
    UART_HWCONTROL_RTS,
    UART_HWCONTROL_CTS,
    UART_HWCONTROL_RTS_CTS
} UART_HWControl_t;

/* ================== INLINE HELPERS ================== */
__STATIC_INLINE void GPIO_SetAF(__IO uint32_t *afr_reg, uint8_t pin, uint8_t af)
{
    *afr_reg &= ~(GPIO_AF_MASK << GPIO_AF_POS(pin));
    *afr_reg |=  (af << GPIO_AF_POS(pin));
}

/* -----------------------------------------------------------------------
 * GPIO API
 * --------------------------------------------------------------------- */

/**
 * @brief  Initialize a GPIO pin based on the provided configuration.
 * @param  cfg Pointer to GPIO_Config_t structure containing pin settings such as
 *             mode, output type, speed, pull configuration, alternate function,
 *             and initial output state.
 * @return GPIO_Status_t
 *         @retval GPIO_OK               Initialization successful
 *         @retval GPIO_ERR_NULL_PTR     cfg pointer is NULL
 *         @retval GPIO_ERR_INVALID_PIN  Pin number out of range
 *         @retval GPIO_ERR_INVALID_MODE Invalid GPIO mode configuration
 */
GPIO_Status_t GPIO_Init(const GPIO_Config_t *cfg);

/**
 * @brief  Write a logic level to a GPIO pin.
 * @param  cfg   Pointer to GPIO configuration structure.
 * @param  state Desired logic level to be written to the pin.
 *               Possible values: GPIO_PIN_SET or GPIO_PIN_RESET.
 * @return GPIO_Status_t
 *         @retval GPIO_OK           Write successful
 *         @retval GPIO_ERR_NULL_PTR cfg pointer is NULL
 */
GPIO_Status_t GPIO_WritePin(const GPIO_Config_t *cfg, GPIO_Pinstate_t state);

/**
 * @brief  Read the current logic level of a GPIO pin.
 * @param  cfg   Pointer to GPIO configuration structure.
 * @param  state Pointer to variable where the read pin state will be stored.
 *               Output values: GPIO_PIN_SET or GPIO_PIN_RESET.
 * @return GPIO_Status_t
 *         @retval GPIO_OK           Read successful
 *         @retval GPIO_ERR_NULL_PTR cfg or state pointer is NULL
 */
GPIO_Status_t GPIO_ReadPin(const GPIO_Config_t *cfg, GPIO_Pinstate_t *state);

/**
 * @brief  Toggle the current output state of a GPIO pin.
 * @param  cfg Pointer to GPIO configuration structure.
 * @return GPIO_Status_t
 *         @retval GPIO_OK           Toggle successful
 *         @retval GPIO_ERR_NULL_PTR cfg pointer is NULL
 */
GPIO_Status_t GPIO_TogglePin(const GPIO_Config_t *cfg);

/* -----------------------------------------------------------------------
 * UART API
 * --------------------------------------------------------------------- */

/**
 * @brief  Initialize UART peripheral using the provided configuration.
 * @param  cfg Pointer to UART_Config_t structure containing baud rate,
 *             word length, stop bits, parity, mode, hardware flow control,
 *             and oversampling settings.
 * @return UART_Status_t
 *         @retval UART_OK    Initialization successful
 *         @retval UART_ERROR Initialization failed or invalid config
 */
UART_Status_t UART_Init(const UART_Config_t *cfg);

/**
 * @brief  Transmit a single byte over UART.
 * @param  cfg  Pointer to UART configuration structure.
 * @param  data Byte to be transmitted.
 * @return UART_Status_t
 *         @retval UART_OK    Transmission successful
 *         @retval UART_ERROR Transmission failed
 */
UART_Status_t UART_SendByte(const UART_Config_t *cfg, uint8_t data);

/**
 * @brief  Receive a single byte over UART.
 * @param  cfg  Pointer to UART configuration structure.
 * @param  data Pointer to variable where received byte will be stored.
 * @return UART_Status_t
 *         @retval UART_OK    Reception successful
 *         @retval UART_ERROR Reception failed or NULL pointer
 */
UART_Status_t UART_ReceiveByte(const UART_Config_t *cfg, uint8_t *data);

/**
 * @brief  Transmit multiple bytes over UART.
 * @param  cfg    Pointer to UART configuration structure.
 * @param  buffer Pointer to data buffer containing bytes to transmit.
 * @param  length Number of bytes to transmit.
 * @return UART_Status_t
 *         @retval UART_OK    Transmission successful
 *         @retval UART_ERROR Transmission failed or invalid buffer
 */
UART_Status_t UART_SendBuffer(const UART_Config_t *cfg, const uint8_t *buffer, uint16_t length);

/**
 * @brief  Receive multiple bytes over UART.
 * @param  cfg    Pointer to UART configuration structure.
 * @param  buffer Pointer to buffer where received data will be stored.
 * @param  length Number of bytes to receive.
 * @return UART_Status_t
 *         @retval UART_OK    Reception successful
 *         @retval UART_ERROR Reception failed or invalid buffer
 */
UART_Status_t UART_ReceiveBuffer(const UART_Config_t *cfg, uint8_t *buffer, uint16_t length);

/**
 * @brief  Transmit a null-terminated string over UART.
 * @param  cfg Pointer to UART configuration structure.
 * @param  str Pointer to null-terminated string.
 * @return UART_Status_t
 *         @retval UART_OK    Transmission successful
 *         @retval UART_ERROR Transmission failed
 */
UART_Status_t UART_SendString(const UART_Config_t *cfg, const char *str);

/**
 * @brief  Check whether UART transmit register is ready.
 * @param  cfg Pointer to UART configuration structure.
 * @return UART_Status_t
 *         @retval UART_OK    Transmit register is ready
 *         @retval UART_ERROR Not ready
 */
UART_Status_t UART_IsTxReady(const UART_Config_t *cfg);

/**
 * @brief  Check whether UART receive register contains data.
 * @param  cfg Pointer to UART configuration structure.
 * @return UART_Status_t
 *         @retval UART_OK    Data available
 *         @retval UART_ERROR No data available
 */
UART_Status_t UART_IsRxReady(const UART_Config_t *cfg);

/**
 * @brief  Enable UART interrupts.
 * @param  cfg Pointer to UART configuration structure.
 * @return UART_Status_t
 *         @retval UART_OK    Interrupt enabled successfully
 *         @retval UART_ERROR Failed to enable interrupt
 */
UART_Status_t UART_EnableInterrupt(const UART_Config_t *cfg);

/**
 * @brief  Disable UART interrupts.
 * @param  cfg Pointer to UART configuration structure.
 * @return UART_Status_t
 *         @retval UART_OK    Interrupt disabled successfully
 *         @retval UART_ERROR Failed to disable interrupt
 */
UART_Status_t UART_DisableInterrupt(const UART_Config_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* STM32F4XX_H */