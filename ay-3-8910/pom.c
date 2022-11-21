#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

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

	lib_pom(0);

	/* Quiet down the chips */
	display_shutdown(display_type);

	return 0;
}
