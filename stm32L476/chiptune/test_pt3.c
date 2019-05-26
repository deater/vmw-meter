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
#include "pt3_lib.h"

#define VERSION "0.0.9"

static const int DEBUG = 0;

#define DEVICE_NAME "/dev/dsp"

#define FREQ	44100
#define CHANS	2
#define BITS	16

static ayemu_ay_t ay;
//static ayemu_ay_reg_frame_t regs;

static int audio_fd;

static int  freq = FREQ;
static int  chans = CHANS;
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


static ayemu_ay_t ay;
static struct pt3_song_t pt3,pt3_2;
static ayemu_ay_reg_frame_t frame;


struct pt3_image_t pt3_image= {
	.data=__EA_PT3,	.length=__EA_PT3_len, };

static int line=0,subframe=0,current_pattern=0;

/* mono (2 channel), 16-bit (2 bytes), play at 50Hz */
#define AUDIO_BUFSIZ (FREQ*CHANS*(BITS/8) / 50)
static unsigned char audio_buf[AUDIO_BUFSIZ];
static int output_pointer=0;

void play (void) {

	int len,line_decode_result=0;

	pt3_load_song("ignored", &pt3_image, &pt3, &pt3_2);
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
		output_pointer=0;

		if ((len = write(audio_fd, audio_buf, AUDIO_BUFSIZ)) == -1) {
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


int main (int argc, char **argv) {

	init_oss();
	if (DEBUG) {
		printf ("OSS sound system initialization success: "
		"bits=%d, chans=%d, freq=%d\n",bits, chans, freq);
	}

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

	play ();

	return 0;
}
