#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/stat.h>

#include "ym_lib.h"

#ifdef USE_LIBLHASA
#include "lhasa.h"
#endif

#define AY38910_CLOCK 1000000 /* 1MHz on our board */

#include "ay-3-8910.h"


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
	ym_song->frame_data=(data+file_offset);

	return 0;
}

int ym_play_frame(struct ym_song_t *ym_song, int frame_num, int shift_size,
			struct frame_stats *ds,
			int diff_mode,
			int play_music) {

	int j;

	unsigned char frame[YM5_FRAME_SIZE];
	unsigned char last_frame[YM5_FRAME_SIZE];

	int a_period,b_period,c_period,n_period,e_period;
	double a_freq=0.0, b_freq=0.0, c_freq=0.0,n_freq=0.0,e_freq=0.0;
	int new_a,new_b,new_c,new_n,new_e;

	if (ym_song->interleaved) {
		for(j=0;j<ym_song->frame_size;j++) {
			frame[j]=ym_song->frame_data[frame_num+
							j*ym_song->num_frames];
		}
	}
	else {
		memcpy(frame,
			&ym_song->frame_data[frame_num*ym_song->frame_size],
			ym_song->frame_size);
	}

	/****************************************/
	/* Write out the music			*/
	/****************************************/


	a_period=((frame[1]&0xf)<<8)|frame[0];
	b_period=((frame[3]&0xf)<<8)|frame[2];
	c_period=((frame[5]&0xf)<<8)|frame[4];
	n_period=frame[6]&0x1f;
	e_period=((frame[12]&0xff)<<8)|frame[11];

	if (a_period>0) a_freq=ym_song->master_clock/(16.0*(double)a_period);
	if (b_period>0) b_freq=ym_song->master_clock/(16.0*(double)b_period);
	if (c_period>0) c_freq=ym_song->master_clock/(16.0*(double)c_period);
	if (n_period>0) n_freq=ym_song->master_clock/(16.0*(double)n_period);
	if (e_period>0) e_freq=ym_song->master_clock/(256.0*(double)e_period);

	if (dump_info) {
		printf("%05d:\tA:%04x B:%04x C:%04x N:%02x M:%02x ",
			frame_num,
			a_period,b_period,c_period,n_period,frame[7]);

		printf("AA:%02x AB:%02x AC:%02x E:%04x,%02x %04x\n",
			frame[8],frame[9],frame[10],
			(frame[12]<<8)+frame[11],frame[13],
			(frame[14]<<8)+frame[15]);

		printf("\t%.1lf %.1lf %.1lf %.1lf %.1lf ",
			a_freq,b_freq,c_freq,n_freq, e_freq);
		printf("N:%c%c%c T:%c%c%c ",
			(frame[7]&0x20)?' ':'C',
			(frame[7]&0x10)?' ':'B',
			(frame[7]&0x08)?' ':'A',
			(frame[7]&0x04)?' ':'C',
			(frame[7]&0x02)?' ':'B',
			(frame[7]&0x01)?' ':'A');

		if (frame[8]&0x10) printf("VA: E ");
		else printf("VA: %d ",frame[8]&0xf);
		if (frame[9]&0x10) printf("VB: E ");
		else printf("VB: %d ",frame[9]&0xf);
		if (frame[10]&0x10) printf("VC: E ");
		else printf("VC: %d ",frame[10]&0xf);

		if (frame[13]==0xff) {
			printf("NOWRITE");
		}
		else {
			if (frame[13]&0x1) printf("Hold");
			if (frame[13]&0x2) printf("Alternate");
			if (frame[13]&0x4) printf("Attack");
			if (frame[13]&0x8) printf("Continue");
		}
		printf("\n");

//		if (a_freq>max_a) max_a=a_freq;
//		if (b_freq>max_b) max_b=b_freq;
//		if (c_freq>max_c) max_c=c_freq;
	}

	if (diff_mode) {
		for(j=0;j<YM5_FRAME_SIZE;j++) {
			if (frame[j]!=last_frame[j]) {
				printf("%d: %d\n",frame_num,j);
			}
		}

		memcpy(last_frame,frame,sizeof(frame));
	}

	/* Scale if needed */
	if (ym_song->master_clock!=AY38910_CLOCK) {

		if (a_period==0) new_a=0;
		else new_a=(double)AY38910_CLOCK/(16.0*a_freq);
		if (b_period==0) new_b=0;
		else new_b=(double)AY38910_CLOCK/(16.0*b_freq);
		if (c_period==0) new_c=0;
		else new_c=(double)AY38910_CLOCK/(16.0*c_freq);
		if (n_period==0) new_n=0;
		else new_n=(double)AY38910_CLOCK/(16.0*n_freq);
		if (e_period==0) new_e=0;
		else new_e=(double)AY38910_CLOCK/(256.0*e_freq);

		if (new_a>0xfff) {
			printf("A TOO BIG %x\n",new_a);
		}
		if (new_b>0xfff) {
			printf("B TOO BIG %x\n",new_b);
		}
		if (new_c>0xfff) {
			printf("C TOO BIG %x\n",new_c);
		}
		if (new_n>0x1f) {
			printf("N TOO BIG %x\n",new_n);
		}
		if (new_e>0xffff) {
			printf("E too BIG %x\n",new_e);
		}

		frame[0]=new_a&0xff;	frame[1]=(new_a>>8)&0xf;
		frame[2]=new_b&0xff;	frame[3]=(new_b>>8)&0xf;
		frame[4]=new_c&0xff;	frame[5]=(new_c>>8)&0xf;
		frame[6]=new_n&0x1f;
		frame[11]=new_e&0xff;	frame[12]=(new_e>>8)&0xff;

		if (dump_info) {
			printf("\t%04x %04x %04x %04x %04x\n",
				new_a,new_b,new_c,new_n,new_e);
		}

	}

	if (play_music) {
		for(j=0;j<13;j++) {
			write_ay_3_8910(j,frame[j],shift_size);
		}

		/* Special case.  Writing r13 resets it,	*/
		/* so special 0xff marker means do not write	*/
		if (frame[13]!=0xff) {
			write_ay_3_8910(13,frame[13],shift_size);
		}
	}

	ds->a_bar=(frame[8]*11)/16;
	ds->b_bar=(frame[9]*11)/16;
	ds->c_bar=(frame[10]*11)/16;
	ds->a_freq=(a_freq)/150;
	ds->b_freq=(b_freq)/150;
	ds->c_freq=(c_freq)/150;

	return 0;

}

