#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* ftp://ftp.modland.com/pub/documents/format_documentation/FastTracker%202%20v2.04%20(.xm).html */

#define FIRST_HEADER_LENGTH	64




void note_to_string(int note) {

	/* 1 = C0 ? */
	/* C,C#,D,D#,E,F,F#,G,G#,A,A#,B */

	int letter,sharp,octave;

	char letters[12]={'C','C','D','D','E','F','F','G','G','A','A','B'};
	int sharps[12]={   0 , 1 , 0 , 1 , 0 , 0 , 1 , 0 , 1 , 0 , 1 , 0};

	if (note==0) printf("...");
	else if (note==97) printf("OFF");
	else if (note>97) printf("???");
	else {
		letter=letters[(note-1)%12];
		octave=(note-1)/12;
		sharp=sharps[(note-1)%12];
		printf("%c%c%d",letter,sharp?'#':'-',octave);
	}
}


struct pattern_element_struct {
	unsigned char note,instrument,volume,effect,param;
};

struct pattern_struct {
	int num_rows;
	int packing_type;
	struct pattern_element_struct p[256][4];
};

// FIXME: bigger channels, up to 16 or more?
// Or calloc it based on num_channels?

struct xm_info_struct {
	char module_name[21];
	char tracker_name[21];
	int version_high,version_low;
	int song_length;
	int restart_position;
	int number_of_channels;
	int number_of_patterns;
	int number_of_instruments;
	int flags;
	int default_tempo;
	int default_bpm;
	unsigned char pattern_order[256];
	struct pattern_struct pattern[256];
};

int dump_xm_file(struct xm_info_struct *xm) {

	int i,j,c;
	int pattern_break;

	printf("NAME: %s\n",xm->module_name);
	printf("TRACKER: %s\n",xm->tracker_name);
	printf("VERSION %d.%d\n",xm->version_high,xm->version_low);
	printf("SONG LEN: %d patterns\n",xm->song_length);
	printf("LOOP: %d\n",xm->restart_position);
	printf("CHANNELS: %d\n",xm->number_of_channels);
	printf("PATTERNS: %d\n",xm->number_of_patterns);
	printf("INSTRUMENTS: %d\n",xm->number_of_instruments);
	printf("FLAGS: %x (%s)\n",xm->flags,
			xm->flags==0?"Amiga Freq":"Linear Freq");
	printf("TEMPO: %d\n",xm->default_tempo);
	printf("BPM: %d\n",xm->default_bpm);

	printf("Pattern order:\n");
	for(i=0;i < xm->song_length;i++) {
		printf("%x ",xm->pattern_order[i]);
	}
	printf("\n");

	for(i=0;i < xm->number_of_patterns;i++) {

		printf("Pattern %x, ",i);
		printf("Num_Rows: %d\n",xm->pattern[i].num_rows);

		pattern_break=0;

		for(j=0; j < xm->pattern[i].num_rows;j++) {
			printf("%02X ",j);
			for(c=0;c<4;c++) {

				note_to_string(xm->pattern[i].p[j][c].note);

				printf(".");

				if (xm->pattern[i].p[j][c].instrument) {
					printf("%X",xm->pattern[i].p[j][c].instrument);
				}
				else {
					printf(".");
				}

				if (xm->pattern[i].p[j][c].volume) {
					printf("%02X",xm->pattern[i].p[j][c].volume-0x10);
				}
				else {
					printf("..");
				}

				if ((xm->pattern[i].p[j][c].effect) ||
					(xm->pattern[i].p[j][c].param)) {
						printf("%X%02X",
							xm->pattern[i].p[j][c].effect,
							xm->pattern[i].p[j][c].param);
				}
				else {
					printf("...");
				}

				printf(" ");

				if (xm->pattern[i].p[j][c].effect==0xd) pattern_break=1;
			}
			printf("\n");
			if (pattern_break) break;
		}
	}

	return 0;
}

int load_xm_file(char *filename, struct xm_info_struct *xm) {

	int fd,result;
	unsigned char first_header[FIRST_HEADER_LENGTH];
	int header_length;

	unsigned char *header=NULL;
	int i,j;

	unsigned char pattern_header[2048];
	unsigned char packed_pattern[2048];

	int pattern_header_length;
	int packed_size;
	int line;

	fd=open(filename,O_RDONLY);
	if (fd<0) {
		fprintf(stderr,"Error opening %s!\n",filename);
		return -1;
	}

	/*********************/
	/* Read first header */
	/*********************/

	result=read(fd,first_header,FIRST_HEADER_LENGTH);
	if (result!=FIRST_HEADER_LENGTH) {
		fprintf(stderr,"Error reading first header!\n");
		return -1;
	}

	/* validate */
	if (memcmp(first_header,"Extended Module: ",17)) {
		fprintf(stderr,"ID text does not match!\n");
		return -1;
	}

	/* module name */
	memcpy(xm->module_name,first_header+17,20);
	xm->module_name[20]=0;

	/* char */
	if (first_header[37]!=0x1a) {
		fprintf(stderr,"Random char doesn't match!\n");
		return -1;
	}

	/* tracker name */
	memcpy(xm->tracker_name,first_header+38,20);
	xm->tracker_name[20]=0;

	/* Little Endian??? */
	xm->version_low=first_header[58];
	xm->version_high=first_header[59];

	/* Little endian???? */
	header_length=(first_header[63]<<24)+(first_header[62]<<16)+
			(first_header[61]<<8)+first_header[60];
	//printf("HEADER LEN: %d\n",header_length);

	/****************************/
	/* Load actual header       */
	/****************************/

	header=calloc(header_length,sizeof(char));
	if (header==NULL) {
		fprintf(stderr,"Error allocating header!\n");
		return -1;
	}

	result=read(fd,header+4,header_length-4);
	if (result!=(header_length-4)) {
		fprintf(stderr,"Error reading header!\n");
		return -1;
	}

	/* Song Length */
	xm->song_length=header[4]+(header[5]<<8);

	/* Restart Position */
	xm->restart_position=header[6]+(header[7]<<8);

	/* Channels */
	xm->number_of_channels=header[8]+(header[9]<<8);

	/* Patterns */
	xm->number_of_patterns=header[10]+(header[11]<<8);

	/* Instruments */
	xm->number_of_instruments=header[12]+(header[13]<<8);

	/* Flags */
	xm->flags=header[14]+(header[15]<<8);

	/* Tempo */
	xm->default_tempo=header[16]+(header[17]<<8);

	/* BPM */
	xm->default_bpm=header[18]+(header[19]<<8);

	/* Pattern Order */
	for(i=0;i<xm->song_length;i++) {
		xm->pattern_order[i]=header[20+i];
	}

	/* Load patterns */



	for(i=0;i < xm->number_of_patterns;i++) {

		result=read(fd,pattern_header,9);
		if (result!=9) {
			fprintf(stderr,"Error reading pattern header %d!\n",i);
			return -1;
		}

		pattern_header_length=pattern_header[0]+
					(pattern_header[1]<<8)+
					(pattern_header[2]<<16)+
					(pattern_header[3]<<24);
		//printf("\tPattern Header Length: %d\n",pattern_header_length);

		if (pattern_header_length!=9) {
			fprintf(stderr,"Error invalid pattern header size %d!\n",i);
			return -1;
		}

		xm->pattern[i].packing_type=pattern_header[4];
		if (xm->pattern[i].packing_type!=0) {
			fprintf(stderr,"Error! packing type not zero!\n");
			return -1;
		}

		xm->pattern[i].num_rows=
			pattern_header[5]+(pattern_header[6]<<8);

		packed_size=pattern_header[7]+(pattern_header[8]<<8);
		//printf("\tPacked Pattern Size: %d\n",packed_size);

		result=read(fd,packed_pattern,packed_size);
		if (result!=packed_size) {
			fprintf(stderr,"Error packed_data %d!\n",i);
			exit(-1);
		}

		j=0;
		line=0;
		int ch;
		int channel=0;
		int pattern_break=0;

		while(1) {
			if (channel==0) {
//				printf("%2X ",line);
				pattern_break=0;
			}

			xm->pattern[i].p[line][channel].note=0;
			xm->pattern[i].p[line][channel].instrument=0;
			xm->pattern[i].p[line][channel].volume=0;
			xm->pattern[i].p[line][channel].effect=0;
			xm->pattern[i].p[line][channel].param=0;

			ch=packed_pattern[j];

			if (ch&0x80) {
				j++;
				if (ch&0x1) {
					xm->pattern[i].p[line][channel].note=
						packed_pattern[j];
					j++;
				}
				if (ch&0x2) {
					xm->pattern[i].p[line][channel].instrument=
						packed_pattern[j];
					j++;
				}
				if (ch&0x4) {
					xm->pattern[i].p[line][channel].volume=
						packed_pattern[j];
					j++;
				}
				if (ch&0x8) {
					xm->pattern[i].p[line][channel].effect=
						packed_pattern[j];
					j++;
				}
				if (ch&0x10) {
					xm->pattern[i].p[line][channel].param=
						packed_pattern[j];
					j++;
				}

				if (xm->pattern[i].p[line][channel].effect==0xd)
					pattern_break=1;

				channel++;
				if (channel>=xm->number_of_channels) {
//					printf("\n");
					channel=0;
					line++;
					if (pattern_break) break;
				}
			}
			else {
				printf("\n?%x?\n",packed_pattern[j]);
				j++;
			}

			if (j>=packed_size) break;
		}
//		printf("\n");

	}

	free(header);

	close(fd);

	return 0;

}


int main(int argc, char **argv) {

	struct xm_info_struct xm;
	int result;
	char *filename;

	if (argc>1) {
		filename=argv[1];
	}
	else {
		filename=strdup("stillalive.xm");
	}

	result=load_xm_file(filename,&xm);

	if (result<0) {

	}
	else {
		dump_xm_file(&xm);
	}

	return 0;
}
