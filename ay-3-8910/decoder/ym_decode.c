/* Parse a YM6 AY-3-8910 Music File */
/* Used file info found here: http://leonard.oxg.free.fr/ymformat.html */
/* Also useful: ftp://ftp.modland.com/pub/documents/format_documentation/Atari%20ST%20Sound%20Chip%20Emulator%20YM1-6%20(.ay,%20.ym).txt */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define YM_HEADER_SIZE	34
#define YM_FRAME_SIZE	16
#define MAX_STRING	256

int main(int argc, char **argv) {

	int fd;
	unsigned char header[YM_HEADER_SIZE];
	unsigned char frame[YM_FRAME_SIZE];
	char filename[]="intro2.ym";
	int result;
	char ym_magic[]="YM6!LeOnArD!";
	int num_frames,song_attributes,num_digidrum,master_clock;
	int frame_rate,loop_frame,extra_data;
	int drum_size,i,j;
	int length_seconds;
	char song_name[MAX_STRING];
	char author[MAX_STRING];
	char comment[MAX_STRING];
	int pointer;
	int interleaved=0;
	off_t file_position;

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

	if (memcmp(header,ym_magic,12)) {
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

	for(i=0;i<num_frames;i++) {
		if (interleaved) {
			file_position++;
			lseek(fd,file_position,SEEK_SET);
			for(j=0;j<16;j++) {
				result=read(fd,&frame[j],1);
				lseek(fd,num_frames,SEEK_CUR);
			}
		}
		else {
			result=read(fd,frame,YM_FRAME_SIZE);
			if (result!=YM_FRAME_SIZE) {
				fprintf(stderr,"ERROR: out of data\n");
				return -1;
			}
		}

		int a_period,b_period,c_period;

		a_period=((frame[1]&0xf)<<8)|frame[0];
		b_period=((frame[3]&0xf)<<8)|frame[2];
		c_period=((frame[5]&0xf)<<8)|frame[4];

		printf("%05d:\tAP:%04x BP:%04x CP:%04x %02x %02x ",
			i,a_period,b_period,c_period,
			frame[6],frame[7]);
		printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
			frame[8],frame[9],frame[10],frame[11],
			frame[12],frame[13],frame[14],frame[15]);

	}

	result=read(fd,header,4);
	if (result!=4) {
		fprintf(stderr,"ERROR! Bad ending!\n");
		return -1;
	}

	if (memcpy(header,"End!",4)) {
		fprintf(stderr,"ERROR! Bad ending!\n");
		return -1;
	}

	close(fd);

	return 0;
}
