/* Play PT3 chiptunes on a Raspberry Pi */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "string.h"
//#include "stdlib.h"

//#include "lcd.h"

#include "pt3_lib.h"
#include "ayemu.h"

#define FREQ	44100
#define CHANS	1
#define BITS	8

/* global variables */
volatile uint32_t TimeDelay;
volatile uint32_t overflows=0;

static ayemu_ay_t ay;
static struct pt3_song_t pt3,pt3_2;

static ayemu_ay_reg_frame_t frame;
//static unsigned char frame[14];

#define MAX_SONGS 1
#include "i2_pt3.h"

static int which_song=0;
static int song_done=0;

void exit(int status) {
	while(1);
}

struct pt3_image_t pt3_image[MAX_SONGS] = {
	[0] = {	.data=__I2_PT3,	.length=__I2_PT3_len, },
};


static int line=0,subframe=0,current_pattern=0;

static void change_song(void) {

	which_song=0;

	pt3_load_song(&pt3_image[which_song], &pt3, &pt3_2);

	current_pattern=0;
	line=0;
	subframe=0;
}

/* mono (2 channel), 16-bit (2 bytes), play at 50Hz */
#define AUDIO_BUFSIZ (FREQ*CHANS*(BITS/8) / 50)
#define NUM_SAMPLES (AUDIO_BUFSIZ/CHANS/(BITS/8))
#define COUNTDOWN_RESET (FREQ/50)

static unsigned char audio_buf[AUDIO_BUFSIZ];
static int output_bufsize=8*1024*1024,totalsize=0;
static unsigned char *output_buffer;

/* Interrupt Handlers */
static void NextBuffer(int which_half) {

	int line_decode_result=0;

	/* Decode next frame */
	if ((line==0) && (subframe==0)) {
		if (current_pattern==pt3.music_len) {
			song_done=1;
			return;
		}
		pt3_set_pattern(current_pattern,&pt3);
	}

	if (subframe==0) {
		line_decode_result=pt3_decode_line(&pt3);
	}

	if (line_decode_result==1) {
		/* line done early? */
		current_pattern++;
		line=0;
		subframe=0;
	}
	else {
		subframe++;
		if (subframe==pt3.speed) {
			subframe=0;
			line++;
			if (line==64) {
				current_pattern++;
				line=0;
			}
		}
	}


	pt3_make_frame(&pt3,frame);

	/* Update AY buffer */
	ayemu_set_regs(&ay,frame);

	/* Generate sound buffer */
	ayemu_gen_sound (&ay, audio_buf, AUDIO_BUFSIZ);

}



int main(void) {

	/* Init first song */
	change_song();

	/* Init ay code */

	ayemu_init(&ay);
	// 44100, 1, 16 -- freq, channels, bits
	ayemu_set_sound_format(&ay, FREQ, CHANS, BITS);

	ayemu_reset(&ay);
	ayemu_set_chip_type(&ay, AYEMU_AY, NULL);
	/* Assume mockingboard/VMW-chiptune freq */
	/* pt3_lib assumes output is 1773400 of zx spectrum */
	ayemu_set_chip_freq(&ay, 1773400);
//	ayemu_set_chip_freq(&ay, 1000000);
	ayemu_set_stereo(&ay, AYEMU_MONO, NULL);

	output_buffer=calloc(output_bufsize,sizeof(unsigned char));
	if (output_buffer==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	while(!song_done) {
		NextBuffer(0);
		memcpy(output_buffer+totalsize,audio_buf,AUDIO_BUFSIZ);
		totalsize+=AUDIO_BUFSIZ;
	}


	printf("Total size=%d\n",totalsize);


	FILE *fff;

	fff=fopen("out.bin","w");
	if (fff==NULL) {
		fprintf(stderr,"Error opening\n");
		return -1;
	}
	fwrite(output_buffer,totalsize,1,fff);
	fclose(fff);
}


