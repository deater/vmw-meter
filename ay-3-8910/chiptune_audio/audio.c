#include <stdio.h>

#include "ay-3-8910.h"
#include "max98306.h"

int play_music=0;
int shift_size=16;

int audio_shutdown(void) {

	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	return 0;
}

int audio_setup(void) {

	int result;

	play_music=1;

	result=initialize_ay_3_8910(0);
	if (result<0) {
		printf("Error initializing bcm2835!!\n");
		play_music=0;
	}

	result=max98306_init();
	if (result<0) {
		printf("Error initializing max98306 amp!!\n");
		play_music=0;
	}
	else {
		result=max98306_enable();
	}

	return play_music;
}
