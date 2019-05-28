#define I2C_WRITE_TO_SLAVE	0
#define I2C_READ_FROM_SLAVE	1


void i2c_init(I2C_TypeDef *I2Cx);
void i2c_start(I2C_TypeDef *I2Cx, uint32_t dev_addr,
                uint8_t size, uint8_t direction);

