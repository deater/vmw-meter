/* pt3_lib:	decode a pt3 file to a stream of AY-3-8910 frames 	*/
/*		by Vince Weaver <vince@deater.net>			*/
/* Version 1.0	-- 17 October 2019					*/

/* Some code based on Formats.pas in Bulba's ay_emul 			*/

/* Version with full tables: 10304 bytes (gcc 9.2.1, x86)		*/
/*                            9544 bytes (no printfs)			*/
/*                            9304 bytes (z80 table)			*/
/*			      9104 bytes (opt z80)			*/
/*			      7464 bytes (gen freq tables)		*/

#include <stdint.h>

#ifdef PT3LIB_EMBEDDED
#include "string.h"
#include "stdlib.h"
#else
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include "pt3_lib.h"



static unsigned char PT3VolumeTable[16][16];


static void pt3_message(char *message) {
#ifdef PT3LIB_EMBEDDED
#else
	fprintf(stderr,"%s",message);
#endif
}

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
		//pt3_message("Found ProTracker 3.");
	}
	else if (!memcmp(pt3->magic,"Vortex Tracke",13)) {
		//pt3_message("Found Vortex Tracke");
	} else {
		pt3_message(pt3->magic);
		pt3_message("Wrong magic != ProTracker 3.\n");
		/* sometimes it is still a valid file... */
		//return -1;
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
	pt3->which_frequency_table=pt3->data[0x63];

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
	/* This isn't really necessary */
	i=0;
	while(1) {
		if (pt3->data[0xc9+i]==0xff) break;
		i++;
		pt3->music_len++;
	}


	return 0;

}

static void calculate_note(struct pt3_note_type *a, struct pt3_song_t *pt3) {
        // B0: XX YYYYY Z  = X= 10=VOLDOWN 11=VOLUP
	//                   Y=NOISE, Z= 0=ENV, 1=NO ENVELOPE
        // B1: X Y Z AAAA  = X= ENVELOPE or NOISE add
	//                   Z= also make sliding
	//                    if Envelope slide:
	//			 B0: Y YYYY = add/sub and slide value
	//                    if Noise slide:
	//                       B0: YYYY = noise add
        //     XXXXXXXX = LOW BYTE FREQ SLIDE
        //     YYYYYYYY = HIGH BYTE FREQ SLIDE
        //
        // 80 8f 00 00
        // 1000 0000 -- VOLDOWN
        // 10 00 1111 --FREQ_SLIDE, VOLUME

	int j,b1,b0; // byte;
	int w;          // word;

	if (a->enabled) {

		/* Get first two bytes of 4-byte sample step */
		b0 = pt3->data[a->sample_pointer +
					(short)(a->sample_position)* 4];
		b1 = pt3->data[a->sample_pointer +
					(short)(a->sample_position)* 4 + 1];

		/* The next two bytes are the freq slide value? */
		a->tone = pt3->data[a->sample_pointer +
				(a->sample_position)*4 + 2];
		a->tone += (pt3->data[a->sample_pointer +
					(a->sample_position)*4+3])<<8;
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
		w = pt3->frequency_table[j]; //GetNoteFreq(j,pt3);

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
		a->amplitude= (b1 & 0xf);

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

		/* Make sure the amplitude stays in bounds */
		if (a->amplitude < 0) a->amplitude = 0;
		else if (a->amplitude > 15) a->amplitude = 15;

		a->amplitude = PT3VolumeTable[a->volume]
						[(unsigned char)a->amplitude];

		/* Bottom bit of b0 indicates our sample has envelope */
		/* Also make sure envelopes are enabled */
		/* Bit 4 of the AY-3-8910 amplitude specifies */
		/* envelope enabled */
		if (((b0 & 0x1) == 0) && ( a->envelope_enabled)) {
			a->amplitude |= 16;
		}

		/* Envelope slide */
		/* If b1 top bits are 10 or 11 */
                if ((b1 & 0x80) != 0) {
			/* Slide down? Sign extend? */
			if ((b0 & 0x20) != 0) {
                                j = ((b0>>1)|0xF0);
				j= (j<<24)>>24;
				j+= a->envelope_sliding;
                        }
			/* Slide up by bottom 4 bits? */
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

static void decode_note(struct pt3_note_type *a,
			struct pt3_song_t *pt3) {

	int a_done=0;
	int current_val;
	unsigned char prev_note;
	signed short prev_sliding;
	int new_spec=0;

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

		current_val=pt3->data[a->addr];

		switch((current_val>>4)&0xf) {
			case 0:
				if (current_val==0x0) {
					//printf("ALL DONE %c\n",a->which);
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

					a->addr++;
					current_val=pt3->data[a->addr];
					pt3->envelope_period=(current_val<<8);

					a->addr++;
					current_val=pt3->data[a->addr];
					pt3->envelope_period|=
							(current_val&0xff);

					a->envelope_enabled=1;
					pt3->envelope_slide=0;
					pt3->envelope_delay=0;
				}
				a->addr++;
				current_val=pt3->data[a->addr];
				a->sample=(current_val/2);
				pt3_load_sample(pt3,a->which);
//				printf("0x1: Sample pointer %d %x\n",
//						a->sample,a->sample_pointer);
				a->ornament_position=0;

				break;
			case 2:
				pt3->noise_period=(current_val&0xf);
				break;
			case 3:
				pt3->noise_period=(current_val&0xf)+0x10;
				break;
			case 4:
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
				a->original_note=(current_val-0x50);
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
					a->addr++;
					current_val=pt3->data[a->addr];
					a->len=current_val;
					a->len_count=a->len;
				}
				else {
					a->envelope_enabled=1;
					pt3->envelope_type_old=0x78;
					pt3->envelope_type=(current_val&0xf)-1;

					a->addr++;
					current_val=pt3->data[a->addr];
					pt3->envelope_period=(current_val<<8);

					a->addr++;
					current_val=pt3->data[a->addr];
					pt3->envelope_period|=
							(current_val&0xff);

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
					// one song needed this to match?
					//pt3->noise_period=0;
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
//					printf("0xd: sample %d sample "
//						"pointer %x\n",
//						a->sample,a->sample_pointer);
				}
				break;
			case 0xe:
				a->sample=(current_val-0xd0);
				pt3_load_sample(pt3,a->which);
				break;
			case 0xf:
                                a->envelope_enabled=0;
				a->ornament_position=0;

				a->ornament=(current_val&0xf);

				pt3_load_ornament(pt3,a->which);

				a->addr++;
				current_val=pt3->data[a->addr];

				a->sample=current_val/2;
				pt3_load_sample(pt3,a->which);
				break;
		}

		a->addr++;
		/* Note, the AY code has code to make sure these are applied */
		/* In the same order they appear.  We don't bother? */
		if (a_done) {

			if (a->spec_command==0x0) {
			}
			/* Tone Down */
			else if (a->spec_command==0x1) {
				current_val=pt3->data[a->addr];
				a->spec_delay=current_val;
				a->tone_slide_delay=current_val;
				a->tone_slide_count=a->tone_slide_delay;

				a->addr++;
				current_val=pt3->data[a->addr];
				a->spec_lo=(current_val);

				a->addr++;
				current_val=pt3->data[a->addr];
				a->spec_hi=(current_val);

				a->tone_slide_step=(a->spec_lo)|(a->spec_hi<<8);
				/* Sign Extend */
				//a->tone_slide_step=
				//		(a->tone_slide_step<<16)>>16;

				a->simplegliss=1;
				a->onoff=0;

				a->addr++;

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

				current_val=pt3->data[a->addr];
				a->spec_delay=current_val;

				a->tone_slide_delay=current_val;
				a->tone_slide_count=a->tone_slide_delay;

				a->addr++;
				a->addr++;
				a->addr++;
				current_val=pt3->data[a->addr];
				a->spec_lo=current_val;

				a->addr++;
				current_val=pt3->data[a->addr];
				a->spec_hi=current_val;

				a->addr++;

				a->tone_slide_step=(a->spec_hi<<8)|(a->spec_lo);
				/* sign extend */
				//a->tone_slide_step=
				//	(a->tone_slide_step<<16)>>16;
				/* abs() */
				if (a->tone_slide_step<0) {
					a->tone_slide_step=-a->tone_slide_step;
				}

				a->tone_delta=pt3->frequency_table[a->note]-
						pt3->frequency_table[prev_note];

				a->slide_to_note=a->note;
				a->note=prev_note;
				if (pt3->version >= 6) {
					a->tone_sliding = prev_sliding;
				}
				if ((a->tone_delta - a->tone_sliding) < 0) {
					a->tone_slide_step=-a->tone_slide_step;
				}
				/* In the tracker it's 3 */
				new_spec=0x3;

			}
			/* Position in Sample */
			else if (a->spec_command==0x3) {
				/* Note, unclear what to do here if */
				/* offset is out of bounds */
				current_val=pt3->data[a->addr];
				a->sample_position=current_val;
				a->addr++;

				new_spec=0x4;
			}
			/* Position in Ornament */
			else if (a->spec_command==0x4) {
				current_val=pt3->data[a->addr];
				a->ornament_position=current_val;
				a->addr++;

				new_spec=0x5;
			}
			/* Vibrato */
			else if (a->spec_command==0x5) {
				current_val=pt3->data[a->addr];
				a->onoff_delay=current_val;
				a->addr++;
				current_val=pt3->data[a->addr];
				a->offon_delay=current_val;
				a->addr++;

				a->onoff=a->onoff_delay;
				a->tone_slide_count=0;
				a->tone_sliding=0;

				new_spec=0x6;
			}
			/* Envelope Down */
			else if (a->spec_command==0x8) {

				/* delay? */
				current_val=pt3->data[a->addr];
				pt3->envelope_delay=current_val;
				pt3->envelope_delay_orig=current_val;
				a->spec_delay=current_val;
				a->addr++;

				/* Low? */
				current_val=pt3->data[a->addr];
				a->spec_lo=current_val&0xff;
				a->addr++;

				/* High? */
				current_val=pt3->data[a->addr];
				a->spec_hi=current_val&0xff;
				a->addr++;

				pt3->envelope_slide_add=
					(a->spec_hi<<8)|(a->spec_lo&0xff);

				/* in the tracker it's 9 */
				if (pt3->envelope_slide_add<0x8000) {
					new_spec=0x9;
				}
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
				current_val=pt3->data[a->addr];
				a->spec_lo=current_val;
				pt3->speed=current_val;
				a->addr++;
				/* in tracker it's B */
				new_spec=0xb;
			}
			else {
				pt3_message("UNKNOWN EFFECT\n");
//				printf("%c UNKNOWN effect %02X\n",
//					a->which,a->spec_command);
			}
			a->spec_command=new_spec;

			break;
		}
	}

}


	/*  PT3VolumeTable_35 = 1 */
	/*  PT3VolumeTable_33334 = 0 */
static void pt3_setup_volume_table(int which) {
	//based on VolTableCreator by Ivan Roshin
	// originally in z80 assembly language
	//A - VersionForVolumeTable (0..4 - 3.xx..3.4x;
	//5.. - 3.5x..3.6x..VTII1.0)

	unsigned int carry,a,hl,de,temp1,temp2,c,ix;

	/* 0x00 or 0x10 */
	de=(which<<4);

	for(ix=1;ix<16;ix++) {

		/* 0x10 or 0x11 */
		hl=(0x11-which);

		hl=hl+de;

		carry=(hl>>16);
		hl&=0xffff;

		/* swap hl and de */
		temp1=hl;
		temp2=de;
		hl=temp2;
		de=temp1;

		/* 0 or 0xffff */
		hl=(0-carry)&0xffff;

		for(c=0;c<16;c++) {

			if (!which) {
				carry=!!(hl&0x80);
			}

			a=(hl>>8)&0xff;
			a=a+carry;

			PT3VolumeTable[ix][c]=a;

			hl=hl+de;

		}

		if ((de&0xff)==0x77) {
			de++;
		}

	}
}

static unsigned short base0_v3[]={
  0xC21,0xB73,0xACE,0xA33,0x9A0,0x916,0x893,0x818,0x7A4,0x736,0x6CE,0x66D,
};

static unsigned short base0_v4[]={
  0xC22,0xB73,0xACF,0xA33,0x9A1,0x917,0x894,0x819,0x7A4,0x737,0x6CF,0x66D,
};

static unsigned short base1[]={
  0xEF8,0xE10,0xD60,0xC80,0xBD8,0xB28,0xA88,0x9F0,0x960,0x8E0,0x858,0x7E0,
};

static unsigned short base2_v3[]={
  0xD3E,0xC80,0xBCC,0xB22,0xA82,0x9EC,0x95C,0x8D6,0x858,0x7E0,0x76E,0x704,
};

static unsigned short base2_v4[]={
  0xD10,0xC55,0xBA4,0xAFC,0xA5F,0x9CA,0x93D,0x8B8,0x83B,0x7C5,0x755,0x6EC,
};

/* note: same for both versions */
static unsigned short base3[]={
  0xCDA,0xC22,0xB73,0xACF,0xA33,0x9A1,0x917,0x894,0x819,0x7A4,0x737,0x6CF,
};

static unsigned char table0_v4_adjust[]={
  0x40, 0xe6, 0x9c, 0x66, 0x40, 0x2c, 0x20, 0x30, 0x48, 0x6c, 0x1c, 0x5a,
};


static unsigned char table2_v3_adjust[]={
  0xf8, 0x80, 0x90, 0xc0, 0x04, 0xf0, 0xf8, 0xec, 0xe0, 0xc0, 0xfc, 0x40,
};

static unsigned char table2_v4_adjust[]={
  0x20, 0xa8, 0x40, 0xf8, 0xbc, 0x90, 0x78, 0x70, 0x74, 0x08, 0x2a, 0x50,
};

static unsigned char table3_v4_adjust[]={
  0xB4, 0x40, 0xe6, 0x9c, 0x66, 0x40, 0x2c, 0x20, 0x30, 0x48, 0x6c, 0x1c,
};

static void NoteTablePropogate(unsigned short *base_table,
						unsigned short *Tone) {

	int x,y;

	for(y=0;y<12;y++) Tone[y]=base_table[y];

	for(x=0;x<84;x++) {
		Tone[x+12]=Tone[x]>>1;
	}

}

static void NoteTableAdjust(unsigned char *adjust_table,
						unsigned short *Tone) {

	int blah,extra;
	int offset=0;
	int x,y;

	for(y=0;y<12;y++) {
		offset=y;
		blah=adjust_table[y];
		for(x=0;x<8;x++) {
			extra=blah&1;
			blah>>=1;

			Tone[offset]+=extra;
			offset+=12;
		}
	}

	return;
}






int pt3_init_song(struct pt3_song_t *pt3) {

	int result;

	/* copy in the header data */
	result=pt3_load_header(1,pt3);
	if (result) {
		pt3_message("Error decoding header!\n");
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
	pt3->envelope_slide_add=0;
	pt3->current_pattern=0;

	/************************/
	/* set up volume table	*/
	/************************/


	if (pt3->version <= 4) {
		/*  PT3VolumeTable_3334 */
		pt3_setup_volume_table(1);
	}
	else {
		/*  PT3VolumeTable_35 */
		pt3_setup_volume_table(0);
	}

	/**************************/
	/* set up frequency table */
	/**************************/

	if (pt3->which_frequency_table==0) {
		if (pt3->version <= 3) {

			/* create table #0 v3.3 "PT3NoteTable_PT_33_34r" */
			NoteTablePropogate(base0_v3,pt3->frequency_table);

		}
		else {

			/* create table #0 v3.4 "PT3NoteTable_PT_34_35" */
			NoteTablePropogate(base0_v4,pt3->frequency_table);
			NoteTableAdjust(table0_v4_adjust,pt3->frequency_table);
		}
	}
	else if (pt3->which_frequency_table==1) {
		/* create table #1 "PT3NoteTable_ST" */
		NoteTablePropogate(base1,pt3->frequency_table);
		pt3->frequency_table[23]+=13;
		pt3->frequency_table[46]-=1;
	}
	else if (pt3->which_frequency_table==2) {
		if (pt3->version <= 3) {
			/* Create Table #2, v3, "PT3NoteTable_ASM_34r" */
			NoteTablePropogate(base2_v3,pt3->frequency_table);
			NoteTableAdjust(table2_v3_adjust,pt3->frequency_table);
			pt3->frequency_table[86]+=1;
			pt3->frequency_table[87]+=1;
		}
		else {
			/* Create Table #2, v4+, "PT3NoteTable_ASM_34_35" */
			NoteTablePropogate(base2_v4,pt3->frequency_table);
			NoteTableAdjust(table2_v4_adjust,pt3->frequency_table);
		}
	}
	else {
		if (pt3->version <= 3) {
			/* Create Table #3, v3, "PT3NoteTable_REAL_34r" */
			NoteTablePropogate(base3,pt3->frequency_table);
			NoteTableAdjust(table3_v4_adjust,pt3->frequency_table);
			pt3->frequency_table[43]++;
		}
		else {
			/* Create Table #3, v4+, "PT3NoteTable_REAL_34_35" */
			NoteTablePropogate(base3,pt3->frequency_table);
			NoteTableAdjust(table3_v4_adjust,pt3->frequency_table);
		}
	}



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


int pt3_decode_line(struct pt3_song_t *pt3) {

	decode_note(&pt3->a,pt3);
	decode_note(&pt3->b,pt3);
	decode_note(&pt3->c,pt3);


	if (pt3->a.all_done && pt3->b.all_done && pt3->c.all_done) {
		return 1;
	}
	return 0;

}

void pt3_set_pattern(int i, struct pt3_song_t *pt3) {

	pt3->current_pattern=pt3->data[0xc9+i]/3;

	pt3->a.addr=pt3->data[(pt3->current_pattern*6)+0+pt3->pattern_loc] |
		(pt3->data[(pt3->current_pattern*6)+1+pt3->pattern_loc]<<8);

	pt3->b.addr=pt3->data[(pt3->current_pattern*6)+2+pt3->pattern_loc] |
		(pt3->data[(pt3->current_pattern*6)+3+pt3->pattern_loc]<<8);

	pt3->c.addr=pt3->data[(pt3->current_pattern*6)+4+pt3->pattern_loc] |
		(pt3->data[(pt3->current_pattern*6)+5+pt3->pattern_loc]<<8);

	pt3->a.all_done=0;
	pt3->b.all_done=0;
	pt3->c.all_done=0;

	pt3->noise_period=0;

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


int pt3_load_song(struct pt3_image_t *pt3_image,
	struct pt3_song_t *pt3, struct pt3_song_t *pt3_2) {

	int result;
	int offset1=0,offset2=0;

	/* Clean up the incoming song structs */

	if (pt3==NULL) {
		return -1;
	}
	else {
		/* Clear out the struct */
		memset(pt3,0,sizeof(struct pt3_song_t));
	}

	if (pt3_2==NULL) {
		return -1;
	}
	else {
		/* Clear out the struct */
		memset(pt3_2,0,sizeof(struct pt3_song_t));
	}

	/* See if we have a ProTracker 3.7 format with 6-channels */
	if (!memcmp("02TS",pt3_image->data+(pt3_image->length-4),4)) {
		pt3_message("6-channels!\n");

		if (!memcmp("PT3!",pt3_image->data+(pt3_image->length-16),4)) {
			pt3->valid=1;
			pt3->data=pt3_image->data;
			offset1=*(pt3_image->data+(pt3_image->length-12));
			offset1|=(*(pt3_image->data+(pt3_image->length-11)))<<8;
		}

		if (!memcmp("PT3!",pt3_image->data+(pt3_image->length-10),4)) {
			pt3_2->valid=1;
			pt3_2->data=pt3_image->data+offset1;
			offset2=*(pt3_image->data+(pt3_image->length-6));
			offset2|=(*(pt3_image->data+(pt3_image->length-5)))<<8;
			(void)offset2;	/* not needed for now */
		}
	}
	else {
		/* Point first song to to beginning of data */
		pt3->valid=1;
		pt3->data=pt3_image->data;
	}

	/* Init Data */
	result=pt3_init_song(pt3);
	if (result<0) return result;

	if (pt3_2->valid) {
		result=pt3_init_song(pt3_2);
		if (result<0) return result;
	}

	return 0;
}

#ifndef PT3LIB_EMBEDDED
int pt3_load_song_from_disk(char *filename, struct pt3_image_t *pt3_image,
	struct pt3_song_t *pt3, struct pt3_song_t *pt3_2) {

	int fd;
	int result;

	/* allocate space */
	pt3_image->data=malloc(sizeof(unsigned char)*MAX_PT3_SIZE);
	if (pt3_image->data==NULL) {
		pt3_message("Error allocating\n");
		return -1;
	}

	/* Clear out our data */
	memset(pt3_image->data,0,MAX_PT3_SIZE);

	/* Open file */
	fd=open(filename,O_RDONLY);
	if (fd<0) {
		pt3_message(filename);
		pt3_message("Error opening\n");
		//fprintf(stderr,"Error opening %s: %s\n",
		//	filename,strerror(errno));
		return -1;

	}

	/* Read entire file into memory (probably not that big) */
	result=read(fd,pt3_image->data,MAX_PT3_SIZE);
	if (result<0) {
		pt3_message("Error reading file\n");
		//fprintf(stderr,"Error reading file: %s\n",
		//	strerror(errno));
		return -1;
	}
	pt3_image->length=result;

	/* close the file */
	close(fd);

	pt3_load_song(pt3_image,pt3,pt3_2);

	return 0;
}
#endif

/* These are unused, left for historical reference */


#if 0
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


/* Table #0 of Pro Tracker 3.3x - 3.4r */
static unsigned short PT3NoteTable_PT_33_34r[]={
  0x0C21,0x0B73,0x0ACE,0x0A33,0x09A0,0x0916,0x0893,0x0818,
  0x07A4,0x0736,0x06CE,0x066D,0x0610,0x05B9,0x0567,0x0519,
  0x04D0,0x048B,0x0449,0x040C,0x03D2,0x039B,0x0367,0x0336,
  0x0308,0x02DC,0x02B3,0x028C,0x0268,0x0245,0x0224,0x0206,
  0x01E9,0x01CD,0x01B3,0x019B,0x0184,0x016E,0x0159,0x0146,
  0x0134,0x0122,0x0112,0x0103,0x00F4,0x00E6,0x00D9,0x00CD,
  0x00C2,0x00B7,0x00AC,0x00A3,0x009A,0x0091,0x0089,0x0081,
  0x007A,0x0073,0x006C,0x0066,0x0061,0x005B,0x0056,0x0051,
  0x004D,0x0048,0x0044,0x0040,0x003D,0x0039,0x0036,0x0033,
  0x0030,0x002D,0x002B,0x0028,0x0026,0x0024,0x0022,0x0020,
  0x001E,0x001C,0x001B,0x0019,0x0018,0x0016,0x0015,0x0014,
  0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,0x000C
};

/* Table #0 of Pro Tracker 3.4x - 3.5x */
static unsigned short PT3NoteTable_PT_34_35[]={
  0x0C22,0x0B73,0x0ACF,0x0A33,0x09A1,0x0917,0x0894,0x0819,
  0x07A4,0x0737,0x06CF,0x066D,0x0611,0x05BA,0x0567,0x051A,
  0x04D0,0x048B,0x044A,0x040C,0x03D2,0x039B,0x0367,0x0337,
  0x0308,0x02DD,0x02B4,0x028D,0x0268,0x0246,0x0225,0x0206,
  0x01E9,0x01CE,0x01B4,0x019B,0x0184,0x016E,0x015A,0x0146,
  0x0134,0x0123,0x0112,0x0103,0x00F5,0x00E7,0x00DA,0x00CE,
  0x00C2,0x00B7,0x00AD,0x00A3,0x009A,0x0091,0x0089,0x0082,
  0x007A,0x0073,0x006D,0x0067,0x0061,0x005C,0x0056,0x0052,
  0x004D,0x0049,0x0045,0x0041,0x003D,0x003A,0x0036,0x0033,
  0x0031,0x002E,0x002B,0x0029,0x0027,0x0024,0x0022,0x0020,
  0x001F,0x001D,0x001B,0x001A,0x0018,0x0017,0x0016,0x0014,
  0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,0x000C,
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

/* Table #2 of Pro Tracker 3.4r */
static unsigned short PT3NoteTable_ASM_34r[]={
  0x0D3E,0x0C80,0x0BCC,0x0B22,0x0A82,0x09EC,0x095C,0x08D6,
  0x0858,0x07E0,0x076E,0x0704,0x069F,0x0640,0x05E6,0x0591,
  0x0541,0x04F6,0x04AE,0x046B,0x042C,0x03F0,0x03B7,0x0382,
  0x034F,0x0320,0x02F3,0x02C8,0x02A1,0x027B,0x0257,0x0236,
  0x0216,0x01F8,0x01DC,0x01C1,0x01A8,0x0190,0x0179,0x0164,
  0x0150,0x013D,0x012C,0x011B,0x010B,0x00FC,0x00EE,0x00E0,
  0x00D4,0x00C8,0x00BD,0x00B2,0x00A8,0x009F,0x0096,0x008D,
  0x0085,0x007E,0x0077,0x0070,0x006A,0x0064,0x005E,0x0059,
  0x0054,0x0050,0x004B,0x0047,0x0043,0x003F,0x003C,0x0038,
  0x0035,0x0032,0x002F,0x002D,0x002A,0x0028,0x0026,0x0024,
  0x0022,0x0020,0x001E,0x001D,0x001B,0x001A,0x0019,0x0018,
  0x0015,0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E
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

/* Table #3 of Pro Tracker 3.4r */
static unsigned short PT3NoteTable_REAL_34r[] = {
  0x0CDA,0x0C22,0x0B73,0x0ACF,0x0A33,0x09A1,0x0917,0x0894,
  0x0819,0x07A4,0x0737,0x06CF,0x066D,0x0611,0x05BA,0x0567,
  0x051A,0x04D0,0x048B,0x044A,0x040C,0x03D2,0x039B,0x0367,
  0x0337,0x0308,0x02DD,0x02B4,0x028D,0x0268,0x0246,0x0225,
  0x0206,0x01E9,0x01CE,0x01B4,0x019B,0x0184,0x016E,0x015A,
  0x0146,0x0134,0x0123,0x0113,0x0103,0x00F5,0x00E7,0x00DA,
  0x00CE,0x00C2,0x00B7,0x00AD,0x00A3,0x009A,0x0091,0x0089,
  0x0082,0x007A,0x0073,0x006D,0x0067,0x0061,0x005C,0x0056,
  0x0052,0x004D,0x0049,0x0045,0x0041,0x003D,0x003A,0x0036,
  0x0033,0x0031,0x002E,0x002B,0x0029,0x0027,0x0024,0x0022,
  0x0020,0x001F,0x001D,0x001B,0x001A,0x0018,0x0017,0x0016,
  0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,
};

/* Table #3 of Pro Tracker 3.4x - 3.5x */
/* NOTE: ONLY DIFFERENCE IS 0x113 -> 0x112 */
static unsigned short PT3NoteTable_REAL_34_35[] = {
  0x0CDA,0x0C22,0x0B73,0x0ACF,0x0A33,0x09A1,0x0917,0x0894,
  0x0819,0x07A4,0x0737,0x06CF,0x066D,0x0611,0x05BA,0x0567,
  0x051A,0x04D0,0x048B,0x044A,0x040C,0x03D2,0x039B,0x0367,
  0x0337,0x0308,0x02DD,0x02B4,0x028D,0x0268,0x0246,0x0225,
  0x0206,0x01E9,0x01CE,0x01B4,0x019B,0x0184,0x016E,0x015A,
  0x0146,0x0134,0x0123,0x0112,0x0103,0x00F5,0x00E7,0x00DA,
  0x00CE,0x00C2,0x00B7,0x00AD,0x00A3,0x009A,0x0091,0x0089,
  0x0082,0x007A,0x0073,0x006D,0x0067,0x0061,0x005C,0x0056,
  0x0052,0x004D,0x0049,0x0045,0x0041,0x003D,0x003A,0x0036,
  0x0033,0x0031,0x002E,0x002B,0x0029,0x0027,0x0024,0x0022,
  0x0020,0x001F,0x001D,0x001B,0x001A,0x0018,0x0017,0x0016,
  0x0014,0x0013,0x0012,0x0011,0x0010,0x000F,0x000E,0x000D,
};

#endif
