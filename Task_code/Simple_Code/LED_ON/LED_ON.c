// STEPS FOR LED_ON:
// SET THE RCC_AHB1ENR.
// SET THE GPIOA_MODER as Required
// SET THE GPIOA_PUPDR as PULL_DOWN for initialization
// SET THE GPIO_ODR

#include<stdio.h>
#include<stdint.h>
#define GPIOA_BASE 0x40020000
#define RCC_BASE 0x40023800

#define RCC_AHB1ENR (*(volatile uint32_t *)(RCC_BASE+0x30))
#define GPIOA5_MODER (*(volatile uint32_t *)(GPIOA_BASE+0x00))
#define GPIOA5_PUPDR (*(volatile uint32_t *)(GPIOA_BASE+0x0C))
#define GPIOA5_ODR (*(volatile uint32_t*)(GPIOA_BASE+0x14))

#define GPIO 5

//For Setting an Nth Bit left Shift 1 by N and Do Or Operation
void GPIO_MODER_OUTPUT(){
	GPIOA5_MODER &= ~(3<<(GPIO*2));
	GPIOA5_MODER |= (1<<(GPIO*2));
}
// void GPIO_INIT(){
	// GPIOA5_PUPDR &= ~(3<<(GPIO*2));
// }
void RCC_ENR(){
	RCC_AHB1ENR |= (1<<0);
}
void LED_ON(){
	GPIOA5_ODR|=(1<<GPIO);
}
int main(){
	RCC_ENR();
	GPIO_MODER_OUTPUT();
	// GPIO_INIT();
	int a=5;
	LED_ON();
	return 0;
}
