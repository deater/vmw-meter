
/* ./src/rp2_common/hardware_i2c/include/hardware/i2c.h */
struct i2c_inst_t {
	struct i2c_hw_t *hw;
	uint32_t restart_on_next;
};

uint32_t i2c_init(struct i2c_inst_t *i2c, uint32_t baudrate);
void i2c_reset(struct i2c_inst_t *i2c);
void i2c_unreset(struct i2c_inst_t *i2c);
