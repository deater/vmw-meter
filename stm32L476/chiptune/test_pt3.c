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

#define VERSION "0.0.9"

static const int DEBUG = 0;

#define DEVICE_NAME "/dev/dsp"

static ayemu_ay_t ay;
static ayemu_ay_reg_frame_t regs;

static void *audio_buf;
static int audio_bufsize;
static int audio_fd;

static int  freq = 44100;
static int  chans = 2;
static int  bits = 16;

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

void play (void) {

  int len;
#if 0
  int audio_bufsize = freq * chans * (bits >> 3) / vtx->playerFreq;
  if ((audio_buf = malloc (audio_bufsize)) == NULL) {
    fprintf (stderr, "Can't allocate sound buffer\n");
    goto free_vtx;
  }

  ayemu_reset(&ay);
  ayemu_set_chip_type(&ay, vtx->chiptype, NULL);
  ayemu_set_chip_freq(&ay, vtx->chipFreq);
  ayemu_set_stereo(&ay, vtx->stereo, NULL);

  size_t pos = 0;

  while (pos++ < vtx->frames) {
    ayemu_vtx_getframe (vtx, pos, regs);
    ayemu_set_regs (&ay, regs);
    ayemu_gen_sound (&ay, audio_buf, audio_bufsize);
    if ((len = write(audio_fd, audio_buf, audio_bufsize)) == -1) {
      fprintf (stderr, "Error writting to sound device, break.\n");
      break;
    }
  }

 free_vtx:
  ayemu_vtx_free(vtx);
#endif
}


int main (int argc, char **argv) {

	init_oss();
	if (DEBUG) {
		printf ("OSS sound system initialization success: "
		"bits=%d, chans=%d, freq=%d\n",bits, chans, freq);
	}

	ayemu_init(&ay);
	ayemu_set_sound_format(&ay, freq, chans, bits);

	play ();

	return 0;
}
