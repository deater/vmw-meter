/* Use with the play_mp3.sh shell script */
/* puts some music visualization on the bar graph */
/* and prints the filename to the display */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "14seg_font.h"
#include "i2c_lib.h"

/* based on the vu code by Toby Shepard */
/*   http://tobiah.org/pub/vu/          */

#define SAMPLE_RATE 44100
#define SAMPLE_WIDTH 16
#define NUM_CHANNELS  2

#define DISPLAY_RATE 10
#define SCROLL_RATE 3

static unsigned short top[7]={
   0x00,
   0x01,
   0x03,
   0x07,
   0x0f,
   0x1f,
   0x3f,
};

static unsigned short bargraph_lut[11]=
	{0x0000,
         0x8000,
         0xc000,
         0xe000,
         0xf000,
         0xf800,
         0xfc00,
         0xfe00,
         0xff00,
         0xff80,
         0xffc0
};

static void properly_escape(char *in, char *out) {

	int i=0,j;

	for(j=0;j<strlen(in);j++) {
#if 0
		if (in[j]==' ') {
			out[i]='\\';
			i++;
		} else
		if (in[j]=='\'') {
			out[i]='\\';
			i++;
		} else
		if (in[j]=='\"') {
			out[i]='\\';
			i++;
		}
#endif
		out[i]=in[j];
		i++;
	}
}


int main(int argc, char **argv) {

	int meter_fd,display_present;
	double maximum;
	int loudest[NUM_CHANNELS], counter, not_done = 1, channel;
	short sample[2], temp_sample;
	char *name,*start;
	int str_len=0;
	double adjusted,average;
	long time = 0;
	int string_count=0,string_add=1;

	unsigned short display_state[8];
	int i;

	FILE *input,*output;
	char input_string[BUFSIZ];
	char song_name[BUFSIZ];

	if (argc<2) {
		printf("Usage: %s file.mp3\n\n",argv[0]);
		exit(1);
	}

	/* Init Display */
	display_present=1;
	meter_fd=init_i2c("/dev/i2c-6");
	if (meter_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		display_present=0;
	}

        if (display_present) {
                init_saa1064_display(meter_fd,SAA1064_ADDRESS1);
                init_saa1064_display(meter_fd,SAA1064_ADDRESS2);
                init_saa1064_display(meter_fd,SAA1064_ADDRESS3);
                init_saa1064_display(meter_fd,SAA1064_ADDRESS4);
        }

	for(i=0;i<8;i++) display_state[i]=0;

	if (display_present) {
		update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
			display_state);
	}
	else {
		update_saa1064_ascii(display_state);
	}

	/* setup the name of the file */
	/* pad if less than 6 chars */
	/* remove preceeding path name */

	if (argc<2) {
		name=strdup("      ");
		start=name;
		str_len=6;
	}
	else {
		name=strdup(argv[1]);

		start=name;
		while(*name!='\0') {
			if (*name=='/') start=name+1;
			name++;
		}
		str_len=strlen(start);
		if (str_len<6) {
			name=strdup("      ");
			strncpy(name,start,str_len);
			str_len=6;
			start=name;
		}
	}

	properly_escape(argv[1],song_name);

	sprintf(input_string,"/usr/bin/mpg321 \"%s\" -s",song_name);
	printf("Trying %s\n",input_string);

	input=popen(input_string,"r");
	if (input==NULL) {
		printf("Error executing %s\n",input_string);
		exit(1);
	}

	output=popen("/usr/bin/aplay -f cd","w");
	if (output==NULL) {
		printf("Error executing /usr/bin/aplay\n");
		exit(1);
	}

	/* maximum amplitude for this many bits */
	maximum = ((1<<SAMPLE_WIDTH ) / 2) - 1;


	while(not_done) {

		/* read in a screen-update's worth of samples */
		for(counter = 0; counter <= (SAMPLE_RATE / DISPLAY_RATE); counter++) {

			/* read both channels */
			for(channel = 0; channel < NUM_CHANNELS; channel++){

				loudest[channel] = 0;
				sample[channel] = 0;

				if (fread(&sample[channel] ,
					(SAMPLE_WIDTH / 8), 1, input) != 1) {
					not_done = 0;
					break;
				}
				time++;

				/* adjust 8-bit samples */
				temp_sample = (SAMPLE_WIDTH == 8 ? sample[channel] - 128 : 
						sample[channel]);
				if (temp_sample < -maximum){
					temp_sample = -maximum;
				}

				if (abs(temp_sample) > loudest[channel]) {
					loudest[channel] = abs(temp_sample);
				}
			}

			for(channel = 0; channel < NUM_CHANNELS; channel++){
				/* write sample back out to stderr */
				if (fwrite(&sample[channel] , SAMPLE_WIDTH/8, 1 , output) != 1){
					not_done = 0;
					break;
				}
			}
		}

		average=0;
		for(channel=0;channel<NUM_CHANNELS;channel++) {
			/* take square-root of our_max / theoretical max */
			adjusted = sqrt( (double)(loudest[channel] ) / maximum) ;
			if (adjusted > 1) {
				adjusted = 1;
			}

			// elapsed time, if we ever want to print it, is
			// ((double)time / (double)SAMPLE_RATE) / NUM_CHANNELS);

			display_state[6+channel]=bargraph_lut[(int)(adjusted*10.0)];
			average+=adjusted*6.0;
		}
		average/=2.0;
		display_state[6]|=top[(int)average];
		display_state[7]|=top[(int)average];

		/* scroll the name of the file */
		for(i=0;i<6;i++) {
			display_state[i]=ascii_lookup[(unsigned char)start[i+(string_count/SCROLL_RATE)]];
		}

		string_count+=string_add;
		if (string_count>((str_len-6)*SCROLL_RATE)) string_add=-string_add;
		if (string_count<=0) string_add=-string_add;

		if (display_present) {
			update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
				display_state);
		}
		else {
			update_saa1064_ascii(display_state);
		}
	}

	/* clear display */
	for(i=0;i<8;i++) display_state[i]=0;

	if (display_present) {
		update_saa1064_display(meter_fd, SAA1064_ADDRESS1,
			display_state);
	}
	else {
		update_saa1064_ascii(display_state);
	}

	fclose(input);
	fclose(output);

	return 0;
}
