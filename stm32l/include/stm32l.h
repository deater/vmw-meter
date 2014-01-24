#define __IO volatile

#define PERIPH_BASE	((uint32_t)0x40000000)
#define AHBPERIPH_BASE	(PERIPH_BASE + 0x20000)

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

#define AHBENR_GPIOAEN	1
#define AHBENR_GPIOBEN	2
#define AHBENR_GPIOCEN	4
#define AHBENR_GPIODEN	8
#define AHBENR_GPIOEEN	16
#define AHBENR_GPIOHEN	32
#define AHBENR_GPIOFEN	64
#define AHBENR_GPIOGEN	128

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

#define MODER_INPUT	0x00
#define MODER_OUTPUT	0x01
#define MODER_ALT	0x02
#define MODER_ANALOG	0x03

#define MODER_SET(_s,_v)	(((_v&0x3) << (_s*2))

