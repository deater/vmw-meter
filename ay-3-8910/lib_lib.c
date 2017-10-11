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

int sleep_unless_keypress(int seconds) {

	int t,i,ch=0;

	t=seconds*10;

	for(i=0;i<t;i++) {
		ch=display_keypad_read(display_type);
                if (ch) break;
		usleep(100000);
	}
	return ch;
}
