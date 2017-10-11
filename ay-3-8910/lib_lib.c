#include <stdio.h>
#include <unistd.h>

#include "stats.h"
#include "display.h"

int display_type=DISPLAY_I2C;

void quiet_and_exit(int sig) {

	if (display_type&DISPLAY_I2C) display_shutdown(DISPLAY_I2C);

	printf("Quieting and exiting\n");
	_exit(0);
}
