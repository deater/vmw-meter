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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

#include <bcm2835.h>

#include "ay-3-8910.h"
#include "display.h"

#define YM_HEADER_SIZE	34
#define YM_FRAME_SIZE	16
#define MAX_STRING	256

#define AY38910_CLOCK	1000000	/* 1MHz on our board */

static int play_music=0;
static int dump_info=0;
static int visualize=1;
static int display_type=DISPLAY_BOTH;

static void quiet(int sig) {

	if (play_music) quiet_ay_3_8910();

	printf("Quieting and exiting\n");
	_exit(0);

}

int main(int argc, char **argv) {

	int fd;
	unsigned char header[YM_HEADER_SIZE];
	unsigned char frame[YM_FRAME_SIZE];
	char filename[BUFSIZ]="intro2.ym";
	int result;
	char ym5_magic[]="YM5!LeOnArD!";
	char ym6_magic[]="YM6!LeOnArD!";
	int num_frames,song_attributes,num_digidrum,master_clock;
	int frame_rate,loop_frame,extra_data;
	int drum_size,i,j;
	int length_seconds;
	char song_name[MAX_STRING];
	char author[MAX_STRING];
	char comment[MAX_STRING];
	int pointer;
	int interleaved=0;
	off_t file_position,curr_position;

	int a_period,b_period,c_period,n_period,e_period;
	double a_freq, b_freq, c_freq,n_freq,e_freq;
	int new_a,new_b,new_c,new_n,new_e;

	struct timeval start,next;

	signal(SIGINT, quiet);

	if (argc>1) {
		strcpy(filename,argv[1]);
	}

	fd=open(filename,O_RDONLY);
	if (fd<1) {
		fprintf(stderr,"Error opening %s, %s!\n",
			filename,strerror(errno));
		return -1;
	}

	result=read(fd,header,YM_HEADER_SIZE);
	if (result<YM_HEADER_SIZE) {
		fprintf(stderr,"Error reading header!\n");
		return -1;
	}

	if (!memcmp(header,ym5_magic,12)) {
		/* YM5 file */
	}
	else if (!memcmp(header,ym6_magic,12)) {
		/* YM6 file */
	}
	else {
		fprintf(stderr,"Error, not a ym6 file!\n");
		if ((header[2]=='l') && (header[3]=='h')) {
			fprintf(stderr,"Probably a LHC compressed ym, decompress before playing.\n");
		}
		return -1;
	}

	num_frames=(header[12]<<24)|(header[13]<<16)|(header[14]<<8)|(header[15]);
	printf("Number of frames: %d\n",num_frames);

	song_attributes=(header[16]<<24)|(header[17]<<16)|(header[18]<<8)|(header[19]);
	printf("Song attributes: %d\n",song_attributes);
	interleaved=song_attributes&0x1;
	printf("\tInterleaved=%s\n",interleaved?"yes":"no");

	/* interleaved makes things compress better */
	/* but much more of a pain to play */

	num_digidrum=(header[20]<<8)|(header[21]);
	printf("Num digidrum samples: %d\n",num_digidrum);

	master_clock=(header[22]<<24)|(header[23]<<16)|(header[24]<<8)|(header[25]);
	printf("Master clock: %d Hz\n",master_clock);

	frame_rate=(header[26]<<8)|(header[27]);
	printf("Frame rate: %d\n",frame_rate);

	loop_frame=(header[28]<<24)|(header[29]<<16)|(header[30]<<8)|(header[31]);
	printf("Loop frame: %d\n",loop_frame);

	extra_data=(header[32]<<8)|(header[33]);
	printf("Extra data size: %d\n",extra_data);

	length_seconds=num_frames/frame_rate;
	printf("Length=%d:%02d\n",length_seconds/60,length_seconds%60);

	if (num_digidrum>0) {
		fprintf(stderr,"Warning!  We don't handle digidrum\n");
		for(i=0;i<num_digidrum;i++) {
			result=read(fd,header,2);
			drum_size=(header[0]<<8)|(header[1]);
			printf("Drum%d: %d bytes\n",i,drum_size);
			lseek(fd,drum_size,SEEK_CUR);
		}
	}

	pointer=0;
	while(1) {
		result=read(fd,header,1);
		if (result!=1) {
			fprintf(stderr,"Error parsing string!\n");
			return -1;
		}
		if (!header[0]) {
			song_name[pointer]=0;
			break;
		}
		if (pointer<MAX_STRING) {
			song_name[pointer]=header[0];
			pointer++;
		}
	}
	printf("Song name: %s\n",song_name);

	pointer=0;
	while(1) {
		result=read(fd,header,1);
		if (result!=1) {
			fprintf(stderr,"Error parsing string!\n");
			return -1;
		}
		if (!header[0]) {
			author[pointer]=0;
			break;
		}
		if (pointer<MAX_STRING) {
			author[pointer]=header[0];
			pointer++;
		}
	}
	printf("Author name: %s\n",author);

	pointer=0;
	while(1) {
		result=read(fd,header,1);
		if (result!=1) {
			fprintf(stderr,"Error parsing string!\n");
			return -1;
		}
		if (!header[0]) {
			comment[pointer]=0;
			break;
		}
		if (pointer<MAX_STRING) {
			comment[pointer]=header[0];
			pointer++;
		}
	}
	printf("Comment: %s\n",comment);

	file_position=lseek(fd,0,SEEK_CUR)-1;
	if (dump_info) printf("Frames start at %lx\n",file_position);
	gettimeofday(&start,NULL);

	/*******************/
	/* Initialize Chip */
	/*******************/

	if (play_music) {
		initialize_ay_3_8910();
	}

	if (visualize) {
		display_init(display_type);
	}

	for(i=0;i<num_frames;i++) {
		if (interleaved) {
			file_position++;
			lseek(fd,file_position,SEEK_SET);
			for(j=0;j<YM_FRAME_SIZE;j++) {
				result=read(fd,&frame[j],1);
				if (j!=15) {
					curr_position=lseek(fd,num_frames-1,SEEK_CUR);
					//printf("LOC=%lx\n",curr_position);
					(void)curr_position;
				}
			}
		}
		else {
			result=read(fd,frame,YM_FRAME_SIZE);
			if (result!=YM_FRAME_SIZE) {
				fprintf(stderr,"ERROR: out of data\n");
				return -1;
			}
		}

		/****************************************/
		/* Write out the music			*/
		/****************************************/


		a_period=((frame[1]&0xf)<<8)|frame[0];
		b_period=((frame[3]&0xf)<<8)|frame[2];
		c_period=((frame[5]&0xf)<<8)|frame[4];
		n_period=frame[6]&0x1f;
		e_period=((frame[12]&0xff)<<8)|frame[11];

		a_freq=master_clock/(16.0*(double)a_period);
		b_freq=master_clock/(16.0*(double)b_period);
		c_freq=master_clock/(16.0*(double)c_period);
		n_freq=master_clock/(16.0*(double)n_period);
		e_freq=master_clock/(256.0*(double)e_period);

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
		}


		/* Scale if needed */
		if (master_clock!=AY38910_CLOCK) {

			new_a=(double)AY38910_CLOCK/(16.0*a_freq);
			new_b=(double)AY38910_CLOCK/(16.0*b_freq);
			new_c=(double)AY38910_CLOCK/(16.0*c_freq);
			new_n=(double)AY38910_CLOCK/(16.0*n_freq);
			new_e=(double)AY38910_CLOCK/(256.0*e_freq);

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
				write_ay_3_8910(j,frame[j]);
			}

			/* Special case.  Writing r13 resets it,	*/
			/* so special 0xff marker means do not write	*/
			if (frame[13]!=0xff) {
				write_ay_3_8910(13,frame[13]);
			}
		}

		if (visualize) {
			if (display_type&DISPLAY_TEXT) {
				printf("\033[H\033[2J");
			}
			bargraph( display_type, (frame[8]*11)/16,
						(frame[9]*11)/16,
						(frame[10]*11)/16);
			freq_display(a_period,b_period,c_period);
		}




		if (play_music) {
//		usleep(1000000/frame_rate);	/* often 50Hz */

//		bcm2835_delayMicroseconds(1000000/frame_rate);	/* often 50Hz = 20000 */
			bcm2835_delayMicroseconds(4000);	/* often 50Hz = 20000 */
		}
		else {
			if (visualize) usleep(1000000/frame_rate);	/* often 50Hz */
		}

		if (i%100==0) {
			double s,n,hz;

			s=start.tv_sec+(start.tv_usec/1000000.0);
			gettimeofday(&next,NULL);
			n=next.tv_sec+(next.tv_usec/1000000.0);

			hz=100/(n-s);

			printf("Done frame %d, %.1lfHz\n",i,hz);
			start.tv_sec=next.tv_sec;
			start.tv_usec=next.tv_usec;
		}
	}

	result=read(fd,header,4);
	header[4]=0;

	if (result!=4) {
		fprintf(stderr,"ERROR! Bad ending! %d\n",result);
		return -1;
	}

	if (memcmp(header,"End!",4)) {
		fprintf(stderr,"ERROR! Bad ending! %s\n",header);
		return -1;
	}

	close(fd);

	if (play_music) quiet_ay_3_8910();

	return 0;
}
