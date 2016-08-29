int spi_open(char *dev_name, int mode, int freq, int bits);
int spi_writeread(int spi_fd, unsigned char *data, int length,
		int freq, int bitsperword );
int spi_close(int spi_fd);
