/* Test the disable amplifier button on the vmw-chiptune */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <bcm2835.h>
#include "max98306.h"

#include "version.h"

#define POLL_GPIO	1


static int done=0;

static void quiet_and_exit(int sig) {

	done=1;
}

int main(int argc, char **argv) {

	int result;

	/* Setup control-C handler */
	signal(SIGINT, quiet_and_exit);

	printf("Testing the amp disable switch on GPIO...\n");

	result=bcm2835_init();
	/* 1 means success, 0 means failure.  Who does that? */
	if (result==0) {
		fprintf(stderr,"Error init libBCM2835!\n");
		return -1;
	}

	printf("Initialized libBCM2835 version %d\n",
		bcm2835_version());



	max98306_init();

#if POLL_GPIO==1

	/* Polling */
	while(!done) {
		printf("Switch status: %d\n",max98306_check_headphone());
		sleep(1);
	}

#else

	/* From within Linux can set an interrupt to happen */
	/* on GPIO status change */
	/* In libbcm2835 you can't, though you can set level-change events */

#endif

	max98306_free();

	printf("Exiting\n");

	return 0;
}
