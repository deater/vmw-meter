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



/* Is this standard for pt3?  I guess on spectrum files? */
static int external_frequency=1773400;
//static int bpm=120;
//static int tempo=6;
//static int frames_per_line=6;   /* frames per tracker line */
//static int frames_per_whole=96; /* numer of frames per whole note */

static int frames=0;

struct ym_header {
        char id[4];                             // 0  -> 4
        char check[8];                          // 4  -> 12
        uint32_t vbl;                           // 12 -> 16
        uint32_t song_attr;                     // 16 -> 20
        uint16_t digidrum;                      // 20 -> 22
        uint32_t external_frequency;            // 22 -> 26
        uint16_t player_frequency;              // 26 -> 28
        uint32_t loop;                          // 28 -> 32
        uint16_t additional_data;               // 32 -> 34
}  __attribute__((packed)) our_header;


#define HEADER_SIZE 0xCB
#define MAX_PT3_SIZE	65536
#define MAX_PT3_STRING	256


struct note_type {
	char which;
	int note;
	int sample;
	int ornament;
	int volume;
	int spec_command;
	int spec_delay;
	int spec_hi;
	int spec_lo;

	int len;
	int len_count;

	int all_done;

        int ornament_pointer;
        int ornament_length;
        int ornament_loop;
        int ornament_position;

        int sample_pointer;
        int sample_length;
        int sample_loop;
        int sample_position;

        int envelope_enabled;

	int amplitude;
        int amplitude_sliding;
        int noise_sliding;
        int envelope_sliding;

        int tone_slide_count;
        int tone_sliding;
	int tone_slide_step;
	int tone_slide_delay;
	int tone_delta;
	int slide_to_note;

	int simplegliss;

        int tone_accumulator;
        int onoff;
	int onoff_delay;
	int offon_delay;
	int tone;
        int enabled;
};

struct pt3_song_t {
	char magic[13+1];
	char version;
	char name[32+1];
	char author[32+1];
	int frequency_table;
	int speed;
	int num_patterns;
	int loop;
	unsigned short pattern_loc;
	unsigned short sample_patterns[32];
	unsigned short ornament_patterns[16];
	unsigned short pattern_order;
	unsigned short a_addr,b_addr,c_addr;
	struct note_type a,b,c;
	struct note_type a_old,b_old,c_old;
	int music_len;
	int current_pattern;

	int envelope_type;
	int envelope_type_old;
	int envelope_period;
	int envelope_period_old;
	int envelope_slide;
	int envelope_slide_add;
	int envelope_add;
	int envelope_delay;
	int envelope_delay_orig;
	int mixer_value;

	int noise_period;
	int noise_base;
	int noise_add;

	unsigned char data[MAX_PT3_SIZE];
};




static int pt3_load_header(int verbose, struct pt3_song_t *pt3) {

	int i;

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
	pt3->version=pt3->data[0xd];

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


/*
 Loop: 0 Length: 18
                        80 8f 00 00
                        80 8f 00 00
                        00 8e 00 00
                        00 8e 00 00
                        00 8d 00 00
                        00 8d 00 00
                        00 8c 00 00
                        00 8c 00 00
                        00 8b 00 00
                        00 8b 00 00
                        00 8a 00 00
                        00 8a 00 00
                        00 89 00 00
                        00 89 00 00
                        00 88 00 00
                        00 88 00 00
                        00 87 00 00
                        00 87 00 00

V			01 00 00 00
			c1 00 00 00

*/

static void calculate_note(struct note_type *a, struct pt3_song_t *pt3) {
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
		a->tone = pt3->data[a->sample_pointer + a->sample_position * 4 + 2];
		a->tone += (pt3->data[a->sample_pointer + a->sample_position * 4 + 3])<<8;
		a->tone += a->tone_accumulator;

		b0 = pt3->data[a->sample_pointer + a->sample_position * 4];
		b1 = pt3->data[a->sample_pointer + a->sample_position * 4 + 1];

		if ((b1 & 0x40) != 0) {
			a->tone_accumulator=a->tone;
		}
		j = a->note + ((pt3->data[a->ornament_pointer + a->ornament_position]<<24)>>24);
//		if (a->which=='C') printf("VMW: ORN %x %x[%x]=%x j=%x\n",
//			a->note,a->ornament_pointer,a->ornament_position,
//				pt3->data[a->ornament_pointer+a->ornament_position],j);
		if (j < 0) j = 0;
                else if (j > 95) j = 95;
		w = GetNoteFreq(j,pt3->frequency_table);

		a->tone = (a->tone + a->tone_sliding + w) & 0xfff;
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

		a->amplitude= (b1 & 0xf);

		if (a->which=='B') {
			printf("VMWB: sample=%d ptr=%x b0=%x\n",
				a->sample,a->sample_pointer,b0);
		}

		if ((b0 & 0x80)!=0) {
			if ((b0&0x40)!=0) {
				if (a->which=='B') printf("VMWB: C0 amp sliding %d\n",a->amplitude_sliding);
				if (a->amplitude_sliding < 15) {
					a->amplitude_sliding++;
				}
			}
			else {
				if (a->which=='B') printf("VMWB: 80 amp sliding %d\n",a->amplitude_sliding);
				if (a->amplitude_sliding > -15) {
					a->amplitude_sliding--;
				}
			}
		}
		a->amplitude+=a->amplitude_sliding;
		if (a->which=='B') printf("VMWB: amp sliding %d AMP=%x\n",
				a->amplitude_sliding,a->amplitude);

		if (a->amplitude < 0) a->amplitude = 0;
		else if (a->amplitude > 15) a->amplitude = 15;

//              if PlParams.PT3.PT3_Version <= 4 {
//			a->amplitude = PT3VolumeTable_33_34[a->volume][a->amplitude];
//              }
//              else {
			a->amplitude = PT3VolumeTable_35[a->volume][a->amplitude];
//              }

		if (((b0 & 0x1) == 0) && ( a->envelope_enabled)) {
			a->amplitude |= 16;
		}

		/* Frequency slide */
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
		else {
			pt3->noise_add = (b0>>1) + a->noise_sliding;
			if ((b1 & 0x20) != 0) {
				a->noise_sliding = pt3->noise_add;
			}
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

static void decode_note(struct note_type *a,
			unsigned short *addr,
			struct pt3_song_t *pt3) {

	int a_done=0;
	int current_val;
	int prev_note;
//	int prev_sliding;

	a->spec_command=0;
	a->spec_delay=0;
	a->spec_lo=0;

	/* Skip decode if note still running */
	if (a->len_count>1) {
		a->len_count--;
		return;
	}

	prev_note=a->note;
//	prev_sliding=a->tone_sliding;

	while(1) {
		a->len_count=a->len;

		current_val=pt3->data[*addr];
		//printf("%02X\n",current_val);

		switch((current_val>>4)&0xf) {
			case 0:
				if (current_val==0x0) {
					printf("ALL DONE %c\n",a->which);
					a->all_done=1;
					a_done=1;
				}
				else if (current_val==0x1) {
					/* tone down */
					a->spec_command=0x1;
				}
				else if (current_val==0x2) {
					/* port */
					a->spec_command=0x3;
				}
				else if (current_val==0x8) {
					a->spec_command=0x9;
				}
				else if (current_val==0x9) {
					a->spec_command=0xb;
				}
				else printf("%c UNKNOWN %02X\n",a->which,current_val);
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

				a->sample_pointer=pt3->sample_patterns[a->sample];
				printf("0x1: Sample pointer %d %x\n",a->sample,a->sample_pointer);
				a->sample_loop=pt3->data[a->sample_pointer];
				a->sample_pointer++;
				a->sample_length=pt3->data[a->sample_pointer];
				a->sample_pointer++;
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
				printf("VMW4: ornament=%x\n",current_val&0xf);
				a->ornament=(current_val&0xf);
                                a->ornament_pointer=pt3->ornament_patterns[a->ornament];
                                a->ornament_loop=pt3->data[a->ornament_pointer];
                                a->ornament_pointer++;
                                a->ornament_length=pt3->data[a->ornament_pointer];
                                a->ornament_pointer++;
				a->ornament_position=0;
				break;
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 0xa:
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
					a->sample_pointer=pt3->sample_patterns[a->sample];
					printf("0xd: sample %d sample pointer %x\n",
						a->sample,a->sample_pointer);
					a->sample_loop=pt3->data[a->sample_pointer];
					a->sample_pointer++;
					a->sample_length=pt3->data[a->sample_pointer];
					a->sample_pointer++;
				}
				break;
			case 0xe:
				a->sample=(current_val-0xd0);
				a->sample_pointer=pt3->sample_patterns[a->sample];
				printf("0xe: sample %d sample pointer %x\n",
					a->sample,a->sample_pointer);
				a->sample_loop=pt3->data[a->sample_pointer];
				a->sample_pointer++;
				a->sample_length=pt3->data[a->sample_pointer];
				a->sample_pointer++;

				break;
			case 0xf:
//               Envelope=15, Ornament=low byte, Sample=arg1/2
                                a->envelope_enabled=0;
				printf("VMWf: ornament=%x\n",current_val&0xf);
				a->ornament=(current_val&0xf);

                                a->ornament_pointer=pt3->ornament_patterns[a->ornament];
                                a->ornament_loop=pt3->data[a->ornament_pointer];
                                a->ornament_pointer++;
                                a->ornament_length=pt3->data[a->ornament_pointer];
                                a->ornament_pointer++;

				(*addr)++;
				current_val=pt3->data[*addr];

				a->sample=current_val/2;
				a->sample_pointer=pt3->sample_patterns[a->sample];
				printf("0xf: sample pointer[%d] %x\n",
						a->sample,
						a->sample_pointer);
                                a->sample_loop=pt3->data[a->sample_pointer];
                                a->sample_pointer++;
                                a->sample_length=pt3->data[a->sample_pointer];
                                a->sample_pointer++;

				break;
		}

		(*addr)++;
		/* Note, the AY code has code to make sure these are applied */
		/* In the same order they appear.  We don't bother? */
		if (a_done) {
			printf("VMW DONE %d\n",a->spec_command);
			if (a->spec_command==0x1) {
				current_val=pt3->data[(*addr)];
				a->spec_delay=current_val;
				a->tone_slide_delay=current_val;
				a->tone_slide_count=a->tone_slide_delay;

				(*addr)++;
				current_val=pt3->data[(*addr)];
				a->spec_lo=(current_val);

				(*addr)++;
				current_val=pt3->data[(*addr)];
				a->spec_hi=(current_val);

				a->tone_slide_step=(a->spec_lo)|(a->spec_hi<<8);
				printf("TONE_SLIDE %x\n",a->tone_slide_step);
				a->simplegliss=1;
				a->onoff=0;

				(*addr)++;
			}
			/* port */
			if (a->spec_command==0x3) {
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
				printf("VMW: slide_step: %x delta %x sliding %x\n",
					a->tone_slide_step,a->tone_delta,
					a->tone_sliding);
//				if (PlParams.PT3.PT3_Version >= 6) {
//					a->tone_sliding = PrSliding;
				if ((a->tone_delta - a->tone_sliding) < 0) {
					a->tone_slide_step = -a->tone_slide_step;
				}
				printf("VMW: slide count: %d newslidestep: %x\n",
					a->tone_slide_count,a->tone_slide_step);
			}

			if (a->spec_command==0xb) {
				current_val=pt3->data[(*addr)];
				a->spec_lo=current_val;
				pt3->speed=current_val;
				(*addr)++;
			}
			if (a->spec_command==0x9) {

				/* delay? */
				current_val=pt3->data[(*addr)];
				a->spec_delay=current_val&0xf;
				(*addr)++;

				/* Low? */
				current_val=pt3->data[(*addr)];
				a->spec_lo=current_val&0xf;
				(*addr)++;

				/* High? */
				current_val=pt3->data[(*addr)];
				a->spec_hi=current_val&0xf;
				(*addr)++;
			}
			break;
		}
	}

}

static void print_note(int which, struct pt3_song_t *pt3) {

	struct note_type *a, *a_old;

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
        if (a->note==a_old->note) printf("---");
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
	if (a->envelope_enabled) printf("%X",pt3->envelope_type);
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

unsigned char frame[16];





void dump_header(struct pt3_song_t *pt3) {

	int i,j,addr,loop,len;

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

	/* copy in the header data */
	result=pt3_load_header(1,pt3);
	if (result) {
		fprintf(stderr,"Error decoding header!\n");
		return -1;
	}

	memset(&pt3->a,0,sizeof(struct note_type));
	memset(&pt3->b,0,sizeof(struct note_type));
	memset(&pt3->c,0,sizeof(struct note_type));
	pt3->a.which='A';
	pt3->b.which='B';
	pt3->c.which='C';

	memset(&pt3->a_old,0,sizeof(struct note_type));
	memset(&pt3->b_old,0,sizeof(struct note_type));
	memset(&pt3->c_old,0,sizeof(struct note_type));

	/* Some defaults */
	pt3->speed=3;

	dump_header(pt3);

	return 0;
}

void pt3_make_frame(struct pt3_song_t *pt3, unsigned char *frame) {

	/* R0 = A period low */
	/* R1 = A period high */
	/* R2 = B period low */
	/* R3 = B period high */
	/* R4 = C period low */
	/* R5 = C period high */
	/* R6 = Noise period */
	/* R7 = Enable XX Noise=!CBA Tone=!CBA */
	/* R8 = Channel A amplitude M3210 */
	/* R9 = Channel B amplitude M3210 */
	/* R10 = Channel C amplitude M3210 */
	/* R11 = Envelope Period L */
	/* R12 = Envelope Period H */
	/* R13 = Envelope Shape */
	/* R14/R15 = I/O (ignored) */

	int temp_envelope;

	/* clear out frame */
	memset(frame,0,16);

	pt3->mixer_value=0;
	pt3->envelope_add=0;

	calculate_note(&pt3->a,pt3);
	calculate_note(&pt3->b,pt3);
	calculate_note(&pt3->c,pt3);

//	if (a.enabled) {
		frame[0]=pt3->a.tone&0xff;
		frame[1]=(pt3->a.tone>>8)&0xff;
//	}
//	if (b.enabled) {
		frame[2]=pt3->b.tone&0xff;
		frame[3]=(pt3->b.tone>>8)&0xff;
//	}
//	if (c.enabled) {
		frame[4]=pt3->c.tone&0xff;
		frame[5]=(pt3->c.tone>>8)&0xff;
//	}

	/* Noise */
	frame[6]= (pt3->noise_base+pt3->noise_add)&0x1f;

	frame[7]=pt3->mixer_value;

//	if (a.enabled) {
		frame[8]=pt3->a.amplitude;
//	}
//	if (b.enabled) {
		frame[9]=pt3->b.amplitude;
//	}
//	if (c.enabled) {
		frame[10]=pt3->c.amplitude;
//	}

	/* Envelope period */

	temp_envelope=pt3->envelope_period+
			pt3->envelope_add+
			pt3->envelope_slide;
	frame[11]=(temp_envelope&0xff);
	frame[12]=(temp_envelope>>8);

	printf("VMW ENV %x, period=%x add=%x slide=%x\n",
			temp_envelope,
			pt3->envelope_period,pt3->envelope_add,pt3->envelope_slide);

	/* Envelope shape */
	if (pt3->envelope_type==pt3->envelope_type_old) {
		frame[13]=0xff;
	}
	else {
		frame[13]=pt3->envelope_type;
	}
	pt3->envelope_type_old=pt3->envelope_type;

	if (pt3->envelope_delay > 0) {
		pt3->envelope_delay--;
		if (pt3->envelope_delay==0) {
			pt3->envelope_delay=pt3->envelope_delay_orig;
			pt3->envelope_slide+=pt3->envelope_slide_add;
		}
	}
}

int main(int argc, char **argv) {

	char filename[BUFSIZ];
	char out_filename[BUFSIZ];
	int out_fd;
	int result;
	struct pt3_song_t pt3;

	int ym5_header_length;
	char ym5_comment[]="VMW pt3_to_ym5";
	int ym5_digidrums=0;
	int ym5_attributes=0;
	int ym5_irq=50;

	int i,j,f;

	/******************************/
	/* Get command line arguments */
	/******************************/

	if (argc>1) {
		strncpy(filename,argv[1],BUFSIZ-1);
	}
	else {
		strncpy(filename,"ea.pt3",BUFSIZ-1);
	}

	strncpy(out_filename,"out.ym",BUFSIZ-1);


	/* Load song */
	result=pt3_load_song(filename, &pt3);
	if (result<0) {
		fprintf(stderr,"Error opening file %s\n",filename);
		return -1;
	}

	/* Open output file */
	out_fd=open(out_filename,O_WRONLY|O_CREAT,0666);
	if (out_fd<0) {
		fprintf(stderr,"Error opening %s\n",out_filename);
		return -1;
	}

	/* Start filling in the ym file */

	/* Skip header, we'll fill in later */
	ym5_header_length=sizeof(struct ym_header)+
		strlen(pt3.name)+1+
		strlen(pt3.author)+1+
		strlen(ym5_comment)+1;
	lseek(out_fd, ym5_header_length, SEEK_SET);



	pt3.noise_period=0;

	for(i=0;i < pt3.music_len;i++) {
		pt3.current_pattern=pt3.data[0xc9+i]/3;
		printf("Chunk %d/%d, 00:00/00:00, Pattern #%d\n",
			i,pt3.music_len-1,pt3.current_pattern);

		pt3.a_addr=pt3.data[(pt3.current_pattern*6)+0+pt3.pattern_loc] |
			(pt3.data[(pt3.current_pattern*6)+1+pt3.pattern_loc]<<8);

		pt3.b_addr=pt3.data[(pt3.current_pattern*6)+2+pt3.pattern_loc] |
			(pt3.data[(pt3.current_pattern*6)+3+pt3.pattern_loc]<<8);

		pt3.c_addr=pt3.data[(pt3.current_pattern*6)+4+pt3.pattern_loc] |
			(pt3.data[(pt3.current_pattern*6)+5+pt3.pattern_loc]<<8);

		printf("a_addr: %04x, b_addr: %04x, c_addr: %04x\n",
				pt3.a_addr,pt3.b_addr,pt3.c_addr);

		printf("Cdata: ");
		for(j=0;j<32;j++) printf("%02x ",pt3.data[pt3.c_addr+j]);
		printf("\n");

		for(j=0;j<64;j++) {

			printf("VMW frame: %d\n",frames);

			decode_note(&pt3.a,&(pt3.a_addr),&pt3);
			decode_note(&pt3.b,&(pt3.b_addr),&pt3);
			decode_note(&pt3.c,&(pt3.c_addr),&pt3);


			if (pt3.a.all_done && pt3.b.all_done && pt3.c.all_done) {
				break;
			}


			/* Print line of tracker */

			/* line */
			printf("%02x|",j);

			/* envelope */
			if ((pt3.envelope_period>>8)==0) printf("..");
			else printf("%02X",pt3.envelope_period>>8);
			if (pt3.envelope_period&0xff) printf("..");
			else printf("%02X",pt3.envelope_period&0xff);

			/* noise */
			printf("|");
			if (pt3.noise_period==0) printf("..");
			else printf("%02X",pt3.noise_period);
			printf("|");

			print_note('A',&pt3);
			print_note('B',&pt3);
			print_note('C',&pt3);

			memcpy(&pt3.a_old,&pt3.a,sizeof(struct note_type));
			memcpy(&pt3.b_old,&pt3.b,sizeof(struct note_type));
			memcpy(&pt3.c_old,&pt3.c,sizeof(struct note_type));
			pt3.envelope_period_old=(pt3.envelope_period);

			for(f=0;f<pt3.speed;f++) {
				pt3_make_frame(&pt3,frame);

				write(out_fd,frame,16);
				frames++;
			}

			printf("\n");
		}
	}


	/* Finish out ym5 file */

	/* Print End! marker */
	write(out_fd,"End!",4);

	/* Go back and print header */

	lseek(out_fd,0,SEEK_SET);

	strncpy(our_header.id,"YM5!",5);
        strncpy(our_header.check,"LeOnArD!",9);
        our_header.vbl=htonl(frames);
        our_header.song_attr=htonl(ym5_attributes);
        our_header.digidrum=htonl(ym5_digidrums);
        our_header.external_frequency=htonl(external_frequency);
        our_header.player_frequency=htons(ym5_irq);
        our_header.loop=htonl(pt3.loop);
        our_header.additional_data=htons(0);

        write(out_fd,&our_header,sizeof(struct ym_header));

        write(out_fd,pt3.name,strlen(pt3.name));
	write(out_fd,"\0",1);
        write(out_fd,pt3.author,strlen(pt3.author));
	write(out_fd,"\0",1);
        write(out_fd,ym5_comment,strlen(ym5_comment));
	write(out_fd,"\0",1);


	close(out_fd);

	return 0;

}
