#include <stdint.h>
#include "include/gpio.h"

#include "include/pads_bank0.h"
#include "include/io_bank0.h"

// from src/host/hardware_gpio/include/hardware/gpio.h:

enum gpio_function {
    GPIO_FUNC_XIP = 0,
    GPIO_FUNC_SPI = 1,
    GPIO_FUNC_UART = 2,
    GPIO_FUNC_I2C = 3,
    GPIO_FUNC_PWM = 4,
    GPIO_FUNC_SIO = 5,
    GPIO_FUNC_PIO0 = 6,
    GPIO_FUNC_PIO1 = 7,
    GPIO_FUNC_GPCK = 8,
    GPIO_FUNC_USB = 9,
    GPIO_FUNC_NULL = 0xf,
};


// from src/rp2040/hardware_regs/include/hardware/regs/addressmap.h
#define PADS_BANK0_BASE	0x4001c000
#define IO_BANK0_BASE	0x40014000
#define SIO_BASE	0xd0000000

// from src/rp2_common/hardware_base/include/hardware/address_mapped.h
typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;
typedef volatile uint32_t io_wo_32;

typedef struct {
    io_rw_32 voltage_select;
    io_rw_32 io[30];
} padsbank0_hw_t;

#define padsbank0_hw ((padsbank0_hw_t *)PADS_BANK0_BASE)


// from src/rp2040/hardware_structs/include/hardware/structs/iobank0.h

typedef struct {
    io_rw_32 inte[4];
    io_rw_32 intf[4];
    io_rw_32 ints[4];
} io_irq_ctrl_hw_t;

/// \tag::iobank0_hw[]
typedef struct {
    struct {
        io_rw_32 status;
        io_rw_32 ctrl;
    } io[30];
    io_rw_32 intr[4];
    io_irq_ctrl_hw_t proc0_irq_ctrl;
    io_irq_ctrl_hw_t proc1_irq_ctrl;
    io_irq_ctrl_hw_t dormant_wake_irq_ctrl;
} iobank0_hw_t;

#define iobank0_hw ((iobank0_hw_t *const)IO_BANK0_BASE)


/* from ./src/rp2040/hardware_structs/include/hardware/structs/interp.h */
typedef struct {
    io_rw_32 accum[2];
    io_rw_32 base[3];
    io_ro_32 pop[3];
    io_ro_32 peek[3];
    io_rw_32 ctrl[2];
    io_rw_32 add_raw[2];
    io_wo_32 base01;
} interp_hw_t;


// from ./src/rp2040/hardware_structs/include/hardware/structs/sio.h

typedef struct {
    io_ro_32 cpuid;
    io_ro_32 gpio_in;
    io_ro_32 gpio_hi_in;
    uint32_t _pad;

    io_wo_32 gpio_out;
    io_wo_32 gpio_set;
    io_wo_32 gpio_clr;
    io_wo_32 gpio_togl;

    io_wo_32 gpio_oe;
    io_wo_32 gpio_oe_set;
    io_wo_32 gpio_oe_clr;
    io_wo_32 gpio_oe_togl;

    io_wo_32 gpio_hi_out;
    io_wo_32 gpio_hi_set;
    io_wo_32 gpio_hi_clr;
    io_wo_32 gpio_hi_togl;

    io_wo_32 gpio_hi_oe;
    io_wo_32 gpio_hi_oe_set;
    io_wo_32 gpio_hi_oe_clr;
    io_wo_32 gpio_hi_oe_togl;

    io_rw_32 fifo_st;
    io_wo_32 fifo_wr;
    io_ro_32 fifo_rd;
    io_ro_32 spinlock_st;

    io_rw_32 div_udividend;
    io_rw_32 div_udivisor;
    io_rw_32 div_sdividend;
    io_rw_32 div_sdivisor;

    io_rw_32 div_quotient;
    io_rw_32 div_remainder;
    io_rw_32 div_csr;

    uint32_t _pad2;

    interp_hw_t interp[2];
} sio_hw_t;

#define sio_hw ((sio_hw_t *)SIO_BASE)


// from src/rp2040/hardware_regs/include/hardware/regs/addressmap.h
#define REG_ALIAS_XOR_BITS (0x1u << 12u)


// from src/rp2_common/hardware_base/include/hardware/address_mapped.h

#define hw_xor_alias_untyped(addr) ((void *)(REG_ALIAS_XOR_BITS | (uintptr_t)(addr)))

/* Atomically flip the specified bits in a HW register */
static void hw_xor_bits(io_rw_32 *addr, uint32_t mask) {
    *(io_rw_32 *) hw_xor_alias_untyped((volatile void *) addr) = mask;
}


static void hw_write_masked(io_rw_32 *addr,
			uint32_t values, uint32_t write_mask) {
    hw_xor_bits(addr, (*addr ^ values) & write_mask);
}



// from ./src/rp2_common/hardware_gpio/gpio.c


void gpio_set_function(uint32_t which, uint32_t function) {

//	if (which>=NUM_BANK0_GPIOS) {
//		printk("Error GPIO");
//	}

//	if (function << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB)&~IO_BANK0_GPIO_CTRL_FUNCSEL_BITS) {
//		printk("Error GPIO");
//	}


	/* Set input enable on, Output Disable Off */

	hw_write_masked(&padsbank0_hw->io[which],
		PADS_BANK0_GPIO0_IE_BITS,
		PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_OD_BITS );

	/* Zero all fields apart from fsel */
	/* We want this IO to do what the peripheral tells it. */
	/* This doesn't affect e.g. pullup/pulldown, */
	/* as these are in pad controls. */
	iobank0_hw->io[which].ctrl = function << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB;
}

// from

void gpio_init(uint32_t which) {
	sio_hw->gpio_oe_clr = 1ul << which;
	sio_hw->gpio_clr = 1ul << which;
	gpio_set_function(which, GPIO_FUNC_SIO);
}

// from ./src/rp2_common/hardware_gpio/include/hardware/gpio.h


static inline void gpio_set_dir_out_masked(uint32_t mask) {
	sio_hw->gpio_oe_set = mask;
}

static inline void gpio_set_dir_in_masked(uint32_t mask) {
	sio_hw->gpio_oe_clr = mask;
}

void gpio_set_dir(uint32_t which, uint32_t out) {

	uint32_t mask = 1ul << which;

	if (out) {
		gpio_set_dir_out_masked(mask);
	}
	else {
		gpio_set_dir_in_masked(mask);
	}
}

static void gpio_set_mask(uint32_t mask) {
	sio_hw->gpio_set = mask;
}


static void gpio_clr_mask(uint32_t mask) {
    sio_hw->gpio_clr = mask;
}

void gpio_put(uint32_t which, uint32_t value) {

	uint32_t mask = 1ul << which;

	if (value) {
		gpio_set_mask(mask);
	}
	else {
		gpio_clr_mask(mask);
	}
}


uint32_t gpio_get(uint32_t which) {
	return !!((1ul << which) & sio_hw->gpio_in);
}
