/* Convert ym5 file to mega, for megademo */

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


#define VERSION "0.8"

static void print_help(int just_version, char *exec_name) {

	printf("\nym5_to_mega version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
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

#define MAX_PATTERNS 128

struct pattern_type {
	unsigned char pattern[128];
	int count;
	int valid;
} patterns[MAX_PATTERNS];

int add_pattern(unsigned char p0, unsigned char p1, unsigned char p2, unsigned char p3,
		unsigned char p4, unsigned char p5, unsigned char p6, unsigned char p7) {

	int i,found=0;

	for(i=0;i<MAX_PATTERNS;i++) {
		if (patterns[i].valid) {
			if ((patterns[i].pattern[0]==p0) &&
				(patterns[i].pattern[1]==p1) &&
				(patterns[i].pattern[2]==p2) &&
				(patterns[i].pattern[3]==p3) &&
				(patterns[i].pattern[4]==p4) &&
				(patterns[i].pattern[5]==p5) &&
				(patterns[i].pattern[6]==p6) &&
				(patterns[i].pattern[7]==p7)) {
				patterns[i].count++;
				return 0;
			}

		}

	}
	for(i=0;i<MAX_PATTERNS;i++) {
		if (patterns[i].valid==0) {
			patterns[i].valid=1;
			patterns[i].count=1;
			patterns[i].pattern[0]=p0;
			patterns[i].pattern[1]=p1;
			patterns[i].pattern[2]=p2;
			patterns[i].pattern[3]=p3;
			patterns[i].pattern[4]=p4;
			patterns[i].pattern[5]=p5;
			patterns[i].pattern[6]=p6;
			patterns[i].pattern[7]=p7;
			return 0;
		}
	}

	return 0;
}

static int dump_song_mega(char *filename1,
		int debug, int size,
		char *outfile, char *a, char *t, int force_end_frame) {

	int result;
	int data_size;

	int x;

	struct ym_song_t ym_song;

	FILE *fff;
	char outname[BUFSIZ];
	unsigned char frame[YM5_FRAME_SIZE];

	unsigned char *adatah,*adatal,*bdatah,*bdatal,
			*cdatah,*cdatal,*ndatah,*ndatal;

	int end_frame;

	for(x=0;x<MAX_PATTERNS;x++) {
		patterns[x].valid=0;
		patterns[x].count=0;
	}

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

	data_size=end_frame*14;

	fprintf(stderr,"total_size %d\n",data_size);

	adatah=calloc(end_frame*14,sizeof(char));
	if (adatah==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	adatal=calloc(end_frame*14,sizeof(char));
	if (adatal==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	bdatah=calloc(end_frame*14,sizeof(char));
	if (bdatah==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	bdatal=calloc(end_frame*14,sizeof(char));
	if (bdatal==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	cdatah=calloc(end_frame*14,sizeof(char));
	if (cdatah==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	cdatal=calloc(end_frame*14,sizeof(char));
	if (cdatal==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	ndatah=calloc(end_frame*14,sizeof(char));
	if (ndatah==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	ndatal=calloc(end_frame*14,sizeof(char));
	if (ndatal==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}



	/* Setup the data */
	for(x=0;x<end_frame;x++) {
		ym_return_frame(&ym_song,x,frame,NULL);
		adatal[x]=frame[0];
		adatah[x]=(frame[8]<<4)|(frame[1]);
		bdatal[x]=frame[2];
		bdatah[x]=(frame[9]<<4)|(frame[3]);
		cdatal[x]=frame[4];
		cdatah[x]=(frame[10]<<4)|(frame[5]);
		ndatal[x]=frame[6];
		ndatah[x]=frame[7];
	}
	fclose(fff);

//	for(x=0;x<end_frame;x++) {
//		printf("%05d: %02x%02x\n",x,adatah[x],adatal[x]);
//	}



	for(x=0;x<MAX_PATTERNS;x++) {
		patterns[x].valid=0;
		patterns[x].count=0;
	}

	for(x=0;x<end_frame/4;x++) {
		add_pattern(adatah[x*4],adatal[x*4],
			adatah[x*4+1],adatal[x*4+1],
			adatah[x*4+2],adatal[x*4+2],
			adatah[x*4+3],adatal[x*4+3]);
	}

	for(x=0;x<MAX_PATTERNS;x++) {
		if (patterns[x].valid) {
			printf("%04d: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				patterns[x].count,
				patterns[x].pattern[0],
				patterns[x].pattern[1],
				patterns[x].pattern[2],
				patterns[x].pattern[3],
				patterns[x].pattern[4],
				patterns[x].pattern[5],
				patterns[x].pattern[6],
				patterns[x].pattern[7]);

		}
	}

	printf("B\n");

	for(x=0;x<MAX_PATTERNS;x++) {
		patterns[x].valid=0;
		patterns[x].count=0;
	}

	for(x=0;x<end_frame/4;x++) {
		add_pattern(bdatah[x*4],bdatal[x*4],
			bdatah[x*4+1],bdatal[x*4+1],
			bdatah[x*4+2],bdatal[x*4+2],
			bdatah[x*4+3],bdatal[x*4+3]);
	}

	for(x=0;x<MAX_PATTERNS;x++) {
		if (patterns[x].valid) {
			printf("%04d: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				patterns[x].count,
				patterns[x].pattern[0],
				patterns[x].pattern[1],
				patterns[x].pattern[2],
				patterns[x].pattern[3],
				patterns[x].pattern[4],
				patterns[x].pattern[5],
				patterns[x].pattern[6],
				patterns[x].pattern[7]);

		}
	}

	printf("C\n");

	for(x=0;x<MAX_PATTERNS;x++) {
		patterns[x].valid=0;
		patterns[x].count=0;
	}

	for(x=0;x<end_frame/4;x++) {
		add_pattern(cdatah[x*4],cdatal[x*4],
			cdatah[x*4+1],cdatal[x*4+1],
			cdatah[x*4+2],cdatal[x*4+2],
			cdatah[x*4+3],cdatal[x*4+3]);
	}

	for(x=0;x<MAX_PATTERNS;x++) {
		if (patterns[x].valid) {
			printf("%04d: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				patterns[x].count,
				patterns[x].pattern[0],
				patterns[x].pattern[1],
				patterns[x].pattern[2],
				patterns[x].pattern[3],
				patterns[x].pattern[4],
				patterns[x].pattern[5],
				patterns[x].pattern[6],
				patterns[x].pattern[7]);

		}
	}

	printf("N\n");

	for(x=0;x<MAX_PATTERNS;x++) {
		patterns[x].valid=0;
		patterns[x].count=0;
	}

	for(x=0;x<end_frame/4;x++) {
		add_pattern(ndatah[x*4],ndatal[x*4],
			ndatah[x*4+1],ndatal[x*4+1],
			ndatah[x*4+2],ndatal[x*4+2],
			ndatah[x*4+3],ndatal[x*4+3]);
	}

	for(x=0;x<MAX_PATTERNS;x++) {
		if (patterns[x].valid) {
			printf("%04d: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				patterns[x].count,
				patterns[x].pattern[0],
				patterns[x].pattern[1],
				patterns[x].pattern[2],
				patterns[x].pattern[3],
				patterns[x].pattern[4],
				patterns[x].pattern[5],
				patterns[x].pattern[6],
				patterns[x].pattern[7]);

		}
	}




	fprintf(stderr,"; Total size = %d bytes\n",end_frame*14);

	free(adatah); free(adatal);
	free(bdatah); free(bdatal);
	free(cdatah); free(cdatal);
	free(ndatah); free(ndatal);

	/* Free the ym file */
	free(ym_song.file_data);

	return 0;
}


int main(int argc, char **argv) {

	char filename1[BUFSIZ]="mockingbird_4.ym";
	char outfile[BUFSIZ]="out.mega";

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
	dump_song_mega(filename1,
			debug,size,outfile,author,title,force_end_frame);

	return 0;
}
