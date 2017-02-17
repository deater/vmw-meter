#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "display.h"
#include "i2c_lib.h"

#include "14seg_font.h"

#define MAX_LYRICS	2048

struct lyric_line {
	int frame;
	char *text;
};

struct lyric_type {
	int num;
	struct lyric_line *l;
};

static int i2c_fd;



int display_string(char *led_string) {

	char buffer1[17],buffer2[17];
	int i;

	buffer1[0]=0;
	buffer2[0]=0;

	for(i=0;i<4;i++) {
		buffer1[(i*2)+1]=adafruit_lookup[led_string[i+4]]>>8;
		buffer1[(i*2)+2]=adafruit_lookup[led_string[i+4]]&0xff;
	}

	for(i=0;i<4;i++) {
		buffer2[(i*2)+1]=adafruit_lookup[led_string[i]]>>8;
		buffer2[(i*2)+2]=adafruit_lookup[led_string[i]]&0xff;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS3) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS3);
		return -1;
	}

	if ( (write(i2c_fd, buffer1, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS7) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS7);
		return -1;
	}

	if ( (write(i2c_fd, buffer2, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
	}

	return 0;
}


static int lyrics_play(struct lyric_type *l) {

	int frame=0,lnum=0,sub=0;
	char led_string[8],ch;
	int i;

	while(1) {
		frame++;

		if (frame==l->l[lnum].frame) {

			sub=0;
			while(1) {
				if (l->l[lnum].text[sub]==0) break;
				if (l->l[lnum].text[sub]=='\\') {
					sub++;
					if (l->l[lnum].text[sub]=='n') {
						write(1,"\n",1);
					}
				}
				else {
					ch=l->l[lnum].text[sub];
					write(1,&ch,1);

					for(i=1;i<8;i++) {
						led_string[i-1]=led_string[i];
					}
					if (isalpha(ch)) {
						led_string[7]=toupper(ch);
					}
					else {
						led_string[7]=ch;
					}
				}

//				strncpy(led_string,"THIS WAS",8);

				display_string(led_string);

				sub++;

				usleep(20000);
				usleep(20000);
				frame+=2;


			}
			lnum++;
		}
		usleep(20000);
		if (lnum==l->num) break;
	}

	return 0;

}

static int load_lyrics(char *filename, struct lyric_type *l) {

	char *result;
	char string[BUFSIZ];

	int num=0,line=0;
	int sp;
	int frame_num;
	char *start;

	l->l=calloc(sizeof(struct lyric_type),MAX_LYRICS);
	if (l->l==NULL) {
		fprintf(stderr,"MEM ERROR\n");
		return -1;
	}

	while(1) {
		result=fgets(string,BUFSIZ,stdin);
		if (result==NULL) break;
		line++;

//		printf("%s",string);

		sp=0;

		/* Ignore comments */
		if (string[0]=='#') continue;

		/* skip whitespace */
		while((string[sp]==' ' || string[sp]=='\t')) sp++;
		if (string[sp]=='\n') continue;

		/* get number */
		frame_num=0;
		while((string[sp]!=' ' && string[sp]!='\t' &&
			string[sp]!='\n')) {
			frame_num*=10;
			frame_num+=string[sp]-'0';
			sp++;
		}

		l->l[num].frame=frame_num;

		/* skip whitespace */
		while((string[sp]==' ' || string[sp]=='\t')) sp++;
		if (string[sp]=='\n') continue;

		if (string[sp]!='\"') {
			fprintf(stderr,"Unknown string line %d\n",line);
			continue;
		}
		sp++;

		start=&string[sp];

		while((string[sp]!='\"' && string[sp]!='\n')) {
			sp++;
		}
		string[sp]='\0';

		l->l[num].text=strdup(start);


//		printf("%d %s\n",l[num].frame,l[num].text);

		num++;
		if (num>MAX_LYRICS) {
			fprintf(stderr,"Too many lyrics, line %d!\n",line);
			return -1;
		}
	}

	l->num=num;

	return 0;
}

static int destroy_lyrics(struct lyric_type *l) {

	free(l->l);

	return 0;
}

int main(int argc, char **argv) {

	int visualize=1;
	int display_type=DISPLAY_I2C;
	int result;


	struct lyric_type l;

	/* Initialize the display */
#if 0
	if (visualize) {
		result=display_init(display_type);
		if (result<0) {
			printf("Error initializing display!\n");
			printf("Turning off display for now!\n");
			display_type=0;
		}
	}
#endif

	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		return -1;
	}


		/* Init display */
		if (init_display(i2c_fd,HT16K33_ADDRESS3,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}
		if (init_display(i2c_fd,HT16K33_ADDRESS7,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}


	translate_to_adafruit();

	load_lyrics(NULL,&l);

	lyrics_play(&l);

	destroy_lyrics(&l);

	close(i2c_fd);

	return 0;
}
