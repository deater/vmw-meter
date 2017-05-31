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



int main(int argc, char **argv) {

	char *filename;
	int fd,result;
	unsigned char first_header[FIRST_HEADER_LENGTH];
	char module_name[20];
	char tracker_name[20];
	int version_high,version_low;
	int header_length;

	unsigned char *header=NULL;
	int song_length;
	int restart_position;
	int number_of_channels;
	int number_of_patterns;
	int number_of_instruments;
	int flags;
	int default_tempo;
	int default_bpm;
	int i,j;

	unsigned char pattern_header[2048];
	unsigned char packed_pattern[2048];

	int pattern_header_length;
	int packing_type;
	int num_rows;
	int packed_size;



	filename=strdup("stillalive.xm");

	fd=open(filename,O_RDONLY);
	if (fd<0) {
		fprintf(stderr,"Error opening %s!\n",filename);
		exit(-1);
	}

	/*********************/
	/* Read first header */
	/*********************/

	result=read(fd,first_header,FIRST_HEADER_LENGTH);
	if (result!=FIRST_HEADER_LENGTH) {
		fprintf(stderr,"Error reading first header!\n");
		exit(-1);
	}

	/* validate */
	if (memcmp(first_header,"Extended Module: ",17)) {
		fprintf(stderr,"ID text does not match!\n");
		exit(-1);
	}
	memcpy(module_name,first_header+17,20);
	printf("NAME: %s\n",module_name);

	if (first_header[37]!=0x1a) {
		fprintf(stderr,"Random char doesn't match!\n");
		exit(-1);
	}

	memcpy(tracker_name,first_header+38,20);
	printf("TRACKER: %s\n",tracker_name);

	/* Little Endian??? */
	version_low=first_header[58];
	version_high=first_header[59];
	printf("VERSION %d.%d\n",version_high,version_low);

	/* Little endian???? */
	header_length=(first_header[63]<<24)+(first_header[62]<<16)+
			(first_header[61]<<8)+first_header[60];
	printf("HEADER LEN: %d\n",header_length);

	/****************************/
	/* Load actual header       */
	/****************************/

	header=calloc(header_length,sizeof(char));
	if (header==NULL) {
		fprintf(stderr,"Error allocating header!\n");
		exit(-1);
	}

	result=read(fd,header+4,header_length-4);
	if (result!=(header_length-4)) {
		fprintf(stderr,"Error reading header!\n");
		exit(-1);
	}

	song_length=header[4]+(header[5]<<8);
	printf("SONG LEN: %d patterns\n",song_length);

	restart_position=header[6]+(header[7]<<8);
	printf("LOOP: %d\n",restart_position);

	number_of_channels=header[8]+(header[9]<<8);
	printf("CHANNELS: %d\n",number_of_channels);

	number_of_patterns=header[10]+(header[11]<<8);
	printf("PATTERNS: %d\n",number_of_patterns);

	number_of_instruments=header[12]+(header[13]<<8);
	printf("INSTRUMENTS: %d\n",number_of_instruments);

	flags=header[14]+(header[15]<<8);
	printf("FLAGS: %x (%s)\n",flags,flags==0?"Amiga Freq":"Linear Freq");

	default_tempo=header[16]+(header[17]<<8);
	printf("TEMPO: %d\n",default_tempo);

	default_bpm=header[18]+(header[19]<<8);
	printf("BPM: %d\n",default_bpm);

	printf("Pattern order:\n");
	for(i=0;i<song_length;i++) {
		printf("%x ",header[20+i]);
	}
	printf("\n");

	/* Load patterns */

	int line;

	int note,instrument,volume,effect,param;


	for(i=0;i<number_of_patterns;i++) {
		printf("Pattern %x\n",i);

		result=read(fd,pattern_header,9);
		if (result!=9) {
			fprintf(stderr,"Error reading pattern header %d!\n",i);
			exit(-1);
		}

		pattern_header_length=pattern_header[0]+
					(pattern_header[1]<<8)+
					(pattern_header[2]<<16)+
					(pattern_header[3]<<24);
		printf("\tPattern Header Length: %d\n",pattern_header_length);

		if (pattern_header_length!=9) {
			fprintf(stderr,"Error invalid pattern header size %d!\n",i);
			exit(-1);
		}

		packing_type=pattern_header[4];
		if (packing_type!=0) {
			fprintf(stderr,"Error! packing type not zero!\n");
			exit(-1);
		}

		num_rows=pattern_header[5]+(pattern_header[6]<<8);
		printf("\tNum_Rows: %d\n",num_rows);

		packed_size=pattern_header[7]+(pattern_header[8]<<8);
		printf("\tPacked Pattern Size: %d\n",packed_size);

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
				printf("%2X ",line);
				pattern_break=0;
			}

			note=0;
			instrument=0;
			volume=0;
			effect=0;
			param=0;

			ch=packed_pattern[j];

			if (ch&0x80) {
				//printf("!%x!",ch);
				j++;
				if (ch&0x1) {
					note=packed_pattern[j];
					j++;
				}
				if (ch&0x2) {
					instrument=packed_pattern[j];
					j++;
				}
				if (ch&0x4) {
					volume=packed_pattern[j];
					j++;
				}
				if (ch&0x8) {
					effect=packed_pattern[j];
					j++;
				}
				if (ch&0x10) {
					param=packed_pattern[j];
					j++;
				}

				note_to_string(note);
				printf(".");
				if (instrument) printf("%X",instrument);
				else printf(".");
				if (volume) printf("%02X",volume-0x10);
				else printf("..");
				if ((effect) || (param)) printf("%X",effect);
				else printf(".");
				if ((effect)||(param)) printf("%02X",param);
				else printf("..");
				printf(" ");

				if (effect==0xd) pattern_break=1;

				channel++;
				if (channel>=number_of_channels) {
					printf("\n");
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
		printf("\n");

	}

	free(header);

	close(fd);

	return 0;

}
