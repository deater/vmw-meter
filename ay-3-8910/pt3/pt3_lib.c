/* Some code based on Formats.pas in Bulba's ay_emul */

/* Convert pt3 file to ym file */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "pt3_lib.h"

static char note_names[96][4]={
	"C-1","C#1","D-1","D#1","E-1","F-1","F#1","G-1", // 50
	"G#1","A-1","A#1","B-1","C-2","C#2","D-2","D#2", // 58
	"E-2","F-2","F#2","G-2","G#2","A-2","A#2","B-2", // 60
	"C-3","C#3","D-3","D#3","E-3","F-3","F#3","G-3", // 68
	"G#3","A-3","A#3","B-3","C-4","C#4","D-4","D#4", // 70
	"E-4","F-4","F#4","G-4","G#4","A-4","A#4","B-4", // 78
	"C-5","C#5","D-5","D#5","E-5","F-5","F#5","G-5", // 80
	"G#5","A-5","A#5","B-5","C-6","C#6","D-6","D#6", // 88
	"E-6","F-6","F#6","G-6","G#6","A-6","A#6","B-6", // 90
	"C-7","C#7","D-7","D#7","E-7","F-7","F#7","G-7", // 98
	"G#7","A-7","A#7","B-7","C-8","C#8","D-8","D#8", // a0
	"E-8","F-8","F#8","G-8","G#8","A-8","A#8","B-8", // a8
};

/* Table #1 of Pro Tracker 3.3x - 3.5x */
static unsigned short PT3NoteTable_ST[]={
  0x0EF8,0x0E10,0x0D60,0x0C80,0x0BD8,0x0B28,0x0A88,0x09F0,  // 50
  0x0960,0x08E0,0x0858,0x07E0,0x077C,0x0708,0x06B0,0x0640,  // 58
  0x05EC,0x0594,0x0544,0x04F8,0x04B0,0x0470,0x042C,0x03FD,  // 60
  0x03BE,0x0384,0x0358,0x0320,0x02F6,0x02CA,0x02A2,0x027C,  // 68
  0x0258,0x0238,0x0216,0x01F8,0x01DF,0x01C2,0x01AC,0x0190,  // 70
  0x017B,0x0165,0x0151,0x013E,0x012C,0x011C,0x010A,0x00FC,  // 78
  0x00EF,0x00E1,0x00D6,0x00C8,0x00BD,0x00B2,0x00A8,0x009F,  // 80
  0x0096,0x008E,0x0085,0x007E,0x0077,0x0070,0x006B,0x0064,  // 88 (g#5-D#6)
  0x005E,0x0059,0x0054,0x004F,0x004B,0x0047,0x0042,0x003F,  // 90 (e6-b6)
  0x003B,0x0038,0x0035,0x0032,0x002F,0x002C,0x002A,0x0027,  // 98 (c7-g7)
  0x0025,0x0023,0x0021,0x001F,0x001D,0x001C,0x001A,0x0019,  // a0 (G#7-d#8)
  0x0017,0x0016,0x0015,0x0013,0x0012,0x0011,0x0010,0x000F   // a8 (e8-b8)
};

/* Table #2 of Pro Tracker 3.4x - 3.5x */
static unsigned short PT3NoteTable_ASM_34_35[] = {
  0x0D10,0x0C55,0x0BA4,0x0AFC,0x0A5F,0x09CA,0x093D,0x08B8,
  0x083B,0x07C5,0x0755,0x06EC,0x0688,0x062A,0x05D2,0x057E,
  0x052F,0x04E5,0x049E,0x045C,0x041D,0x03E2,0x03AB,0x0376,
  0x0344,0x0315,0x02E9,0x02BF,0x0298,0x0272,0x024F,0x022E,
  0x020F,0x01F1,0x01D5,0x01BB,0x01A2,0x018B,0x0174,0x0160,
  0x014C,0x0139,0x0128,0x0117,0x0107,0x00F9,0x00EB,0x00DD,
  0x00D1,0x00C5,0x00BA,0x00B0,0x00A6,0x009D,0x0094,0x008C,
  0x0084,0x007C,0x0075,0x006F,0x0069,0x0063,0x005D,0x0058,
  0x0053,0x004E,0x004A,0x0046,0x0042,0x003E,0x003B,0x0037,
  0x0034,0x0031,0x002F,0x002C,0x0029,0x0027,0x0025,0x0023,
  0x0021,0x001F,0x001D,0x001C,0x001A,0x0019,0x0017,0x0016,
  0x0015,0x0014,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,
};

unsigned char PT3VolumeTable_33_34[16][16]={
  {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
  {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
  {0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x2,0x2,0x2,0x2,0x2},
  {0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3},
  {0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x2,0x2,0x2,0x3,0x3,0x3,0x4,0x4,0x4},
  {0x0,0x0,0x0,0x1,0x1,0x1,0x2,0x2,0x3,0x3,0x3,0x4,0x4,0x4,0x5,0x5},
  {0x0,0x0,0x0,0x1,0x1,0x2,0x2,0x3,0x3,0x3,0x4,0x4,0x5,0x5,0x6,0x6},
  {0x0,0x0,0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x4,0x5,0x5,0x6,0x6,0x7,0x7},
  {0x0,0x0,0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x5,0x5,0x6,0x6,0x7,0x7,0x8},
  {0x0,0x0,0x1,0x1,0x2,0x3,0x3,0x4,0x5,0x5,0x6,0x6,0x7,0x8,0x8,0x9},
  {0x0,0x0,0x1,0x2,0x2,0x3,0x4,0x4,0x5,0x6,0x6,0x7,0x8,0x8,0x9,0xA},
  {0x0,0x0,0x1,0x2,0x3,0x3,0x4,0x5,0x6,0x6,0x7,0x8,0x9,0x9,0xA,0xB},
  {0x0,0x0,0x1,0x2,0x3,0x4,0x4,0x5,0x6,0x7,0x8,0x8,0x9,0xA,0xB,0xC},
  {0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x7,0x8,0x9,0xA,0xB,0xC,0xD},
  {0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE},
  {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF},
};



unsigned char PT3VolumeTable_35[16][16]={
  {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
  {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1},
  {0x0,0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x2,0x2,0x2,0x2},
  {0x0,0x0,0x0,0x1,0x1,0x1,0x1,0x1,0x2,0x2,0x2,0x2,0x2,0x3,0x3,0x3},
  {0x0,0x0,0x1,0x1,0x1,0x1,0x2,0x2,0x2,0x2,0x3,0x3,0x3,0x3,0x4,0x4},
  {0x0,0x0,0x1,0x1,0x1,0x2,0x2,0x2,0x3,0x3,0x3,0x4,0x4,0x4,0x5,0x5},
  {0x0,0x0,0x1,0x1,0x2,0x2,0x2,0x3,0x3,0x4,0x4,0x4,0x5,0x5,0x6,0x6},
  {0x0,0x0,0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x4,0x5,0x5,0x6,0x6,0x7,0x7},
  {0x0,0x1,0x1,0x2,0x2,0x3,0x3,0x4,0x4,0x5,0x5,0x6,0x6,0x7,0x7,0x8},
  {0x0,0x1,0x1,0x2,0x2,0x3,0x4,0x4,0x5,0x5,0x6,0x7,0x7,0x8,0x8,0x9},
  {0x0,0x1,0x1,0x2,0x3,0x3,0x4,0x5,0x5,0x6,0x7,0x7,0x8,0x9,0x9,0xA},
  {0x0,0x1,0x1,0x2,0x3,0x4,0x4,0x5,0x6,0x7,0x7,0x8,0x9,0xA,0xA,0xB},
  {0x0,0x1,0x2,0x2,0x3,0x4,0x5,0x6,0x6,0x7,0x8,0x9,0xA,0xA,0xB,0xC},
  {0x0,0x1,0x2,0x3,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xA,0xB,0xC,0xD},
  {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE},
  {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF},
};

static void pt3_load_ornament(struct pt3_song_t *pt3, int which) {

	struct pt3_note_type *a;

	if (which=='A') a=&(pt3->a);
	else if (which=='B') a=&(pt3->b);
	else if (which=='C') a=&(pt3->c);
	else return;

	a->ornament_pointer=pt3->ornament_patterns[a->ornament];
	a->ornament_loop=pt3->data[a->ornament_pointer];
	a->ornament_pointer++;
	a->ornament_length=pt3->data[a->ornament_pointer];
	a->ornament_pointer++;
}

static void pt3_load_sample(struct pt3_song_t *pt3, int which) {

	struct pt3_note_type *a;

	if (which=='A') a=&(pt3->a);
	else if (which=='B') a=&(pt3->b);
	else if (which=='C') a=&(pt3->c);
	else return;

	a->sample_pointer=pt3->sample_patterns[a->sample];
	a->sample_loop=pt3->data[a->sample_pointer];
	a->sample_pointer++;
	a->sample_length=pt3->data[a->sample_pointer];
	a->sample_pointer++;

}

static int pt3_load_header(int verbose, struct pt3_song_t *pt3) {

	int i;

	pt3->music_len=0;

	/* Magic: 13 bytes at offset 0 */
	memcpy(&(pt3->magic),&(pt3->data[0]),13);
	if (!memcmp(pt3->magic,"ProTracker 3.",13)) {
		printf("Found ProTracker 3.");
	}
	else if (!memcmp(pt3->magic,"Vortex Tracke",13)) {
		printf("Found Vortex Tracke");
	} else {
		fprintf(stderr,"Wrong magic %s != %s\n",
			pt3->magic,"ProTracker 3.");

		return -1;
	}

	/* version -- byte at offset 0xd */
	//pt3->version=pt3->data[0xd];
	pt3->version=6;
	if ((pt3->data[0xd]>='0') && (pt3->data[0xd]<='9')) {
		pt3->version=pt3->data[0xd]-'0';
	}

	/* Name -- 32 bytes at offset 0x1e */
	memcpy(&(pt3->name),&(pt3->data[0x1e]),32);

	/* Author -- 32 bytes at offset 0x42 */
	memcpy(&(pt3->author),&(pt3->data[0x42]),32);

	/* Frequency Table */
	pt3->frequency_table=pt3->data[0x63];

	/* Speed */
	pt3->speed=pt3->data[0x64];

	/* Number of Patterns */
	pt3->num_patterns=pt3->data[0x65]+1;

	/* Loop Pointer */
	pt3->loop=pt3->data[0x66];

	/* Pattern Position */
	pt3->pattern_loc=(pt3->data[0x68]<<8)|pt3->data[0x67];

	/* Sample positions */
	for(i=0;i<32;i++) {
		pt3->sample_patterns[i]=
			(pt3->data[0x6a+(i*2)]<<8)|pt3->data[0x69+(i*2)];
	}

	/* Ornament Positions */
	for(i=0;i<16;i++) {
		pt3->ornament_patterns[i]=
			(pt3->data[0xaa+(i*2)]<<8)|pt3->data[0xa9+(i*2)];
	}

	/* Pattern Order */
	pt3->pattern_order=(pt3->data[0xca]<<8)|pt3->data[0xc9];

	/* Calculate number of patterns in song */
	i=0;
	while(1) {
		if (pt3->data[0xc9+i]==0xff) break;
		i++;
		pt3->music_len++;
	}


	return 0;

}




static int GetNoteFreq(int j, int freq_table) {

 // case RAM.PT3_TonTableId of
//  0:if PlParams.PT3.PT3_Version <= 3 then
 //    Result := PT3NoteTable_PT_33_34r[j]
 //   else
 //    Result := PT3NoteTable_PT_34_35[j];
 // 1:
//	return PT3NoteTable_ST[j];
//  2:if PlParams.PT3.PT3_Version <= 3 then
//     Result := PT3NoteTable_ASM_34r[j]
//    else
 //    Result := PT3NoteTable_ASM_34_35[j];
 // else if PlParams.PT3.PT3_Version <= 3 then
 //       Result := PT3NoteTable_REAL_34r[j]
 //      else
 //       Result := PT3NoteTable_REAL_34_35[j]
 // end
	if (freq_table==1) {
		return PT3NoteTable_ST[j];
	}

	if (freq_table==2) {
		return PT3NoteTable_ASM_34_35[j];
	}

	return PT3NoteTable_ST[j];

}


static void calculate_note(struct pt3_note_type *a, struct pt3_song_t *pt3) {
        // XX YYYYY Z  = X= 10=VOLDOWN 11=VOLUP, Y=NOISE, Z= 0=ENV, 1=NO ENVELOPE
        // XX YY ZZZZ  = X= FREQ SLIDE YY=NOISE SLIDE ZZ=VOLUME
        // XXXXXXXX = LOW BYTE FREQ SLIDE
        // YYYYYYYY = HIGH BYTE FREQ SLIDE
        //
        // 80 8f 00 00
        // 1000 0000 -- VOLDOWN
        // 10 00 1111 --FREQ_SLIDE, VOLUME

	int j,b1,b0; // byte;
	int w;          // word;

	if (a->enabled) {

		/* Get first two bytes of 4-byte sample step */
		b0 = pt3->data[a->sample_pointer + a->sample_position * 4];
		b1 = pt3->data[a->sample_pointer + a->sample_position * 4 + 1];

		/* The next two bytes are the freq slide value? */
		a->tone = pt3->data[a->sample_pointer +
				a->sample_position*4 + 2];
		a->tone += (pt3->data[a->sample_pointer +
					a->sample_position*4+3])<<8;
		a->tone += a->tone_accumulator;

		/* Accumulate tone if set */
		if ((b1 & 0x40) != 0) {
			a->tone_accumulator=a->tone;
		}

		/* Get the Note and add in the ornament value */
		/* It's a single byte, sign-extend it */
		j = a->note + ((pt3->data[a->ornament_pointer +
				a->ornament_position]<<24)>>24);

		/* Make sure the note stays in bounds */
		if (j < 0) j = 0;
                else if (j > 95) j = 95;

		/* Look up the note in the frequency table */
		/* Which technically is a period table */
		w = GetNoteFreq(j,pt3->frequency_table);

		/* Take the sample tone, and combine with sliding */
		/* and ornament */
		a->tone = (a->tone + a->tone_sliding + w) & 0xfff;

		/* If we are sliding, handle the sliding part */
                if (a->tone_slide_count > 0) {
			a->tone_slide_count--;
			if (a->tone_slide_count==0) {
				a->tone_sliding+=a->tone_slide_step;
				a->tone_slide_count = a->tone_slide_delay;
				if (!a->simplegliss) {
					if ( ((a->tone_slide_step < 0) &&
					      (a->tone_sliding <= a->tone_delta)) ||
					     ((a->tone_slide_step >= 0) &&
				 	      (a->tone_sliding >= a->tone_delta)) ) {
						a->note = a->slide_to_note;
						a->tone_slide_count = 0;
						a->tone_sliding = 0;
					}
				}
			}
		}

		/* Calculate the amplitude */
		/* First get the value from the sample */

		if (a->which=='A') {
			printf("VMW sample=%x sample_pos=%x\n",a->sample,
				a->sample_position);
		}

		a->amplitude= (b1 & 0xf);

		if (a->which=='A') {
			printf("VMW sample=%x sample_pos=%x amp=%x\n",a->sample,
				a->sample_position,a->amplitude);
		}

		/* Top bit indicates sliding */
		if ((b0 & 0x80)!=0) {
			/* Next bit high (0b11) means slide up */
			if ((b0&0x40)!=0) {
				if (a->amplitude_sliding < 15) {
					a->amplitude_sliding++;
				}
			}
			/* Next bit low (0b10) means slide down */
			else {
				if (a->amplitude_sliding > -15) {
					a->amplitude_sliding--;
				}
			}
		}
		a->amplitude+=a->amplitude_sliding;

		if (a->which=='A') {
			printf("VMW amp_sliding=%x final=%x\n",a->amplitude_sliding,
				a->amplitude);
		}
		/* Make sure the amplitude stays in bounds */
		if (a->amplitude < 0) a->amplitude = 0;
		else if (a->amplitude > 15) a->amplitude = 15;

		if (a->which=='A') {
			printf("VMW version=%d %x\n",
				pt3->version,pt3->version);
		}

		if (pt3->version <= 4) {
			a->amplitude = PT3VolumeTable_33_34[a->volume]
							[a->amplitude];
		}
		else {
			/* This seems to be the more common case */
			a->amplitude = PT3VolumeTable_35[a->volume]
							[a->amplitude];
		}

		/* Bottom bit of b0 indicates our sample has envelope */
		/* Also make sure envelopes are enabled */
		/* Bit 4 of the AY-3-8910 amplitude specifies */
		/* envelope enabled */
		if (((b0 & 0x1) == 0) && ( a->envelope_enabled)) {
			a->amplitude |= 16;
		}

		/* Frequency slide */
		/* If b1 top bits are 10 or 11 */
                if ((b1 & 0x80) != 0) {
                        if ((b0 & 0x20) != 0) {
                                j = ((b0>>1)|0xF0) + a->envelope_sliding;
                        }
                        else {
                                j = ((b0>>1)&0xF) + a->envelope_sliding;
                        }

                        if (( b1 & 0x20) != 0) {
                                a->envelope_sliding = j;
                        }
			pt3->envelope_add+=j;
		}
		/* Noise slide */
		else {
//			printf("VMW before %c: %d %d b0=%x\n",a->which,pt3->noise_add,a->noise_sliding,b0);
			pt3->noise_add = (b0>>1) + a->noise_sliding;
			if ((b1 & 0x20) != 0) {
				a->noise_sliding = pt3->noise_add;
			}
//			printf("VMW after %c: noise_add=%d noise_sliding=%d\n",a->which,pt3->noise_add,a->noise_sliding);

		}

		pt3->mixer_value = ((b1 >>1) & 0x48) | pt3->mixer_value;

		a->sample_position++;
		if (a->sample_position >= a->sample_length) {
                        a->sample_position = a->sample_loop;
                }

		a->ornament_position++;
		if (a->ornament_position >= a->ornament_length) {
			a->ornament_position = a->ornament_loop;
                }

	} else {
		a->amplitude=0;
	}

        pt3->mixer_value=pt3->mixer_value>>1;

        if (a->onoff>0) {
                a->onoff--;
                if (a->onoff==0) {
                        a->enabled=!a->enabled;
                        if (a->enabled) a->onoff=a->onoff_delay;
                        else a->onoff=a->offon_delay;
                }
        }

}

static void decode_note(struct pt3_note_type *a,
			unsigned short *addr,
			struct pt3_song_t *pt3) {

	int a_done=0;
	int current_val;
	int prev_note;
	int prev_sliding;
	a->new_note=0;

	a->spec_command=0;
	a->spec_delay=0;
	a->spec_lo=0;

	/* Skip decode if note still running */
	if (a->len_count>1) {
		a->len_count--;
		return;
	}

	prev_note=a->note;
	prev_sliding=a->tone_sliding;

	while(1) {
		a->len_count=a->len;

		current_val=pt3->data[*addr];
		//printf("%02X\n",current_val);

		switch((current_val>>4)&0xf) {
			case 0:
				if (current_val==0x0) {
					printf("ALL DONE %c\n",a->which);
					a->len_count=0;
					a->all_done=1;
					a_done=1;
				}
				else {
					a->spec_command=current_val&0xf;
				}
				break;
			case 1:
				if ((current_val&0xf)==0x0) {
					a->envelope_enabled=0;
				}
				else {
					pt3->envelope_type_old=0x78;
					pt3->envelope_type=(current_val&0xf);

					(*addr)++;
					current_val=pt3->data[*addr];
					pt3->envelope_period=(current_val<<8);

					(*addr)++;
					current_val=pt3->data[(*addr)];
					pt3->envelope_period|=(current_val&0xff);

					a->envelope_enabled=1;
					pt3->envelope_slide=0;
					pt3->envelope_delay=0;
				}
				(*addr)++;
				current_val=pt3->data[(*addr)];
				a->sample=(current_val/2);
				pt3_load_sample(pt3,a->which);
//				printf("0x1: Sample pointer %d %x\n",a->sample,a->sample_pointer);
				a->ornament_position=0;

				break;
			case 2:
				/* Reset noise? */
				pt3->noise_period=(current_val&0xf);
//				if (current_val==0x20) {
//					noise_period=0;
//				}
//				else {
//					printf("UNKNOWN %02X\n",current_val);
//				}
				break;
			case 3:
				pt3->noise_period=(current_val&0xf)+0x10;
				break;
			case 4:
//				printf("VMW4: ornament=%x\n",current_val&0xf);
				a->ornament=(current_val&0xf);
				pt3_load_ornament(pt3,a->which);
				a->ornament_position=0;
				break;
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 0xa:
				a->new_note=1;
				a->note=(current_val-0x50);
                                a->sample_position=0;
                                a->amplitude_sliding=0;
                                a->noise_sliding=0;
                                a->envelope_sliding=0;
                                a->ornament_position=0;
                                a->tone_slide_count=0;
                                a->tone_sliding=0;
                                a->tone_accumulator=0;
                                a->onoff=0;
                                a->enabled=1;
				a_done=1;
				break;
			case 0xb:
				/* Disable envelope */
				if (current_val==0xb0) {
					a->envelope_enabled=0;
					a->ornament_position=0;
				}
				/* set len */
				else if (current_val==0xb1) {
					(*addr)++;
					current_val=pt3->data[(*addr)];
					a->len=current_val;
					a->len_count=a->len;
				}
				else {
					a->envelope_enabled=1;
					pt3->envelope_type_old=0x78;
					pt3->envelope_type=(current_val&0xf)-1;

					(*addr)++;
					current_val=pt3->data[(*addr)];
					pt3->envelope_period=(current_val<<8);

					(*addr)++;
					current_val=pt3->data[(*addr)];
					pt3->envelope_period|=(current_val&0xff);

					a->ornament_position=0;
					pt3->envelope_slide=0;
					pt3->envelope_delay=0;

				}
				break;
			case 0xc:	/* volume */
				if ((current_val&0xf)==0) {
					//a->note=0xff;

                                        a->sample_position=0;
                                        a->amplitude_sliding=0;
					pt3->noise_period=0; // one song needed this to match?
					a->noise_sliding=0;
                                        a->envelope_sliding=0;
                                        a->ornament_position=0;
                                        a->tone_slide_count=0;
                                        a->tone_sliding=0;
                                        a->tone_accumulator=0;
                                        a->onoff=0;
                                        a->enabled=0;
					a_done=1;
				}
                                else {
        				a->volume=current_val&0xf;
                                }
				break;
			case 0xd:
				if (current_val==0xd0) {
					a_done=1;
				}
				else {
					a->sample=(current_val&0xf);
					pt3_load_sample(pt3,a->which);
//					printf("0xd: sample %d sample pointer %x\n",
//						a->sample,a->sample_pointer);
				}
				break;
			case 0xe:
				a->sample=(current_val-0xd0);
				pt3_load_sample(pt3,a->which);
//				printf("0xe: sample %d sample pointer %x\n",
//					a->sample,a->sample_pointer);

				break;
			case 0xf:
//               Envelope=15, Ornament=low byte, Sample=arg1/2
                                a->envelope_enabled=0;
//				printf("VMWf: ornament=%x\n",current_val&0xf);
				a->ornament=(current_val&0xf);

				pt3_load_ornament(pt3,a->which);

				(*addr)++;
				current_val=pt3->data[*addr];

				a->sample=current_val/2;
				a->sample_pointer=pt3->sample_patterns[a->sample];
				pt3_load_sample(pt3,a->which);
//				printf("0xf: sample pointer[%d] %x\n",
//						a->sample,
//						a->sample_pointer);

				break;
		}

		(*addr)++;
		/* Note, the AY code has code to make sure these are applied */
		/* In the same order they appear.  We don't bother? */
		if (a_done) {
			int new_spec=0;

//			if (a->spec_command) printf("VMW: special command $%x\n",a->spec_command);
			if (a->spec_command==0x0) {
			}
			/* Tone Down */
			else if (a->spec_command==0x1) {
				current_val=pt3->data[(*addr)];
				a->spec_delay=current_val;
				a->tone_slide_delay=current_val;
				a->tone_slide_count=a->tone_slide_delay;
				printf("VMW: SLIDE DELAY=%x\n",a->tone_slide_delay);

				(*addr)++;
				current_val=pt3->data[(*addr)];
				a->spec_lo=(current_val);

				(*addr)++;
				current_val=pt3->data[(*addr)];
				a->spec_hi=(current_val);

				a->tone_slide_step=(a->spec_lo)|(a->spec_hi<<8);
				/* Sign Extend */
				a->tone_slide_step=(a->tone_slide_step<<16)>>16;

				printf("VMW: TONE_SLIDE DELAY %x\n",a->tone_slide_delay);
				//printf("TONE_SLIDE %x\n",a->tone_slide_step);
				a->simplegliss=1;
				a->onoff=0;

				(*addr)++;

				printf("VMW: %x\n",a->tone_slide_step);

				/* in the tracker it's 1 */
				if (a->tone_slide_step>=0) new_spec=0x1;
				else {
					signed short new_parm;
					new_spec=0x2;
					new_parm=a->tone_slide_step;
					new_parm=-new_parm;
					a->spec_hi=new_parm>>8;
					a->spec_lo=new_parm&0xff;
				}
			}
			/* port */
			else if (a->spec_command==0x2) {
				a->simplegliss=0;
				a->onoff=0;

				current_val=pt3->data[(*addr)];
				a->spec_delay=current_val;

				a->tone_slide_delay=current_val;
				a->tone_slide_count=a->tone_slide_delay;

				(*addr)++;
				(*addr)++;
				(*addr)++;
				current_val=pt3->data[(*addr)];
				a->spec_lo=current_val;

				(*addr)++;
				current_val=pt3->data[(*addr)];
				a->spec_hi=current_val;

				(*addr)++;

				a->tone_slide_step=(a->spec_hi<<8)|(a->spec_lo);
				/* sign extend */
				a->tone_slide_step=(a->tone_slide_step<<16)>>16;
				/* abs() */
				if (a->tone_slide_step<0) a->tone_slide_step=-a->tone_slide_step;


				a->tone_delta=GetNoteFreq(a->note,pt3->frequency_table)-
					GetNoteFreq(prev_note,pt3->frequency_table);
				a->slide_to_note=a->note;
				a->note=prev_note;
//				printf("VMW: slide_step: %x delta %x sliding %x\n",
//					a->tone_slide_step,a->tone_delta,
//					a->tone_sliding);
				if (pt3->version >= 6) {
					a->tone_sliding = prev_sliding;
				}
				if ((a->tone_delta - a->tone_sliding) < 0) {
					a->tone_slide_step = -a->tone_slide_step;
				}
//				printf("VMW: slide count: %d newslidestep: %x\n",
//					a->tone_slide_count,a->tone_slide_step);
				/* In the tracker it's 3 */
				new_spec=0x3;

			}
			/* Position in Sample */
			else if (a->spec_command==0x3) {
				current_val=pt3->data[(*addr)];
				a->sample_position=current_val;
				(*addr)++;

				new_spec=0x4;
			}
			/* Position in Ornament */
			else if (a->spec_command==0x4) {
				current_val=pt3->data[(*addr)];
				a->ornament_position=current_val;
				(*addr)++;

				new_spec=0x5;
			}
			/* Vibrato */
			else if (a->spec_command==0x5) {
				current_val=pt3->data[(*addr)];
				a->onoff_delay=current_val;
				(*addr)++;
				current_val=pt3->data[(*addr)];
				a->offon_delay=current_val;
				(*addr)++;

				a->onoff=a->onoff_delay;
				a->tone_slide_count=0;
				a->tone_sliding=0;

				new_spec=0x6;
			}
			/* Envelope Down */
			else if (a->spec_command==0x8) {

				/* delay? */
				current_val=pt3->data[(*addr)];
				pt3->envelope_delay=current_val;
				pt3->envelope_delay_orig=current_val;
				a->spec_delay=current_val;
				(*addr)++;

				/* Low? */
				current_val=pt3->data[(*addr)];
				a->spec_lo=current_val&0xff;
				(*addr)++;

				/* High? */
				current_val=pt3->data[(*addr)];
				a->spec_hi=current_val&0xff;
				(*addr)++;

				pt3->envelope_slide_add=(a->spec_hi<<8)|(a->spec_lo&0xff);

//				printf("VMW: envelope down delay=%x slide=%x\n",
//					pt3->envelope_delay,
//					pt3->envelope_slide_add);

				/* in the tracker it's 9 */
				if (pt3->envelope_slide_add<0x8000) new_spec=0x9;
				else {
					signed short new_parm;
					new_spec=0xa;
					new_parm=pt3->envelope_slide_add;
					new_parm=-new_parm;
					a->spec_hi=new_parm>>8;
					a->spec_lo=new_parm&0xff;
				}
			}
			/* Set Speed */
			else  if (a->spec_command==0x9) {
				current_val=pt3->data[(*addr)];
				a->spec_lo=current_val;
				pt3->speed=current_val;
				(*addr)++;
				/* in tracker it's B */
				new_spec=0xb;
			}
			else {
				printf("%c UNKNOWN effect %02X\n",
					a->which,a->spec_command);
			}
			a->spec_command=new_spec;

			break;
		}
	}

}

static void print_note(int which, struct pt3_song_t *pt3,int line) {

	struct pt3_note_type *a, *a_old;

	if (which=='A') {
		a=&(pt3->a);
		a_old=&(pt3->a_old);
	}
	else if (which=='B') {
		a=&(pt3->b);
		a_old=&(pt3->b_old);
	}
	else if (which=='C') {
		a=&(pt3->c);
		a_old=&(pt3->c_old);
	}
	else {
		fprintf(stderr,"ERROR unknown note %c\n",which);
		exit(1);
	}




	/* A note */
	if (!a->new_note) printf("---");
//        if (a->note==a_old->note)
	else if (a->note==0xff) printf("R--");
	else printf("%s",note_names[a->note]);
	printf(" ");

	/* A sample */
	if (a->sample==a_old->sample) printf(".");
	else if (a->sample<16) printf("%X",a->sample);
	else printf("%c",(a->sample-10)+'A');

	/* A envelope */
//	if ((envelope_period_l==0) || (a->envelope==0)) printf(".");
//	else printf("%X",a->envelope);

//	if ((envelope_period_l==0) || (a->envelope==0)) printf(".");
	if ((a->new_note) && (a->envelope_enabled)) printf("%X",pt3->envelope_type);
	else printf(".");

	/* A ornament */
	if (a->ornament==0) printf(".");
	else if (a->ornament==a_old->ornament) printf(".");
	else printf("%X",a->ornament);

	/* A volume */
	if (a->volume==0) printf(".");
	else if (a->volume==a_old->volume) printf(".");
	else printf("%X",a->volume);

	/* Special */
	printf(" ");
	if (a->spec_command==0) printf(".");
	else printf("%X",a->spec_command);

	if (a->spec_delay==0) printf(".");
	else printf("%X",a->spec_delay);

	if ((a->spec_lo&0xf0)==0) printf(".");
	else printf("%X",(a->spec_lo>>4)&0xf);

	if (a->spec_lo==0) printf(".");
	else printf("%X",(a->spec_lo&0xf));

	printf("|");
}

void dump_header(struct pt3_song_t *pt3) {

	int i,j,addr,loop,len;

	printf("\tPT Version: %d\n",pt3->version);
	printf("\tNAME: %s\n",pt3->name);
	printf("\tBY  : %s\n",pt3->author);
	printf("\tFreqTable: %d Speed: %d  Patterns: %d Loop: %d\n",
			pt3->frequency_table,
			pt3->speed,
			pt3->num_patterns,
			pt3->loop);

	/**************************/
	/* Print pattern location */
	/**************************/
	printf("\tPattern Location Offset: %04x\n",pt3->pattern_loc);


	/**************************/
	/* Print Sample addresses */
	/**************************/
	printf("\tSample pattern addresses:");
	for(i=0;i<32;i++) {
		if (i%8==0) printf("\n\t\t");
		printf("%04x ",pt3->sample_patterns[i]);
	}
	printf("\n");

	/****************************/
	/* Print Ornamemt addresses */
	/****************************/
	printf("\tOrnament addresses:");
	for(i=0;i<16;i++) {
		if (i%8==0) printf("\n\t\t");
		printf("%04x ",pt3->ornament_patterns[i]);
	}
	printf("\n");
//	printf("\tPattern order @%04x\n",pt3->pattern_order);

	/**************************/
	/* Print Pattern Order    */
	/**************************/
	i=0;
	printf("\tPattern order:");
	while(1) {
		if (i%16==0) printf("\n\t\t");
		if (pt3->data[0xc9+i]==0xff) break;
		printf("%02d ",pt3->data[0xc9+i]/3);
		i++;
	}
	printf("\n");

	/***************************/
	/* Print Pattern addresses */
	/***************************/
	printf("\tPattern Locations:\n");
	for(i=0;i<pt3->num_patterns;i++) {
		printf("\t\t%d (%4x):\t",i,(i*6)+pt3->pattern_loc);

		addr=pt3->data[(i*6)+0+pt3->pattern_loc] |
			(pt3->data[(i*6)+1+pt3->pattern_loc]<<8);
		printf("A: %04x ",addr);

		addr=pt3->data[(i*6)+2+pt3->pattern_loc] |
			(pt3->data[(i*6)+3+pt3->pattern_loc]<<8);
		printf("B: %04x ",addr);

		addr=pt3->data[(i*6)+4+pt3->pattern_loc] |
			(pt3->data[(i*6)+5+pt3->pattern_loc]<<8);
		printf("C: %04x ",addr);

		printf("\n");
	}

	/**************************/
	/* Print Sample Data      */
	/**************************/
	printf("\tSample Dump:\n");
	for(i=0;i<32;i++) {
		printf("\t\t%i: ",i);
		if (pt3->sample_patterns[i]==0) printf("N/A\n");
		else {
			loop=pt3->data[0+pt3->sample_patterns[i]];
			len=pt3->data[1+pt3->sample_patterns[i]];
			printf("Loop: %d Length: %d\n",loop,len);
			for(j=0;j<len;j++) {
				printf("\t\t\t%02x %02x %02x %02x\n",
					pt3->data[2+(j*4)+
						pt3->sample_patterns[i]],
					pt3->data[3+(j*4)+
						pt3->sample_patterns[i]],
					pt3->data[4+(j*4)+
						pt3->sample_patterns[i]],
					pt3->data[5+(j*4)+
						pt3->sample_patterns[i]]);
			}
		}
	}


	/**************************/
	/* Print Ornament Data    */
	/**************************/
	printf("\tOrnament Dump:\n");
	for(i=0;i<16;i++) {
		printf("\t\t%i: ",i);
		if (pt3->ornament_patterns[i]==0) printf("N/A\n");
		else {
			loop=pt3->data[0+	pt3->ornament_patterns[i]];
			len=pt3->data[1+pt3->ornament_patterns[i]];
			printf("Loop: %d Length: %d\n\t\t\t",loop,len);
			for(j=0;j<len;j++) {
				printf("%02x ",
					pt3->data[2+j+
					pt3->ornament_patterns[i]]);
			}
			printf("\n");
		}
	}

}

int pt3_init_song(struct pt3_song_t *pt3) {

	int result;

	/* copy in the header data */
	result=pt3_load_header(1,pt3);
	if (result) {
		fprintf(stderr,"Error decoding header!\n");
		return -1;
	}

	memset(&pt3->a,0,sizeof(struct pt3_note_type));
	memset(&pt3->b,0,sizeof(struct pt3_note_type));
	memset(&pt3->c,0,sizeof(struct pt3_note_type));

	pt3->a.which='A';
	pt3->a.volume=15;
	pt3->a.tone_sliding=0;
	pt3->a.amplitude_sliding=0;
	pt3->a.enabled=0;
	pt3->a.envelope_enabled=0;
	pt3->a.ornament=0;
	pt3->a.ornament_position=0;
	pt3_load_ornament(pt3,'A');
	pt3->a.sample_position=0;
	pt3->a.sample=1;
	pt3_load_sample(pt3,'A');

	pt3->b.which='B';
	pt3->b.volume=15;
	pt3->b.tone_sliding=0;
	pt3->b.amplitude_sliding=0;
	pt3->b.enabled=0;
	pt3->b.envelope_enabled=0;
	pt3->b.ornament=0;
	pt3->b.ornament_position=0;
	pt3_load_ornament(pt3,'B');
	pt3->b.sample_position=0;
	pt3->b.sample=1;
	pt3_load_sample(pt3,'B');

	pt3->c.which='C';
	pt3->c.volume=15;
	pt3->c.tone_sliding=0;
	pt3->c.amplitude_sliding=0;
	pt3->c.enabled=0;
	pt3->c.envelope_enabled=0;
	pt3->c.ornament=0;
	pt3->c.ornament_position=0;
	pt3_load_ornament(pt3,'C');
	pt3->c.sample_position=0;
	pt3->c.sample=1;
	pt3_load_sample(pt3,'C');

	memset(&pt3->a_old,0,sizeof(struct pt3_note_type));
	memset(&pt3->b_old,0,sizeof(struct pt3_note_type));
	memset(&pt3->c_old,0,sizeof(struct pt3_note_type));

	/* Some defaults */
	pt3->noise_period=0;
	pt3->noise_add=0;
	pt3->envelope_period=0;
	pt3->envelope_type=0;
	pt3->envelope_type_old=0;
	pt3->current_pattern=0;

	return 0;
}

int pt3_load_song(char *filename, struct pt3_song_t *pt3) {

	int fd;
	int result;

	/* Clear out the struct */
	memset(pt3,0,sizeof(struct pt3_song_t));

	/* Open file */
	fd=open(filename,O_RDONLY);
	if (fd<0) {
		fprintf(stderr,"Error opening %s: %s\n",
			filename,strerror(errno));
		return -1;

	}

	/* Clear out our data */
	memset(&pt3->data,0,MAX_PT3_SIZE);

	/* Read entire file into memory (probably not that big) */
	result=read(fd,pt3->data,MAX_PT3_SIZE);
	if (result<0) {
		fprintf(stderr,"Error reading file: %s\n",
			strerror(errno));
		return -1;
	}

	/* close the file */
	close(fd);

	/* Init Data */
	result=pt3_init_song(pt3);
	if (result<0) return result;


	dump_header(pt3);

	return 0;
}

void pt3_make_frame(struct pt3_song_t *pt3, unsigned char *frame) {

	/* R0/R1 = A period low/high */
	/* R2/R3 = B period low/high */
	/* R4/R5 = C period low/high */
	/* R6 = Noise period */
	/* R7 Mixer Enable: XX Noise=!CBA Tone=!CBA */
	/* R8/R9/R10 = Channel A/B/C amplitude E3210 */
	/* R11/R12 = Envelope Period low/high */
	/* R13 = Envelope Shape (tracker: 0xff means no change) */
	/* R14/R15 = I/O (ignored) */

	int temp_envelope;

	/* clear out frame */
	memset(frame,0,16);

	pt3->mixer_value=0;
	pt3->envelope_add=0;

	calculate_note(&pt3->a,pt3);
	calculate_note(&pt3->b,pt3);
	calculate_note(&pt3->c,pt3);

	/* Set Period for the 3 channels */
	frame[0]=pt3->a.tone&0xff;
	frame[1]=(pt3->a.tone>>8)&0xff;
	frame[2]=pt3->b.tone&0xff;
	frame[3]=(pt3->b.tone>>8)&0xff;
	frame[4]=pt3->c.tone&0xff;
	frame[5]=(pt3->c.tone>>8)&0xff;

	/* Noise */
	frame[6]= (pt3->noise_period+pt3->noise_add)&0x1f;

	/* Mixer Value */
	frame[7]=pt3->mixer_value;

	/* Amplitude/Volume */
	frame[8]=pt3->a.amplitude;
	frame[9]=pt3->b.amplitude;
	frame[10]=pt3->c.amplitude;

	/* Envelope period */

	temp_envelope=pt3->envelope_period+
			pt3->envelope_add+
			pt3->envelope_slide;
	frame[11]=(temp_envelope&0xff);
	frame[12]=(temp_envelope>>8);

	/* Envelope shape */
	if (pt3->envelope_type==pt3->envelope_type_old) {
		frame[13]=0xff;
	}
	else {
		frame[13]=pt3->envelope_type;
	}
	pt3->envelope_type_old=pt3->envelope_type;

	/* Update envelope delay */
	if (pt3->envelope_delay > 0) {
		pt3->envelope_delay--;
		if (pt3->envelope_delay==0) {
			pt3->envelope_delay=pt3->envelope_delay_orig;
			pt3->envelope_slide+=pt3->envelope_slide_add;
		}
	}
}

void pt3_print_tracker_line(struct pt3_song_t *pt3, int line) {

	/* Print line of tracker */

	/* line */
	printf("%02x|",line);

	/* envelope */
	if ((pt3->envelope_period>>8)==0) printf("..");
	else printf("%02X",pt3->envelope_period>>8);
	if ((pt3->envelope_period&0xff)==0) printf("..");
	else printf("%02X",pt3->envelope_period&0xff);

	/* noise */
	printf("|");
	if (pt3->noise_period==0) printf("..");
	else printf("%02X",pt3->noise_period);
	printf("|");

	print_note('A',pt3,line);
	print_note('B',pt3,line);
	print_note('C',pt3,line);

	memcpy(&pt3->a_old,&pt3->a,sizeof(struct pt3_note_type));
	memcpy(&pt3->b_old,&pt3->b,sizeof(struct pt3_note_type));
	memcpy(&pt3->c_old,&pt3->c,sizeof(struct pt3_note_type));
	pt3->envelope_period_old=(pt3->envelope_period);
}


int pt3_decode_line(struct pt3_song_t *pt3) {

	decode_note(&pt3->a,&(pt3->a_addr),pt3);
	decode_note(&pt3->b,&(pt3->b_addr),pt3);
	decode_note(&pt3->c,&(pt3->c_addr),pt3);


	if (pt3->a.all_done && pt3->b.all_done && pt3->c.all_done) {
		return 1;
	}
	return 0;

}

void pt3_set_pattern(int i, struct pt3_song_t *pt3) {

	pt3->current_pattern=pt3->data[0xc9+i]/3;

	pt3->a_addr=pt3->data[(pt3->current_pattern*6)+0+pt3->pattern_loc] |
		(pt3->data[(pt3->current_pattern*6)+1+pt3->pattern_loc]<<8);

	pt3->b_addr=pt3->data[(pt3->current_pattern*6)+2+pt3->pattern_loc] |
		(pt3->data[(pt3->current_pattern*6)+3+pt3->pattern_loc]<<8);

	pt3->c_addr=pt3->data[(pt3->current_pattern*6)+4+pt3->pattern_loc] |
		(pt3->data[(pt3->current_pattern*6)+5+pt3->pattern_loc]<<8);

	pt3->a.all_done=0;
	pt3->b.all_done=0;
	pt3->c.all_done=0;

	pt3->noise_period=0;

}

static char empty_note[]="---";

char *pt3_current_note(int which, struct pt3_song_t *pt3) {

	if (which=='A') {
		if (pt3->a.volume) return note_names[pt3->a.note];
		else return empty_note;
	}

	if (which=='B') {
		if (pt3->b.volume) return note_names[pt3->b.note];
		else return empty_note;
	}

	if (which=='C') {
		if (pt3->c.volume) return note_names[pt3->c.note];
		else return empty_note;
	}

	return empty_note;
}

void pt3_calc_frames(struct pt3_song_t *pt3, int *total, int *loop) {

	int i,j,f;

	*total=0;

	for(i=0;i < pt3->music_len;i++) {

		if (i==pt3->loop) *loop=*total;

                pt3_set_pattern(i,pt3);

                for(j=0;j<64;j++) {
                        if (pt3_decode_line(pt3)) break;

                        for(f=0;f < pt3->speed;f++) {
				(*total)++;
                        }
		}
	}
	/* Re-init the song as finding these values upsets */
	/* the initial conditions */

	pt3_init_song(pt3);
}
