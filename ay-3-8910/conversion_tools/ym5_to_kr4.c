/* Convert ym5 file to kr4, optimized for playing on AppleII/Mockingboard */

/* Note, need to have liblz4-dev installed, apt-get install liblz4-dev */

/* krw file format */
/* KRW (ASCII), SKIPVALUE (bytes to first block) */
/* ((40-TITLE_LEN)/2) NULL_TERMINATED_TITLE_STRING */
/* ((40-AUTHOR_LEN)/2) NULL_TERMINATED_AUTHOR_STRING */
/* 14, 0:00 /  M:SS\0, where M/SS is the length */
/* LENL/LENH followed by LZ4 block of first 3 chunks (768*14) of ym5 data */
/* repeat, when done LENL/LENH is 0/0 */
/* The data is Interleaved, zero-padded, and frame[0]=0xff on last frame */

/* This is like a KRW, but optimized for playing 4-channel files */
/* Such as those from converted MOD files. */
/* Takes two ym5 files as input, outputs 1 2 3, 1 4 3 */
/* Also strips off envelope */


#define DEFAULT_PAGES_PER_CHUNK	3

static int pages_per_chunk=DEFAULT_PAGES_PER_CHUNK;

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

	printf("\nym5_to_kr4 version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This converts ym5 files to kr4 (AppleII/Mockingboard)\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-t] [-a] [-f] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");
	printf("\t-t: override title\n");
	printf("\t-a: override author\n");
	printf("\t-f: force early end frame\n");

	exit(0);
}



static int dump_song_kr4(char *filename1, char *filename2,
		int debug, int size,
		char *outfile, char *a, char *t, int force_end_frame) {

	int result;
	int data_size;

	int x,y;

	struct ym_song_t ym_song,ym_song2;

	int num_chunks;
	FILE *fff;
	char outname[BUFSIZ];
	int j;
	int minutes,seconds;
	int compressed_size;
	int fake_frames;

	unsigned char *interleaved_data;
	char *raw_data,*compressed_data;
	unsigned char frame[YM5_FRAME_SIZE];
	int skip;
	unsigned char temp_value;

	char title[BUFSIZ];
	char author[BUFSIZ];

	int end_frame;


	/* FIXME: if "-" then use stdout? */
	sprintf(outname,"%s",outfile);
	fff=fopen(outname,"w");
	if (fff==NULL) {
		fprintf(stderr,"Error opening %s\n",outname);
		return -1;
	}

	fprintf(stderr, "\nDumping song %s+%s to %s\n",
		filename1,filename2,outname);

	/*****************************************/
	/* LOAD SONG				*/
	/***************************************/
	result=load_ym_song(filename1,&ym_song);
	if (result<0) {
		return -1;
	}

	result=load_ym_song(filename2,&ym_song2);
	if (result<0) {
		return -1;
	}

	if (force_end_frame) {
		end_frame=force_end_frame;
	}
	else {
		end_frame=ym_song.num_frames;
	}

	seconds=end_frame/ym_song.frame_rate;
	minutes=seconds/60;
	seconds-=(minutes*60);

	if (minutes>9) {
		fprintf(stderr,"Warning!  Decoder doesn't "
				"necessarily handle files > 9min\n");
	}

	fprintf(stderr,"\tFrames: %d, %d:%02d\n",
		end_frame,minutes,seconds);

	fputc('K',fff);
	fputc('R',fff);
	fputc('W',fff);

	/* default */
//	fprintf(fff,"INTRO2: JUNGAR OF BIT WORLD FROM KIEV%c",0);
//	fprintf(fff,"BY: SURGEON (ALEKSEY LUTSENKO)%c",0);

	/* Get from title */
	if (t) {
		strcpy(title,t);
	}
	else {
		strcpy(title,ym_song.song_name);
	}
	title[40]=0;

	if (a) {
		sprintf(author,"%s",a);
	}
	else {
		sprintf(author,"BY: %s",ym_song.author);
	}
	author[40]=0;

	fprintf(stderr,"TITLE: %s\n",title);
	fprintf(stderr,"AUTHOR: %s\n",author);


	skip=1+3+3+
		strlen(title)+
		strlen(author)+
		strlen("0:00  /  0:00");

	fputc(skip,fff);

	fputc( (40-strlen(title))/2,fff);
	fprintf(fff,title);
	fputc(0,fff);
	fputc( (40-strlen(author))/2,fff);
	fprintf(fff,author);
	fputc(0,fff);
	fputc(13,fff);
	fprintf(fff,"0:00  / %2d:%02d",minutes,seconds);
	fputc(0,fff);

	/******************/
	/* Play the song! */
	/******************/


	/* plus one for end frame */
		// 8960 /768 = 11.6
		// num_chunks = 12
		// 8960/50=179.2 = 2:59
		// stops at 2:48 = 8400 (11 is 8448)
	num_chunks=(end_frame+1)/(pages_per_chunk*256);
	/* pad to even number of frames */
	num_chunks+=1;
	data_size=num_chunks*pages_per_chunk*256*14;

	fake_frames=data_size/14;

	fprintf(stderr,"%d frames %d fake_frames %d chunks total total_size %d\n",
			end_frame,fake_frames,num_chunks,data_size);


	interleaved_data=calloc(data_size,sizeof(char));
	if (interleaved_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	/* 0xFFs at end are end-of-song marker */
	/* Tricky if exact multiple of 256 :( */
	memset(interleaved_data,0xff,data_size);

	raw_data=calloc(pages_per_chunk*256*14,sizeof(char));
	if (raw_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	compressed_data=calloc(pages_per_chunk*256*14*2,sizeof(char));
	if (raw_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}


	/* Interleave the data */
	for(y=0;y<14;y++) {
		for(x=0;x<end_frame;x++) {

			if (y<11) {
				ym_return_frame(&ym_song,x,frame,NULL);
				interleaved_data[(y*fake_frames)+x]=
					frame[y];
			}
			else {
				ym_return_frame(&ym_song2,x,frame,NULL);
				if (y==11) temp_value=frame[2];
				if (y==12) temp_value=frame[3];
				if (y==13) temp_value=frame[9];

				interleaved_data[(y*fake_frames)+x]=
					temp_value;

			}
		}
	}
	/* HACK! make sure we have end-marker */
	interleaved_data[(1*fake_frames)+(end_frame-1)]=0xff;

	for(j=0;j<num_chunks;j++) {
		for(y=0;y<14;y++) {
			for(x=0;x<(256*pages_per_chunk);x++) {
				raw_data[x+y*(256*pages_per_chunk)]=
					interleaved_data[x+
					j*(256*pages_per_chunk)+
					(y*fake_frames)];
			}

		}

//		printf("%d: ",j);
//		for(y=0;y<14;y++) {
//			printf("%02X ",
//				(unsigned char)raw_data[(255*pages_per_chunk)+(y*256*pages_per_chunk)]);
//		}
//		printf("\n");

		compressed_size=LZ4_compress_HC (raw_data,
						compressed_data,
						256*pages_per_chunk*14,
						256*pages_per_chunk*14*2,
						16);

		if (compressed_size>65536) {
			fprintf(stderr,"Error!  Compressed data too big!\n");
		}
		fputc(compressed_size%256,fff);
		fputc(compressed_size/256,fff);

		fwrite(compressed_data,sizeof(unsigned char),
			compressed_size,fff);

//		fprintf(stderr,"%d size %x\n",j,compressed_size);

//		for(y=0;y<14;y++) {
//			for(x=0;x<(256*pages_per_chunk);x++) {
//				fputc(raw_data[x+y*(256*pages_per_chunk)],fff);
//			}
//		}


	}

	fputc(0,fff);
	fputc(0,fff);


	fclose(fff);

	fprintf(stderr,"; Total size = %d bytes\n",end_frame*14);

	free(interleaved_data);
	free(raw_data);
	free(compressed_data);

	/* Free the ym file */
	free(ym_song.file_data);

	return 0;
}


int main(int argc, char **argv) {

	char filename1[BUFSIZ]="intro2.ym";
	char filename2[BUFSIZ]="intro2.ym";
	char outfile[BUFSIZ]="out.krw";

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
			case 'a':
				/* author */
				author=strdup(optarg);
				break;
			case 'f':
				/* force end-frame */
				force_end_frame=atoi(optarg);
				break;
			case 't':
				/* title */
				title=strdup(optarg);
				break;

			default:
				print_help(0,argv[0]);
				break;
		}
	}

	first_song=optind;

	if (argv[first_song]!=NULL) {
		strcpy(filename1,argv[first_song]);
	}

	if (first_song+1<argc) {
		strcpy(filename2,argv[first_song+1]);
	}

	if (first_song+2<argc) {
		strcpy(outfile,argv[first_song+2]);
	}

	/* Dump the song */
	dump_song_kr4(filename1,filename2,
			debug,size,outfile,author,title,force_end_frame);

	return 0;
}
