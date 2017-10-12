/* A simple clock appication.  */
/* Prints the time on the alphanum segments */
/* Puts UNIX time along the bargraphs segments */
/* Day of the week on the colored LEDs */
/* Occasionally the month/day/year is also displayed */

// Day/date on 8x16
// Unix time on green bargraphs
// Time/ AM/PM on Alphanum

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>

#include "ay-3-8910.h"
#include "max98306.h"

#include "stats.h"
#include "display.h"

#include "lib_lib.h"

static int alarm_hour=10;
static int alarm_minute=00;

#if 0

static void quiet_and_exit(int sig) {

	if (play_music) {
		quiet_ay_3_8910(16);
		close_ay_3_8910();
		max98306_free();
	}

	display_shutdown(display_type);

	printf("Quieting and exiting\n");
	_exit(0);

}

#endif

int main(int argc, char **argv) {


	int result;

	/* Setup control-C handler to quiet the music   */
	/* otherwise if you force quit it keeps playing */
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	result=display_init(DISPLAY_I2C);
	if (result<0) {
		display_type=DISPLAY_TEXT;
	}

	lib_clock(alarm_hour,alarm_minute);

	display_shutdown(display_type);

	return 0;
}
