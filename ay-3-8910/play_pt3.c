/* Parse/Play a PT3 pro-tracker 3 zx file */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <math.h>
#include <sys/resource.h>

#include <bcm2835.h>

//#include "stats.h"
#include "ay-3-8910.h"
//#include "display.h"
//#include "ym_lib.h"
#include "max98306.h"
//#include "visualizations.h"

#include "version.h"

//#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int play_music=1;
static int dump_info=0;
static int shift_size=16;
static int music_repeat=0;

static void quiet_and_exit(int sig) {

	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	printf("Quieting and exiting\n");
	_exit(0);

}

static void print_help(int just_version, char *exec_name) {

	printf("\nplay_pt3 version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This plays PT3 music files out of the VMW AY-3-8910 device\n");
	printf("on a suitably configured Raspberry Pi machine.\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-i] [-t] [-m] [-s] [-n] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");

	exit(0);
}

extern int play_pt3(char *filename);


int main(int argc, char **argv) {

	char filename[BUFSIZ]="ea.pt3";
	int result;
	int c,first_song=0,next_song=0;

	/* Setup control-C handler to quiet the music	*/
	/* otherwise if you force quit it keeps playing	*/
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dDmhvmsnitr"))!=-1) {
		switch (c) {
			case 'd':
				/* Debug messages */
				printf("Debug enabled\n");
				dump_info=1;
				break;
			case 'h':
				/* help */
				print_help(0,argv[0]);
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
				break;
			case 'r':
				/* repeat */
				music_repeat=1;
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	first_song=optind;

	/* Initialize the Chip interface */
	if (play_music) {
		result=initialize_ay_3_8910(0);
		if (result<0) {
			printf("Error initializing bcm2835!\n");
			printf("Maybe try running as root?\n\n");
			exit(0);
		}
		result=max98306_init();
		if (result<0) {
			printf("Error initializing max98306 amp\n");
			exit(0);
		}
	}


	while(1) {

		if (argv[first_song+next_song]!=NULL) {
			strcpy(filename,argv[first_song+next_song]);
			next_song++;
		}
		else {
			break;
		}

		/* Play the song */
		result=play_pt3(filename);

		/* Quiet down the chips */
		if (play_music) {
			quiet_ay_3_8910(shift_size);
		}

		usleep(500000);

	}

	/* Get ready to shut down */
	/* Quiet down the chips */
	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
		max98306_free();
	}

	return 0;
}
