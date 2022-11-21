/* see ./host/hardware_gpio/include/hardware/gpio.h */

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


#define GPIO_OUT 1
#define GPIO_IN	0

#define NUM_BANK0_GPIOS	30


// --- I2C ---
//#ifndef PICO_DEFAULT_I2C
//#define PICO_DEFAULT_I2C 0
//#endif
//#ifndef PICO_DEFAULT_I2C_SDA_PIN
#define PICO_DEFAULT_I2C_SDA_PIN 4
//#endif
//#ifndef PICO_DEFAULT_I2C_SCL_PIN
#define PICO_DEFAULT_I2C_SCL_PIN 5
//#endif


void gpio_init(uint32_t which);
void gpio_set_dir(uint32_t which, uint32_t dir);
void gpio_put(uint32_t which, uint32_t value);
uint32_t gpio_get(uint32_t which);
void gpio_set_function(uint32_t which, uint32_t function); //enum gpio_function fn);
void gpio_pull_up(uint32_t which);
void gpio_pull_down(uint32_t which);

// from src/rp2040/hardware_regs/include/hardware/regs/addressmap.h
#define PADS_BANK0_BASE	0x4001c000
#define IO_BANK0_BASE	0x40014000
#define SIO_BASE	0xd0000000

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

