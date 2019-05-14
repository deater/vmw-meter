/* Dump a ym5 file for Apple II */

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

//#include <bcm2835.h>

//#include "ay-3-8910.h"
//#include "display.h"

#include "stats.h"
#include "ym_lib.h"
//#include "max98306.h"

#define VERSION "0.6"

static void print_help(int just_version, char *exec_name) {

	printf("\ndump_ym5 version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This dumps the contents of a ym5 music file.\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-r] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");
	printf("\t-r: don't pretty print values\n");

	exit(0);
}

static int dump_song(char *filename, int debug) {

	int result;
	int frame_num=0;

	unsigned char frame[16],frame2[16];

	struct ym_song_t ym_song;

	printf("\nDumping song %s\n",filename);

	result=load_ym_song(filename,&ym_song);
	if (result<0) {
		return -1;
	}

	/**********************/
	/* Print song summary */
	/**********************/

	printf("%s\n",ym_song.song_name);
	printf("%s\n\n",ym_song.author);


	/******************/
	/* Play the song! */
	/******************/

	frame_num=0;
	while(1) {

		ym_return_frame_raw(&ym_song,frame_num,frame,frame2);

		printf("%04X: A:%02X%02X B:%02X%02X C:%02X%02X N:%02X M:%02X\n",
			frame_num,
			frame[1],frame[0],
			frame[3],frame[2],
			frame[5],frame[4],
			frame[6],
			frame[7]);
		printf("      V:%02X   V:%02X   V:%02X   E:%02X%02X,%02X\n",
			frame[8],frame[9],frame[10],
			frame[12],frame[11],frame[13]);

		frame_num++;

		/* Check to see if done with file */
		if (frame_num>=ym_song.num_frames) {
			break;
		}
	}

	/* Free the ym file */
	free(ym_song.file_data);

	return 0;
}


int main(int argc, char **argv) {

	char filename[BUFSIZ]="intro2.ym";

	int c,debug=0;
	int first_song;

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dhvrs"))!=-1) {
		switch (c) {
			case 'd':
				/* Debug messages */
				printf("Debug enabled\n");
				debug=1;
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
				/* raw */
				debug=2;
				break;
			case 's':
				/* skip */
				debug|=4;
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	first_song=optind;

	if (argv[first_song]!=NULL) {
		strcpy(filename,argv[first_song]);
	}

	/* Dump the song */
	dump_song(filename,debug);

	return 0;
}
