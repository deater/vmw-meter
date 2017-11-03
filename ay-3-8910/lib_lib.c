#include <stdio.h>
#include <unistd.h>

#include "ymlib/stats.h"
#include "display.h"
#include "audio.h"

int display_type=DISPLAY_I2C;

void quiet_and_exit(int sig) {

	display_shutdown(display_type);
	audio_shutdown();

//	if (play_music) {
//		quiet_ay_3_8910(shift_size);
//		close_ay_3_8910();
//		max98306_free();
//	}
	printf("\033[0m\n");
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
