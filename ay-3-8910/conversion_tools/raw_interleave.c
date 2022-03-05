/* Interleave a ym file */
/* Many players won't play unless it is interleaved */
/* Interleaving makes it a lot more compressible */
/* We should LHA compress as well, but no open-source code will do that? */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

static int debug=0;

#define MAXSIZE 65536*16

unsigned char data[MAXSIZE];

static void print_help(int v, char *cmd) {
	printf("%s -- interleave a raw file\n",cmd);
	printf("\n\n");
	printf("Usage:\t%s BASE\n",cmd);
	printf("\tWill convert BASE.raw to BASE.partX\n\n");
	/* All your BASE are belong to us */
	exit(1);
}

int main(int argc, char **argv) {

	char in_file[BUFSIZ];
	char out_file[BUFSIZ];

	FILE *fin,*fout;
	int num_frames;
	int i,j,c;
	int chunksize=0,max=14;
	int frame_start,frame_end;
	int filesize;
	int which_chunk=0;

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dhv:c:m:"))!=-1) {
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
			case 'c':
				/* chunk size */
				chunksize=atoi(optarg);
				break;
			case 'm':
				/* max register */
				max=atoi(optarg);
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	if (optind>argc) {
		print_help(0,argv[0]);
	}

	sprintf(in_file,"%s.raw",argv[optind]);

	fin=fopen(in_file,"r");
	if (fin==NULL) {
		fprintf(stderr,"Could not open %s\n",in_file);
		return -1;
	}

	filesize=read(fileno(fin),data,MAXSIZE);
	if (filesize>=MAXSIZE) {
		fprintf(stderr,"File too big!\n");
		return -1;
	}
	if (filesize<0) {
		fprintf(stderr,"Error reading %s\n",strerror(errno));
		return -1;
	}

	num_frames=filesize/14;
	fprintf(stderr,"filesize %d, frames %d, leftover %d\n",
		filesize,num_frames,filesize%14);
	if (chunksize) {
		fprintf(stderr,
			"Splitting up into chunks of %d frames (%d parts)\n",
			chunksize,(num_frames/chunksize)+1);
	}

	while(1) {

		sprintf(out_file,"%s.part%d",argv[optind],which_chunk+1);
		fout=fopen(out_file,"w");
		if (fout==NULL) {
			fprintf(stderr,"Could not open %s\n",out_file);
			return -1;
		}

		/* write out frames interleaved */

		frame_start=0;
		frame_end=num_frames;

		if (chunksize) {
			frame_start=chunksize*which_chunk;
			frame_end=frame_start+chunksize;
		}

		for(j=0;j<max;j++) {
			for(i=frame_start;i<frame_end;i++) {
				if (i>num_frames) fputc(0,fout);
				else fputc(data[(i*14)+j],fout);
			}
		}

		fclose(fout);

		which_chunk+=1;

		if ((which_chunk*chunksize) > num_frames) break;
	}

	fclose(fin);
	return 0;
}
