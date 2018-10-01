/* Convert ym5 file to tfv, optimized for playing on AppleII/Mockingboard */
/* It's designed to be used in low-overhead bitbang mode */
/* Also assumes very simple sound files, only on Channel A+B */

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


#define VERSION "0.7"

static void print_help(int just_version, char *exec_name) {

	printf("\nym5_to_tfv version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This converts ym5 files to tfv (simple AppleII/Mockingboard)\n\n");

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



static int dump_song_kr4(char *filename1,
		int debug, int size,
		char *outfile, char *a, char *t, int force_end_frame) {

	int result;
	int data_size;

	int x,y;

	struct ym_song_t ym_song;

	int num_chunks;
	FILE *fff;
	char outname[BUFSIZ];
	int j;
	int fake_frames;

	unsigned char *interleaved_data;
	char *raw_data,*compressed_data;
	unsigned char frame[YM5_FRAME_SIZE];

	int end_frame;


	/* FIXME: if "-" then use stdout? */
	sprintf(outname,"%s",outfile);
	fff=fopen(outname,"w");
	if (fff==NULL) {
		fprintf(stderr,"Error opening %s\n",outname);
		return -1;
	}

	fprintf(stderr, "\nDumping song %s to %s\n",
		filename1,outname);

	/*****************************************/
	/* LOAD SONG				*/
	/***************************************/
	result=load_ym_song(filename1,&ym_song);
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

	int pages_per_chunk=1;

	/* plus one for end frame */
	/* also we assume every other frame not needed */
	num_chunks=(end_frame+1)/(256);

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
			ym_return_frame(&ym_song,x,frame,NULL);
			interleaved_data[(y*fake_frames)+x]=frame[y];
		}
	}
	/* HACK! make sure we have end-marker */
	interleaved_data[(1*fake_frames)+(end_frame-1)]=0xff;

	for(y=0;y<14;y++) {
		if (y==1) continue; // skip coarse A
		if (y==4) continue; // skip fine C
		if (y==5) continue; // skip coarse C
		if (y==6) continue; // skip noise
		if (y==7) continue; // skip enable
		if (y==10) continue; // skip C volume
		if (y==11) continue; // skip envelope
		if (y==12) continue; // skip envelope
		if (y==13) continue; // skip envelope
		for(j=0;j<num_chunks;j++) {
			for(x=0;x<(256*pages_per_chunk);x+=2) {
				fputc(
					interleaved_data[x+
					j*(256*pages_per_chunk)+
					(y*fake_frames)],
				fff);
			}

		}
		/* hack */
//		for(j=0;j<128;j++) fputc(0xff,fff);
//		for(j=0;j<64;j++) fputc(0xff,fff);

	}


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
		strcpy(outfile,argv[first_song+1]);
	}

	/* Dump the song */
	dump_song_kr4(filename1,
			debug,size,outfile,author,title,force_end_frame);

	return 0;
}
