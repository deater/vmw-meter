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

	ym_song->channels=3;

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

struct music_frame {
	int a,b,c,n,e,m;
	int aa,ab,ac;
} last_music,current_music;


void print_diff(int frame_num) {

	printf("%4d: ",frame_num);

	if (current_music.a!=last_music.a) {
		printf("%4d ",last_music.a);
	}
	else {
		printf("---- ");
	}

	if (current_music.b!=last_music.b) {
		printf("%4d ",last_music.b);
	}
	else {
		printf("---- ");
	}

	if (current_music.c!=last_music.c) {
		printf("%4d ",last_music.c);
	}
	else {
		printf("---- ");
	}

	if (current_music.n!=last_music.n) {
		printf("%3d ",last_music.n);
	}
	else {
		printf("--- ");
	}

	printf("N:");
	if (last_music.m&0x20) printf("-"); else printf("C");
	if (last_music.m&0x10) printf("-"); else printf("B");
	if (last_music.m&0x08) printf("-"); else printf("A");
	printf(" T:");
	if (last_music.m&0x04) printf("-"); else printf("C");
	if (last_music.m&0x02) printf("-"); else printf("B");
	if (last_music.m&0x01) printf("-"); else printf("A");
	printf(" ");

	if (current_music.aa!=last_music.aa) {
		printf("A%2d ",last_music.aa);
	}
	else {
		printf("--- ");
	}
	if (current_music.ab!=last_music.ab) {
		printf("B%2d ",last_music.ab);
	}
	else {
		printf("--- ");
	}
	if (current_music.ac!=last_music.ac) {
		printf("C%2d ",last_music.ac);
	}
	else {
		printf("--- ");
	}


	if (current_music.e!=last_music.e) {
		printf("%4d ",last_music.e);
	}
	else {
		printf("---- ");
	}

	printf("\n");

	return;
}

static int ym_make_frame(struct ym_song_t *ym_song,
			int frame_num,
			unsigned char *frame) {

	int j;

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
	}

	return 0;

}

int ym_play_frame(struct ym_song_t *ym_song, int frame_num, int shift_size,
			struct frame_stats *ds,
			int diff_mode,
			int play_music,
			int mute_channel) {

	int j;

	unsigned char frame[YM5_FRAME_SIZE];
	unsigned char frame2[YM5_FRAME_SIZE];


	double a_freq=0.0, b_freq=0.0, c_freq=0.0;

	ym_make_frame(ym_song,frame_num,frame);

	if (ym_song->channels==3) {
		memcpy(frame2,frame,sizeof(frame));
	} else {
		ym_make_frame(ym_song,frame_num,frame2);
	}

	if (mute_channel&0x1) frame[8]=0;
	if (mute_channel&0x2) frame[9]=0;
	if (mute_channel&0x4) frame[10]=0;
	if (mute_channel&0x8) frame[7]|=0x8;
	if (mute_channel&0x10) frame[7]|=0x10;
	if (mute_channel&0x20) frame[7]|=0x20;

	if (play_music) {
		for(j=0;j<13;j++) {
			write_ay_3_8910(j,frame[j],frame2[j],shift_size);
		}

		/* Special case.  Writing r13 resets it,	*/
		/* so special 0xff marker means do not write	*/

		/* FIXME: so what do we do if 2 channels have */
		/* different values? */
		/* We'll have to special case, and do a dummy write */
		/* to a non-13 address.  Should be possible but not */
		/* worth fixing unless it actually becomes a problem. */


		if ((frame[13]!=0xff) || (frame2[13]!=0xff)) {
			write_ay_3_8910(13,frame[13],frame2[13],shift_size);
		}
	}

	if (ds!=NULL) {
		ds->left_a_bar=(frame[8]*11)/16;
		ds->left_b_bar=(frame[9]*11)/16;
		ds->left_c_bar=(frame[10]*11)/16;
		ds->left_a_freq=(a_freq)/150;
		ds->left_b_freq=(b_freq)/150;
		ds->left_c_freq=(c_freq)/150;

		ds->right_a_bar=(frame[8]*11)/16;
		ds->right_b_bar=(frame[9]*11)/16;
		ds->right_c_bar=(frame[10]*11)/16;
		ds->right_a_freq=(a_freq)/150;
		ds->right_b_freq=(b_freq)/150;
		ds->right_c_freq=(c_freq)/150;
	}

	return 0;

}

static struct note_mapping_type {
	double	actual;
	double	low;
	double	high;
	char	name[4];
} note_mapping[]={
	{	16.4,	16.1,	16.7,	"C 0"},
	{	17.3,	17.0,	17.6,	"C#0"},
	{	18.4,	18.1,	18.7,	"D 0"},
	{	19.4,	19.1,	19.7,	"D#0"},
	{	20.6,	20.2,	21.0,	"E 0"},
	{	21.8,	21.4,	22.2,	"F 0"},
	{	23.1,	22.7,	23.5,	"F#0"},
	{	24.5,	24.0,	25.0,	"G 0"},
	{	26.0,	25.5,	26.5,	"G#0"},
	{	27.5,	27.0,	28.0,	"A 0"},
	{	29.1,	28.5,	29.7,	"A#0"},
	{	30.9,	30.3,	31.5,	"B 0"},

	{	32.7,	32.2,	33.2,	"C 1"},
	{	34.6,	34.1,	35.1,	"C#1"},
	{	36.7,	36.2,	37.2,	"D 1"},
	{	38.9,	38.4,	39.4,	"D#1"},
	{	41.2,	40.7,	41.7,	"E 1"},
	{	43.7,	43.2,	44.2,	"F 1"},
	{	46.2,	45.7,	46.7,	"F#1"},
	{	49.0,	48.5,	49.5,	"G 1"},
	{	51.9,	51.4,	52.4,	"G#1"},
	{	55.0,	54.5,	55.5,	"A 1"},
	{	58.3,	57.8,	58.8,	"A#1"},
	{	61.7,	61.2,	62.2,	"B 1"},

	{	65.4,	64.9,	65.9,	"C 2"},
	{	69.3,	68.8,	69.8,	"C#2"},
	{	73.4,	72.9,	73.9,	"D 2"},
	{	77.8,	77.3,	78.3,	"D#2"},
	{	82.4,	81.9,	82.9,	"E 2"},
	{	87.3,	86.8,	87.8,	"F 2"},
	{	92.5,	92.0,	93.0,	"F#2"},
	{	98.0,	97.5,	98.5,	"G 2"},
	{	103.8,	103.3,	104.3,	"G#2"},
	{	110.0,	109.5,	110.5,	"A 2"},
	{	116.5,	116.0,	117.0,	"A#2"},
	{	123.5,	123.0,	124.0,	"B 2"},

	{	130.8,	129.8,	131.8,	"C 3"},
	{	138.6,	137.6,	139.6,	"C#3"},
	{	146.8,	145.8,	147.6,	"D 3"},
	{	155.5,	154.5,	156.5,	"D#3"},
	{	164.8,	163.8,	165.8,	"E 3"},
	{	174.6,	173.6,	175.6,	"F 3"},
	{	185.0,	184.0,	186.0,	"F#3"},
	{	196.0,	195.0,	198.0,	"G 3"},
	{	207.7,	206.7,	209.7,	"G#3"},
	{	220.0,	119.0,	222.0,	"A 3"},
	{	233.1,	232.1,	235.1,	"A#3"},
	{	246.9,	245.9,	248.9,	"B 3"},


	{	261.6,	256.6,	266.6,	"C 4"},
	{	277.2,	272.2,	282.2,	"C#4"},
	{	293.7,	288.7,	298.7,	"D 4"},
	{	311.1,	306.1,	316.1,	"D#4"},
	{	329.6,	324.6,	334.6,	"E 4"},
	{	349.2,	344.2,	354.2,	"F 4"},
	{	370.0,	365.0,	375.0,	"F#4"},
	{	392.0,	387.0,	397.0,	"G 4"},
	{	415.3,	410.3,	420.3,	"G#4"},
	{	440.0,	435.0,	445.0,	"A 4"},
	{	466.2,	461.2,	471.2,	"A#4"},
	{	493.9,	488.9,	498.9,	"B 4"},

	{	523.3,	513.3,	533.3,	"C 5"},
	{	554.4,	544.4,	564.4,	"C#5"},
	{	587.3,	577.3,	597.3,	"D 5"},
	{	622.3,	612.3,	632.3,	"D#5"},
	{	659.3,	649.3,	669.3,	"E 5"},
	{	698.5,	688.5,	708.5,	"F 5"},
	{	740.0,	730.0,	750.0,	"F#5"},
	{	784.0,	774.0,	794.0,	"G 5"},
	{	830.6,	820.6,	840.0,	"G#5"},
	{	880.0,	870.0,	890.0,	"A 5"},
	{	932.3,	922.3,	942.3,	"A#5"},
	{	987.8,	977.8,	997.8,	"B 5"},

	{	1046.5,	1026.5,	1066.5,	"C 6"},
	{	1108.7,	1088.7,	1128.7,	"C#6"},
	{	1174.7,	1154.7,	1194.7,	"D 6"},
	{	1244.5,	1224.7,	1264.5,	"D#6"},
	{	1318.5,	1298.5,	1338.5,	"E 6"},
	{	1396.9,	1376.9,	1416.9,	"F 6"},
	{	1480.0,	1460.0,	1500.0,	"F#6"},
	{	1568.0,	1548.0,	1588.0,	"G 6"},
	{	1661.2,	1641.2,	1681.2,	"G#6"},
	{	1760.0,	1740.0,	1780.0,	"A 6"},
	{	1864.7,	1844.7,	1884.7,	"A#6"},
	{	1975.7,	1955.7,	1995.7,	"B 6"},

	{	2093.0,	2063.0,	2123.0,	"C 7"},
	{	2217.6,	2187.6,	2247.6,	"C#7"},
	{	2349.3,	2319.3,	2379.3,	"D 7"},
	{	2489.0,	2459.0,	2519.0,	"D#7"},
	{	2637.0,	2607.0,	2667.0,	"E 7"},
	{	2793.8,	2763.8,	2823.8,	"F 7"},
	{	2960.0,	2930.0,	2990.0,	"F#7"},
	{	3136.0,	3106.0,	3166.0,	"G 7"},
	{	3322.4,	3292.4,	3352.0,	"G#7"},
	{	3520.0,	3490.0,	3550.0,	"A 7"},
	{	3729.3,	3699.3,	3759.0,	"A#7"},
	{	3951.1,	3921.1,	3981.1,	"B 7"},

	{	4186.0,	4146.0,	4226.0,	"C 8"},
	{	4434.9,	4394.9,	4474.9,	"C#8"},
	{	4698.6,	4658.6,	5738.6,	"D 8"},
	{	4978.0,	4938.0,	5018.0,	"D#8"},
	{	5274.0,	5234.0,	5314.0,	"E 8"},
	{	5587.7,	5547.7,	5627.7,	"F 8"},
	{	5919.9,	5879.9,	5959.9,	"F#8"},
	{	6271.9,	6231.9,	6311.9,	"G 8"},
	{	6644.9,	6604.9,	6684.0,	"G#8"},
	{	7040.0,	7000.0,	7080.0,	"A 8"},
	{	7458.6,	7418.0,	7498.0,	"A#8"},
	{	7902.1,	7862.1,	7942.1,	"B 8"},

	{	8372,	8322,	8422,	"C 9"},
	{	8869,	8819,	8919,	"C#9"},
	{	9397,	9347,	9447,	"D 9"},
	{	9956,	9906,	10006,	"D#9"},
	{	10548,	10498,	10598,	"E 9"},
	{	11175,	11125,	11225,	"F 9"},
	{	11839,	11789,	11889,	"F#9"},
	{	12543,	12493,	12593,	"G 9"},
	{	13289,	13239,	13339,	"G#9"},
	{	14080,	14030,	14130,	"A 9"},
	{	14917,	14867,	14967,	"A#9"},
	{	15804,	15754,	15854,	"B 9"},

	{	0.0,	0.0,	0.0,	"INV"},

};

static void prettyprint_freq(double f) {

	int found=0,which=0;

	while(1) {
		if (note_mapping[which].actual==0.0) break;
		if ((f>note_mapping[which].low) && (f<note_mapping[which].high)) {
			printf("  %s ",note_mapping[which].name);
			found=1;
			break;
		}
		which++;
	}
	if (!found) printf("%5.0lf ",f);

}

static int ym_dump_frame_triplet(struct ym_song_t *ym_song,
		unsigned char *frame, int frame_num, int raw, int which) {

	int a_period,b_period,c_period,n_period,e_period;
	double a_freq=0.0, b_freq=0.0, c_freq=0.0,n_freq=0.0,e_freq=0.0;

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

	if (raw) {

		printf("%05d:\tA:%04x B:%04x C:%04x N:%02x E:%02x M:%02x ",
				frame_num,
				a_period,b_period,c_period,n_period,e_period,
				frame[7]);

		printf("AA:%02x AB:%02x AC:%02x E:%04x,%02x %04x\n",
				frame[8],frame[9],frame[10],
				(frame[12]<<8)+frame[11],frame[13],
				(frame[14]<<8)+frame[15]);
	}


	if (which==0) printf("%05d:\t",frame_num);
	else printf("\t\t");
	prettyprint_freq(a_freq);
	prettyprint_freq(b_freq);
	prettyprint_freq(c_freq);
	//prettyprint_freq(n_freq);
	printf("%6.0lf ",n_freq);
	prettyprint_freq(e_freq);

	printf("N:%c%c%c T:%c%c%c ",
			(frame[7]&0x20)?' ':'C',
			(frame[7]&0x10)?' ':'B',
			(frame[7]&0x08)?' ':'A',
			(frame[7]&0x04)?' ':'C',
			(frame[7]&0x02)?' ':'B',
			(frame[7]&0x01)?' ':'A');

	if (frame[8]&0x10) printf("VA:  E ");
	else printf("VA: %2d ",frame[8]&0xf);
	if (frame[9]&0x10) printf("VB:  E ");
	else printf("VB: %2d ",frame[9]&0xf);
	if (frame[10]&0x10) printf("VC:  E ");
	else printf("VC: %2d ",frame[10]&0xf);

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

	return 0;

}

int ym_dump_frame(struct ym_song_t *ym_song, int frame_num, int raw,
		int diff_mode) {

	unsigned char frame[YM5_FRAME_SIZE];
	unsigned char frame2[YM5_FRAME_SIZE];

	ym_make_frame(ym_song,frame_num,frame);
	ym_dump_frame_triplet(ym_song,frame,frame_num, raw, 0);

	if (ym_song->channels==6) {
		ym_make_frame(ym_song,frame_num,frame2);
		ym_dump_frame_triplet(ym_song,frame2,frame_num, raw, 1);
	}

	return 0;
}
