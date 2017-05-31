/* Dump a ym5 file */

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
#include "ym_lib.h"
//#include "max98306.h"

#include "version.h"

static void print_help(int just_version, char *exec_name) {

	printf("\ndump_ym5 version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This plays YM5 music files out of the VMW AY-3-8910 device\n");
	printf("on a suitably configured Raspberry Pi machine.\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-i] [-t] [-m] [-s] [-n] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");

	exit(0);
}

static int play_song(char *filename) {

	int length_seconds;

	int result;
	int frame_num=0;

	struct ym_song_t ym_song;

	printf("\nDumping song %s\n",filename);

	result=load_ym_song(filename,&ym_song);
	if (result<0) {
		return -1;
	}

	/**********************/
	/* Print song summary */
	/**********************/

	printf("\tYM%d",ym_song.type);
	printf("\tSong attributes (%d) : ",ym_song.attributes);
	printf("Interleaved=%s\n",ym_song.interleaved?"yes":"no");
	if (ym_song.num_digidrum>0) {
		printf("Num digidrum samples: %d\n",ym_song.num_digidrum);
	}
	printf("\tFrames: %d, ",ym_song.num_frames);
	printf("Chip clock: %d Hz, ",ym_song.master_clock);
	printf("Frame rate: %d Hz, ",ym_song.frame_rate);
	if (ym_song.frame_rate!=50) {
		fprintf(stderr,"FIX ME framerate %d\n",ym_song.frame_rate);
		exit(1);
	}
	length_seconds=ym_song.num_frames/ym_song.frame_rate;
	printf("Length=%d:%02d\n",length_seconds/60,length_seconds%60);
	printf("\tLoop frame: %d, ",ym_song.loop_frame);
	printf("Extra data size: %d\n",ym_song.extra_data);
	printf("\tSong name: %s\n",ym_song.song_name);
	printf("\tAuthor name: %s\n",ym_song.author);
	printf("\tComment: %s\n",ym_song.comment);

	/******************/
	/* Play the song! */
	/******************/

	frame_num=0;
	while(1) {

		if (frame_num%96==0) {
			printf("FRAME ---- A    B    C    N    E -------------------------------\n");
		}

		ym_dump_frame(&ym_song,frame_num,0);

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
	int result;
	int c,dump_info=0;
	int next_song,first_song;

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
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	first_song=optind;

	if (argv[first_song+next_song]!=NULL) {
		strcpy(filename,argv[first_song+next_song]);
	}

	/* Dump the song */
	result=play_song(filename);

	return 0;
}
