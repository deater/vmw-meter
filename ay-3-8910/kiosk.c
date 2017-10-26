/* vmw-chiptune-player kiosk mode */
/* Designed for when using the player in standalone mode */
/* Everything should be possible to control with the buttons alone */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>

#include "ay-3-8910.h"
#include "max98306.h"

#include "ymlib/stats.h"
#include "display.h"

#include "lib_lib.h"

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

	lib_clock(10,00);

	display_shutdown(display_type);

	return 0;
}
