/* Convert ym5 file to krg, optimized for playing on AppleII/Mockingboard */

/* Note, need to have liblz4-dev installed, apt-get install liblz4-dev */

/* krg file format */
/* tries to fit full song into roughly 16k of RAM */
/* 25Hz */
/* no envelope */

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

#include "stats.h"
#include "ym_lib.h"

/* Need liblz4 installed */
#include "lz4.h"
#include "lz4hc.h"

#define VERSION "0.7"

static void print_help(int just_version, char *exec_name) {

	printf("\nym5_to_krg version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This converts ym5 files to krg (AppleII/Mockingboard)\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-f] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");
	printf("\t-f: force early end frame\n");

	exit(0);
}



static int dump_song_krg(char *filename, int debug, int size,
		char *outfile, char *a, char *t, int force_end_frame) {

	int result;
	int data_size;

	int x,y;

	struct ym_song_t ym_song;

	int num_chunks;
	FILE *fff;
	char outname[BUFSIZ];
	int fake_frames;

	unsigned char *interleaved_data;
	char *raw_data;
	unsigned char frame[YM5_FRAME_SIZE];

	int end_frame;


	/* FIXME: if "-" then use stdout? */
	sprintf(outname,"%s",outfile);
	fff=fopen(outname,"w");
	if (fff==NULL) {
		fprintf(stderr,"Error opening %s\n",outname);
		return -1;
	}

	fprintf(stderr, "\nDumping song %s to %s\n",filename,outname);

	/*****************************************/
	/* LOAD SONG				*/
	/***************************************/
	result=load_ym_song(filename,&ym_song);
	if (result<0) {
		return -1;
	}

	if (force_end_frame) {
		end_frame=force_end_frame;
	}
	else {
		end_frame=ym_song.num_frames;
	}

	/******************/
	/* Play the song! */
	/******************/
#define FRAMESIZE	8

	/* plus one for end frame */
		// 8960 /768 = 11.6
		// num_chunks = 12
		// 8960/50=179.2 = 2:59
		// stops at 2:48 = 8400 (11 is 8448)

	/* Round to multiple of 256 */
	num_chunks=(((end_frame+1)/256)+1);
	data_size=(num_chunks)*256*FRAMESIZE;
	fake_frames=data_size/FRAMESIZE;

	fprintf(stderr,"%d frames %d fake_frames %d num_chunks total_size %d\n",
			end_frame,fake_frames,num_chunks,data_size);


	interleaved_data=calloc(data_size,sizeof(char));
	if (interleaved_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	/* 0xFFs at end are end-of-song marker */
	memset(interleaved_data,0xff,data_size);

	raw_data=calloc(data_size,sizeof(char));
	if (raw_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	/* Interleave the data */
	for(x=0;x<end_frame;x++) {
		ym_return_frame(&ym_song,x,frame,NULL);

		y=0; //afine
		interleaved_data[y*(num_chunks*256)+x]=frame[0];

		y=1; //bfine
		interleaved_data[y*(num_chunks*256)+x]=frame[2];

		y=2; //cfine
		interleaved_data[y*(num_chunks*256)+x]=frame[4];

		y=3; //noise
		interleaved_data[y*(num_chunks*256)+x]=frame[6];

		y=4; //enable
		interleaved_data[y*(num_chunks*256)+x]=frame[7];

		y=5; //acoarse/bcoarse
		interleaved_data[y*(num_chunks*256)+x]=((frame[1]&0xf)<<4)|(frame[3]&0xf);

		y=6; //ccoarse/aamp
		interleaved_data[y*(num_chunks*256)+x]=((frame[5]&0xf)<<4)|(frame[8]&0xf);

		y=7; //bamp/camp
		interleaved_data[y*(num_chunks*256)+x]=((frame[9]&0xf)<<4)|(frame[10]&0xf);
	}

	for(x=0;x<data_size;x++) {
		fputc(interleaved_data[x],fff);
	}

	fclose(fff);

	fprintf(stderr,"; Total size = %d bytes\n",data_size);


	free(interleaved_data);
	free(raw_data);

	/* Free the ym file */
	free(ym_song.file_data);

	return 0;
}


int main(int argc, char **argv) {

	char filename[BUFSIZ]="intro2.ym";
	char outfile[BUFSIZ]="out.krg";

	int c,debug=0;
	int first_song;
	int force_end_frame=0;

	int size=4096;

	char *author=NULL;
	char *title=NULL;


	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dhva:f:t:"))!=-1) {
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

			case 'f':
				/* force end-frame */
				force_end_frame=atoi(optarg);
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

	if (first_song+1<argc) {
		strcpy(outfile,argv[first_song+1]);
	}

	/* Dump the song */
	dump_song_krg(filename,debug,size,outfile,author,title,force_end_frame);

	return 0;
}
