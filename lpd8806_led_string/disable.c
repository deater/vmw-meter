/* turn off the string */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lpd8806.h"
#include "effects.h"

int disable(int spi_fd) {

	int i;
	unsigned char data[128];

	/* Set data to all black */
	for(i=0;i<128;i++) data[i]=128;

	lpd8806_write(spi_fd,data);

	return 0;
}
