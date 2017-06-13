#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <math.h>

#include "xm_lib.h"

/* ftp://ftp.modland.com/pub/documents/format_documentation/FastTracker%202%20v2.04%20(.xm).html */
/* http://lclevy.free.fr/mo3/mod.txt */

#define FIRST_HEADER_LENGTH	64



/* 1 = C0 ? */
/* C,C#,D,D#,E,F,F#,G,G#,A,A#,B */
static char letters[12]={'C','C','D','D','E','F','F','G','G','A','A','B'};
static int sharps[12]={   0 , 1 , 0 , 1 , 0 , 0 , 1 , 0 , 1 , 0 , 1 , 0};


static void note_to_ym_string(FILE *fff,int note,int shiftup) {

	/* 1 = C0 ? */
	/* C,C#,D,D#,E,F,F#,G,G#,A,A#,B */

	int letter,sharp,octave;

	if (note==0) fprintf(fff,"---");
	else if (note==97) fprintf(fff,"---");
//	else if (note>97) fprintf(fff,"???");
	else {
		if (shiftup) note+=24;
		letter=letters[(note-1)%12];
		octave=(note-1)/12;
		sharp=sharps[(note-1)%12];
		fprintf(fff,"%c%c%X",letter,sharp?'#':' ',octave);
	}
}


static char channel_to_channel(int c,int ch0,int ch1,int ch2) {

	if (c==ch0) return 'A';
	if (c==ch1) return 'B';
	if (c==ch2) return 'C';

	return '?';
}

	/* Note, ay-3-8910 volume is not linear */
static int convert_volume(int v) {

	double dv,nv;

	if (v==0) return 0;
	if (v==0x40) return 15;

	dv=v;

	nv= 15.0 - ( (log(64.0/dv)) / (log(sqrt(2))) );

			//http://www.cpcwiki.eu/index.php/PSG
			//amplitude = max / sqrt(2)^(15-nn)
			// amiga = 40 / sqrt(2)^(15-nn)
			// amiga/40 = 1 / sqrt(2)^(15-nn)
			// (amiga/40)(sqrt(2)^(15-nn)) = 1
			// (sqrt(2)^(15-nn) = 40/amiga
			// (15-nn) = log(sqrt(2)) (40/amiga)
			// -nn = log(sqrt(2))(40/amiga) - 15
			// nn = 15 - log(sqrt(2))(40/amiga)
			//
			// 40 = 15
			// 30 = 14
			// 20 = 13
			// 10 = 11
			//  1 = 4
			// 0
	return (int)nv;
}

static int dump_pattern(FILE *fff, int which, struct pattern_struct *p,
	int num_channels,
	int ch0, int ch1, int ch2) {

	int j,c;
	int pattern_break=0;
	int last_instrument[16]={0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, };

	fprintf(fff,"\' Pattern %x\n",which);

	for(j=0; j < p->num_rows;j++) {

		/* Handle effects */
		for(c=0;c<num_channels;c++) {
			int effect;
			int param;

			/* Check early in case we aren't outputting the */
			/* channel with the break */
			/* Technically some effects should go in effect */
			/* globally so we should handle this better */

			if (p->p[j][c].effect==0xd) pattern_break=1;

// 0,1,9
// 0N 1N 2C 3C 

			if ((c!=ch0) && (c!=ch1) && (c!=ch2)) continue;

			effect=p->p[j][c].effect;
			param=p->p[j][c].param;

			switch(effect) {
				case 0:	/* arpeggio */
					if (param==0) break;
					fprintf(fff,"* %c E %d %d\n",
						channel_to_channel(c,ch0,ch1,ch2),
						effect,param);
					break;
				case 1: /* portamento up */
					fprintf(fff,"* %c E %d %d\n",
						channel_to_channel(c,ch0,ch1,ch2),
						effect,param);
					break;
				case 2:	/* portamento down */
					fprintf(fff,"* %c E %d %d\n",
						channel_to_channel(c,ch0,ch1,ch2),
						effect,param);
					break;
				case 4: /* Vibrato */
					fprintf(fff,"* %c E %d %d\n",
						channel_to_channel(c,ch0,ch1,ch2),
						effect,param);
					break;
				case 0xd:	/* early exit */
					pattern_break=1;
					break;
				case 0xf:	/* Change temp */
					/* FIXME: warn if changing */
					break;
				case 3:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 0xa:
				case 0xb:
				case 0xc:
				case 0xe:
				default:
					fprintf(stderr,"Unhandled effect %x\n",
						effect);
			}

		}



		/* Handle instrument */

		for(c=0;c<num_channels;c++) {
			int instrument=0;

			if ((c!=ch0) && (c!=ch1) && (c!=ch2)) continue;

			instrument=p->p[j][c].instrument;

			/* Hardcoded match for stillalive */
			if ((instrument) && (instrument!=last_instrument[c])) {

				last_instrument[c]=instrument;

					/* 1 = sine */
				if (instrument==1) instrument=0;
					/* 2 = muted */
				else if (instrument==2) instrument=6;
					/* 3 = triangle wave */
				else if (instrument==3) instrument=10;
					/* 4 = bass drum */
				else if (instrument==4) instrument=7;
					/* 5 = snare drum */
				else if (instrument==5) instrument=8;
					/* 6 = cymbal */
				else if (instrument==6) instrument=9;

				fprintf(fff,"* %c I %d\n",
					channel_to_channel(c,ch0,ch1,ch2),
					instrument);

			}

		}

		/* Handle volume */

		for(c=0;c<num_channels;c++) {
			int volume=0;

			if ((c!=ch0) && (c!=ch1) && (c!=ch2)) continue;

			if (p->p[j][c].note) {
				if (p->p[j][c].note==97) volume=0x10;
				else volume=0x50;
			}

			if (p->p[j][c].volume) volume=p->p[j][c].volume;


			if (volume) {
				if (volume>0x50) {
					fprintf(stderr,"Unhandled volume %d\n",
						volume);
					continue;
				}


				//volume=(volume-0x11)/4;
				volume=convert_volume(volume-0x10);

				fprintf(fff,"* %c L %d\n",
					channel_to_channel(c,ch0,ch1,ch2),
					volume);
			}

		}

		fprintf(fff,"%02X ",j);

		/* Handle Notes */
		for(c=0;c<num_channels;c++) {

			if ((c!=ch0) && (c!=ch1) && (c!=ch2)) continue;

			note_to_ym_string(fff,p->p[j][c].note,c==0);

			if ((p->p[j][c].note<97) &&
				(p->p[j][c].note!=0)) {
				fprintf(fff," ?\t");
			}
			else {
				fprintf(fff,"--\t");
			}
#if 0
			if (p->p[j][c].instrument) {
				fprintf(fff,"%X",p->p[j][c].instrument);
			}
			else {
				fprintf(fff,".");
			}

			if (p->p[j][c].volume) {
				fprintf(fff,"%02X",p->p[j][c].volume-0x10);
			}
			else {
				fprintf(fff,"..");
			}

			if ((p->p[j][c].effect) ||
				(p->p[j][c].param)) {
					fprintf(fff,"%X%02X",
						p->p[j][c].effect,
						p->p[j][c].param);
			}
			else {
				fprintf(fff,"...");
			}

			fprintf(fff," ");
#endif


		}

		fprintf(fff,"\n");
		if (pattern_break) break;
	}

	return 0;
}


int xm_to_text(FILE *fff,struct xm_info_struct *xm,
		int which1, int which2, int which3) {

	int i;

	fprintf(fff,"\'\n");
	fprintf(fff,"\' TITLE:\t%s\n",xm->module_name);
	fprintf(fff,"\'\n");
	fprintf(fff,"\' LOOP:\t\t%d\n",xm->restart_position);
	fprintf(fff,"\' BPM:\t\t%d\n",xm->default_bpm);
	fprintf(fff,"\' TEMPO:\t%d\n",xm->default_tempo);
	fprintf(fff,"\' FREQ:\t\t1000000\n");
	fprintf(fff,"\' IRQ:\t\t50\n");
	fprintf(fff,"\'\n");
	fprintf(fff,"\' ENDHEADER\n");
	fprintf(fff,"\'\n");

	for(i=0;i < xm->song_length;i++) {
		dump_pattern( fff, xm->pattern_order[i],
				&(xm->pattern[xm->pattern_order[i]]),
				xm->number_of_channels,
				which1,which2,which3);
	}

	return 0;
}

/**************************************/
/* DUMP FILE                          */
/**************************************/

static void note_to_string(int note) {


	int letter,sharp,octave;

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
			for(c=0;c < xm->number_of_channels;c++) {

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



/****************************************/
/* LOAD FILE				*/
/****************************************/

#define MAX_PACKED_PATTERN	8192

int load_xm_file(char *filename, struct xm_info_struct *xm) {

	int fd,result;
	unsigned char first_header[FIRST_HEADER_LENGTH];
	int header_length;

	unsigned char *header=NULL;
	int i,j;

	unsigned char pattern_header[2048];
	unsigned char packed_pattern[MAX_PACKED_PATTERN];

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

	if (xm->number_of_channels>NUM_CHANNELS) {
		fprintf(stderr,"Error!  Too many channels %d\n",
			xm->number_of_channels);
		return -1;
	}

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

		printf("Reading pattern header %d file offset %lx\n",i,
			lseek(fd,0,SEEK_CUR));

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
//		printf("\tPacked Pattern Size: %d\n",packed_size);

		if (packed_size>MAX_PACKED_PATTERN) {
			fprintf(stderr,"Error! pattern size %d too big!\n",
				packed_size);
			return -1;
		}

		printf("Reading pattern %d file offset %lx\n",i,
			lseek(fd,0,SEEK_CUR));

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
			}

			else {
				printf("Unpacked: Line %d Val %x\n",
					line,packed_pattern[j]);
				xm->pattern[i].p[line][channel].note=
						packed_pattern[j];
				xm->pattern[i].p[line][channel].instrument=
						packed_pattern[j+1];
				xm->pattern[i].p[line][channel].volume=
						packed_pattern[j+2];
				xm->pattern[i].p[line][channel].effect=
						packed_pattern[j+3];
				xm->pattern[i].p[line][channel].param=
						packed_pattern[j+4];

				j+=5;
			}

			channel++;
			if (channel>=xm->number_of_channels) {
				channel=0;
				line++;
				if (pattern_break) break;
			}


			if (j>=packed_size) break;
		}

//		printf("\n");

	}

	free(header);

	close(fd);

	return 0;
}
