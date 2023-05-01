/* Interleave a ym file */
/* Many players won't play unless it is interleaved */
/* Interleaving makes it a lot more compressible */
/* We should LHA compress as well, but no open-source code will do that? */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

struct ym_header {
	char id[4];				// 0  -> 4
	char check[8];				// 4  -> 12
	uint32_t vbl;				// 12 -> 16
	uint32_t song_attr;			// 16 -> 20
	uint16_t digidrum;			// 20 -> 22
	uint32_t external_frequency;		// 22 -> 26
	uint16_t player_frequency;		// 26 -> 28
	uint32_t loop;				// 28 -> 32
	uint16_t additional_data;		// 32 -> 34
}  __attribute__((packed)) our_header;


static int debug=0;

static void print_help(int v, char *cmd) {
	printf("%s -- interleave a ym file\n",cmd);
	printf("\n\n");
	printf("Usage:\t%s BASE\n",cmd);
	printf("\tWill convert BASE.ym to BASE.ym5\n\n");
	/* All your BASE are belong to us */
	exit(1);
}

int main(int argc, char **argv) {

	char in_file[BUFSIZ];
	char out_file[BUFSIZ];

	FILE *fin,*fout;
	int ch;
	int num_frames;
	char *frames;
	int i,j,c;
	int chunksize=0,skip=0,noheader=0;
	int frame_start,frame_end;

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dhnv:c:s:"))!=-1) {
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
			case 'n':
				/* noheader */
				noheader=1;
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
				break;
			case 'c':
				/* chunk size */
				chunksize=atoi(optarg);
				break;
			case 's':
				/* skip */
				skip=atoi(optarg);
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	if (optind>argc) {
		print_help(0,argv[0]);
	}

	sprintf(in_file,"%s.ym",argv[optind]);
	sprintf(out_file,"%s.ym5",argv[optind]);

	fin=fopen(in_file,"r");
	if (fin==NULL) {
		fprintf(stderr,"Could not open %s\n",in_file);
		return -1;
	}

	fout=fopen(out_file,"w");
	if (fout==NULL) {
		fprintf(stderr,"Could not open %s\n",out_file);
		return -1;
	}

	fread(&our_header,sizeof(struct ym_header),1,fin);

	if (!noheader) {

		/* make it an interleaved file */
		our_header.song_attr=htonl(0x1);

		fwrite(&our_header,sizeof(struct ym_header),1,fout);

		/* write out title */
		do {
			ch=fgetc(fin);
			fputc(ch,fout);
		} while (ch!=0);

		/* write out author */
		do {
			ch=fgetc(fin);
			fputc(ch,fout);
		} while (ch!=0);

		/* write out comments */
		do {
			ch=fgetc(fin);
			fputc(ch,fout);
		} while (ch!=0);
	}

	num_frames=ntohl(our_header.vbl);
	printf("%d frames\n",num_frames);

	frames=calloc(num_frames,16);
	if (frames==NULL) {
		fprintf(stderr,"Cannot allocate memory\n");
		return -1;
	}

	/* read in frames */
	for(i=0;i<num_frames;i++) {
		for(j=0;j<16;j++) {
			ch=fgetc(fin);
			frames[ (i*16)+j]=ch;
		}
	}

	/* write out frames interleaved */

	frame_start=0;
	frame_end=num_frames;

	if (chunksize) {
		frame_start=chunksize*skip;
		frame_end=frame_start+chunksize;
	}

	for(j=0;j<16;j++) {
		for(i=frame_start;i<frame_end;i++) {
			if (i>num_frames) fputc(0,fout);
			else fputc(frames[ (i*16)+j],fout);
		}
	}

	if (!noheader) {
		fprintf(fout,"End!");
	}

	free(frames);
	fclose(fin);
	fclose(fout);

	return 0;
}
