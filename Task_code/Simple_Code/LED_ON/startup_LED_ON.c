#include<stdint.h>


#define SRAM_START 0x20000000U
#define LENGTH_SRAM (128U*1024U)
#define SRAM_END ((SRAM_START) + (LENGTH_SRAM))

#define STACK_START SRAM_END

void Reset_handler(void);


// ================= DEFAULT HANDLER =================
void Default_handler(void);

// ================= CORE HANDLERS =================
void NMI_handler(void)            __attribute__((weak, alias("Default_handler")));
void HardFault_handler(void)      __attribute__((weak, alias("Default_handler")));
void MemManage_handler(void)      __attribute__((weak, alias("Default_handler")));
void BusFault_handler(void)       __attribute__((weak, alias("Default_handler")));
void UsageFault_handler(void)     __attribute__((weak, alias("Default_handler")));
void SVC_handler(void)            __attribute__((weak, alias("Default_handler")));
void DebugMon_handler(void)       __attribute__((weak, alias("Default_handler")));
void PendSV_handler(void)         __attribute__((weak, alias("Default_handler")));
void SysTick_handler(void)        __attribute__((weak, alias("Default_handler")));

// ================= EXTERNAL IRQ HANDLERS =================
void WWDG_handler(void)               __attribute__((weak, alias("Default_handler")));
void PVD_handler(void)                __attribute__((weak, alias("Default_handler")));
void TAMP_STAMP_handler(void)         __attribute__((weak, alias("Default_handler")));
void RTC_WKUP_handler(void)           __attribute__((weak, alias("Default_handler")));
void FLASH_handler(void)              __attribute__((weak, alias("Default_handler")));
void RCC_handler(void)                __attribute__((weak, alias("Default_handler")));
void EXTI0_handler(void)              __attribute__((weak, alias("Default_handler")));
void EXTI1_handler(void)              __attribute__((weak, alias("Default_handler")));
void EXTI2_handler(void)              __attribute__((weak, alias("Default_handler")));
void EXTI3_handler(void)              __attribute__((weak, alias("Default_handler")));
void EXTI4_handler(void)              __attribute__((weak, alias("Default_handler")));

void DMA1_Stream0_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA1_Stream1_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA1_Stream2_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA1_Stream3_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA1_Stream4_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA1_Stream5_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA1_Stream6_handler(void)       __attribute__((weak, alias("Default_handler")));

void ADC_handler(void)                __attribute__((weak, alias("Default_handler")));

void CAN1_TX_handler(void)            __attribute__((weak, alias("Default_handler")));
void CAN1_RX0_handler(void)           __attribute__((weak, alias("Default_handler")));
void CAN1_RX1_handler(void)           __attribute__((weak, alias("Default_handler")));
void CAN1_SCE_handler(void)           __attribute__((weak, alias("Default_handler")));

void EXTI9_5_handler(void)            __attribute__((weak, alias("Default_handler")));

void TIM1_BRK_TIM9_handler(void)      __attribute__((weak, alias("Default_handler")));
void TIM1_UP_TIM10_handler(void)      __attribute__((weak, alias("Default_handler")));
void TIM1_TRG_COM_TIM11_handler(void) __attribute__((weak, alias("Default_handler")));
void TIM1_CC_handler(void)            __attribute__((weak, alias("Default_handler")));

void TIM2_handler(void)               __attribute__((weak, alias("Default_handler")));
void TIM3_handler(void)               __attribute__((weak, alias("Default_handler")));
void TIM4_handler(void)               __attribute__((weak, alias("Default_handler")));

void I2C1_EV_handler(void)            __attribute__((weak, alias("Default_handler")));
void I2C1_ER_handler(void)            __attribute__((weak, alias("Default_handler")));
void I2C2_EV_handler(void)            __attribute__((weak, alias("Default_handler")));
void I2C2_ER_handler(void)            __attribute__((weak, alias("Default_handler")));

void SPI1_handler(void)               __attribute__((weak, alias("Default_handler")));
void SPI2_handler(void)               __attribute__((weak, alias("Default_handler")));

void USART1_handler(void)             __attribute__((weak, alias("Default_handler")));
void USART2_handler(void)             __attribute__((weak, alias("Default_handler")));
void USART3_handler(void)             __attribute__((weak, alias("Default_handler")));

void EXTI15_10_handler(void)          __attribute__((weak, alias("Default_handler")));
void RTC_Alarm_handler(void)          __attribute__((weak, alias("Default_handler")));
void OTG_FS_WKUP_handler(void)        __attribute__((weak, alias("Default_handler")));

void TIM8_BRK_TIM12_handler(void)     __attribute__((weak, alias("Default_handler")));
void TIM8_UP_TIM13_handler(void)      __attribute__((weak, alias("Default_handler")));
void TIM8_TRG_COM_TIM14_handler(void) __attribute__((weak, alias("Default_handler")));
void TIM8_CC_handler(void)            __attribute__((weak, alias("Default_handler")));

void DMA1_Stream7_handler(void)       __attribute__((weak, alias("Default_handler")));

void FSMC_handler(void)               __attribute__((weak, alias("Default_handler")));
void SDIO_handler(void)               __attribute__((weak, alias("Default_handler")));

void TIM5_handler(void)               __attribute__((weak, alias("Default_handler")));
void SPI3_handler(void)               __attribute__((weak, alias("Default_handler")));

void UART4_handler(void)              __attribute__((weak, alias("Default_handler")));
void UART5_handler(void)              __attribute__((weak, alias("Default_handler")));

void TIM6_DAC_handler(void)           __attribute__((weak, alias("Default_handler")));
void TIM7_handler(void)               __attribute__((weak, alias("Default_handler")));

void DMA2_Stream0_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA2_Stream1_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA2_Stream2_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA2_Stream3_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA2_Stream4_handler(void)       __attribute__((weak, alias("Default_handler")));

void CAN2_TX_handler(void)            __attribute__((weak, alias("Default_handler")));
void CAN2_RX0_handler(void)           __attribute__((weak, alias("Default_handler")));
void CAN2_RX1_handler(void)           __attribute__((weak, alias("Default_handler")));
void CAN2_SCE_handler(void)           __attribute__((weak, alias("Default_handler")));

void OTG_FS_handler(void)             __attribute__((weak, alias("Default_handler")));

void DMA2_Stream5_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA2_Stream6_handler(void)       __attribute__((weak, alias("Default_handler")));
void DMA2_Stream7_handler(void)       __attribute__((weak, alias("Default_handler")));

void USART6_handler(void)             __attribute__((weak, alias("Default_handler")));

void I2C3_EV_handler(void)            __attribute__((weak, alias("Default_handler")));
void I2C3_ER_handler(void)            __attribute__((weak, alias("Default_handler")));

void OTG_HS_EP1_OUT_handler(void)     __attribute__((weak, alias("Default_handler")));
void OTG_HS_EP1_IN_handler(void)      __attribute__((weak, alias("Default_handler")));
void OTG_HS_WKUP_handler(void)        __attribute__((weak, alias("Default_handler")));
void OTG_HS_handler(void)             __attribute__((weak, alias("Default_handler")));

void DCMI_handler(void)               __attribute__((weak, alias("Default_handler")));

void FPU_handler(void)                __attribute__((weak, alias("Default_handler")));

void SPI4_handler(void)               __attribute__((weak, alias("Default_handler")));
void SAI1_handler(void)               __attribute__((weak, alias("Default_handler")));
void SAI2_handler(void)               __attribute__((weak, alias("Default_handler")));
void QuadSPI_handler(void)            __attribute__((weak, alias("Default_handler")));
void HDMI_CEC_handler(void)           __attribute__((weak, alias("Default_handler")));
void SPDIF_RX_handler(void)           __attribute__((weak, alias("Default_handler")));
void FMPI2C1_EV_handler(void)         __attribute__((weak, alias("Default_handler")));
void FMPI2C1_ER_handler(void)         __attribute__((weak, alias("Default_handler")));

uint32_t vector_table[] __attribute__((section(".isr_vector"))) =
{
    STACK_START,
    (uint32_t)&Reset_handler,
    (uint32_t)&NMI_handler,
    (uint32_t)&HardFault_handler,
    (uint32_t)&MemManage_handler,
    (uint32_t)&BusFault_handler,
    (uint32_t)&UsageFault_handler,
    0, 0, 0, 0,
    (uint32_t)&SVC_handler,
    (uint32_t)&DebugMon_handler,
    0,
    (uint32_t)&PendSV_handler,
    (uint32_t)&SysTick_handler,

    // ===== External Interrupts =====
    (uint32_t)&WWDG_handler,          // 0
    (uint32_t)&PVD_handler,           // 1
    (uint32_t)&TAMP_STAMP_handler,    // 2
    (uint32_t)&RTC_WKUP_handler,      // 3
    (uint32_t)&FLASH_handler,         // 4
    (uint32_t)&RCC_handler,           // 5
    (uint32_t)&EXTI0_handler,         // 6
    (uint32_t)&EXTI1_handler,         // 7
    (uint32_t)&EXTI2_handler,         // 8
    (uint32_t)&EXTI3_handler,         // 9
    (uint32_t)&EXTI4_handler,         // 10
    (uint32_t)&DMA1_Stream0_handler,  // 11
    (uint32_t)&DMA1_Stream1_handler,  // 12
    (uint32_t)&DMA1_Stream2_handler,  // 13
    (uint32_t)&DMA1_Stream3_handler,  // 14
    (uint32_t)&DMA1_Stream4_handler,  // 15
    (uint32_t)&DMA1_Stream5_handler,  // 16
    (uint32_t)&DMA1_Stream6_handler,  // 17
    (uint32_t)&ADC_handler,           // 18
    (uint32_t)&CAN1_TX_handler,       // 19
    (uint32_t)&CAN1_RX0_handler,      // 20
    (uint32_t)&CAN1_RX1_handler,      // 21
    (uint32_t)&CAN1_SCE_handler,      // 22
    (uint32_t)&EXTI9_5_handler,       // 23
    (uint32_t)&TIM1_BRK_TIM9_handler, // 24
    (uint32_t)&TIM1_UP_TIM10_handler, // 25
    (uint32_t)&TIM1_TRG_COM_TIM11_handler,// 26
    (uint32_t)&TIM1_CC_handler,       // 27
    (uint32_t)&TIM2_handler,          // 28
    (uint32_t)&TIM3_handler,          // 29
    (uint32_t)&TIM4_handler,          // 30
    (uint32_t)&I2C1_EV_handler,       // 31
    (uint32_t)&I2C1_ER_handler,       // 32
    (uint32_t)&I2C2_EV_handler,       // 33
    (uint32_t)&I2C2_ER_handler,       // 34
    (uint32_t)&SPI1_handler,          // 35
    (uint32_t)&SPI2_handler,          // 36
    (uint32_t)&USART1_handler,        // 37
    (uint32_t)&USART2_handler,        // 38
    (uint32_t)&USART3_handler,        // 39
    (uint32_t)&EXTI15_10_handler,     // 40
    (uint32_t)&RTC_Alarm_handler,     // 41
    (uint32_t)&OTG_FS_WKUP_handler,   // 42
    (uint32_t)&TIM8_BRK_TIM12_handler,// 43
    (uint32_t)&TIM8_UP_TIM13_handler, // 44
    (uint32_t)&TIM8_TRG_COM_TIM14_handler,// 45
    (uint32_t)&TIM8_CC_handler,       // 46
    (uint32_t)&DMA1_Stream7_handler,  // 47
    (uint32_t)&FSMC_handler,          // 48
    (uint32_t)&SDIO_handler,          // 49
    (uint32_t)&TIM5_handler,          // 50
    (uint32_t)&SPI3_handler,          // 51
    (uint32_t)&UART4_handler,         // 52
    (uint32_t)&UART5_handler,         // 53
    (uint32_t)&TIM6_DAC_handler,      // 54
    (uint32_t)&TIM7_handler,          // 55
    (uint32_t)&DMA2_Stream0_handler,  // 56
    (uint32_t)&DMA2_Stream1_handler,  // 57
    (uint32_t)&DMA2_Stream2_handler,  // 58
    (uint32_t)&DMA2_Stream3_handler,  // 59
    (uint32_t)&DMA2_Stream4_handler,  // 60
    0,0,			      // 61, 62	
    (uint32_t)&CAN2_TX_handler,       // 63
    (uint32_t)&CAN2_RX0_handler,      // 64
    (uint32_t)&CAN2_RX1_handler,      // 65
    (uint32_t)&CAN2_SCE_handler,      // 66
    (uint32_t)&OTG_FS_handler,        // 67
    (uint32_t)&DMA2_Stream5_handler,  // 68
    (uint32_t)&DMA2_Stream6_handler,  // 69
    (uint32_t)&DMA2_Stream7_handler,  // 70
    (uint32_t)&USART6_handler,        // 71
    (uint32_t)&I2C3_EV_handler,       // 72
    (uint32_t)&I2C3_ER_handler,       // 73
    (uint32_t)&OTG_HS_EP1_OUT_handler,// 74
    (uint32_t)&OTG_HS_EP1_IN_handler, // 75
    (uint32_t)&OTG_HS_WKUP_handler,   // 76
    (uint32_t)&OTG_HS_handler,        // 77
    (uint32_t)&DCMI_handler,          // 78
    0, 0,                             // 79, 80 Reserved
    (uint32_t)&FPU_handler,           // 81
    0, 0,                             // 82–83 Reserved
    (uint32_t)&SPI4_handler,          // 84
    0, 0,                             // 85-86 Reserved 
    (uint32_t)&SAI1_handler,          // 87
    0, 0, 0,                          // 88–90 Reserved
    (uint32_t)&SAI2_handler,          // 91
    (uint32_t)&QuadSPI_handler,       // 92
    (uint32_t)&HDMI_CEC_handler,      // 93
    (uint32_t)&SPDIF_RX_handler,      // 94
    (uint32_t)&FMPI2C1_EV_handler,    // 95
    (uint32_t)&FMPI2C1_ER_handler     // 96
};
void Reset_handler(void){
}
void Default_handler(void){
	while(1);
}
