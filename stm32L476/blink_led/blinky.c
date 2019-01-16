#include <stdint.h>

#include "stm32l4.h"

// ORIGIN = 0x20000000, LENGTH = 96K
#define STACK_LOCATION (0x20000000+(96*1024))

void System_Clock_Init(void) {

	/* Reset the RCC clock to the default reset state */
	/* Set MSION bit */
	RCC->CR |= RCC_CR_MSION;

	/* Reset CFGR register */
	RCC->CFGR = 0x00000000U;

	/* Reset HSEON, CSSON , HSION, and PLLON bits */
	RCC->CR &= 0xEAF6FFFFU;

	/* Reset PLLCFGR register */
	RCC->PLLCFGR = 0x00001000U;

	/* Reset HSEBYP bit */
	RCC->CR &= ~(RCC_CR_HSEBYP);

	/* Disable all interrupts */
	RCC->CIER = 0x00000000U;
}

static void delay(int length) {

        volatile int i;

        for(i=0;i<length;i++) asm("nop");

}

static void GPIOB_Clock_Enable(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
}

static void GPIOB_Pin_Init(int pin) {

        /* Set mode of pin 2 as output */
        /* 00 = input
           01 = output
           10 = alternate
           11 = analog (default) */

        GPIOB->MODER &= ~(3UL<<(pin*2));
        GPIOB->MODER |= 1UL<<(pin*2);

        /* set output type of pin2 as push-pull */
        /* 0 = push-pull (default)
           1 = open-drain */
        GPIOB->OTYPER &= ~(1<<pin);
        /* Set output speed of pin2 as low */
        /* 00 = Low speed
           01 = Medium speed
           10 = Fast speed
           11 = High speed */
        GPIOB->OSPEEDR &= ~(3UL<<(pin*2));

        /* Set pin 2 as no pull-up, no pull-down */
        /* 00 = no pull-up, no pull-down
           01 = pull-up
           10 = pull-down
           11 = reserved */
        GPIOB->PUPDR &=~(3UL<<(pin*2));
}

int main(void) {

	System_Clock_Init();

	GPIOB_Clock_Enable();
	GPIOB_Pin_Init(2);

	while(1) {
		GPIOB->ODR |= (1UL<<2);
		delay(300000);

		GPIOB->ODR &= ~(1UL<<2);
		delay(300000);
	}

	return 0;
}

void Reset_Handler(void) {
	main();
}

static void default_handler(void) {
	while(1);
}


/* Vector Table */
uint32_t *myvectors[128]
__attribute__ ((section(".isr_vector"))) = {
        (uint32_t *) STACK_LOCATION,	/* stack pointer      */
        (uint32_t *) Reset_Handler,	/* code entry point   */
        (uint32_t *) default_handler,	/* NMI handler        */
        (uint32_t *) default_handler,	/* hard fault handler */
};

