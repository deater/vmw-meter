/* Play PT3 chiptunes out the DAC port on the stm32l476 using DMA */

/* Outputs audio on pin PA5 */

#include <stdint.h>
#include "stm32l476xx.h"
#include "string.h"
#include "stdlib.h"

#include "lcd.h"

#include "pt3_lib.h"
#include "ayemu.h"

#define FREQ	44100
#define CHANS	1
#define BITS	16

/* global variables */
volatile uint32_t TimeDelay;
volatile uint32_t overflows=0;

static ayemu_ay_t ay;
static struct pt3_song_t pt3,pt3_2;

static ayemu_ay_reg_frame_t frame;
//static unsigned char frame[14];

#define MAX_SONGS 4
#include "i2_pt3.h"
#include "ba_pt3.h"
#include "ea_pt3.h"
#include "vc_pt3.h"

static int which_song=0;
static int title_len;
static int scrolling=0,scrolldir=1;


void exit(int status) {
	LCD_Display_String("ERROR");
	while(1);
}

struct pt3_image_t pt3_image[MAX_SONGS] = {
	[2] = {	.data=__I2_PT3,	.length=__I2_PT3_len, },
	[3] = {	.data=__BA_PT3,	.length=__BA_PT3_len, },
	[0] = {	.data=__EA_PT3,	.length=__EA_PT3_len, },
	[1] = {	.data=__VC_PT3,	.length=__VC_PT3_len, },
};


static int line=0,subframe=0,current_pattern=0;

static void change_song(void) {

	scrolling=0;
	scrolldir=1;

	if (which_song>=MAX_SONGS) {
		which_song=0;
	}

	pt3_load_song(&pt3_image[which_song], &pt3, &pt3_2);
	title_len=32;
	while(pt3_image[which_song].data[0x1e + title_len]==' ') {
		title_len--;
		if (title_len==0) break;
	}
	if (title_len<6) title_len=6;

	current_pattern=0;
	line=0;
	subframe=0;
}


/* advance definitions */
void System_Clock_Init(void);
void NVIC_SetPriority(int irq, int priority);
void NVIC_EnableIRQ(int irq);



/* mono (2 channel), 16-bit (2 bytes), play at 50Hz */
#define AUDIO_BUFSIZ (FREQ*CHANS*(BITS/8) / 50)
#define NUM_SAMPLES (AUDIO_BUFSIZ/CHANS/(BITS/8))
#define COUNTDOWN_RESET (FREQ/50)

static unsigned char audio_buf[AUDIO_BUFSIZ*2];

/* Interrupt Handlers */
static void NextBuffer(int which_half) {

	int line_decode_result=0;

	/* Decode next frame */
	if ((line==0) && (subframe==0)) {
		if (current_pattern==pt3.music_len) {
			which_song++;
			change_song();
			current_pattern=0;
		}
		pt3_set_pattern(current_pattern,&pt3);
	}

	if (subframe==0) {
		line_decode_result=pt3_decode_line(&pt3);
	}

	if (line_decode_result==1) {
		/* line done early? */
		current_pattern++;
		line=0;
		subframe=0;
	}
	else {
		subframe++;
		if (subframe==pt3.speed) {
			subframe=0;
			line++;
			if (line==64) {
				current_pattern++;
				line=0;
			}
		}
	}


	pt3_make_frame(&pt3,frame);

	/* Update AY buffer */
	ayemu_set_regs(&ay,frame);

	/* Generate sound buffer */
	if (which_half==0) {
		ayemu_gen_sound (&ay, audio_buf, AUDIO_BUFSIZ);
	}
	else {
		ayemu_gen_sound (&ay, audio_buf+AUDIO_BUFSIZ, AUDIO_BUFSIZ);
	}
}




static void DMA_IRQHandler(void) {

	/* This is called at both half-full and full DMA */
	/* We double buffer */

	/* At half full, we should load next buffer at start */
	/* At full, we should load next buffer to end */

	if ((DMA1->ISR&DMA_ISR_TCIF4)==DMA_ISR_TCIF4) {
		NextBuffer(1);
#if 0
		static int led_count=0,led_on=0;

		/* This should happen at roughly 50Hz */
		led_count++;

		if (led_count==50) {

			if (led_on) {
				led_on=0;
				GPIOB->ODR &= ~(1<<2);
			}
			else {
				led_on=1;
				GPIOB->ODR |= (1<<2);
			}
			led_count=0;
		}
#endif
	}

	if ((DMA1->ISR&DMA_ISR_HTIF4)==DMA_ISR_HTIF4) {
		NextBuffer(0);
	}



	/* ACK interrupt */
	/* Set to 1 to clear */
	DMA1->IFCR |= DMA_IFCR_CGIF4;

}




void SysTick_Handler(void) {

	if (TimeDelay > 0) TimeDelay--;
}

static void TIM7_Init(void) {

	/* enable Timer 7 clock */
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;

	/* Trigger DMA */
	TIM7->DIER |= TIM_DIER_UDE;

	/* Master Mode Update Trigger */
	TIM7->CR2 |= 2<<4;

	/* Prescaler. slow down the input clock by a factor of (1+prescaler) */
	TIM7->PSC=54;		// 16MHz / (1+10) = 1.454MHz

	/* Auto-reload, max */
	TIM7->ARR=32;		// 1.454MHz /(33) = roughly 44kHz, 44077

	/* Enable Timer */
	TIM7->CR1 |= TIM_CR1_CEN;

	/* Set highest priority intterupt */
//	NVIC_SetPriority(TIM7_IRQn, 0);

	/* Set highest priority intterupt */
//	NVIC_EnableIRQ(TIM7_IRQn);

}



/* Example 21-5 in book */

/* DAC Channel 2: DAC_OUT2 = PA5 */
void DAC2_Channel2_Init(void) {


	/* Enable the clock of GPIO port A */
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	/* Set I/O mode of pin A5 as analog */
	GPIOA->MODER |= 3U<<(5*2);



	/* Enable DAC clock */
	RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;

	/* Disable DACs so we can program them */
	DAC->CR &= ~(DAC_CR_EN1 | DAC_CR_EN2);

	/* DAC mode control register */
	/* 000 = conected to external pin with buffer enabled */
	DAC->MCR &= ~DAC_MCR_MODE2;

	/* Enable trigger for DAC channel 2 */
	DAC->CR |= DAC_CR_TEN2;

	/* Select TIM7_TRG0 as the trigger for DAC channel 2 */
	DAC->CR &= ~DAC_CR_TSEL2;
	DAC->CR |= (DAC_CR_TSEL2_1);

	/* Enable DMA */
	DAC->CR |= DAC_CR_DMAEN2;

	/* Enable DAC Channel 2 */
	DAC->CR |= DAC_CR_EN2;

}


static void GPIOB_Clock_Enable(void) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
}

static void GPIOB_Pin_Output_Init(int pin) {

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


static void GPIOE_Clock_Enable(void) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
}

static void GPIOE_Pin_Output_Init(int pin) {

        /* Set mode of pin 2 as output */
        /* 00 = input
           01 = output
           10 = alternate
           11 = analog (default) */

        GPIOE->MODER &= ~(3UL<<(pin*2));
        GPIOE->MODER |= 1UL<<(pin*2);

        /* set output type of pin2 as push-pull */
        /* 0 = push-pull (default)
           1 = open-drain */
        GPIOE->OTYPER &= ~(1<<pin);
        /* Set output speed of pin2 as low */
        /* 00 = Low speed
           01 = Medium speed
           10 = Fast speed
           11 = High speed */
        GPIOE->OSPEEDR &= ~(3UL<<(pin*2));
       /* Set pin 2 as no pull-up, no pull-down */
        /* 00 = no pull-up, no pull-down
           01 = pull-up
           10 = pull-down
           11 = reserved */
        GPIOE->PUPDR &=~(3UL<<(pin*2));
}



static void GPIOA_Clock_Enable(void) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
}

static void GPIOA_Pin_Input_Init(int pin) {

        /* Set mode to input 00 */
        /* 00 = input
           01 = output
           10 = alternate
           11 = analog (default) */

        GPIOA->MODER &= ~(3UL<<(pin*2));

        /* Set output speed of pin2 as low */
        /* 00 = Low speed
           01 = Medium speed
           10 = Fast speed
           11 = High speed */
        //GPIOA->OSPEEDR &= ~(3UL<<(pin*2));

        /* Set pin 2 as pull-down */
	/* 00 = no pull-up, no pull-down
           01 = pull-up
           10 = pull-down
           11 = reserved */
        GPIOA->PUPDR &=~(3UL<<(pin*2));
        GPIOA->PUPDR |= (2UL<<(pin*2));
}

void DMA_Init(void) {

	/* Enable DMA1 clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	/* Disable DMA1 Channel 4 */
	DMA1->CCR4 &=~DMA_CCR_EN;

	/* Peripheral data size = 32 bits */
	DMA1->CCR4 &=~DMA_CCR_PSIZE;
	DMA1->CCR4 |= DMA_CCR_PSIZE_32;

	/* Memory data size = 16 bits */
	DMA1->CCR4 &=~DMA_CCR_MSIZE;
	DMA1->CCR4 |= DMA_CCR_MSIZE_16;

	/* Disable peripheral increment mode */
	DMA1->CCR4 &=~DMA_CCR_PINC;

	/* Enable memory increment mode */
	DMA1->CCR4 |= DMA_CCR_MINC;

	/* Transfer Direction: to perihpheral */
	DMA1->CCR4 |= DMA_CCR_DIR;

	/* Circular Buffer */
	DMA1->CCR4 |= DMA_CCR_CIRC;

	/* Amount of data to transfer */
	DMA1->CNDTR4 = NUM_SAMPLES*2;

	/* Peripheral Address */
	DMA1->CPAR4 = (uint32_t)&(DAC->DHR12L2);

	/* Memory Address */
	DMA1->CMAR4 = (uint32_t)&(audio_buf);

	/* Set up Channel Select */
	DMA1->CSELR &=~DMA_CSELR_C4S;

	/* TIM7_UP/DAC_CH2 */
	DMA1->CSELR |= 5<<12;

	/* Enable Half-done interrupt */
	DMA1->CCR4 |= DMA_CCR_HTIE;
	/* Enable Transder complete  interrupt */
	DMA1->CCR4 |= DMA_CCR_TCIE;

	/* Enable DMA1 Channel 4 */
	DMA1->CCR4 |= DMA_CCR_EN;

	/* Set highest priority interrupt */
	NVIC_SetPriority(DMA1_CH4_IRQn, 0);

	/* Enable Interrupt */
	NVIC_EnableIRQ(DMA1_CH4_IRQn);

	return;
}

int main(void) {


	char buffer[7];
	volatile int d;

	System_Clock_Init();

	/* Enable Joystick Up Button -- GPIO-PA3 */
	GPIOA_Clock_Enable();
	GPIOA_Pin_Input_Init(3);

	/* Enable RED LED -- GPIO-PB2 */
        GPIOB_Clock_Enable();
        GPIOB_Pin_Output_Init(2);

        /* Enable GREEN LED -- GPIO-PE8 */
        GPIOE_Clock_Enable();
        GPIOE_Pin_Output_Init(8);

        /* Enable DAC Channel2 -- GPIO-PA5 */
	DAC2_Channel2_Init();

	/* Set up LCD */
	LCD_Clock_Init();
	LCD_Pin_Init();
	LCD_Configure();

	/* Init first song */
	change_song();

	/* Init ay code */

	ayemu_init(&ay);
	// 44100, 1, 16 -- freq, channels, bits
	ayemu_set_sound_format(&ay, FREQ, CHANS, BITS);

	ayemu_reset(&ay);
	ayemu_set_chip_type(&ay, AYEMU_AY, NULL);
	/* Assume mockingboard/VMW-chiptune freq */
	/* pt3_lib assumes output is 1773400 of zx spectrum */
	ayemu_set_chip_freq(&ay, 1773400);
//	ayemu_set_chip_freq(&ay, 1000000);
	ayemu_set_stereo(&ay, AYEMU_MONO, NULL);


	TIM7_Init();
	asm volatile ( "cpsie i" );

	NextBuffer(0);
	NextBuffer(1);

	DMA_Init();

	while(1) {

		/* output title */
		memcpy(buffer,&pt3_image[which_song].data[0x1e + scrolling],6);
		buffer[6]=0;
		LCD_Display_String(buffer);
		for(d=0;d<400000;d++);
		scrolling+=scrolldir;
		if ((scrolling>title_len-5) || (scrolling==-1)) {
			for(d=0;d<800000;d++);
			scrolldir=-scrolldir;
			scrolling+=scrolldir;
		}

		/* Change song based on joystick */
		if (GPIOA->IDR & (1<<3)) {
			/* disable interrupt to be safe */
			asm volatile ( "cpsid i" );
			which_song++;
			change_song();
			asm volatile ( "cpsie i" );
		}

		/* Blink RED LED (GPIOB2) based on note A */
		if ((pt3.a.new_note) || (pt3.b.new_note)) {
			GPIOB->ODR |= (1<<2);
		}
		else {
			GPIOB->ODR &= ~(1<<2);
		}

		/* Blink GREEN LED (GPIOE8) based on note B */
		if (pt3.c.new_note) {
			GPIOE->ODR |= (1<<8);
		}
		else {
			GPIOE->ODR &= ~(1<<8);
		}

	}


}

/* Note: no need to touch the code beyond this point */

/* Set 80MHz PLL based on 16MHz HSI clock */
void System_Clock_Init(void) {

        /* Enable the HSI clock */
        RCC->CR |= RCC_CR_HSION;

	/* Wait until HSI is ready */
	while ( (RCC->CR & RCC_CR_HSIRDY) == 0 );

	/* Set the FLASH latency to 4 (BLARGH!) */
	/* Need to set *some* wait states if you are running >16MHz */
	/* See 3.3.3 (p100) in manual */
        FLASH->ACR|=FLASH_ACR_LATENCY_4;

	/* Configure PLL */
	RCC->CR &= ~RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY);

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;

/*
	The system Clock is configured as follows :
            System Clock source            = PLL (HSI)
            AHB Prescaler                  = 1
            APB1 Prescaler                 = 1
            APB2 Prescaler                 = 1
            PLL_M                          = 2
            PLL_N                          = 20
            PLL_R                          = 2
	( SYSCLK = PLLCLK = VCO=(16MHz)*N = 320 /M/R = 80 MHz )
            PLL_P                          = 7 (No reason for this...)
            PLL_Q                          = 8
		 ( PLL48M1CLK = VCO /M/Q) = 40 MHz ?! )
            Flash Latency(WS)              = 4
*/

	// SYSCLK = (IN*N)/M

	/* PLL_N=20 --- VCO=IN*PLL_N */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;		// set to HSI*20=320MHz
	RCC->PLLCFGR |= 20<<8;

	/* PLL_M=2 (00:M=1 01:M=2, 10:M=3, etc) */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;		// 320MHz/2 = 160MHz
	RCC->PLLCFGR |= 1<<4;

	/* PLL_R =2 (00=2, 01=4 10=6 11=8) */
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;		// 160MHz/2 = 80MHz

	/* PLL_P=7 (register=0 means 7, 1=17) for SAI1/SAI2 clock */
//	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;

	/* PLL_Q=8 (11) sets 48MHz clock to 320/8=40? */
//	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
//	RCC->PLLCFGR |= 3<<21;

	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) ;

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

	/* Select PLL as system clock source  */
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;  /* 11: PLL used as sys clock */
	while ((RCC->CFGR & RCC_CFGR_SWS) == 0 );

}

void NVIC_SetPriority(int irq, int priority) {

	if (irq<0) {
		// for -1, 0xff -> f -> -4 = b (11)
		SCB->SHP[(((uint8_t)irq)&0xf)-4]=(priority<<4)&0xff;
	}
	else {
		NVIC->IP[irq]=(priority<<4)&0xff;
	}

	return;
}

void NVIC_EnableIRQ(int irq) {

	NVIC->ISER[irq >> 5] = (1UL << (irq & 0x1F));

	return;
}



static void nmi_handler(void) {
	for(;;);
}

static void hardfault_handler(void) {
	for(;;);
}

extern unsigned long _etext,_data,_edata,_bss_start,_bss_end;

	/* Copy DATA and BSS segments into RAM */
void Reset_Handler(void)	{

	unsigned long *src, *dst;

	/* Copy data segment */
	/* Using linker symbols */
	src = &_etext;
	dst = &_data;
	while(dst < &_edata) *(dst++) = *(src++);

	/* Zero out the BSS */
	src = &_bss_start;
	while(src < &_bss_end) *(src++) = 0;

	/* Call main() */
	main();

}

#define STACK_LOCATION (0x20000000+(96*1024))

/* Vector Table */
uint32_t *myvectors[256]
__attribute__ ((section(".isr_vector"))) = {
	(uint32_t *) STACK_LOCATION,	/*   0:00  = stack pointer	*/
	(uint32_t *) Reset_Handler,	/* -15:04  = code entry point	*/
	(uint32_t *) nmi_handler,	/* -14:08  = NMI handler	*/
	(uint32_t *) hardfault_handler,	/* -13:0c = hard fault handler	*/
	(uint32_t *) nmi_handler,	/* -12:10 = MemManage		*/
	(uint32_t *) nmi_handler,	/* -11:14 = BusFault		*/
	(uint32_t *) nmi_handler,	/* -10:18 = UsageFault		*/
	(uint32_t *) nmi_handler,	/*  -9:1c = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -8:20 = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -7:24 = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -6:28 = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -5:2c = SVC Handler		*/
	(uint32_t *) nmi_handler,	/*  -4:30 = Debugmon		*/
	(uint32_t *) nmi_handler,	/*  -3:34 = Reserved		*/
	(uint32_t *) nmi_handler,	/*  -2:38 = PendSV		*/
	(uint32_t *) SysTick_Handler,	/*  -1:3c = SysTick		*/
	(uint32_t *) nmi_handler,	/*   0:40 = WWDG		*/
	(uint32_t *) nmi_handler,	/*   1:44 = PVD_PVM		*/
	(uint32_t *) nmi_handler,	/*   2:48 = RTC_TAMP_STAMP	*/
	(uint32_t *) nmi_handler,	/*   3:4C = RTC_WKUP		*/
	(uint32_t *) nmi_handler,	/*   4:50 = FLASH		*/
	(uint32_t *) nmi_handler,	/*   5:54 = RCC			*/
	(uint32_t *) nmi_handler,	/*   6:58 = EXTI0		*/
	(uint32_t *) nmi_handler,	/*   7:5c = EXTI1		*/
	(uint32_t *) nmi_handler,	/*   8:60 = EXTI2		*/
	(uint32_t *) nmi_handler,	/*   9:64 = EXTI3		*/
	(uint32_t *) nmi_handler,	/*  10:68 = EXTI4		*/
	(uint32_t *) nmi_handler,	/*  11:6C = DMA1_CH1		*/
	(uint32_t *) nmi_handler,	/*  12:70 = DMA1_CH2		*/
	(uint32_t *) nmi_handler,	/*  13:74 = DMA1_CH3		*/
	(uint32_t *) DMA_IRQHandler,	/*  14:78 = DMA1_CH4		*/
	(uint32_t *) nmi_handler,	/*  15:7c = DMA1_CH5		*/
	(uint32_t *) nmi_handler,	/*  16:80 = DMA1_CH6		*/
	(uint32_t *) nmi_handler,	/*  17:84 = DMA1_CH7		*/
	(uint32_t *) nmi_handler,	/*  18:84 = ADC1_2		*/
	(uint32_t *) nmi_handler,	/*  19:88 = CAN1_TX		*/
	(uint32_t *) nmi_handler,	/*  20:90 = CAN1_RX0		*/
	(uint32_t *) nmi_handler,	/*  21:94 = CAN1_RX1		*/
	(uint32_t *) nmi_handler,	/*  22:98 = CAN1_SCE		*/
	(uint32_t *) nmi_handler,	/*  23:9C = EXTI9_5		*/
	(uint32_t *) nmi_handler,	/*  24:A0 = TIM1_BRK/TIM15	*/
	(uint32_t *) nmi_handler,	/*  25:A4 = TIM1_UP/TIM16	*/
	(uint32_t *) nmi_handler,	/*  26:A8 = TIM1_TRG_COM/TIM17	*/
	(uint32_t *) nmi_handler,	/*  27:AC = TIM1_CC		*/
	(uint32_t *) nmi_handler,	/*  28:B0 = TIM2		*/
	(uint32_t *) nmi_handler,	/*  29:B4 = TIM3		*/
	(uint32_t *) nmi_handler,	/*  30:B8 = TIM4		*/
	(uint32_t *) nmi_handler,	/*  31:BC = I2C1_EV		*/
	(uint32_t *) nmi_handler,	/*  32:C0 = I2C1_ER		*/
	(uint32_t *) nmi_handler,	/*  33:C4 = I2C2_EV		*/
	(uint32_t *) nmi_handler,	/*  34:C8 = I2C2_ER		*/
	(uint32_t *) nmi_handler,	/*  35:CC = SPI1		*/
	(uint32_t *) nmi_handler,	/*  36:D0 = SPI2		*/
	(uint32_t *) nmi_handler,	/*  37:D4 = USART1		*/
	(uint32_t *) nmi_handler,	/*  38:D8 = USART2		*/
	(uint32_t *) nmi_handler,	/*  39:DC = USART3		*/
	(uint32_t *) nmi_handler,	/*  40:E0 = EXTI5_10		*/
	(uint32_t *) nmi_handler,	/*  41:E4 = RTC_ALART		*/
	(uint32_t *) nmi_handler,	/*  42:E8 = DFSDM1_FLT3		*/
	(uint32_t *) nmi_handler,	/*  43:EC = TIM8_BRK		*/
	(uint32_t *) nmi_handler,	/*  44:F0 = TIM8_UP		*/
	(uint32_t *) nmi_handler,	/*  45:F4 = TIM8_TRG_COM	*/
	(uint32_t *) nmi_handler,	/*  46:F8 = TIM8_CC		*/
	(uint32_t *) nmi_handler,	/*  47:FC = ADC3		*/
	(uint32_t *) nmi_handler,	/*  48:100 = FMC		*/
	(uint32_t *) nmi_handler,	/*  49:104 = SDMMC1		*/
	(uint32_t *) nmi_handler,	/*  50:108 = TIM5		*/
	(uint32_t *) nmi_handler,	/*  51:10C = SPI3		*/
	(uint32_t *) nmi_handler,	/*  52:110 = UART4		*/
	(uint32_t *) nmi_handler,	/*  53:114 = UART5		*/
	(uint32_t *) nmi_handler,	/*  54:118 = TIM6_DACUNDER	*/
	(uint32_t *) nmi_handler,	/*  55:11C = TIM7		*/
	(uint32_t *) nmi_handler,	/*  56:120 = DMA2_CH1		*/
	(uint32_t *) nmi_handler,	/*  57:124 = DMA2_CH2		*/
	(uint32_t *) nmi_handler,	/*  58:128 = DMA2_CH3		*/
	(uint32_t *) nmi_handler,	/*  59:12C = DMA2_CH4		*/
	(uint32_t *) nmi_handler,	/*  60:130 = DMA2_CH5		*/
	(uint32_t *) nmi_handler,	/*  61:134 = DFSDM1_FLT0	*/
	(uint32_t *) nmi_handler,	/*  62:138 = DFSDM1_FLT1	*/
	(uint32_t *) nmi_handler,	/*  63:13C = DFSDM1_FLT2	*/
	(uint32_t *) nmi_handler,	/*  64:140 = COMP		*/
	(uint32_t *) nmi_handler,	/*  65:144 = LPTIM1		*/
	(uint32_t *) nmi_handler,	/*  66:148 = LPTIM2		*/
	(uint32_t *) nmi_handler,	/*  67:14C = OTG_FS		*/
	(uint32_t *) nmi_handler,	/*  68:150 = DMA2_CH6		*/
	(uint32_t *) nmi_handler,	/*  69:154 = DMA2_CH7		*/
	(uint32_t *) nmi_handler,	/*  70:158 = LPUART1		*/
	(uint32_t *) nmi_handler,	/*  71:15C = QUADSPI		*/
	(uint32_t *) nmi_handler,	/*  72:160 = I2C3_EV		*/
	(uint32_t *) nmi_handler,	/*  73:164 = I2C3_ER		*/
	(uint32_t *) nmi_handler,	/*  74:168 = SAI1		*/
	(uint32_t *) nmi_handler,	/*  75:16C = SAI2		*/
	(uint32_t *) nmi_handler,	/*  76:170 = SWPMI1		*/
	(uint32_t *) nmi_handler,	/*  77:174 = TSC		*/
	(uint32_t *) nmi_handler,	/*  78:178 = LCD		*/
	(uint32_t *) nmi_handler,	/*  79:17C = AES		*/
	(uint32_t *) nmi_handler,	/*  80:180 = RNG		*/
	(uint32_t *) nmi_handler,	/*  81:184 = FPU		*/
};
