/* Play a YM AY-3-8910 Music File */
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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <bcm2835.h>

#define YM_HEADER_SIZE	34
#define YM_FRAME_SIZE	16
#define MAX_STRING	256

#define DELAY_PAUSE	10
#define DELAY_BETWEEN	10
#define DELAY_SHIFT	10
#define DELAY_RESET	5000

int initialize_ay_3_8910(void) {

	printf("Initializing AY-3-8910\n");

	if (!bcm2835_init()) {
		fprintf(stderr,"Error init libBCM2835!\n");
		return -1;
	}

	/* Enable GPIO17 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_11, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO27 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_13, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO22 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_15, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO18 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_12, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO23 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_16, BCM2835_GPIO_FSEL_OUTP);
	/* Enable GPIO24 */
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_18, BCM2835_GPIO_FSEL_OUTP);

	/* Pull reset low than high */
	printf("Resetting...\n");
	bcm2835_gpio_write(RPI_GPIO_P1_18, LOW);
	bcm2835_delayMicroseconds(DELAY_RESET);
	bcm2835_gpio_write(RPI_GPIO_P1_18, HIGH);

	return 0;
}

int shift_74hc595(int value) {


	int i,shifted_val;

	shifted_val=value;

	for(i=0;i<8;i++) {
		/* Set clock low */
		bcm2835_gpio_write(27, LOW);

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		if (shifted_val&0x1) {
//		if (shifted_val&0x80) {
			bcm2835_gpio_write(RPI_V2_GPIO_P1_11, HIGH);
		}
		else {
			bcm2835_gpio_write(RPI_V2_GPIO_P1_11, LOW);
		}
//		shifted_val<<=1;
		shifted_val>>=1;

		bcm2835_delayMicroseconds(DELAY_SHIFT);

		/* Set clock high */
		bcm2835_gpio_write(27, HIGH);

		bcm2835_delayMicroseconds(DELAY_SHIFT);
	}

	bcm2835_gpio_write(22, HIGH);

	bcm2835_delayMicroseconds(DELAY_SHIFT);

	bcm2835_gpio_write(22, LOW);

	bcm2835_delayMicroseconds(DELAY_SHIFT);

	return 0;
}

int ay_3_8910_write(int addr, int value) {


	/* Be sure BDIR and BC1 are low */
	bcm2835_gpio_write(RPI_GPIO_P1_16, LOW);
	bcm2835_gpio_write(RPI_GPIO_P1_12, LOW);

	/* Set address on bus */
	shift_74hc595(addr);

	/* Set BDIR and BC1 high */
	bcm2835_gpio_write(RPI_GPIO_P1_16, HIGH);
	bcm2835_gpio_write(RPI_GPIO_P1_12, HIGH);

	/* pause */
	bcm2835_delayMicroseconds(DELAY_PAUSE);

	/* Set BDIR and BC1 low */
	bcm2835_gpio_write(RPI_GPIO_P1_16, LOW);
	bcm2835_gpio_write(RPI_GPIO_P1_12, LOW);

	bcm2835_delayMicroseconds(DELAY_BETWEEN);

	/* Be sure BDIR and BC1 are low */
	/* Put value on bus */
	shift_74hc595(value);

	/* Put BDIR high */
	bcm2835_gpio_write(RPI_GPIO_P1_16, HIGH);

	/* pause */
	bcm2835_delayMicroseconds(DELAY_PAUSE);

	/* Put BDIR low */
	bcm2835_gpio_write(RPI_GPIO_P1_16, LOW);

	bcm2835_delayMicroseconds(DELAY_BETWEEN);

	return 0;
}

void quiet(int sig) {

	int j;

	for(j=0;j<14;j++) {
		ay_3_8910_write(j,0);
	}

	printf("Quieting and exiting\n");
	_exit(0);

}

int main(int argc, char **argv) {

	int fd;
	unsigned char header[YM_HEADER_SIZE];
	unsigned char frame[YM_FRAME_SIZE];
	char filename[BUFSIZ]="intro2.ym";
	int result;
	char ym6_magic[]="YM6!LeOnArD!";
	char ym5_magic[]="YM5!LeOnArD!";
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

	/*******************/
	/* Initialize Chip */
	/*******************/

	initialize_ay_3_8910();

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

		for(j=0;j<14;j++) {
			ay_3_8910_write(j,frame[j]);
		}

		usleep(20000);	/* 50Hz roughly */

		if (i%100==0) printf("Done frame %d\n",i);
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
