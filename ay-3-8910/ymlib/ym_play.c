#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stats.h"
#include "ym_lib.h"
#include "ay-3-8910.h"

int ym_play_frame(struct ym_song_t *ym_song, int frame_num, int shift_size,
			struct display_stats *ds,
			int diff_mode,
			int play_music,
			int mute_channel) {

	int j;

	unsigned char frame[YM5_FRAME_SIZE];
	unsigned char frame2[YM5_FRAME_SIZE];

	int left_a_period,left_b_period,left_c_period;
	int right_a_period,right_b_period,right_c_period;

	double left_a_freq=0.0, left_b_freq=0.0, left_c_freq=0.0;
	double right_a_freq=0.0, right_b_freq=0.0, right_c_freq=0.0;

	ym_make_frame(ym_song,ym_song->frame_data,frame_num,frame,0);

	if (ym_song->channels==3) {
		memcpy(frame2,frame,sizeof(frame));
	} else {
		ym_make_frame(ym_song,ym_song->frame_data2,frame_num,frame2,0);
	}

	left_a_period=((frame[1]&0xf)<<8)|frame[0];
	left_b_period=((frame[3]&0xf)<<8)|frame[2];
	left_c_period=((frame[5]&0xf)<<8)|frame[4];

	right_a_period=((frame2[1]&0xf)<<8)|frame2[0];
	right_b_period=((frame2[3]&0xf)<<8)|frame2[2];
	right_c_period=((frame2[5]&0xf)<<8)|frame2[4];

	if (left_a_period>0) left_a_freq=ym_song->master_clock/(16.0*(double)left_a_period);
	if (left_b_period>0) left_b_freq=ym_song->master_clock/(16.0*(double)left_b_period);
	if (left_c_period>0) left_c_freq=ym_song->master_clock/(16.0*(double)left_c_period);

	if (right_a_period>0) right_a_freq=ym_song->master_clock/(16.0*(double)right_a_period);
	if (right_b_period>0) right_b_freq=ym_song->master_clock/(16.0*(double)right_b_period);
	if (right_c_period>0) right_c_freq=ym_song->master_clock/(16.0*(double)right_c_period);

	if (mute_channel&0x1) frame[8]=0;
	if (mute_channel&0x2) frame[9]=0;
	if (mute_channel&0x4) frame[10]=0;
	if (mute_channel&0x8) frame[7]|=0x8;
	if (mute_channel&0x10) frame[7]|=0x10;
	if (mute_channel&0x20) frame[7]|=0x20;
	/* Mute envelope */
	if (mute_channel&0x40) {
		frame[8]&=0xf;
		frame[9]&=0xf;
		frame[10]&=0xf;
	}

	/* FIXME: do this right? */
	if (mute_channel&0x1) frame2[8]=0;
	if (mute_channel&0x2) frame2[9]=0;
	if (mute_channel&0x4) frame2[10]=0;
	if (mute_channel&0x8) frame2[7]|=0x8;
	if (mute_channel&0x10) frame2[7]|=0x10;
	if (mute_channel&0x20) frame2[7]|=0x20;
	/* Mute envelope */
	if (mute_channel&0x40) {
		frame2[8]&=0xf;
		frame2[9]&=0xf;
		frame2[10]&=0xf;
	}

	if (play_music) {
		for(j=0;j<13;j++) {
			write_ay_3_8910(j,frame[j],frame2[j],shift_size);
		}

		/* Special case.  Writing r13 resets it,	*/
		/* so special 0xff marker means do not write	*/

		/* FIXME: so what do we do if 2 channels have */
		/* different values? */
		/* We'll have to special case, and do a dummy write */
		/* to a non-13 address.  Should be possible but not */
		/* worth fixing unless it actually becomes a problem. */


		if ((frame[13]!=0xff) || (frame2[13]!=0xff)) {
			write_ay_3_8910(13,frame[13],frame2[13],shift_size);
		}
	}

	if (ds!=NULL) {
		ds->left_amplitude[0]=frame[8];
		ds->left_amplitude[1]=frame[9];
		ds->left_amplitude[2]=frame[10];

		ds->right_amplitude[0]=frame2[8];
		ds->right_amplitude[1]=frame2[9];
		ds->right_amplitude[2]=frame2[10];

		ds->left_freq[0]=left_a_freq;
		ds->left_freq[1]=left_b_freq;
		ds->left_freq[2]=left_c_freq;

		ds->right_freq[0]=right_a_freq;
		ds->right_freq[1]=right_b_freq;
		ds->right_freq[2]=right_c_freq;
	}

	return 0;

}

/* Play song on left, effects on right */
int ym_play_frame_effects(struct ym_song_t *ym_song,
			int frame_num, int shift_size,
			struct display_stats *ds,
			int play_music,
			unsigned char *frame2) {

	int j;

	unsigned char frame[YM5_FRAME_SIZE];

	int left_a_period,left_b_period,left_c_period;
	int right_a_period,right_b_period,right_c_period;

	double left_a_freq=0.0, left_b_freq=0.0, left_c_freq=0.0;
	double right_a_freq=0.0, right_b_freq=0.0, right_c_freq=0.0;

	if (ym_song) {
		ym_make_frame(ym_song,ym_song->frame_data,frame_num,frame,0);
	}
	else {
		memset(frame,0,16);
	}

	left_a_period=((frame[1]&0xf)<<8)|frame[0];
	left_b_period=((frame[3]&0xf)<<8)|frame[2];
	left_c_period=((frame[5]&0xf)<<8)|frame[4];

	right_a_period=((frame2[1]&0xf)<<8)|frame2[0];
	right_b_period=((frame2[3]&0xf)<<8)|frame2[2];
	right_c_period=((frame2[5]&0xf)<<8)|frame2[4];

	if (ym_song) {
	if (left_a_period>0) left_a_freq=ym_song->master_clock/(16.0*(double)left_a_period);
	if (left_b_period>0) left_b_freq=ym_song->master_clock/(16.0*(double)left_b_period);
	if (left_c_period>0) left_c_freq=ym_song->master_clock/(16.0*(double)left_c_period);

	if (right_a_period>0) right_a_freq=ym_song->master_clock/(16.0*(double)right_a_period);
	if (right_b_period>0) right_b_freq=ym_song->master_clock/(16.0*(double)right_b_period);
	if (right_c_period>0) right_c_freq=ym_song->master_clock/(16.0*(double)right_c_period);
	}
#if 0
	if (mute_channel&0x1) frame[8]=0;
	if (mute_channel&0x2) frame[9]=0;
	if (mute_channel&0x4) frame[10]=0;
	if (mute_channel&0x8) frame[7]|=0x8;
	if (mute_channel&0x10) frame[7]|=0x10;
	if (mute_channel&0x20) frame[7]|=0x20;
#endif

	if (play_music) {
		for(j=0;j<13;j++) {
			write_ay_3_8910(j,frame[j],frame2[j],shift_size);
		}

		/* Special case.  Writing r13 resets it,	*/
		/* so special 0xff marker means do not write	*/

		/* FIXME: so what do we do if 2 channels have */
		/* different values? */
		/* We'll have to special case, and do a dummy write */
		/* to a non-13 address.  Should be possible but not */
		/* worth fixing unless it actually becomes a problem. */


		if ((frame[13]!=0xff) || (frame2[13]!=0xff)) {
			write_ay_3_8910(13,frame[13],frame2[13],shift_size);
		}
	}

	if (ds!=NULL) {
		ds->left_amplitude[0]=frame[8];
		ds->left_amplitude[1]=frame[9];
		ds->left_amplitude[2]=frame[10];

		ds->left_freq[0]=left_a_freq;
		ds->left_freq[1]=left_b_freq;
		ds->left_freq[2]=left_c_freq;

		ds->right_amplitude[0]=frame2[8];
		ds->right_amplitude[1]=frame2[9];
		ds->right_amplitude[2]=frame2[10];

		ds->right_freq[0]=right_a_freq;
		ds->right_freq[1]=right_b_freq;
		ds->right_freq[2]=right_c_freq;
	}

	return 0;

}
