/* Parse/Play a YM AY-3-8910 Music File */
/* Used file info found here: http://leonard.oxg.free.fr/ymformat.html */
/* Also useful: ftp://ftp.modland.com/pub/documents/format_documentation/Atari%20ST%20Sound%20Chip%20Emulator%20YM1-6%20(.ay,%20.ym).txt */

/* Hooked up to Raspberry Pi via 74HC595 */
/* DS GPIO17     */
/* SHCP GPIO27   */
/* STCP GPIO22   */
/* BC1  GPIO18   */
/* BDIR GPIO23   */
/* |RESET GPIO24 */


#define VERSION "0.2"

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

#include <bcm2835.h>

#include "ay-3-8910.h"
#include "display.h"

#ifdef USE_LIBLHASA
#include "lhasa.h"
#endif

#define YM4_HEADER_SIZE	26
#define YM5_HEADER_SIZE	34
#define YM5_FRAME_SIZE	16
#define YM3_FRAME_SIZE	16
#define MAX_STRING	256

#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int play_music=1;
static int dump_info=0;
static int visualize=1;
static int display_type=DISPLAY_I2C;
static int shift_size=16;
static int music_repeat=0;
static int music_paused=0;
static int music_loop=0;

static void quiet(int sig) {

	if (play_music) quiet_ay_3_8910(shift_size);

	display_shutdown(display_type);

	printf("Quieting and exiting\n");
	_exit(0);

}

void print_help(int just_version, char *exec_name) {

	printf("\nym_play version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This plays YM5 music files out of the VMW AY-3-8910 device\n");
	printf("on a suitably configured Raspberry Pi machine.\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-i] [-t] [-m] [-s] [-n] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");
	printf("\t-i: use i2c LEDs for visualization\n");
	printf("\t-t: use ASCII text visualization\n");
	printf("\t-m: use mono (only one AY-3-8910 hooked up)\n");
	printf("\t-s: use stereo (two AY-3-8910s hooked up)\n");
	printf("\t-n: no sound (useful when debugging)\n");
	// printf("\t-k: kiosk mode (standalone without keyboard/monitor)\n");
	printf("\tfilename: must be uncompressed YM5 file for now\n\n");

	exit(0);
}

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

	printf("Decompressing %s, size %d\n",header->filename,header->length);

	data=calloc(header->length,sizeof(unsigned char));
	if (data==NULL) {
		fprintf(stderr,"Error allocating %d bytes\n",header->length);
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

static unsigned char *load_ym_song(char *filename, int *ym_type, int *file_size) {

	int fd,result,compressed=0;
	unsigned char *data=NULL;
	unsigned char header[YM5_HEADER_SIZE];

	char ym2_magic[]="YM2!";
	char ym3_magic[]="YM3!";
	char ym3b_magic[]="YM3b";
	char ym4_magic[]="YM4!LeOnArD!";
	char ym5_magic[]="YM5!LeOnArD!";
	char ym6_magic[]="YM6!LeOnArD!";


	fd=open(filename,O_RDONLY);
	if (fd<1) {
		fprintf(stderr,"Error opening %s, %s!\n",
			filename,strerror(errno));
		return NULL;
	}

	result=read(fd,header,YM5_HEADER_SIZE);
	if (result<YM5_HEADER_SIZE) {
		fprintf(stderr,"Error reading header!\n");
		return NULL;
	}

	close(fd);

	if ((header[3]=='l') && (header[4]=='h')) {
		/* LHA compressed */
		data=load_lha_compressed_song(filename,file_size);
		memcpy(header,data,YM5_HEADER_SIZE);
		compressed=1;
	}

	if (!memcmp(header,ym2_magic,4)) {
		/* YM2 file */
		if (!compressed) data=load_uncompressed_song(filename,file_size);
		*ym_type=2;
	} else
	if (!memcmp(header,ym3_magic,4)) {
		/* YM3 file */
		if (!compressed) data=load_uncompressed_song(filename,file_size);
		*ym_type=3;
	} else
	if (!memcmp(header,ym3b_magic,4)) {
		/* YM3b file */
		if (!compressed) data=load_uncompressed_song(filename,file_size);
		*ym_type=3;
	} else
	if (!memcmp(header,ym4_magic,12)) {
		/* YM4 file */
		if (!compressed) data=load_uncompressed_song(filename,file_size);
		*ym_type=4;
	} else
	if (!memcmp(header,ym5_magic,12)) {
		/* YM5 file */
		if (!compressed) data=load_uncompressed_song(filename,file_size);
		*ym_type=5;
	}
	else if (!memcmp(header,ym6_magic,12)) {
		/* YM6 file */
		if (!compressed) data=load_uncompressed_song(filename,file_size);
		*ym_type=6;
	} else {
		fprintf(stderr,"Error, not a known YM  file!\n");
		return NULL;
	}

	return data;
}

static int play_song(char *filename) {

	unsigned char *ym_file;
	int ym_type,ym_size,ym_frame_size;
	unsigned char frame[YM5_FRAME_SIZE];
	int num_frames,song_attributes,num_digidrum,master_clock;
	int frame_rate,loop_frame,extra_data;
	int drum_size,i,j;
	int length_seconds;
	char song_name[MAX_STRING];
	char author[MAX_STRING];
	char comment[MAX_STRING];
	int pointer;
	long int file_offset=0,data_begin;
	int interleaved=0;
	double s,n,hz,diff;
	double max_a=0.0,max_b=0.0,max_c=0.0;

	int a_period,b_period,c_period,n_period,e_period;
	double a_freq=0.0, b_freq=0.0, c_freq=0.0,n_freq=0.0,e_freq=0.0;
	int new_a,new_b,new_c,new_n,new_e;

	int display_command=0;

	struct timeval start,next;

	printf("\nPlaying song %s\n",filename);

	ym_file=load_ym_song(filename,&ym_type,&ym_size);
	if (ym_file==NULL) {
		return -1;
	}

	/* Decode header */


	if (ym_type>3) {
		/* version 4, 5, 6 */

		num_frames=(ym_file[12]<<24)|(ym_file[13]<<16)|
			(ym_file[14]<<8)|(ym_file[15]);

		song_attributes=(ym_file[16]<<24)|(ym_file[17]<<16)|
			(ym_file[18]<<8)|(ym_file[19]);
		interleaved=song_attributes&0x1;

		/* interleaved makes things compress better */
		/* but much more of a pain to play */

		num_digidrum=(ym_file[20]<<8)|(ym_file[21]);

		if (ym_type==4) {

			/* assume atari */
			master_clock=2000000;
			frame_rate=50;
			extra_data=0;

			loop_frame=(ym_file[22]<<24)|(ym_file[23]<<16)|
				(ym_file[24]<<8)|(ym_file[25]);

			file_offset=YM4_HEADER_SIZE;
		}
		else {

			master_clock=(ym_file[22]<<24)|(ym_file[23]<<16)|
				(ym_file[24]<<8)|(ym_file[25]);

			frame_rate=(ym_file[26]<<8)|(ym_file[27]);

			loop_frame=(ym_file[28]<<24)|(ym_file[29]<<16)|
				(ym_file[30]<<8)|(ym_file[31]);

			extra_data=(ym_file[32]<<8)|(ym_file[33]);

			file_offset=YM5_HEADER_SIZE;
		}

		/* Skip digidrums (we can't play those) */

		if (num_digidrum>0) {
			fprintf(stderr,"Warning!  We don't handle digidrum\n");
			fprintf(stderr,"\tskipping %d digidrums\n",num_digidrum);
			for(i=0;i<num_digidrum;i++) {
				drum_size=(ym_file[file_offset]<<8)|
					(ym_file[file_offset+1]);
				file_offset+=2;
//				printf("Drum%d: %d bytes\n",i,drum_size);
				file_offset+=drum_size;
			}
		}

		/* Get the song name */

		pointer=0;
		while(1) {
			if (!ym_file[file_offset]) {
				song_name[pointer]=0;
				break;
			}
			if (pointer<MAX_STRING) {
				song_name[pointer]=ym_file[file_offset];
				pointer++;
			}
			file_offset++;
		}

		/* Get the author name */

		pointer=0;
		file_offset++;
		while(1) {
			if (!ym_file[file_offset]) {
				author[pointer]=0;
				break;
			}
			if (pointer<MAX_STRING) {
				author[pointer]=ym_file[file_offset];
				pointer++;
			}
			file_offset++;
		}

		/* Get the comment */
		pointer=0;
		file_offset++;
		while(1) {
			if (!ym_file[file_offset]) {
				comment[pointer]=0;
				break;
			}
			if (pointer<MAX_STRING) {
				comment[pointer]=ym_file[file_offset];
				pointer++;
			}
			file_offset++;
		}

		file_offset++;
		ym_frame_size=YM5_FRAME_SIZE;

	}
	else {
		/* version 2, 3, 3b */
		file_offset=4;
		frame_rate=50;
		/* Assuming Atari */
		master_clock=2000000;
		interleaved=1;
		song_attributes=0x1;
		num_frames=(ym_size-4)/14;
		ym_frame_size=YM3_FRAME_SIZE;
		extra_data=0;
		num_digidrum=0;
		strcpy(song_name,"UNKNOWN");
		strcpy(author,"UNKNOWN");
		strcpy(comment,"UNKNOWN");

		//if (ym_type=3)
		// check if filesize is 4 too many
		// in that case, type 3b and loop_frame is last 4 bytes
		// really we probably don't care
		loop_frame=0;

	}

	if (dump_info) printf("Frames start at %lx\n",file_offset);
	data_begin=file_offset;

	gettimeofday(&start,NULL);

	/**********************/
	/* Print song summary */
	/**********************/

	printf("\tYM%d",ym_type);
	printf("\tSong attributes (%d) : ",song_attributes);
	printf("Interleaved=%s\n",interleaved?"yes":"no");
	if (num_digidrum>0) {
		printf("Num digidrum samples: %d\n",num_digidrum);
	}
	printf("\tFrames: %d, ",num_frames);
	printf("Chip clock: %d Hz, ",master_clock);
	printf("Frame rate: %d Hz, ",frame_rate);
		if (frame_rate!=50) {
			fprintf(stderr,"FIX ME framerate %d\n",frame_rate);
			exit(1);
		}
	length_seconds=num_frames/frame_rate;
	printf("Length=%d:%02d\n",length_seconds/60,length_seconds%60);
	printf("\tLoop frame: %d, ",loop_frame);
	printf("Extra data size: %d\n",extra_data);
	printf("\tSong name: %s\n",song_name);
	printf("\tAuthor name: %s\n",author);
	printf("\tComment: %s\n",comment);

	/******************/
	/* Play the song! */
	/******************/

	i=0;
	while(1) {

	if (!music_paused) {
		if (interleaved) {
			for(j=0;j<ym_frame_size;j++) {
				frame[j]=ym_file[file_offset+j*num_frames];
			}
			file_offset++;
		}
		else {
			memcpy(frame,&ym_file[file_offset],ym_frame_size);
			file_offset+=ym_frame_size;
		}

		/****************************************/
		/* Write out the music			*/
		/****************************************/


		a_period=((frame[1]&0xf)<<8)|frame[0];
		b_period=((frame[3]&0xf)<<8)|frame[2];
		c_period=((frame[5]&0xf)<<8)|frame[4];
		n_period=frame[6]&0x1f;
		e_period=((frame[12]&0xff)<<8)|frame[11];

		if (a_period>0) a_freq=master_clock/(16.0*(double)a_period);
		if (b_period>0) b_freq=master_clock/(16.0*(double)b_period);
		if (c_period>0) c_freq=master_clock/(16.0*(double)c_period);
		if (n_period>0) n_freq=master_clock/(16.0*(double)n_period);
		if (e_period>0) e_freq=master_clock/(256.0*(double)e_period);

		if (dump_info) {
			printf("%05d:\tA:%04x B:%04x C:%04x N:%02x M:%02x ",
				i,a_period,b_period,c_period,n_period,frame[7]);

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

			if (a_freq>max_a) max_a=a_freq;
			if (b_freq>max_b) max_b=b_freq;
			if (c_freq>max_c) max_c=c_freq;
		}


		/* Scale if needed */
		if (master_clock!=AY38910_CLOCK) {

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
	}
		if (visualize) {
			if (display_type&DISPLAY_TEXT) {
				printf("\033[H\033[2J");
			}

			display_command=display_update(display_type,
						(frame[8]*11)/16,
						(frame[9]*11)/16,
						(frame[10]*11)/16,
						(a_freq)/150,
						(b_freq)/150,
						(c_freq)/150,
						i,num_frames,
						filename,0);
		}

		/* Calculate time it took to play/visualize */
		gettimeofday(&next,NULL);
		s=start.tv_sec+(start.tv_usec/1000000.0);
		n=next.tv_sec+(next.tv_usec/1000000.0);
		diff=(n-s)*1000000.0;

		/* Delay until time for next update (often 50Hz) */
		if (play_music) {
			if (diff>0) bcm2835_delayMicroseconds(20000-diff);
			/* often 50Hz = 20000 */
			/* TODO: calculate correctly */
		}
		else {
			if (visualize) usleep(1000000/frame_rate);
		}

		/* Calculate time it actually took, and print		*/
		/* so we can see if things are going horribly wrong	*/
		gettimeofday(&next,NULL);
		s=start.tv_sec+(start.tv_usec/1000000.0);
		n=next.tv_sec+(next.tv_usec/1000000.0);

		if (i%100==0) {
			hz=1/(n-s);
			printf("Done frame %d/%d, %.1lfHz\n",i,num_frames,hz);
		}
		start.tv_sec=next.tv_sec;
		start.tv_usec=next.tv_usec;

		if (display_command==CMD_EXIT_PROGRAM) {
			free(ym_file);
			return CMD_EXIT_PROGRAM;
		}

		/* prev song */
		if (display_command==CMD_BACK) {
			free(ym_file);
			return CMD_BACK;
		}
		/* next song */
		if (display_command==CMD_FWD) {
			free(ym_file);
			return CMD_FWD;
		}

		/* rewind = Beginning of track */
		if (display_command==CMD_RW) {
			i=0;
			file_offset=data_begin;
		}

		/* fastfwd = skip ahead 5s */
		if (display_command==CMD_FF) {
			i+=5*frame_rate;
			if (interleaved) {
				file_offset+=5*frame_rate;
			}
			else {
				file_offset+=(5*frame_rate)*ym_frame_size;
			}
		}


		if (display_command==CMD_PAUSE) {
			if (music_paused) {
				music_paused=0;
			}
			else {
				music_paused=1;
				quiet_ay_3_8910(shift_size);
			}
		}

		if (display_command==CMD_LOOP) {
			music_loop=!music_loop;
			if (music_loop) printf("MUSIC LOOP ON\n");
			else printf("MUSIC LOOP OFF\n");
		}


		/* increment frame */
		if (!music_paused) i++;

		/* Check to see if done with file */
		if (i>=num_frames) {
			if (music_loop) {
				i=loop_frame;
				if (interleaved) {
					file_offset=data_begin+i;
				}
				else {
					file_offset=data_begin+i*ym_frame_size;
				}
			}
			else {
				break;
			}
		}

	}

	if (i>num_frames) {
		printf("Fast-forwarded, skipping end check\n");
	}
	else {

		if (interleaved) {
			file_offset+=15*num_frames;
		}

		if (ym_type>3) {
			/* Read the tail of the file and ensure */
			/* it has the proper trailer */
			if (memcmp(&ym_file[file_offset],"End!",4)) {
				fprintf(stderr,"ERROR! Bad ending! %x\n",
					ym_file[file_offset]);
				return -1;
			}
		}
	}

	/* Free the ym file */
	free(ym_file);

	if (dump_info) {
		printf("Max a=%.2lf b=%.2lf c=%.2lf\n",max_a,max_b,max_c);
	}

	return 0;
}


int main(int argc, char **argv) {

	char filename[BUFSIZ]="intro2.ym";
	int result;
	int c;
	int next_song,first_song;

	/* Setup control-C handler to quiet the music	*/
	/* otherwise if you force quit it keeps playing	*/
	/* the last tones */
	signal(SIGINT, quiet);

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dmhvmsnitr"))!=-1) {
		switch (c) {
			case 'd':
				/* Debug messages */
				printf("Debug enabled\n");
				dump_info=1;
				break;
			case 'h':
				/* help */
				print_help(0,argv[0]);
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
				break;
			case 'm':
				/* mono sound */
				shift_size=8;
				break;
			case 's':
				/* stereo sound */
				shift_size=16;
				break;
			case 'n':
				/* no sound */
				play_music=0;
				break;
			case 'i':
				/* i2c visualization */
				display_type=DISPLAY_I2C;
				break;
			case 't':
				/* text visualization */
				display_type=DISPLAY_TEXT;
				break;
			case 'r':
				/* repeat */
				music_repeat=1;
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	first_song=optind;
	next_song=0;

	/* Initialize the Chip interface */
	if (play_music) {
		result=initialize_ay_3_8910();
		if (result<0) {
			printf("Error initializing bcm2835!\n");
			printf("Maybe try running as root?\n\n");
			exit(0);
		}
	}

	/* Initialize the displays */
	if (visualize) {
		result=display_init(display_type);
		if (result<0) {
			printf("Error initializing display!\n");
			printf("Turning off display for now!\n");
			display_type=0;
		}
	}

	while(1) {

		if (argv[first_song+next_song]!=NULL) {
			strcpy(filename,argv[first_song+next_song]);
			next_song++;
		}
		else {
			break;
		}

		/* Play the song */
		result=play_song(filename);
		if (result==CMD_EXIT_PROGRAM) {
			break;
		}

		if (result==CMD_BACK) {
			next_song-=2;
			if (next_song<0) next_song=0;
		}
		if (result==CMD_FWD) {
			/* already point to next song */
		}


		/* Quiet down the chips */
		if (play_music) {
			quiet_ay_3_8910(shift_size);
		}

		usleep(500000);

	}

	/* Get ready to shut down */

	/* Quiet down the chips */
	if (play_music) {
		quiet_ay_3_8910(shift_size);
	}

	/* Clear out display */
	if (visualize) {
		display_shutdown(display_type);
	}

	return 0;
}
