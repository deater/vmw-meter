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

#include "display.h"

#include "version.h"

static void quiet_and_exit(int sig) {

	display_shutdown(DISPLAY_I2C);

	printf("Quieting and exiting\n");
	_exit(0);

}

void print_help(int just_version, char *exec_name) {

	printf("\ntest_font version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	exit(0);
}

static int text_input(void) {

	char text[13];
	int i,result;
	int ch,pointer=0;

	for(i=0;i<13;i++) text[i]=0;

	while(1) {

		/* Read from Keyboard */
		result=read(0,&ch,1);
		if (result<0) {
			usleep(50000);
			continue;
		}
		if (ch==27) break;

		if (ch=='\n') {
			for(i=0;i<13;i++) text[i]=0;
			pointer=0;
		}
		else if (ch==127) {
			if (pointer>0) pointer--;
			text[pointer]=' ';
		}
		else {
			if (pointer>11) {
				for(i=1;i<12;i++) text[i-1]=text[i];
				text[11]=ch;
			}
			else {
				text[pointer]=ch;
				pointer++;
			}
		}


		display_string(DISPLAY_I2C,text);

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
			default:
				print_help(0,argv[0]);
				break;
		}
	}


	display_init(DISPLAY_I2C);


	/* Play the song */
	text_input();

	/* Quiet down the chips */
	display_shutdown(DISPLAY_I2C);

	return 0;
}
