#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "ayemu.h"

#include "i2_pt3.h"
#include "pt3_lib.h"

static const int DEBUG = 0;

#define FREQ	44100
#define CHANS	1
#define BITS	16

static int  freq = FREQ;
static int  chans = 2; // force stereo CHANS;
static int  bits = BITS;



#define PCM_DEVICE "default"

snd_pcm_t *pcm_handle;

/* ALSA init code based on example from Alessandro Ghedini */
int init_alsa(void) {

	unsigned int result, tmp;
	unsigned int rate, channels;
	snd_pcm_hw_params_t *params;

	rate 	 = freq;
	channels = chans;

	/* Open the PCM device in playback mode */
	result = snd_pcm_open(&pcm_handle, PCM_DEVICE,
					SND_PCM_STREAM_PLAYBACK, 0);
	if (result<0) {
		fprintf(stderr,"ERROR: Can't open \"%s\" PCM device. %s\n",
					PCM_DEVICE, snd_strerror(result));
	}

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
	result = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED);
	if (result<0) {
		fprintf(stderr,"ERROR: Can't set interleaved mode. %s\n", snd_strerror(result));
	}

	/* set to signed 16-bit little endian */
	result = snd_pcm_hw_params_set_format(pcm_handle, params,
						SND_PCM_FORMAT_S16_LE);
	if (result<0) {
		fprintf(stderr,"ERROR: Can't set format. %s\n", snd_strerror(result));
	}

	/* set to two channels */
	result = snd_pcm_hw_params_set_channels(pcm_handle, params, channels);
	if (result<0) {
		fprintf(stderr,"ERROR: Can't set channels number. %s\n", snd_strerror(result));
	}

	/* Set rate */
	result = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0);
	if (result < 0) {
		fprintf(stderr, "ERROR: Can't set rate. %s\n", snd_strerror(result));
	}

	/* Write parameters */
	result = snd_pcm_hw_params(pcm_handle, params);
	if (result<0) {
		fprintf(stderr, "ERROR: Can't set harware parameters. %s\n", snd_strerror(result));
	}

	/* Resume information */
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1) {
		printf("(mono)\n");
	}
	else if (tmp == 2) {
		printf("(stereo)\n");
	}
	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);

	return 0;
}


static ayemu_ay_t ay,ay2;
static struct pt3_song_t pt3,pt3_2;
static ayemu_ay_reg_frame_t frame,frame2;

struct pt3_image_t pt3_image;

static int line=0,subframe=0,current_pattern=0;

/* mono (2 channel), 16-bit (2 bytes), play at 50Hz */
#define AUDIO_BUFSIZ (FREQ*CHANS*(BITS/8) / 50)
static unsigned char audio_buf[AUDIO_BUFSIZ];
static unsigned char audio_buf2[AUDIO_BUFSIZ];
static unsigned char audio_combined[AUDIO_BUFSIZ*2];

static int output_pointer=0;

void play (void) {

	int line_decode_result=0,i;
	int result;

	pt3_load_song(&pt3_image, &pt3, &pt3_2);
	current_pattern=0;
	line=0;
	subframe=0;

	while(1) {

		if (current_pattern==pt3.music_len) {
			break;
		}

		/* Decode next frame */
		if ((line==0) && (subframe==0)) {
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

//		if ((len = write(audio_fd, audio_combined, AUDIO_BUFSIZ*2)) == -1) {
//			fprintf (stderr, "Error writting to sound device, break.\n");
//			break;
//		}

#if 0
	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	buff_size = frames * channels * 2 /* 2 -> sample size */;
	buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);

	for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) {

		result = read(0, buff, buff_size);
		if (result == 0) {
			printf("Early end of file.\n");
			return 0;
		}
#endif
		result = snd_pcm_writei(pcm_handle, audio_combined, AUDIO_BUFSIZ/2);

//		result = snd_pcm_writei(pcm_handle, buff, frames);
		if (result == -EPIPE) {
			printf("XRUN.\n");
			snd_pcm_prepare(pcm_handle);
		} else if (result < 0) {
			printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(result));
		}

	}
#if 0
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(buff);


	}
#endif


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

	init_alsa();
	if (DEBUG) {
		printf ("ALSA sound system initialization success: "
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

	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);

	return 0;
}
