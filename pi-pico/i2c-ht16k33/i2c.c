#include <stdint.h>

#include "include/mmio.h"
#include "include/resets.h"
#include "reset.h"
#include "clocks.h"
#include "include/i2c.h"
#include "i2c.h"




#define PICO_ERROR_TIMEOUT	-1
#define PICO_ERROR_GENERIC	-2

static int i2c_write_blocking_internal(struct i2c_inst_t *i2c,
	uint8_t addr, const uint8_t *src, uint32_t len, uint32_t nostop) {

//	invalid_params_if(I2C, addr >= 0x80); // 7-bit addresses
//	invalid_params_if(I2C, i2c_reserved_addr(addr));
	// Synopsys hw accepts start/stop flags alongside data items in the same
	// FIFO word, so no 0 byte transfers.
//	invalid_params_if(I2C, len == 0);
//	invalid_params_if(I2C, ((int)len) < 0);

	i2c->hw->enable = 0;
	i2c->hw->tar = addr;
	i2c->hw->enable = 1;

	uint32_t abort = 0;
	uint32_t timeout = 0;

	uint32_t abort_reason;
	int byte_ctr;
	int ilen = (int)len;

    for (byte_ctr = 0; byte_ctr < ilen; ++byte_ctr) {
        uint32_t first = byte_ctr == 0;
        uint32_t last = byte_ctr == ilen - 1;

        i2c->hw->data_cmd =
                (first && i2c->restart_on_next) << I2C_IC_DATA_CMD_RESTART_LSB |
                (last && !nostop) << I2C_IC_DATA_CMD_STOP_LSB |
                *src++;

        do {
            // Note clearing the abort flag also clears the reason, and this
            // instance of flag is clear-on-read!
            abort_reason = i2c->hw->tx_abrt_source;
            abort = (uint32_t) i2c->hw->clr_tx_abrt;

//            if (timeout_check) {
  //              timeout = timeout_check(ts);
    //            abort |= timeout;
      //      }
            tight_loop_contents();
        } while (!abort && !(i2c->hw->status & I2C_IC_STATUS_TFE_BITS));
// Note the hardware issues a STOP automatically on an abort condition.
        // Note also the hardware clears RX FIFO as well as TX on abort,
        // because we set hwparam IC_AVOID_RX_FIFO_FLUSH_ON_TX_ABRT to 0.
        if (abort)
            break;
    }

    int rval;

    // A lot of things could have just happened due to the ingenious and
    // creative design of I2C. Try to figure things out.
    if (abort) {
        if (timeout)
            rval = PICO_ERROR_TIMEOUT;
        else if (!abort_reason || abort_reason & I2C_IC_TX_ABRT_SOURCE_ABRT_7B_ADDR_NOACK_BITS) {
            // No reported errors - seems to happen if there is nothing connected to the bus.
            // Address byte not acknowledged
            rval = PICO_ERROR_GENERIC;
        } else if (abort_reason & I2C_IC_TX_ABRT_SOURCE_ABRT_TXDATA_NOACK_BITS) {
// Address acknowledged, some data not acknowledged
            rval = byte_ctr;
        } else {
            //panic("Unknown abort from I2C instance @%08x: %08x\n", (uint32_t) i2c->hw, abort_reason);
            rval = PICO_ERROR_GENERIC;
        }
    } else {
        rval = byte_ctr;
    }

    // nostop means we are now at the end of a *message* but not the end of a *transfer*
    i2c->restart_on_next = nostop;
    return rval;
}


uint32_t i2c_write_blocking(struct i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, uint32_t len, uint32_t nostop) {
    return i2c_write_blocking_internal(i2c, addr, src, len, nostop);
}


/* Quick helper function for single byte transfers */
void i2c_write_byte(struct i2c_inst_t *i2c, uint32_t addr, uint8_t val) {
	i2c_write_blocking(i2c, addr, &val, 1, 0);
}


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
