/* padsp ./test_pt3 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#include "ayemu.h"

#include "ea_pt3.h"
#include "i2_pt3.h"
#include "pt3_lib.h"

#define VERSION "0.0.9"

static const int DEBUG = 0;

#define DEVICE_NAME "/dev/dsp"

#define FREQ	44100
#define CHANS	1
#define BITS	16

static int audio_fd;

static int  freq = FREQ;
static int  chans = 2; // force stereo CHANS;
static int  bits = BITS;

void init_oss(void)
{
  if ((audio_fd = open(DEVICE_NAME, O_WRONLY, 0)) == -1) {
    fprintf (stderr, "Can't open /dev/dsp\n");
  }
  else if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &bits) == -1) {
    fprintf (stderr, "Can't set sound format\n");
  }
  else if (ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &chans) == -1) {
    fprintf (stderr, "Can't set number of channels\n");
  }
  else if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &freq) == -1) {
    fprintf (stderr, "Can't set audio freq\n");
  }
  else
    return;

  exit(1);
}


static ayemu_ay_t ay,ay2;
static struct pt3_song_t pt3,pt3_2;
static ayemu_ay_reg_frame_t frame,frame2;

struct pt3_image_t pt3_image;


//struct pt3_image_t pt3_image= {
//	.data=__EA_PT3,	.length=__EA_PT3_len,
//};


static int line=0,subframe=0,current_pattern=0;

/* mono (2 channel), 16-bit (2 bytes), play at 50Hz */
#define AUDIO_BUFSIZ (FREQ*CHANS*(BITS/8) / 50)
static unsigned char audio_buf[AUDIO_BUFSIZ];
static unsigned char audio_buf2[AUDIO_BUFSIZ];
static unsigned char audio_combined[AUDIO_BUFSIZ*2];

static int output_pointer=0;

void play (void) {

	int len,line_decode_result=0,i;

	pt3_load_song(&pt3_image, &pt3, &pt3_2);
	current_pattern=0;
	line=0;
	subframe=0;

	while(1) {

		/* Decode next frame */
		if ((line==0) && (subframe==0)) {
			if (current_pattern==pt3.music_len) {
				exit(1);
			}
			pt3_set_pattern(current_pattern,&pt3);
			if (pt3_2.valid) pt3_set_pattern(current_pattern,&pt3_2);
		}

		if (subframe==0) {
			line_decode_result=pt3_decode_line(&pt3);
			if (pt3_2.valid) pt3_decode_line(&pt3_2);
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
		if (pt3_2.valid) {
			pt3_make_frame(&pt3_2,frame2);
		}


		/* Update AY buffer */
		ayemu_set_regs(&ay,frame);
		ayemu_set_regs(&ay2,frame2);

		/* Generate sound buffer */
		ayemu_gen_sound (&ay, audio_buf, AUDIO_BUFSIZ);
		if (pt3_2.valid) {
			ayemu_gen_sound (&ay2, audio_buf2, AUDIO_BUFSIZ);
		}

		output_pointer=0;

		for(i=0;i<AUDIO_BUFSIZ;i+=2) {
			audio_combined[i*2]=audio_buf[i];
			audio_combined[(i*2)+1]=audio_buf[i+1];
			if (pt3_2.valid) {
				audio_combined[(i*2)+2]=audio_buf2[i];
				audio_combined[(i*2)+3]=audio_buf2[i+1];
			}
			else {
				audio_combined[(i*2)+2]=audio_buf[i];
				audio_combined[(i*2)+3]=audio_buf[i+1];
			}
		}

		if ((len = write(audio_fd, audio_combined, AUDIO_BUFSIZ*2)) == -1) {
			fprintf (stderr, "Error writting to sound device, break.\n");
			break;
		}

	}


//  int audio_bufsize = freq * chans * (bits >> 3) / vtx->playerFreq;
//  if ((audio_buf = malloc (audio_bufsize)) == NULL) {
//    fprintf (stderr, "Can't allocate sound buffer\n");
//    goto free_vtx;
//  }
//

//  while (pos++ < vtx->frames) {
//    ayemu_vtx_getframe (vtx, pos, regs);
//    ayemu_set_regs (&ay, regs);
//    ayemu_gen_sound (&ay, audio_buf, audio_bufsize);

}

#define MAXFILESIZE 32*1024

int main (int argc, char **argv) {

	unsigned char *pt3_data;
	int fd,size;

	if (argc>2) {
		enable_emulate_bug();
	}

	if (argc<2) {
		pt3_image.data=__I2_PT3;
		pt3_image.length=__I2_PT3_len;
	}
	else {
		pt3_data=calloc(MAXFILESIZE,sizeof(unsigned char));
		if (pt3_data==NULL) {
			fprintf(stderr,"Error allocating space!\n");
			return -1;
		}
		fd=open(argv[1],O_RDONLY);
		if (fd<0) {
			fprintf(stderr,"Error opening file %s!\n",argv[1]);
			return -1;
		}
		size=read(fd,pt3_data,MAXFILESIZE);
		if (size==MAXFILESIZE) {
			fprintf(stderr,"File too big %s!\n",argv[1]);
			return -1;
		}
		if (size<0) {
			fprintf(stderr,"Error reading file %s!\n",argv[1]);
			return -1;
		}
		pt3_image.data=pt3_data;
		pt3_image.length=size;
	}

	init_oss();
	if (DEBUG) {
		printf ("OSS sound system initialization success: "
		"bits=%d, chans=%d, freq=%d\n",bits, chans, freq);
	}

	/* Init ay code */

	ayemu_init(&ay);
	ayemu_init(&ay2);
	// 44100, 1, 16 -- freq, channels, bits
	ayemu_set_sound_format(&ay, FREQ, CHANS, BITS);

	ayemu_reset(&ay);
	ayemu_set_chip_type(&ay, AYEMU_AY, NULL);

	ayemu_reset(&ay2);
	ayemu_set_chip_type(&ay2, AYEMU_AY, NULL);
	/* Assume mockingboard/VMW-chiptune freq */
	/* pt3_lib assumes output is 1773400 of zx spectrum */
	ayemu_set_chip_freq(&ay, 1773400);
//	ayemu_set_chip_freq(&ay, 1000000);
	ayemu_set_stereo(&ay, AYEMU_MONO, NULL);
	ayemu_set_chip_freq(&ay2, 1773400);
	ayemu_set_stereo(&ay2, AYEMU_MONO, NULL);

	play ();

	return 0;
}
