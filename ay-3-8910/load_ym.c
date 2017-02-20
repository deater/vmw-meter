#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/stat.h>
//#include <sys/time.h>
//#include <math.h>
//#include <sys/resource.h>

//#include <bcm2835.h>

//#include "ay-3-8910.h"
//#include "display.h"

#include "load_ym.h"

#ifdef USE_LIBLHASA
#include "lhasa.h"
#endif

#define YM4_HEADER_SIZE	26
#define YM5_HEADER_SIZE	34
#define YM5_FRAME_SIZE	16
#define YM3_FRAME_SIZE	16


//#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int dump_info=0;

static unsigned char *load_uncompressed_song(char *filename, int *read_size) {

	unsigned char *data=NULL;
	struct stat file_stats;
	int file_size=0,fd;
	int result;

	stat(filename,&file_stats);

	file_size=file_stats.st_size;
	data=calloc(file_size,sizeof(unsigned char));
	if (data==NULL) {
		fprintf(stderr,"Error allocating %d bytes\n",file_size);
		return NULL;
	}

	fd=open(filename,O_RDONLY);
	if (fd<0) {
		fprintf(stderr,"Error opening %s\n",filename);
		return NULL;
	}

	result=read(fd,data,file_size);
	close(fd);

	if (result<file_size) {
		fprintf(stderr,"Error reading %d bytes\n",file_size);
		return NULL;
	}

	*read_size=file_size;

	return data;
}


static unsigned char *load_lha_compressed_song(char *filename, int *file_size) {

	unsigned char *data=NULL;

	size_t bytes;
	FILE *fff;
	LHAInputStream *stream;
	LHAReader *reader;
	LHAFileHeader *header;

#ifndef USE_LIBLHASA
	fprintf(stderr,"Probably a LHC compressed ym, "
			"decompress with lhasa before playing.\n");
	return data;
#endif

	fff=fopen(filename,"rb");
	if (fff==NULL) {
		fprintf(stderr,"Error opening %s!\n",filename);
		return NULL;
	}

	stream = lha_input_stream_from_FILE(fff);
	reader = lha_reader_new(stream);

	header = lha_reader_next_file(reader);

	printf("Decompressing %s, size %zu\n",header->filename,header->length);

	data=calloc(header->length,sizeof(unsigned char));
	if (data==NULL) {
		fprintf(stderr,"Error allocating %zu bytes\n",header->length);
		return NULL;
	}

	bytes = lha_reader_read(reader, data, header->length);
	if (bytes != header->length) {
		fprintf(stderr,"Error reading data!\n");
		free(data);
		return NULL;
	}

	*file_size=header->length;

	return data;
}

int load_ym_song(
	char *filename,
	struct ym_song_t *ym_song ) {

	int fd,result,compressed=0;
	unsigned char *data=NULL;
	unsigned char header[YM5_HEADER_SIZE];

	char ym2_magic[]="YM2!";
	char ym3_magic[]="YM3!";
	char ym3b_magic[]="YM3b";
	char ym4_magic[]="YM4!LeOnArD!";
	char ym5_magic[]="YM5!LeOnArD!";
	char ym6_magic[]="YM6!LeOnArD!";

	int i,pointer=0;
	long int file_offset=0;


	/* Open the file */
	fd=open(filename,O_RDONLY);
	if (fd<1) {
		fprintf(stderr,"Error opening %s, %s!\n",
			filename,strerror(errno));
		return -1;
	}

	/* read the header */
	result=read(fd,header,YM5_HEADER_SIZE);
	if (result<YM5_HEADER_SIZE) {
		fprintf(stderr,"Error reading header!\n");
		return -1;
	}

	/* close the file */
	close(fd);

	/* Handle a LHA compressed file */
	if ((header[3]=='l') && (header[4]=='h')) {
		/* LHA compressed */
		data=load_lha_compressed_song(filename,&ym_song->file_size);
		memcpy(header,data,YM5_HEADER_SIZE);
		compressed=1;
	}

	/* Load the file otherwise */
	if (!memcmp(header,ym2_magic,4)) {
		/* YM2 file */
		if (!compressed) {
			data=load_uncompressed_song(filename,&ym_song->file_size);
		}
		ym_song->type=2;
	} else
	if (!memcmp(header,ym3_magic,4)) {
		/* YM3 file */
		if (!compressed) {
			data=load_uncompressed_song(filename,&ym_song->file_size);
		}
		ym_song->type=3;
	} else
	if (!memcmp(header,ym3b_magic,4)) {
		/* YM3b file */
		if (!compressed) {
			data=load_uncompressed_song(filename,&ym_song->file_size);
		}
		ym_song->type=3;
	} else
	if (!memcmp(header,ym4_magic,12)) {
		/* YM4 file */
		if (!compressed) {
			data=load_uncompressed_song(filename,&ym_song->file_size);
		}
		ym_song->type=4;
	} else
	if (!memcmp(header,ym5_magic,12)) {
		/* YM5 file */
		if (!compressed) {
			data=load_uncompressed_song(filename,&ym_song->file_size);
		}
		ym_song->type=5;
	}
	else if (!memcmp(header,ym6_magic,12)) {
		/* YM6 file */
		if (!compressed) {
			data=load_uncompressed_song(filename,&ym_song->file_size);
		}
		ym_song->type=6;
	} else {
		fprintf(stderr,"Error, not a known YM file!\n");
		return -1;
	}

	ym_song->file_data=data;

	/*****************/
	/* Decode header */
	/*****************/

	if (ym_song->type>3) {
		/* version 4, 5, 6 */

		ym_song->num_frames=(data[12]<<24)|(data[13]<<16)|
			(data[14]<<8)|(data[15]);

		ym_song->attributes=(data[16]<<24)|(data[17]<<16)|
			(data[18]<<8)|(data[19]);
		ym_song->interleaved=(ym_song->attributes)&0x1;

		/* interleaved makes things compress better */
		/* but much more of a pain to play */

		ym_song->num_digidrum=(data[20]<<8)|(data[21]);

		if (ym_song->type==4) {

			/* assume atari */
			ym_song->master_clock=2000000;
			ym_song->frame_rate=50;
			ym_song->extra_data=0;

			ym_song->loop_frame=(data[22]<<24)|(data[23]<<16)|
				(data[24]<<8)|(data[25]);

			file_offset=YM4_HEADER_SIZE;
		}
		else {

			ym_song->master_clock=(data[22]<<24)|(data[23]<<16)|
				(data[24]<<8)|(data[25]);

			ym_song->frame_rate=(data[26]<<8)|(data[27]);

			ym_song->loop_frame=(data[28]<<24)|(data[29]<<16)|
				(data[30]<<8)|(data[31]);

			ym_song->extra_data=(data[32]<<8)|(data[33]);

			file_offset=YM5_HEADER_SIZE;
		}

		/* Skip digidrums (we can't play those) */

		if (ym_song->num_digidrum>0) {
			fprintf(stderr,"Warning!  We don't handle digidrum\n");
			fprintf(stderr,"\tskipping %d digidrums\n",ym_song->num_digidrum);
			for(i=0;i<ym_song->num_digidrum;i++) {
				ym_song->drum_size=
					(data[file_offset]<<24)|
					(data[file_offset+1]<<16)|
					(data[file_offset+2]<<8)|
					(data[file_offset+3]);
				file_offset+=4;
				printf("\tDrum%d: %d bytes\n",i,ym_song->drum_size);
				file_offset+=ym_song->drum_size;
			}
		}

		/* Get the song name */

		pointer=0;
		while(1) {
			if (!data[file_offset]) {
				ym_song->song_name[pointer]=0;
				break;
			}
			if (pointer<MAX_YM_STRING) {
				ym_song->song_name[pointer]=data[file_offset];
				pointer++;
			}
			file_offset++;
		}

		/* Get the author name */

		pointer=0;
		file_offset++;
		while(1) {
			if (!data[file_offset]) {
				ym_song->author[pointer]=0;
				break;
			}
			if (pointer<MAX_YM_STRING) {
				ym_song->author[pointer]=data[file_offset];
				pointer++;
			}
			file_offset++;
		}

		/* Get the comment */
		pointer=0;
		file_offset++;
		while(1) {
			if (!data[file_offset]) {
				ym_song->comment[pointer]=0;
				break;
			}
			if (pointer<MAX_YM_STRING) {
				ym_song->comment[pointer]=data[file_offset];
				pointer++;
			}
			file_offset++;
		}

		file_offset++;
		ym_song->frame_size=YM5_FRAME_SIZE;

	}
	else {
		/* version 2, 3, 3b */
		file_offset=4;
		ym_song->frame_rate=50;
		/* Assuming Atari */
		ym_song->master_clock=2000000;
		ym_song->interleaved=1;
		ym_song->attributes=0x1;
		ym_song->num_frames=(ym_song->file_size-4)/14;
		ym_song->frame_size=YM3_FRAME_SIZE;
		ym_song->extra_data=0;
		ym_song->num_digidrum=0;
		strcpy(ym_song->song_name,"UNKNOWN");
		strcpy(ym_song->author,"UNKNOWN");
		strcpy(ym_song->comment,"UNKNOWN");

		//if (ym_type=3)
		// check if filesize is 4 too many
		// in that case, type 3b and loop_frame is last 4 bytes
		// really we probably don't care
		ym_song->loop_frame=0;

	}

	if (dump_info) printf("Frames start at %lx\n",file_offset);

	ym_song->file_data=data;
	ym_song->data_begin=file_offset;

	return 0;
}
