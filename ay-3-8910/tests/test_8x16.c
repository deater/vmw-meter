/* Test the 14-seg font */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "stats.h"
#include "display.h"

#include "version.h"
static int display_type=DISPLAY_I2C;

static short led_art[10][8]={{
	0x001c,		/*          ***   */
	0x003c,		/*         ****   */
	0x0078,		/*        ****    */
	0x18f0,		/*  **   ****     */
	0x3de0,		/* **** ****      */
	0x1fc0,		/*  *******       */
	0x0f80,		/*   *****        */
	0x0700,		/*    ***         */
}};


static void quiet_and_exit(int sig) {

	display_shutdown(display_type);

	printf("Quieting and exiting\n");
	_exit(0);

}

void print_help(int just_version, char *exec_name) {

	printf("\ntest_8x16 version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	exit(0);
}

static int text_input(void) {

	int result;
	int ch;

	while(1) {

		/* Read from Keyboard */
		result=read(0,&ch,1);
		if (result<0) {
			usleep(50000);
			continue;
		}
		if (ch==27) break;
		if (ch=='q') break;
	}

	return 0;
}


int main(int argc, char **argv) {

	int c;

	/* Setup control-C handler to quiet the music	*/
	/* otherwise if you force quit it keeps playing	*/
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Set to have highest possible priority */
	setpriority(PRIO_PROCESS, 0, -20);

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dmhvmsnitr"))!=-1) {
		switch (c) {
			case 'h':
				/* help */
				print_help(0,argv[0]);
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
				break;
			case 't':
				display_type=DISPLAY_TEXT;
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}


	display_init(display_type);

//	char buffer[17];
//	int x;
//	for(x=0;x<17;x++) buffer[x]=0;
//	buffer[3]=0xff;
//	display_8x16_raw(display_type,buffer);

	display_8x16_led_art(display_type,
		led_art,0);

	text_input();

	/* Quiet down the chips */
	display_shutdown(display_type);

	return 0;
}
