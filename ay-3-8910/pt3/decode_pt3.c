/* Decode a vortex-tracker PT3 file */
/* Vortex Tracker itself is written in Pascal and comments are in Russian */
/* which makes it a bit hard to understand */
/* This is based partly on the writeup here: */
/*   http://karoshi.auic.es/index.php?topic=397.msg4641#msg4641 */

/* PT3 Format */

/* Header */
/* Note: 16-bit values are little-endian */

// $00 - $0C : 13 bytes : Magic       : "ProTracker 3."
// $0D       :  1 byte  : Version     : '5' for Vortex Tracker II
// $0E - $1D : 16 bytes : String      : " compilation of "
// $1E - $3E : 32 bytes : Name        : Name of the module
// $3E - $41 :  4 bytes : String      : " by "
// $42 - $62 : 32 bytes : Author      : Author of the module.
// $63       :  1 byte  : Frequency table (from 1 to 4)
// $64       :  1 byte  : Speed/Delay
// $65       :  1 byte  : Number of patterns (-1?  Max Patterns?)
// $66       :  1 byte  : LPosPtr     : Pattern loop/LPosPtr
// $67 - $68 :  2 bytes : PatsPtrs    : Position of patterns inside the module
// $69 - $A8 : 64 bytes : SamPtrs[32] : Position of samples inside the module
// $A9 - $C8 : 32 bytes : OrnPtrs[16] : Position of ornaments inside the module
// ***************** this is wrong? $C9 - $CA :  2 bytes : CrPsPtr     : Points to pattern order
// pattern order follows starting at $C9, $ff terminated
// each pattern number is multiplied by 3


// Actual Pattern Data, Stream of bytes, null terminated
//      $00           -- nul teminate
//      $01-$0f       -- effects
// OnDisk  InTracker
//   01      $01: Tone Down
//                First byte indicates the delay used to add the new frequency.
//                Next 2 bytes will indicate the frequency to add. Example:
//                $02,$23,$00 will add $23 to the final frequency in that raster
//                and another $23 every 2 rasters.
//   ??      $02: Tone Up
//                It's the same as above but the value is rested to $FFFF. Example:
//                $01,$DD,$FF will rest $23 to the final frequency in every raster.
//   $02     $03: Tone portamento
//   ??      $04: Sample Offset
//		  Starts playing sample from a particular position/line definition.
//                Byte indicates from which position.
//   ??      $05: Ornament offset
//		  Starts playing ornament from a particular position/line definition.
//                Byte indicates from which position.
//   ??      $06: Vibrato
//		  Periodic sound off/on in that channel
//                Two bytes are used here. The first one tells how many rasters will be
//                played and the second how many will be not. Example. $03,$04 means
//                that that channel will be played 3 rasters, 4 not played, 3 played, ...
//   $08     $09: Envelope frequency decreasing.
//                Same as increasing but the value is rested to $FFFF.
//                Example: $CC,$FF will rest $34 to R11.
//   $??     $0A: Envelope frequency increasing.
//                Two bytes will indicate the frequency to add to registers R12 and R11.
//                Example: $34,$00 will add $34 to R11.
//   $09     $0B: Set playing speed (new Delay).
//                The byte after the note will tell the new delay.

//      $10-$1f       -- envelope.   (1-E, F=envelope off)
//	$20-$3f       -- set noise
//      $40-$4f       -- set ornament 0-F
//		Ornament 0 can't be set directly, instead $40
//		is reported as envelope off (F)?
//      $50-$ad       -- play note, see below
//	$b0           -- env=$f, ornament=saved ornament
//	$b1, arg1     -- set skip value to arg1 (how long note plays)
//	$b2-$bf,arg1/2-- envelope?
//      $c0-$cf       -- set volume, value-$c0.  $c0 means sound off
//	$d0           -- do nothing?
//      $d1-$ef       -- set sample, value-$d0.
//	$f0-$ff, arg1 -- Initialize?
//               Envelope=15, Ornament=low byte, Sample=arg1/2

// if you reach a note, a 0xd0, a 0xc0 then done this note.

// f0 10 cf b1 40 74 00


// 50-AF = notes
//      0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
//50: C-1 C#1 D-1 D#1 E-1 F-1 F#1 G-1 G#1 A-1 A#1 B-1 C-2 C#2 D-2 D#2
//60: E-2 F-2 F#2 G-2 G#2 A-2 A#2 B-2 C-3 C#3 D-3 D#3 E-3 F-3 F#3 G-3
//70: G#3 A-3 A#3 B-3 C-4 C#4 D-4 D#4 E-4 F-2 F#4 G-4 G#4 A-4 A#4 B-4
//80: C-5 C#5 D-5 D#5 E-5 F-5 F#5 G-5 G#5 A-5 A#5 B-5 C-6 C#6 D-6 D#6
//90: E-6 F-6 F#6 G-6 G#6 A-6 A#6 B-6 C-7 C#7 D-7 D#7 E-7 F-7 F#7 G-7
//a0: G#7 A-7 A#7 B-7 C-8 C#8 D-8 D#8 E-8 F-8 F#8 G-8 G#8 A-8 A#8 B-8

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


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

struct header_info_t {
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

static int debug=1;

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
	int prev_ornament;
	int volume;
	int spec_command;
	int spec_delay;
	int spec_hi;
	int spec_lo;

	int len;
	int len_count;

	int all_done;
};

static int envelope_period_h=0;
static int envelope_period_l=0;
static int envelope_period_h_old=0;
static int envelope_period_l_old=0;
static int noise_period=0;

//static int delay=0;

static void decode_note(struct note_type *a,
			unsigned short *addr) {

	int a_done=0;
	int current_val;

	a->spec_command=0;
	a->spec_delay=0;
	a->spec_lo=0;
	a->note=0;
	a->envelope=0;

	/* Skip decode if note still running */
	if (a->len_count>1) {
		a->len_count--;
		return;
	}

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
					//printf("UNKNOWN %02X ",current_val);
					a->envelope=0xf; // (disable)
				}
				else {
					a->envelope=(current_val&0xf);

					current_val=pt3_data[*addr+1];
					envelope_period_h=current_val;
					//printf("%02X ",current_val);
					(*addr)++;

					current_val=pt3_data[(*addr)+1];
					envelope_period_l=current_val;
					//printf("%02X ",current_val);
					(*addr)++;
				}
				current_val=pt3_data[(*addr)+1];
				a->ornament=a->prev_ornament;
				a->sample=(current_val/2);
				//printf("%02X\n",current_val);
				(*addr)++;

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
				a->prev_ornament=a->ornament;

				//printf("%x envelope=%x\n",current_val,
				//	a->envelope);
				break;
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 0xa:
				a->note=current_val;
				a_done=1;
				break;
			case 0xb:
				/* Set noise? */
				if (current_val==0xb0) {
					a->envelope=0xf;
					a->ornament=a->prev_ornament;
					//current_val=pt3_data[(*addr)+1];
					//noise_period=(current_val&0xf);
					(*addr)++;
				}
				/* set len */
				else if (current_val==0xb1) {
					current_val=pt3_data[(*addr)+1];
					a->len=current_val;
					a->len_count=a->len;
					(*addr)++;
				}
				else {
					a->envelope=(current_val&0xf)-1;
					a->ornament=a->prev_ornament;
					current_val=pt3_data[(*addr)+1];
					(*addr)++;
					envelope_period_h=current_val;
					current_val=pt3_data[(*addr)+1];
					(*addr)++;
					envelope_period_l=current_val;
				}
				break;
			case 0xc:	/* volume */
				a->volume=current_val&0xf;
				if (a->volume==0) {
					a->note=0xff;
					a_done=1;
				}
				break;
			case 0xd:
				if (current_val==0xd0) {
					a->note=0;
					a_done=1;
				}
				else {
					a->sample=(current_val&0xf);
				}
				break;
			case 0xe:
				a->sample=(current_val-0xd0);
				break;
			case 0xf:
//               Envelope=15, Ornament=low byte, Sample=arg1/2
				a->envelope=0xf;
				a->ornament=(current_val&0xf);
				a->prev_ornament=(current_val&0xf);
				current_val=pt3_data[(*addr)+1];
				a->sample=current_val/2;
				(*addr)++;
				break;
		}

		(*addr)++;
		if (a_done) {
			if (a->spec_command==0xb) {
				current_val=pt3_data[(*addr)];
				a->spec_lo=current_val;
				(*addr)++;
			}
			if (a->spec_command==0x1) {
				current_val=pt3_data[(*addr)];
				a->spec_delay=current_val;
				(*addr)++;

				current_val=pt3_data[(*addr)];
				a->spec_lo=(current_val);
				(*addr)++;

				current_val=pt3_data[(*addr)];
				a->spec_hi=(current_val);
				(*addr)++;
			}
			if (a->spec_command==0x3) {
				current_val=pt3_data[(*addr)];
				a->spec_delay=current_val;
				(*addr)++;

				current_val=pt3_data[(*addr)];
//				a->spec_lo=(current_val)&0xf;
				(*addr)++;

				current_val=pt3_data[(*addr)];
//				a->spec_lo=(current_val)&0xf;
				(*addr)++;
				current_val=pt3_data[(*addr)];
				a->spec_lo=(current_val)&0xf;
				(*addr)++;
				current_val=pt3_data[(*addr)];
//				a->spec_lo=(current_val)&0xf;
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
	if (a->note==0xff) printf("R--");
	else if (a->note==0) printf("---");
	else printf("%s",note_names[a->note-0x50]);
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



int main(int argc, char **argv) {

	char filename[BUFSIZ];
	int fd,i,j,addr;
	int result,sample_loop,sample_len;

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



	/* Print header info */
	if (debug) {
		printf("%s%c\n",header.magic,header.version);
		printf("\tNAME: %s\n",header.name);
		printf("\tBY  : %s\n",header.author);
		printf("\tFreqTable: %d Speed: %d  Patterns: %d Loop: %d\n",
			header.frequency_table,
			header.speed,
			header.num_patterns,
			header.loop);
		printf("\tPattern Location Offset: %04x\n",header.pattern_loc);
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


		printf("Song Dump:\n");
		// Not easy to know song length in advance?
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

			j=0;
		//	while(pt3_data[c_addr+j]) {

			while(j<80) {
				printf("%02X ",pt3_data[c_addr+j]);
				j++;
			};
//			printf("\n");

			for(j=0;j<64;j++) {

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

				printf("\n");
			}
		}

	}

	return 0;
}
