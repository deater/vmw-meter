#include <stdint.h>
#include "include/mmio.h"
#include "include/resets.h"
#include "reset.h"
#include "clocks.h"
#include "include/i2c.h"
#include "i2c.h"

uint32_t i2c_set_baudrate(struct i2c_inst_t *i2c, uint32_t baudrate) {

//	invalid_params_if(I2C, baudrate == 0);

	// I2C is synchronous design that runs from clk_sys
	uint32_t freq_in = clock_get_hz(clk_sys);

	// TODO (pi) there are some subtleties to I2C timing which we are completely ignoring here
	uint32_t period = (freq_in + baudrate / 2) / baudrate;
	uint32_t hcnt = period * 3 / 5; // oof this one hurts
	uint32_t lcnt = period - hcnt;

	// Check for out-of-range divisors:
//	invalid_params_if(I2C, hcnt > I2C_IC_FS_SCL_HCNT_IC_FS_SCL_HCNT_BITS);
//	invalid_params_if(I2C, lcnt > I2C_IC_FS_SCL_LCNT_IC_FS_SCL_LCNT_BITS);
//	invalid_params_if(I2C, hcnt < 8);
//	invalid_params_if(I2C, lcnt < 8);

	i2c->hw->enable = 0;

	// Always use "fast" mode (<= 400 kHz, works fine for standard mode too)
	hw_write_masked(&i2c->hw->con,
                   I2C_IC_CON_SPEED_VALUE_FAST << I2C_IC_CON_SPEED_LSB,
                   I2C_IC_CON_SPEED_BITS
	);

	i2c->hw->fs_scl_hcnt = hcnt;
	i2c->hw->fs_scl_lcnt = lcnt;
	i2c->hw->fs_spklen = lcnt < 16 ? 1 : lcnt / 16;

	i2c->hw->enable = 1;
	return freq_in / period;
}


uint32_t i2c_init(struct i2c_inst_t *i2c, uint32_t baudrate) {

    i2c_reset(i2c);
    i2c_unreset(i2c);
    i2c->restart_on_next = 0;

    i2c->hw->enable = 0;

    // Configure as a fast-mode master with RepStart support, 7-bit addresses
    i2c->hw->con =
            I2C_IC_CON_SPEED_VALUE_FAST << I2C_IC_CON_SPEED_LSB |
            I2C_IC_CON_MASTER_MODE_BITS |
            I2C_IC_CON_IC_SLAVE_DISABLE_BITS |
            I2C_IC_CON_IC_RESTART_EN_BITS;

    // Set FIFO watermarks to 1 to make things simpler. This is encoded by a register value of 0.
    i2c->hw->tx_tl = 0;
    i2c->hw->rx_tl = 0;

    // Always enable the DREQ signalling -- harmless if DMA isn't listening
    i2c->hw->dma_cr = I2C_IC_DMA_CR_TDMAE_BITS | I2C_IC_DMA_CR_RDMAE_BITS;

// Re-sets i2c->hw->enable upon returning:
    return i2c_set_baudrate(i2c, baudrate);
}

void i2c_reset(struct i2c_inst_t *i2c) {
//	invalid_params_if(I2C, i2c != i2c0 && i2c != i2c1);
//	reset_block(i2c == i2c0 ? RESETS_RESET_I2C0_BITS : RESETS_RESET_I2C1_BITS);
	reset_block(RESETS_RESET_I2C0_BITS);
}

void i2c_unreset(struct i2c_inst_t *i2c) {
//	invalid_params_if(I2C, i2c != i2c0 && i2c != i2c1);
//	unreset_block_wait(i2c == i2c0 ? RESETS_RESET_I2C0_BITS : RESETS_RESET_I2C1_BITS);
	unreset_block_wait(RESETS_RESET_I2C0_BITS);
}
