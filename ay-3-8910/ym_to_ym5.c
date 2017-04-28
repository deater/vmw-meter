/* Interleave a ym file */
/* Many players won't play unless it is interleaved */
/* Interleaving makes it a lot more compressible */
/* We should LHA compress as well, but no open-source code will do that? */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

int main(int argc, char **argv) {

	char in_file[BUFSIZ];
	char out_file[BUFSIZ];

	FILE *fin,*fout;
	int ch;
	int num_frames;
	char *frames;
	int i,j;

	if (argc<2) {
		printf("%s -- interleave a ym file\n",argv[0]);
		printf("\n\n");
		printf("Usage:\t%s BASE\n",argv[0]);
		printf("\tWill convert BASE.ym to BASE.ym5\n\n");
		/* All your BASE are belong to us */
		exit(1);
	}

	sprintf(in_file,"%s.ym",argv[1]);
	sprintf(out_file,"%s.ym5",argv[1]);


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

#if 0
	for(i=0;i<num_frames;i++) {
		for(j=0;j<16;j++) {
			fputc(frames[ (i*16)+j],fout);
		}
	}
#endif


	for(j=0;j<16;j++) {
		for(i=0;i<num_frames;i++) {
			fputc(frames[ (i*16)+j],fout);
		}
	}


	fprintf(fout,"End!");

	free(frames);
	fclose(fin);
	fclose(fout);

	return 0;
}
