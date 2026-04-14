#include<assert.h>
#include "stm32f4xx_gpio.h"
/* -----------------------------------------------------------------------
 * Board-level pin descriptors — one struct per logical signal.
 * Keeping them const lets the linker place them in flash.
 * --------------------------------------------------------------------- */
static const GPIO_Config_t LED_PA5 = {
    .port_base  = (volatile uint32_t *)GPIOA_BASE,
    .rcc_bit    = RCC_AHB1ENR_GPIOAEN,
    .pin        = 5U,
    .mode       = GPIO_MODER_OUTPUT,
    .otype      = GPIO_OTYPER_PP,        /* Push-pull — drive both edges  */
    .speed      = GPIO_OSPEEDR_LOW,      /* Low speed → lower EMI & power */
    .pull       = GPIO_PUPDR_NONE,       /* Output: no pull needed        */
    .init_state = GPIO_PIN_RESET,        /* LED off at startup            */
};

/* -----------------------------------------------------------------------
 * Error handler — replace with project-specific fault management.
 * In production this might log to a fault register and reset, or
 * trigger a watchdog if the error is non-recoverable.
 * --------------------------------------------------------------------- */
static void Error_Handler(GPIO_Status_t err)
{
    (void)err;          /* Suppress unused-variable warning in release builds */
    /* assert(0); */    /* Uncomment to halt in debug builds                  */
    while (1) { }       /* Trap — never return from an unrecoverable error    */
}

int main(void)
{
    GPIO_Status_t status;

    /* Initialise LED pin — clock, mode, pull, output level all configured */
    status = GPIO_Init(&LED_PA5);
    if (status != GPIO_OK) {
        Error_Handler(status);
    }

    /* Drive the LED high */
    status = GPIO_WritePin(&LED_PA5, GPIO_PIN_SET);
    if (status != GPIO_OK) {
        Error_Handler(status);
    }

    /* Main application loop */
    for (;;) {
        /* Application logic here */
    }

    return 0; /* Unreachable on bare-metal, but satisfies the compiler */
}
