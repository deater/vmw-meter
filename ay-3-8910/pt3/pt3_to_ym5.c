/* Some code based on Formats.pas in Bulba's ay_emul */

/* Convert pt3 file to ym file */


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


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

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



struct pt3_header {
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
} header;

#define HEADER_SIZE 0xCB
#define MAX_PT3_SIZE	65536

static unsigned char raw_header[HEADER_SIZE];
static unsigned char pt3_data[MAX_PT3_SIZE];

unsigned char *aptr,*bptr,*cptr;
unsigned short a_addr,b_addr,c_addr;

static int music_len=0,current_pattern=0;

static int load_header(void) {

	int i;

	/* Magic */
	memcpy(&header.magic,&raw_header[0],13);
	if (!memcmp(header.magic,"ProTracker 3.",13)) {
		printf("Found ProTracker 3.");
	}
	else if (!memcmp(header.magic,"Vortex Tracke",13)) {
		printf("Found Vortex Tracke");
	} else {
		fprintf(stderr,"Wrong magic %s != %s\n",
			header.magic,"ProTracker 3.");

		return -1;
	}

	/* version */
	header.version=raw_header[0xd];

	/* Name */
	memcpy(&header.name,&raw_header[0x1e],32);

	/* Author */
	memcpy(&header.author,&raw_header[0x42],32);

	/* Frequency Table */
	header.frequency_table=raw_header[0x63];

	/* Speed */
	header.speed=raw_header[0x64];

	/* Number of Patterns */
	header.num_patterns=raw_header[0x65]+1;

	/* Loop Pointer */
	header.loop=raw_header[0x66];

	/* Pattern Position */
	header.pattern_loc=(raw_header[0x68]<<8)|raw_header[0x67];

	/* Sample positions */
	for(i=0;i<32;i++) {
		header.sample_patterns[i]=
			(raw_header[0x6a+(i*2)]<<8)|raw_header[0x69+(i*2)];
	}

	/* Ornament Positions */
	for(i=0;i<16;i++) {
		header.ornament_patterns[i]=
			(raw_header[0xaa+(i*2)]<<8)|raw_header[0xa9+(i*2)];
	}

	/* Pattern Order */
	header.pattern_order=(raw_header[0xca]<<8)|raw_header[0xc9];

	return 0;

}

struct note_type {
	char which;
	int note;
	int sample;
	int envelope;
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

static int envelope_period_h=0;
static int envelope_period_l=0;
static int envelope_period_h_old=0;
static int envelope_period_l_old=0;
static int envelope_slide=0;
static int envelope_add=0;
static int envelope_delay=0;
static int mixer_value=0;

static int noise_period=0;
static int noise_base=0;
static int noise_add=0;

static int delay=6;



int GetNoteFreq(int j) {

 // case RAM.PT3_TonTableId of
//  0:if PlParams.PT3.PT3_Version <= 3 then
 //    Result := PT3NoteTable_PT_33_34r[j]
 //   else
 //    Result := PT3NoteTable_PT_34_35[j];
 // 1:
	return PT3NoteTable_ST[j];
//  2:if PlParams.PT3.PT3_Version <= 3 then
//     Result := PT3NoteTable_ASM_34r[j]
//    else
 //    Result := PT3NoteTable_ASM_34_35[j];
 // else if PlParams.PT3.PT3_Version <= 3 then
 //       Result := PT3NoteTable_REAL_34r[j]
 //      else
 //       Result := PT3NoteTable_REAL_34_35[j]
 // end
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

static void calculate_note(struct note_type *a) {
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
		a->tone = pt3_data[a->sample_pointer + a->sample_position * 4 + 2];
		a->tone += (pt3_data[a->sample_pointer + a->sample_position * 4 + 3])<<8;
		a->tone += a->tone_accumulator;

		b0 = pt3_data[a->sample_pointer + a->sample_position * 4];
		b1 = pt3_data[a->sample_pointer + a->sample_position * 4 + 1];

		if ((b1 & 0x40) != 0) {
			a->tone_accumulator=a->tone;
		}

		j = a->note + pt3_data[a->ornament_pointer + a->ornament_position];
		if (j < 0) j = 0;
                else if (j > 95) j = 95;
		w = GetNoteFreq(j);

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

		if (a->which=='C') {
			printf("VMWC: sample=%d ptr=%x b0=%x\n",
				a->sample,a->sample_pointer,b0);
		}

		if ((b0 & 0x80)!=0) {
			if ((b0&0x40)!=0) {
				if (a->which=='C') printf("VMWC: C0 amp sliding %d\n",a->amplitude_sliding);
				if (a->amplitude_sliding < 15) {
					a->amplitude_sliding++;
				}
			}
			else {
				if (a->which=='C') printf("VMWC: 80 amp sliding %d\n",a->amplitude_sliding);
				if (a->amplitude_sliding > -15) {
					a->amplitude_sliding--;
				}
			}
		}
		printf("VMW: amp sliding %d\n",a->amplitude_sliding);
		a->amplitude+=a->amplitude_sliding;

		if (a->amplitude < 0) a->amplitude = 0;
		else if (a->amplitude > 15) a->amplitude = 15;

//              if PlParams.PT3.PT3_Version <= 4 {
			a->amplitude = PT3VolumeTable_33_34[a->volume][a->amplitude];
//              }
//              else {
//			amplitude = PT3VolumeTable_35[a->volume][a->amplitude];
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
                                j = ((b0>>1)|0xF) + a->envelope_sliding;
                        }

                        if (( b1 & 0x20) != 0) {
                                a->envelope_sliding = j;
                        }
			envelope_add+=j;
		}
		else {
			noise_add = (b0>>1) + a->noise_sliding;
			if ((b1 & 0x20) != 0) {
				a->noise_sliding = noise_add;
			}
		}

		mixer_value = ((b1 >>1) & 0x48) | mixer_value;

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

        mixer_value=mixer_value>>1;

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
			unsigned short *addr) {

	int a_done=0;
	int current_val;
	int prev_note,prev_sliding;

	a->spec_command=0;
	a->spec_delay=0;
	a->spec_lo=0;
	a->envelope=0;

	/* Skip decode if note still running */
	if (a->len_count>1) {
		a->len_count--;
		return;
	}

	prev_note=a->note;
	prev_sliding=a->tone_sliding;

	while(1) {
		a->len_count=a->len;

		current_val=pt3_data[*addr];
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
					a->envelope=(current_val&0xf);

					(*addr)++;
					current_val=pt3_data[*addr];
					envelope_period_h=current_val;

					(*addr)++;
					current_val=pt3_data[(*addr)];
					envelope_period_l=current_val;

					a->envelope_enabled=1;
					envelope_slide=0;
					envelope_delay=0;
				}
				(*addr)++;
				current_val=pt3_data[(*addr)];
				a->sample=(current_val/2);

				a->sample_pointer=header.sample_patterns[a->sample];
				printf("0x1: Sample pointer %d %x\n",a->sample,a->sample_pointer);
				a->sample_loop=pt3_data[a->sample_pointer];
				a->sample_pointer++;
				a->sample_length=pt3_data[a->sample_pointer];
				a->sample_pointer++;
				a->ornament_position=0;

				break;
			case 2:
				/* Reset noise? */
				noise_period=(current_val&0xf);
//				if (current_val==0x20) {
//					noise_period=0;
//				}
//				else {
//					printf("UNKNOWN %02X\n",current_val);
//				}
				break;
			case 3:
				noise_period=(current_val&0xf)+0x10;
				break;
			case 4:
				if (a->envelope==0) a->envelope=0xf;
				a->ornament=(current_val&0xf);
                                a->ornament_pointer=header.ornament_patterns[a->ornament];
                                a->ornament_loop=pt3_data[a->ornament_pointer];
                                a->ornament_pointer++;
                                a->ornament_length=pt3_data[a->ornament_pointer];
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
					current_val=pt3_data[(*addr)];
					a->len=current_val;
					a->len_count=a->len;
				}
				else {
					a->envelope_enabled=1;
					a->envelope=(current_val&0xf)-1;

					(*addr)++;
					current_val=pt3_data[(*addr)];
					envelope_period_h=current_val;

					(*addr)++;
					current_val=pt3_data[(*addr)];
					envelope_period_l=current_val;

					a->ornament_position=0;
					envelope_slide=0;
					envelope_delay=0;

				}
				break;
			case 0xc:	/* volume */
				if ((current_val&0xf)==0) {
					a->note=0xff;

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
					a->sample_pointer=header.sample_patterns[a->sample];
					printf("0xd: sample %d sample pointer %x\n",
						a->sample,a->sample_pointer);
					a->sample_loop=pt3_data[a->sample_pointer];
					a->sample_pointer++;
					a->sample_length=pt3_data[a->sample_pointer];
					a->sample_pointer++;
				}
				break;
			case 0xe:
				a->sample=(current_val-0xd0);
				a->sample_pointer=header.sample_patterns[a->sample];
				printf("0xe: sample %d sample pointer %x\n",
					a->sample,a->sample_pointer);
				a->sample_loop=pt3_data[a->sample_pointer];
				a->sample_pointer++;
				a->sample_length=pt3_data[a->sample_pointer];
				a->sample_pointer++;

				break;
			case 0xf:
//               Envelope=15, Ornament=low byte, Sample=arg1/2
				a->envelope=0xf;
                                a->envelope_enabled=0;
				a->ornament=(current_val&0xf);

                                a->ornament_pointer=header.ornament_patterns[a->ornament];
                                a->ornament_loop=pt3_data[a->ornament_pointer];
                                a->ornament_pointer++;
                                a->ornament_length=pt3_data[a->ornament_pointer];
                                a->ornament_pointer++;

				(*addr)++;
				current_val=pt3_data[*addr];

				a->sample=current_val/2;
				a->sample_pointer=header.sample_patterns[a->sample];
				printf("0xf: sample pointer[%d] %x\n",
						a->sample,
						a->sample_pointer);
                                a->sample_loop=pt3_data[a->sample_pointer];
                                a->sample_pointer++;
                                a->sample_length=pt3_data[a->sample_pointer];
                                a->sample_pointer++;

				break;
		}

		(*addr)++;
		/* Note, the AY code has code to make sure these are applied */
		/* In the same order they appear.  We don't bother? */
		if (a_done) {
			printf("VMW DONE %d\n",a->spec_command);
			if (a->spec_command==0x1) {
				current_val=pt3_data[(*addr)];
				a->spec_delay=current_val;
				a->tone_slide_delay=current_val;
				a->tone_slide_count=a->tone_slide_delay;

				(*addr)++;
				current_val=pt3_data[(*addr)];
				a->spec_lo=(current_val);

				(*addr)++;
				current_val=pt3_data[(*addr)];
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

				current_val=pt3_data[(*addr)];
				a->spec_delay=current_val;

				a->tone_slide_delay=current_val;
				a->tone_slide_count=a->tone_slide_delay;

				(*addr)++;
				(*addr)++;
				(*addr)++;
				current_val=pt3_data[(*addr)];
				a->spec_lo=current_val;

				(*addr)++;
				current_val=pt3_data[(*addr)];
				a->spec_hi=current_val;

				(*addr)++;

				a->tone_slide_step=(a->spec_hi<<8)|(a->spec_lo);
				/* sign extend */
				a->tone_slide_step=(a->tone_slide_step<<16)>>16;
				/* abs() */
				if (a->tone_slide_step<0) a->tone_slide_step=-a->tone_slide_step;


				a->tone_delta=GetNoteFreq(a->note)-
					GetNoteFreq(prev_note);
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
				current_val=pt3_data[(*addr)];
				a->spec_lo=current_val;
                                delay=current_val;
				(*addr)++;
			}
			if (a->spec_command==0x9) {

				/* delay? */
				current_val=pt3_data[(*addr)];
				a->spec_delay=current_val&0xf;
				(*addr)++;

				/* Low? */
				current_val=pt3_data[(*addr)];
				a->spec_lo=current_val&0xf;
				(*addr)++;

				/* High? */
				current_val=pt3_data[(*addr)];
				a->spec_hi=current_val&0xf;
				(*addr)++;
			}
			break;
		}
	}

}

static void print_note(struct note_type *a, struct note_type *a_old) {
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
	if (a->envelope!=0) printf("%X",a->envelope);
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

int main(int argc, char **argv) {

	char filename[BUFSIZ];
	int i,j,f;
	int fd,out,addr;
	int result,sample_loop,sample_len;
	int digidrums=0;
	int attributes=0;
	int irq=50,loop=0;
	char comments[]="VMW pt3_to_ym5";
	int header_length;

	if (argc>1) {
		strncpy(filename,argv[1],BUFSIZ-1);
	}
	else {
		strncpy(filename,"ea.pt3",BUFSIZ-1);
	}

	fd=open(filename,O_RDONLY);
	if (fd<0) {
		fprintf(stderr,"Error opening %s: %s\n",
			filename,strerror(errno));
		return -1;

	}

	out=open("out.ym",O_WRONLY|O_CREAT,0666);
	if (out<0) {
		fprintf(stderr,"Error opening \n");
		return -1;
	}

	memset(&pt3_data,0,MAX_PT3_SIZE);

	result=read(fd,pt3_data,MAX_PT3_SIZE);
	if (result<0) {
		fprintf(stderr,"Error reading file: %s\n",
			strerror(errno));
		return -1;
	}

	close(fd);

	memcpy(&raw_header,&pt3_data,HEADER_SIZE);
	result=load_header();
	if (result) {
		fprintf(stderr,"Error decoding header!\n");
		return -1;
	}



	printf("\tNAME: %s\n",header.name);
	printf("\tBY  : %s\n",header.author);
	printf("\tFreqTable: %d Speed: %d  Patterns: %d Loop: %d\n",
			header.frequency_table,
			header.speed,
			header.num_patterns,
			header.loop);
	printf("\tPattern Location Offset: %04x\n",header.pattern_loc);

	/* Skip header, we'll fill in later */
	header_length=sizeof(struct ym_header)+
		strlen(header.name)+1+
		strlen(header.author)+1+
		strlen(comments)+1;

	lseek(out, header_length, SEEK_SET);

	printf("\tSample pattern addresses:");
		for(i=0;i<32;i++) {
			if (i%8==0) printf("\n\t\t");
			printf("%04x ",header.sample_patterns[i]);
		}
		printf("\n");
		printf("\tOrnament addresses:");
		for(i=0;i<16;i++) {
			if (i%8==0) printf("\n\t\t");
			printf("%04x ",header.ornament_patterns[i]);
		}
		printf("\n");
//		printf("\tPattern order @%04x\n",header.pattern_order);

		i=0;
		printf("\tPattern order:");
		while(1) {
			if (i%16==0) printf("\n\t\t");
			if (pt3_data[0xc9+i]==0xff) break;
			printf("%02d ",pt3_data[0xc9+i]/3);
			i++;
			music_len++;
		}
		printf("\n");

		printf("\tPattern Locations:\n");
		for(i=0;i<header.num_patterns;i++) {
			printf("\t\t%d (%4x):\t",i,(i*6)+header.pattern_loc);

			addr=pt3_data[(i*6)+0+header.pattern_loc] |
				(pt3_data[(i*6)+1+header.pattern_loc]<<8);
			printf("A: %04x ",addr);

			addr=pt3_data[(i*6)+2+header.pattern_loc] |
				(pt3_data[(i*6)+3+header.pattern_loc]<<8);
			printf("B: %04x ",addr);

			addr=pt3_data[(i*6)+4+header.pattern_loc] |
				(pt3_data[(i*6)+5+header.pattern_loc]<<8);
			printf("C: %04x ",addr);

			printf("\n");
		}

		printf("\tSample Dump:\n");
		for(i=0;i<32;i++) {
			printf("\t\t%i: ",i);
			if (header.sample_patterns[i]==0) printf("N/A\n");
			else {
				sample_loop=pt3_data[0+
						header.sample_patterns[i]];
				sample_len=pt3_data[1+
						header.sample_patterns[i]];
				printf("Loop: %d Length: %d\n",
						sample_loop,
						sample_len);
				for(j=0;j<sample_len;j++) {
					printf("\t\t\t%02x %02x %02x %02x\n",
						pt3_data[2+(j*4)+
						header.sample_patterns[i]],
						pt3_data[3+(j*4)+
						header.sample_patterns[i]],
						pt3_data[4+(j*4)+
						header.sample_patterns[i]],
						pt3_data[5+(j*4)+
						header.sample_patterns[i]]);
				}
			}
		}


		printf("\tOrnament Dump:\n");
		for(i=0;i<16;i++) {
			printf("\t\t%i: ",i);
			if (header.ornament_patterns[i]==0) printf("N/A\n");
			else {
				sample_loop=pt3_data[0+
						header.ornament_patterns[i]];
				sample_len=pt3_data[1+
						header.ornament_patterns[i]];
				printf("Loop: %d Length: %d\n\t\t\t",
						sample_loop,
						sample_len);
				for(j=0;j<sample_len;j++) {
					printf("%02x ",
						pt3_data[2+j+
						header.sample_patterns[i]]);
				}
				printf("\n");
			}
		}


	struct note_type a,b,c;
	struct note_type a_old,b_old,c_old;

	memset(&a,0,sizeof(struct note_type));
	memset(&b,0,sizeof(struct note_type));
	memset(&c,0,sizeof(struct note_type));
	a.which='A';
	b.which='B';
	c.which='C';

	memset(&a_old,0,sizeof(struct note_type));
	memset(&b_old,0,sizeof(struct note_type));
	memset(&c_old,0,sizeof(struct note_type));

	noise_period=0;

	for(i=0;i<music_len;i++) {
		current_pattern=pt3_data[0xc9+i]/3;
		printf("Chunk %d/%d, 00:00/00:00, Pattern #%d\n",
			i,music_len-1,current_pattern);

		a_addr=pt3_data[(current_pattern*6)+0+header.pattern_loc] |
			(pt3_data[(current_pattern*6)+1+header.pattern_loc]<<8);

		b_addr=pt3_data[(current_pattern*6)+2+header.pattern_loc] |
			(pt3_data[(current_pattern*6)+3+header.pattern_loc]<<8);

		c_addr=pt3_data[(current_pattern*6)+4+header.pattern_loc] |
			(pt3_data[(current_pattern*6)+5+header.pattern_loc]<<8);

		printf("a_addr: %04x, b_addr: %04x, c_addr: %04x\n",
				a_addr,b_addr,c_addr);

		aptr=&pt3_data[a_addr];
		bptr=&pt3_data[b_addr];
		cptr=&pt3_data[c_addr];

		printf("Cdata: ");
		for(j=0;j<32;j++) printf("%02x ",pt3_data[c_addr+j]);
		printf("\n");

		for(j=0;j<64;j++) {

			printf("VMW frame: %d\n",frames);


			envelope_period_h=0;
			envelope_period_l=0;

			decode_note(&a,&a_addr);
			decode_note(&b,&b_addr);
			decode_note(&c,&c_addr);


			if (a.all_done && b.all_done && c.all_done) {
				break;
			}


			/* Print line of tracker */

			/* line */
			printf("%02x|",j);

			/* envelope */
			if (envelope_period_h==0) printf("..");
			else printf("%02X",envelope_period_h);
			if (envelope_period_l==0) printf("..");
			else printf("%02X",envelope_period_l);

			/* noise */
			printf("|");
			if (noise_period==0) printf("..");
			else printf("%02X",noise_period);
			printf("|");

			print_note(&a,&a_old);
			print_note(&b,&b_old);
			print_note(&c,&c_old);

			memcpy(&a_old,&a,sizeof(struct note_type));
			memcpy(&b_old,&b,sizeof(struct note_type));
			memcpy(&c_old,&c,sizeof(struct note_type));
			envelope_period_h_old=envelope_period_h;
			envelope_period_l_old=envelope_period_l;

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

			for(f=0;f<delay;f++) {

				/* clear out frame */
				memset(frame,0,16);

				mixer_value=0;
				envelope_add=0;

				calculate_note(&a);
				calculate_note(&b);
				calculate_note(&c);

				if (a.enabled) {
//					frame[0]=PT3NoteTable_ST[a.note]&0xff;
//					frame[1]=(PT3NoteTable_ST[a.note]>>8)&0xff;
					frame[0]=a.tone&0xff;
					frame[1]=(a.tone>>8)&0xff;
				}
				if (b.enabled) {
//					frame[2]=PT3NoteTable_ST[b.note]&0xff;
//					frame[3]=(PT3NoteTable_ST[b.note]>>8)&0xff;
					frame[2]=b.tone&0xff;
					frame[3]=(b.tone>>8)&0xff;
				}
				if (c.enabled) {
//					frame[4]=PT3NoteTable_ST[c.note]&0xff;
//					frame[5]=(PT3NoteTable_ST[c.note]>>8)&0xff;
					frame[4]=c.tone&0xff;
					frame[5]=(c.tone>>8)&0xff;
				}

				/* Noise */
				frame[6]= (noise_base+noise_add)&0x1f;

				frame[7]=mixer_value;

				if (a.enabled) {
					frame[8]=a.amplitude;
				}
				if (b.enabled) {
					frame[9]=b.amplitude;
				}
				if (c.enabled) {
					frame[10]=c.amplitude;
				}

				/* Envelope period */
				frame[11]=0x0;
				frame[12]=0x0;

				/* Envelope shape */
				frame[13]=0xff;
#if 0
RegisterAY.Envelope := Env_Base.wrd + AddToEnv + Cur_Env_Slide;

  if Cur_Env_Delay > 0 then
   begin
    Dec(Cur_Env_Delay);
    if Cur_Env_Delay = 0 then
     begin
      Cur_Env_Delay := Env_Delay;
      Inc(Cur_Env_Slide,Env_Slide_Add)
     end
   end
#endif

				write(out,frame,16);
				frames++;
			}

			printf("\n");
		}
	}



	/* Print End! marker */
	write(out,"End!",4);

	/* Go back and print header */

	lseek(out,0,SEEK_SET);

	strncpy(our_header.id,"YM5!",5);
        strncpy(our_header.check,"LeOnArD!",9);
        our_header.vbl=htonl(frames);
        our_header.song_attr=htonl(attributes);
        our_header.digidrum=htonl(digidrums);
        our_header.external_frequency=htonl(external_frequency);
        our_header.player_frequency=htons(irq);
        our_header.loop=htonl(loop);
        our_header.additional_data=htons(0);

        write(out,&our_header,sizeof(struct ym_header));

        write(out,header.name,strlen(header.name));
	write(out,"\0",1);
        write(out,header.author,strlen(header.author));
	write(out,"\0",1);
        write(out,comments,strlen(comments));
	write(out,"\0",1);


	close(out);

	return 0;
}
