/* Convert ym5 file to krw, optimized for playing on AppleII/Mockingboard */

/* Note, need to have liblz4-dev installed, apt-get install liblz4-dev */

/* krw file format */
/* ((40-TITLE_LEN)/2) NULL_TERMINATED_TITLE_STRING */
/* ((40-AUTHOR_LEN)/2) NULL_TERMINATED_AUTHOR_STRING */
/* 14, 0:00 /  M:SS\0, where M/SS is the length */
/* LENL/LENH followed by LZ4 block of first 3 chunks (768*14) of ym5 data */
/* repeat, when done LENL/LENH is 0/0 */
/* The data is Interleaved, zero-padded, and frame[0]=0xff on last frame */

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

	printf("\nym5_to_krw version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This converts ym5 files to krw (AppleII/Mockingboard)\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");

	exit(0);
}

#define NUMPAGES	3

static int dump_song_krw(char *filename, int debug, int size,
		char *outfile) {

	int result;
	int data_size;

	int x,y;

	struct ym_song_t ym_song;

	int num_chunks;
	FILE *fff;
	char outname[BUFSIZ];
	int j;
	int minutes,seconds;
	int compressed_size;

	unsigned char *interleaved_data;
	char *raw_data,*compressed_data;
	unsigned char frame[YM5_FRAME_SIZE];

	/* FIXME: if "-" then use stdout? */
	sprintf(outname,"%s",outfile);
	fff=fopen(outname,"w");
	if (fff==NULL) {
		fprintf(stderr,"Error opening %s\n",outname);
		return -1;
	}

	fprintf(stderr, "\nDumping song %s to %s\n",filename,outname);

	result=load_ym_song(filename,&ym_song);
	if (result<0) {
		return -1;
	}

	seconds=ym_song.num_frames/ym_song.frame_rate;
	minutes=seconds/60;
	seconds-=(minutes*60);

	if (minutes>9) {
		fprintf(stderr,"Warning!  Decoder doesn't "
				"necessarily handle files > 9min\n");
	}

	fprintf(stderr,"\tFrames: %d, %d:%02d\n",
		ym_song.num_frames,minutes,seconds);


	fputc(1,fff);
	fprintf(fff,"INTRO2: JUNGAR OF BIT WORLD FROM KIEV%c",0);
	fputc(5,fff);
	fprintf(fff,"BY: SURGEON (ALEKSEY LUTSENKO)%c",0);
	fputc(14,fff);
	fprintf(fff,"0:00 / %d:%02d%c",minutes,seconds,0);


	/**********************/
	/* Print song summary */
	/**********************/
#if 0
	int length_seconds;

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
#endif
	/******************/
	/* Play the song! */
	/******************/


	/* plus one for end frame */
	num_chunks=(ym_song.num_frames+1)/(NUMPAGES*256);
	/* pad to even number of frames */
	num_chunks+=1;
	data_size=num_chunks*NUMPAGES*256*14;

	fprintf(stderr,"%d frames %d chunks total total_size %d\n",
			ym_song.num_frames,num_chunks,data_size);


	interleaved_data=calloc(data_size,sizeof(char));
	if (interleaved_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	raw_data=calloc(NUMPAGES*256*14,sizeof(char));
	if (raw_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	compressed_data=calloc(NUMPAGES*256*14*2,sizeof(char));
	if (raw_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}


	for(y=0;y<14;y++) {
		for(x=0;x<ym_song.num_frames;x++) {

			ym_return_frame(&ym_song,x,frame,NULL);
			interleaved_data[(y*ym_song.num_frames)+x]=
				frame[y];
		}
	}

	for(j=0;j<num_chunks;j++) {
		for(y=0;y<14;y++) {
			for(x=0;x<(256*NUMPAGES);x++) {
				raw_data[x+y*(256*NUMPAGES)]=
					interleaved_data[x+
					j*(256*NUMPAGES)+
					(y*ym_song.num_frames)];
			}
		}

		compressed_size=LZ4_compress_HC (raw_data,
						compressed_data,
						256*NUMPAGES*14,
						256*NUMPAGES*14*2,
						16);

		if (compressed_size>65536) {
			fprintf(stderr,"Error!  Compressed data too big!\n");
		}
		fputc(compressed_size%256,fff);
		fputc(compressed_size/256,fff);

		fwrite(compressed_data,sizeof(unsigned char),
			compressed_size,fff);

//		for(y=0;y<14;y++) {
//			for(x=0;x<(256*NUMPAGES);x++) {
//				fputc(raw_data[x+y*(256*NUMPAGES)],fff);
//			}
//		}


	}

	fputc(0,fff);
	fputc(0,fff);


	fclose(fff);

	fprintf(stderr,"; Total size = %d bytes\n",ym_song.num_frames*14);

	free(interleaved_data);
	free(raw_data);
	free(compressed_data);

	/* Free the ym file */
	free(ym_song.file_data);

	return 0;
}


int main(int argc, char **argv) {

	char filename[BUFSIZ]="intro2.ym";
	char outfile[BUFSIZ]="out.krw";

	int c,debug=0;
	int first_song;

	int size=4096;

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dhv"))!=-1) {
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
	dump_song_krw(filename,debug,size,outfile);

	return 0;
}
