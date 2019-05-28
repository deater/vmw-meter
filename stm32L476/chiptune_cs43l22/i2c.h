#define I2C_WRITE_TO_SLAVE	0
#define I2C_READ_FROM_SLAVE	1


void i2c_init(I2C_TypeDef *I2Cx);
int8_t  i2c_send_data(I2C_TypeDef *I2Cx, uint8_t slave_addr,
                uint8_t *data, uint8_t size);
int8_t i2c_receive_data(I2C_TypeDef *I2Cx, uint8_t slave_addr,
                uint8_t *data, uint8_t size);
