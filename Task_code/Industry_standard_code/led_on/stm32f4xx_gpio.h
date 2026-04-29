/**
 * @file    stm32f4xx_gpio.h
 * @brief   GPIO HAL driver for STM32F4xx series
 * @version 1.0.0
 *
 * Provides a clean, validated API over raw memory-mapped registers.
 * All functions are reentrant and safe to call from ISR context
 * provided the caller does not share pin state.
 */
#ifndef STM32F4XX_GPIO_H
#define STM32F4XX_GPIO_H

#include<stdio.h>
#include<stddef.h>

/* -----------------------------------------------------------------------
 * Peripheral base addresses
 * --------------------------------------------------------------------- */
#define RCC_BASE        0x40023800UL
#define GPIOA_BASE      0x40020000UL
#define GPIOB_BASE      0x40020400UL
#define GPIOC_BASE      0x40020800UL

#define RCC_CR          (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR     (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR        (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_CIR         (*(volatile uint32_t *)(RCC_BASE + 0x0C))
#define RCC_AHB1RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x10))
#define RCC_AHB2RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_AHB3RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x20))
#define RCC_APB2RSTR    (*(volatile uint32_t *)(RCC_BASE + 0x24))
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_AHB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x34))
#define RCC_AHB3ENR     (*(volatile uint32_t *)(RCC_BASE + 0x38))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44))
#define RCC_AHB1LPENR   (*(volatile uint32_t *)(RCC_BASE + 0x50))
#define RCC_APB1LPENR   (*(volatile uint32_t *)(RCC_BASE + 0x60))
#define RCC_APB2LPENR   (*(volatile uint32_t *)(RCC_BASE + 0x64))
#define RCC_BDCR        (*(volatile uint32_t *)(RCC_BASE + 0x70))
#define RCC_CSR         (*(volatile uint32_t *)(RCC_BASE + 0x74))
#define RCC_PLLI2SCFGR  (*(volatile uint32_t *)(RCC_BASE + 0x84))
#define RCC_PLLSAICFGR  (*(volatile uint32_t *)(RCC_BASE + 0x88))
#define RCC_DCKCFGR     (*(volatile uint32_t *)(RCC_BASE + 0x8C))


/* -----------------------------------------------------------------------
 * Bit definitions for RCC  Pheripheral registers
 * --------------------------------------------------------------------- */

#define RCC_AHB1ENR_GPIOAEN   (1U << 0)
#define RCC_AHB1ENR_GPIOBEN   (1U << 1)
#define RCC_AHB1ENR_GPIOCEN   (1U << 2)
#define RCC_AHB1ENR_GPIODEN   (1U << 3)
/* ------------------------------------------------------------------------------- */
#define RCC_CR_HSION          (1U << 0)
#define RCC_CR_HSIRDY         (1U << 1)
#define RCC_CR_HSEON          (1U << 16)
#define RCC_CR_HSERDY         (1U << 17)
#define RCC_CR_PLLON          (1U << 24)
#define RCC_CR_PLLRDY         (1U << 25)
#define RCC_CFGR_SW_HSI       (0U << 0)
#define RCC_CFGR_SW_HSE       (1U << 0)
#define RCC_CFGR_SW_PLL       (2U << 0)
/* -----------------------------------------------------------------------
 * Register offsets (relative to GPIOx_BASE)
 * --------------------------------------------------------------------- */
#define GPIO_MODER_OFFSET   0x00UL
#define GPIO_OTYPER_OFFSET  0x04UL
#define GPIO_OSPEEDR_OFFSET 0x08UL
#define GPIO_PUPDR_OFFSET   0x0CUL
#define GPIO_IDR_OFFSET     0x10UL
#define GPIO_ODR_OFFSET     0x14UL
#define GPIO_BSRR_OFFSET    0x18UL
/* RCC AHB1 enable register offset */
#define RCC_AHB1ENR_OFFSET  0x30UL
/* -----------------------------------------------------------------------
 * Register access macros
 * All accesses are volatile to prevent optimisation across I/O boundaries.
 * --------------------------------------------------------------------- */