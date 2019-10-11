void uart_init(void);
void uart_putc(unsigned char byte);
unsigned char uart_getc(void);
int32_t uart_getc_noblock(void);
uint32_t uart_write(const unsigned char *buffer, size_t size);

