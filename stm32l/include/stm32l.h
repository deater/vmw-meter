#define __IO volatile

#define PERIPH_BASE	((uint32_t)0x40000000)
#define APB1PERIPH_BASE	PERIPH_BASE
#define AHBPERIPH_BASE	(PERIPH_BASE + 0x20000)

/* LCD */
typedef struct {
__IO uint32_t CR;	/* LCD control register		0x00 */
__IO uint32_t FCR;	/* LCD frame control register	0x04 */
__IO uint32_t SR;	/* LCD status register		0x08 */
__IO uint32_t CLR;	/* LCD clear register		0x0c */
uint32_t RESERVED;	/* Reserved			0x10 */
__IO uint32_t RAM[16];	/* LCD display memory		0x14 */
} LCD_TypeDef;

/* Control Register 0x0 */
#define LCD_CR_MUX_SEG		(0x1<<7)
#define LCD_CR_BIAS_MASK	(0x3<<5)
#define LCD_CR_BIAS_1_4		(0x0<<5)
#define LCD_CR_BIAS_1_2		(0x1<<5)
#define LCD_CR_BIAS_1_3		(0x2<<5)
#define LCD_CR_DUTY_MASK	(0x7<<2)
#define LCD_CR_DUTY_STATIC	(0x0<<2)
#define LCD_CR_DUTY_1_2		(0x1<<2)
#define LCD_CR_DUTY_1_3		(0x2<<2)
#define LCD_CR_DUTY_1_4		(0x3<<2)
#define LCD_CR_DUTY_1_8		(0x4<<2)
#define LCD_CR_VSEL		(0x1<<1)
#define LCD_CR_LCDEN		(0x1<<0)

/* Frame Control 0x04 */
#define LCD_FCR_CC_MASK		(0x7<<10)
#define LCD_FCR_CC_LCD4		(0x4<<10)

/* Status Register 0x8 */
#define LCD_SR_FCRSF	(1<<5)
#define LCD_SR_RDY	(1<<4)
#define LCD_SR_UDR	(1<<2)
#define LCD_SR_ENS	(1<<0)

#define LCD_BASE	(APB1PERIPH_BASE + 0x2400)

/* Real Time Clock */
typedef struct {
__IO uint32_t TR;	/* RTC time register		0x00 */
__IO uint32_t DR;	/* RTC date register		0x04 */
__IO uint32_t CR;	/* RTC control register		0x08 */
__IO uint32_t ISR;	/* RTC init and status register	0x0c */
__IO uint32_t PRER;	/* RTC prescaler register	0x10 */
__IO uint32_t WUTR;	/* RTC wakeup timer register	0x14 */
__IO uint32_t CALIBR;	/* RTC calibration register	0x18 */
__IO uint32_t ALRMAR;	/* RTC alarm A register		0x1c */
__IO uint32_t ALRMBR;	/* RTC alarm B register		0x20 */
__IO uint32_t WPR;	/* RTC write protection reg	0x24 */
__IO uint32_t SSR;	/* RTC sub second register	0x28 */
__IO uint32_t SHIFTR;	/* RTC shift control register	0x2c */
__IO uint32_t TSTR;	/* RTC time stamp time register	0x30 */
__IO uint32_t TSDR;	/* RTC time stamp date register	0x34 */
__IO uint32_t TSSSR;	/* RTC time-stamp sub second	0x38 */
__IO uint32_t CALR;	/* RTC calibration register	0x3c */
__IO uint32_t TAFCR;	/* RTC tamper/alt function	0x40 */
__IO uint32_t ALRMASSR;	/* RTC alarm A sub second reg	0x44 */
__IO uint32_t ALRMBSSR;	/* RTC alarm B sub second reg	0x48 */
uint32_t RESERVED7;	/* Reserved			0x4C */
__IO uint32_t BKP0R;	/* RTC backup register		0x50 */
__IO uint32_t BKP1R;	/* RTC backup register		0x54 */
__IO uint32_t BKP2R;	/* RTC backup register		0x58 */
__IO uint32_t BKP3R;	/* RTC backup register		0x5c */
__IO uint32_t BKP4R;	/* RTC backup register		0x60 */
__IO uint32_t BKP5R;	/* RTC backup register		0x64 */
__IO uint32_t BKP6R;	/* RTC backup register		0x68 */
__IO uint32_t BKP7R;	/* RTC backup register		0x6c */
__IO uint32_t BKP8R;	/* RTC backup register		0x70 */
__IO uint32_t BKP9R;	/* RTC backup register		0x74 */
__IO uint32_t BKP10R;	/* RTC backup register		0x78 */
__IO uint32_t BKP11R;	/* RTC backup register		0x7c */
__IO uint32_t BKP12R;	/* RTC backup register		0x80 */
__IO uint32_t BKP13R;	/* RTC backup register		0x84 */
__IO uint32_t BKP14R;	/* RTC backup register		0x88 */
__IO uint32_t BKP15R;	/* RTC backup register		0x8c */
__IO uint32_t BKP16R;	/* RTC backup register		0x90 */
__IO uint32_t BKP17R;	/* RTC backup register		0x94 */
__IO uint32_t BKP18R;	/* RTC backup register		0x98 */
__IO uint32_t BKP19R;	/* RTC backup register		0x9c */
__IO uint32_t BKP20R;	/* RTC backup register		0xa0 */
__IO uint32_t BKP21R;	/* RTC backup register		0xa4 */
__IO uint32_t BKP22R;	/* RTC backup register		0xa8 */
__IO uint32_t BKP23R;	/* RTC backup register		0xac */
__IO uint32_t BKP24R;	/* RTC backup register		0xb0 */
__IO uint32_t BKP25R;	/* RTC backup register		0xb4 */
__IO uint32_t BKP26R;	/* RTC backup register		0xb8 */
__IO uint32_t BKP27R;	/* RTC backup register		0xbc */
__IO uint32_t BKP28R;	/* RTC backup register		0xc0 */
__IO uint32_t BKP29R;	/* RTC backup register		0xc4 */
__IO uint32_t BKP30R;	/* RTC backup register		0xc8 */
__IO uint32_t BKP31R;	/* RTC backup register		0xcc */
} RTC_TypeDef;

#define RTC_BASE	(APB1PERIPH_BASE + 0x2800)

typedef struct {
  __IO uint32_t CR;   /* PWR control reg		0x00 */
  __IO uint32_t CSR;  /* PWR control status reg		0x04 */
} PWR_TypeDef;

#define PWR_CR_DBP	(1<<8)	/* Disable backup write protection */

#define PWR_BASE	(APB1PERIPH_BASE + 0x7000)

/* Reset and Clock Control */
typedef struct {
__IO uint32_t CR;		/* 0x00 */
__IO uint32_t ICSCR;		/* 0x04 */
__IO uint32_t CFGR;		/* 0x08 */
__IO uint32_t CIR;		/* 0x0c */
__IO uint32_t AHBRSTR;		/* 0x10 */
__IO uint32_t APB2RSTR;		/* 0x14 */
__IO uint32_t APB1RSTR;		/* 0x18 */
__IO uint32_t AHBENR;		/* 0x1c */
__IO uint32_t APB2ENR;		/* 0x20 */
__IO uint32_t APB1ENR;		/* 0x24 */
__IO uint32_t AHBLPENR;		/* 0x28 */
__IO uint32_t APB2LPENR;	/* 0x2c */
__IO uint32_t APB1LPENR;	/* 0x30 */
__IO uint32_t CSR;		/* 0x34 */
} RCC_TypeDef;

/* 0x0 Control Register */
#define RCC_CR_MSIRDY		(0x1<<9)	/* MSI Clock Ready */

/* 0x1c */
#define AHBENR_GPIOAEN	1
#define AHBENR_GPIOBEN	2
#define AHBENR_GPIOCEN	4
#define AHBENR_GPIODEN	8
#define AHBENR_GPIOEEN	16
#define AHBENR_GPIOHEN	32
#define AHBENR_GPIOFEN	64
#define AHBENR_GPIOGEN	128

/* 0x20 APB2 Clock Enable Register */
#define RCC_APB2ENR_SYSCFGEN	(1<<0)

/* 0x24 APB1 Clock Enable Register */
#define RCC_APB1ENR_DACEN	(1<<29)	/* DAC enable */
#define RCC_APB1ENR_PWREN	(1<<28)	/* power interface clock enable */
#define RCC_APB1ENR_USBEN	(1<<23) /* enable USB clock */
#define RCC_APB1ENR_I2C2EN	(1<<22) /* i2c-2 clock */
#define RCC_APB1ENR_I2C1EN	(1<<21) /* i2c-1 clock */
#define RCC_APB1ENR_UART5EN	(1<<20) /* uart5 clock enable */
#define RCC_APB1ENR_UART4EN	(1<<19) /* uart4 clock enable */
#define RCC_APB1ENR_UART3EN	(1<<18) /* uart3 clock enable */
#define RCC_APB1ENR_UART2EN	(1<<17) /* uart2 clock enable */
#define RCC_APB1ENR_SPI3EN	(1<<15) /* spi-3 clock enable */
#define RCC_APB1ENR_SPI2EN	(1<<14) /* spi-2 clock enable */
#define RCC_APB1ENR_WWDGEN	(1<<11) /* window watchdog clock enable */
#define RCC_APB1ENR_LCDEN	(1<<9)	/* lcd clock enable */
#define RCC_APB1ENR_TIM7EN	(1<<5)	/* timer 7 clock enable */
#define RCC_APB1ENR_TIM6EN	(1<<4)	/* timer 6 clock enable */
#define RCC_APB1ENR_TIM5EN	(1<<3)	/* timer 5 clock enable */
#define RCC_APB1ENR_TIM4EN	(1<<2)	/* timer 4 clock enable */
#define RCC_APB1ENR_TIM3EN	(1<<1)	/* timer 3 clock enable */
#define RCC_APB1ENR_TIM2EN	(1<<0)	/* timer 2 clock enable */

/* 0x34 Control-Status Register */
#define RCC_CSR_RTCEN		(1<<22)		/* RTC enable */
#define RCC_CSR_RTCSEL_LSE	(0x1<<16)
#define RCC_CSR_RTCSEL_LSI	(0x2<<16)
#define RCC_CSR_RTCSEL_HSE	(0x3<<16)
#define RCC_CSR_LSIRDY		(0x1<<1)	/* LSI ready */
#define RCC_CSR_LSION		(0x1<<0)	/* internal low-speed oscilator */



#define RCC_BASE	(AHBPERIPH_BASE + 0x3800)


/* General Purpose IO */
typedef struct {
__IO uint32_t MODER;	/* 0x00 = Mode Register */
__IO uint16_t OTYPER;   /* 0x04 = Output Type Register */
uint16_t RESERVED0;     /* 0x06 */
__IO uint32_t OSPEEDR;	/* 0x08 = Output Speed Register */
__IO uint32_t PUPDR;	/* 0x0C = Pull Up/Pull Down Register */
__IO uint16_t IDR;	/* 0x10 = Input Data Register */
uint16_t RESERVED1;     /* 0x12 */
__IO uint16_t ODR;	/* 0x14 = Output Data Register */
uint16_t RESERVED2;     /* 0x16 */
__IO uint16_t BSRRL;	/* 0x18 = Bit Set/Reset Register (LOW) */
__IO uint16_t BSRRH;	/* 0x1A = Bit Set/Reset Register (HIGH) */
__IO uint32_t LCKR;	/* 0x1C = Configuration Lock Register */
__IO uint32_t AFR[2];	/* 0x20 = Alternate Function (Low/High) Registers */
} GPIO_TypeDef;

#define GPIOA_BASE	(AHBPERIPH_BASE + 0x0000)
#define GPIOB_BASE	(AHBPERIPH_BASE + 0x0400)
#define GPIOC_BASE	(AHBPERIPH_BASE + 0x0800)

#define MODER_INPUT	0x00
#define MODER_OUTPUT	0x01
#define MODER_ALT	0x02
#define MODER_ANALOG	0x03

#define MODER_SET(_s,_v)	(((_v&0x3) << (_s*2))

