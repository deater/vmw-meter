#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <math.h>

#include "notes.h"


#define NUM_CHANNELS	3

#define MAX_INSTRUMENTS	25

#define DEFAULT	0

struct instrument_type {
	int length;
	int adsr;
	int attack[32];
	int attack_size;
	int decay[32];
	int decay_size;
	int sustain;
	int release[32];
	int release_size;
	int once;
	int noise;
	int noise_size;
	int noise_period[32];
        char *name;
};


struct instrument_type instruments[MAX_INSTRUMENTS] = {
	{
	.name="raw",		// 0
	.adsr=1,
	.noise=0,
	.attack_size=0,
	.decay_size=0,
	.release_size=1,
	.release={0},
	.sustain=15,
	.once=0,
	},
	{
	.name="silence",	// 1
	.adsr=1,
	.noise=0,
	.attack_size=0,
	.decay_size=0,
	.release_size=1,
	.release={0},
	.sustain=0,
	.once=0,
	},
	{
	.name="piano",		// 2
	.adsr=1,
	.noise=0,
	.attack={14,15},
	.attack_size=2,
	.decay={14},
	.decay_size=1,
	.sustain=13,
	.release={10,5},
	.release_size=2,
	.once=0,
	},
	{
	.name="piano2",		// 3
	.adsr=1,
	.noise=0,
	.attack={13,14,15},
	.attack_size=3,
	.decay={14,13,12},
	.decay_size=3,
	.sustain=11,
	.release={10},
	.release_size=1,
	.once=0,
	},
	{
	.name="trill",		// 4
	.adsr=0,
	.noise=0,
	.attack={9,15,9,15},
	.attack_size=4,
	.release_size=1,
	.release={0},
	.once=0,
	},
	{
	.name="triangle", // 5
	.adsr=0,
	.noise=0,
	.attack={13,14,15,14,13,12,9,12},
	.attack_size=8,
	.release_size=1,
	.release={0},
	.once=0,
	},
	{
	.name="quiet raw",		// 6
	.adsr=1,
	.noise=0,
	.attack_size=0,
	.decay_size=0,
	.release_size=1,
	.release={0},
	.sustain=13,
	.once=0,
	},
	{
	.name="bass",			// 7
	.adsr=1,
	.noise=1,
	.attack_size=6,
	.attack={14,14,13, 13, 9, 9, },
	.release_size=1,
	.release={0},
	.noise_size=6,
	.noise_period =  { 10, 10, 10, 10, 10, 10 },
	.once=1,
	.length=7,
	},
	{
	.name="snare",			// 8
	.adsr=1,
	.noise=1,
	.attack_size=6,
	.attack={13,14,14,13, 12, 9 },
	.release_size=1,
	.release={0},
	.noise_size=6,
        .noise_period=  { 4, 5, 6, 7, 8, 8 },
	.once=1,
	.length=6,
	},
	{
	.name="cymabl",			// 9
	.adsr=1,
	.noise=1,
	.attack_size=6,
	.attack={13,13,11,11, 10, 10},
	.release_size=1,
	.release={0},
	.noise_size=6,
	.noise_period =  { 1, 1, 1, 1, 1, 0},
	.once=1,
	.length=6,
	},
};

#define MAX_LOUDS	8

/* Note, ay-3-8910 is logarithmic which makes this troublesome */
/* nv= 15.0 - ( (log(127.0/dv)) / (log(sqrt(2))) ); */

struct loudness_type {
	char name[8];
	int value;
} loudness_values[MAX_LOUDS]={
			/* sym  midi	ay*/
	{ "ppp",9},	/* ppp  16 	 9.0*/
	{ "pp", 11},	/* pp   32	11.0 */
	{ "p",  12},	/* p    48	12.2 */
	{ "mp", 13},	/* mp   64	13.0 */
	{ "mf", 14},	/* mf   80	13.7 */
	{ "f",  14},	/* f    96	14.2 */
	{ "ff", 15},	/* ff  112	14.7 */
	{ "fff",15},	/* fff 127	15 */
};

static int debug=0;

static int external_frequency=1000000;
static int bpm=120;
static int tempo=1;
static int baselen=96;  /* 120/minute, 50Hz, should really be 100 */
static int frames_per_line=6;

struct ym_header {
	char id[4];				// 0  -> 4
	char check[8];				// 4  -> 12
	uint32_t vbl;				// 12 -> 16
	uint32_t song_attr;			// 16 -> 20
	uint16_t digidrum;			// 20 -> 22
	uint32_t external_frequency;		// 22 -> 26
	uint16_t player_frequency;		// 26 -> 28
	uint32_t loop;				// 28 -> 32
	uint16_t additional_data;		// 32 -> 34
}  __attribute__((packed)) our_header;

static int note_to_length(int length) {

	int len=1;

	switch(length) {
		case 0: len=(baselen*5)/2; break;	// 0 = 2.5
		case 1:	len=baselen; break;		// 1 =   1 whole
		case 2: len=baselen/2; break;		// 2 = 1/2 half
		case 3: len=(baselen*3)/8; break;	// 3 = 3/8 dotted quarter
		case 4: len=baselen/4; break;		// 4 = 1/4 quarter
		case 5: len=(baselen*5)/8; break;	// 5 = 5/8 ?
		case 8: len=baselen/8; break;		// 8 = 1/8 eighth
		case 9: len=(baselen*3)/16; break;	// 9 = 3/16 dotted eighth
		case 6: len=baselen/16; break;		// 6 = 1/16 sixteenth
		case 10: len=(baselen*3)/4; break;	// : = 3/4 dotted half
		case 11: len=(baselen*9)/8; break;	// ; = 9/8 dotted half + dotted quarter
		case 12: len=(baselen*3)/2; break;	// < = 3/2 dotted whole
		case 13: len=(baselen*2); break;	// = = 2   double whole
		case 14: len=(baselen/3); break;	// > = 1/3   triple note
		case 15: len=99999;	break;		// ? = forever
		default:
			fprintf(stderr,"Unknown length %d\n",length);
	}

	return len;
}

struct note_type {
	unsigned char which;
	unsigned char note;
	int sub_adjust;

	int sharp,flat;
	int octave;
	int len;
	int enabled;
	int freq;
	int length;
	int left;

	int loud;
	struct instrument_type *instrument;

	int arpeggio;
	int arpeggio2;
	int arpeggio3;
	int freq2,freq3;

	int portamento;
	int port_speed;

	int vibrato;
	int vibrato_type;
	int vibrato_lookup[64];
	int vibrato_count;
	int vibrato_max;
};

static struct note_type a,b,c;


static int get_note(char *string, int sp, struct note_type *n, int line) {

	int ch;

	/* Skip white space */
	while((string[sp]==' ' || string[sp]=='\t')) sp++;

	if (string[sp]=='\n') return -1;

	/* return early if no change */
	ch=string[sp];
	if (ch=='-') return sp+6;

	/* get note info */
	n->sharp=0;
	n->flat=0;
	n->note=ch;
	sp++;
	if (string[sp]==' ') ;
	else if (string[sp]=='#') n->sharp=1;
	else if (string[sp]=='-') n->flat=1;
	else if (string[sp]=='=') n->flat=2;
	else {
		fprintf(stderr,"Unknown note modifier %c\n",string[sp]);
	}
	sp++;
	n->octave=string[sp]-'0';
	sp++;
	sp++;
	n->len=string[sp]-'0';
	sp++;

	n->enabled=1;

	n->sub_adjust=0;

	if (n->instrument->once) {
		n->length=n->instrument->length;
	}
	else {
		n->length=note_to_length(n->len);
	}
	n->left=n->length-1;


	if (n->length<=0) {
		printf("Error line %d\n",line);
		exit(-1);
	}

	return sp;
}


static int update_note(struct note_type *n) {

	double freq,freq2,freq3;
	int note2_adjust=0,note3_adjust=0;
	int sub1_adjust=0,sub2_adjust=0,sub3_adjust=0;

	if (n->arpeggio) {
		note2_adjust=n->arpeggio2;
		note3_adjust=n->arpeggio3;
	}

	if (n->portamento) {
		sub1_adjust=n->sub_adjust;
		sub2_adjust=n->sub_adjust+n->port_speed;
		sub3_adjust=n->sub_adjust+n->port_speed*2;
		n->sub_adjust+=3*n->port_speed;
//		printf("Setting port sub: %d ps: %d\n",
//			n->sub_adjust,n->port_speed);
	}
	else {
		sub1_adjust=n->sub_adjust;
	}

	if (n->vibrato) {
		sub1_adjust+=n->vibrato_lookup[n->vibrato_count];
		n->vibrato_count++;
		if (n->vibrato_count>n->vibrato_max) n->vibrato_count=0;
		sub1_adjust+=n->vibrato_lookup[n->vibrato_count%n->vibrato_max];
		n->vibrato_count++;
		if (n->vibrato_count>n->vibrato_max) n->vibrato_count=0;
		sub2_adjust+=n->vibrato_lookup[n->vibrato_count%n->vibrato_max];
		n->vibrato_count++;
		if (n->vibrato_count>n->vibrato_max) n->vibrato_count=0;
	}

	freq=note_to_freq(n->note,n->sharp,n->flat,n->octave,sub1_adjust);
	freq2=note_to_freq(n->note,n->sharp+note2_adjust,n->flat,n->octave,sub2_adjust);
	freq3=note_to_freq(n->note,n->sharp+note3_adjust,n->flat,n->octave,sub3_adjust);

//	if (n->portamento) {
//		printf("Freqs: %lf %lf %lf\n",freq,freq2,freq3);
//	}

	if (debug) printf("(%c) %c%c L=%d O=%d f=%lf\n",
				n->which,
				n->note,
				n->sharp?'#':' ',
				n->len,
				n->octave,
				freq);

	n->freq=external_frequency/(16.0*freq);
	if ((n->freq>0xfff)||(n->freq<0)) n->freq=0xfff;

	n->freq2=external_frequency/(16.0*freq2);
	if ((n->freq2>0xfff)||(n->freq2<0)) n->freq2=0xfff;

	n->freq3=external_frequency/(16.0*freq3);
	if ((n->freq3>0xfff)||(n->freq3<0)) n->freq3=0xfff;

//	if (n->portamento) printf("VMW %d %d %d\n",n->freq,n->freq2,n->freq3);

	return 0;
}



static int calculate_noise(struct note_type *n) {

	struct instrument_type *i;
	int result=0;

	i=n->instrument;

	if (i->noise) {
		result=i->noise_period[i->length - n->left];
	}

	return result;
}

static int enable_noise(struct note_type *n, int which) {
	int mask;

	mask=1<<(3+which);

	return mask;
}


static int calculate_amplitude(struct note_type *n) {

	int result=0;
	struct instrument_type *i;

	i=n->instrument;

/*
 A  A  A  D  D  D                    R R
16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0

length=17
*/

	if (i->adsr) {

		/* ADSR type envelope */

		if ( n->left < i->release_size ) {
			result=i->release[i->release_size-n->left];
		}
		else if (n->left>(n->length-(i->attack_size+1))) {
			result=i->attack[(n->length-n->left-1)];
		}
		else if (n->left>(n->length-i->attack_size-i->decay_size-1)) {
			result=i->decay[(n->length-n->left-i->attack_size-1)];
		}
		else {
			result=i->sustain;
		}
	}
	else {

		/* no envelope */

		if ( n->left < i->release_size ) {
			result=i->release[i->release_size-n->left];
		}
		else {
			result=i->attack[n->left%i->attack_size];
		}
	}

	/* scale by loudness */

	result=(result * n->loud)/15;

	if (debug) printf("%d %d %d (%d)\n",n->length,n->left,result,
		n->loud);
//		((n->length-n->left)*16)/n->length,
//		instruments[DEFAULT].envelope[
//			((n->length-n->left)*16)/n->length
//		]);

	return result;
}

static struct note_type *find_note(char which) {

	if (which==a.which) return &a;
	if (which==b.which) return &b;
	if (which==c.which) return &c;

	return NULL;
}

static int get_effect(struct note_type *n,char *string) {

	int effect,param;
	int speed,depth=0;
	int i;

	effect=atoi(string);

	if (debug) printf("Found effect %x\n",effect);

	param=atoi(string+2);

	if (debug) printf("Found effect_param %x\n",param);

	switch(effect) {
		case 0x0:	// Arpeggio
			if (param!=0) {
				n->arpeggio=1;
				n->arpeggio2=((param)>>4)&0xf;
				n->arpeggio3=(param)&0xf;
			}
			break;
		case 0x1:	// Portamento Up
			n->portamento=1;
			if (param!=0) {
				n->port_speed=param;
			}
			break;
		case 0x2:	// Portamento Down
			n->portamento=1;
			if (param!=0) {
				n->port_speed=-param;
			}
			break;
		case 0x4:	// Vibrato

			n->vibrato=1;
			if ((param>>4)!=0) {
				speed=param>>4;
				n->vibrato_max=64/speed/2;
				n->vibrato_count=0;
			}
			if ((param&0xf)!=0) {
				depth=param&0xf;
			}

//			printf("VMW %X%02X max=%d: ",effect,param,n->vibrato_max);
			for(i=0;i<n->vibrato_max;i++) {
				n->vibrato_lookup[i]=16*(
				sin(i*(2.0*3.14159/n->vibrato_max))*
				(depth/8.0));
//				printf("%d (%.2lf,%2lf) ",
//					n->vibrato_lookup[i],depth/8.0,
//					i*(2.0*3.14159/n->vibrato_max));
			}
//			printf("\n");
			/* period is 64 / (vibrato speed) / (song speed - 1) */
			// min of 2, max of 32?
			/* peak-to-peak amplitude is (vibrato depth) / 4 half-steps */
			// min of 1/8 max of 2
			break;
		default:
			fprintf(stderr,"Unknown effect %X%02X\n",effect,param);
	}

	return 0;
}

static int get_instrument(struct note_type *n,char *string) {

	int num;

	num=atoi(string);

	if (debug) printf("Found instrument %d\n",num);

	if (num<0) {
		fprintf(stderr,"Instrument too small: %d\n",num);
	}

	if (num>=MAX_INSTRUMENTS) {
		fprintf(stderr,"Instrument too big: %d\n",num);
	}

	n->instrument=&instruments[num];


	return 0;
}

static int get_loudness(struct note_type *n,char *string) {

	int i,which,found=0;

	for(i=0;i<MAX_LOUDS;i++) {
		//printf("looking for %s in %s\n",loudness_values[i].name,pointer);
		if (!strncmp(loudness_values[i].name,string,
				strlen(loudness_values[i].name))) {
			if (debug) printf("Found %d %s\n",which,
						loudness_values[i].name);
			n->loud=loudness_values[i].value;
			found=1;
			break;
		}
	}
	if (!found) {
		n->loud=atoi(string);
		if (debug) printf("Found %d\n",n->loud);
	}

	if (n->loud>15) {
		fprintf(stderr,"Too loud %d\n",n->loud);
		n->loud=15;
	}
	if (n->loud<0) {
		fprintf(stderr,"Too soft %d\n",n->loud);
		n->loud=0;
	}

	return 0;
}

static int get_directive(char *string) {

	int which,type;
	char *pointer;
	struct note_type *n;

	pointer=string+2;

	// FIXME: proper check
	which=*pointer;
	pointer+=2;

	n=find_note(which);
	if (n==NULL) {
		fprintf(stderr,"Unknown note %c \"%s\"\n",which,pointer);
		return -1;
	}

	type=*pointer;

	switch(type) {
		case 'E':	get_effect(n,pointer+2);
				break;
		case 'L':	get_loudness(n,pointer+2);
				break;
		case 'I':	get_instrument(n,pointer+2);
				break;
		default:	fprintf(stderr,"Unknown directive %c\n",type);
				break;
	}

	return 0;
}

static int get_string(char *string, char *key, char *output, int strip_linefeed) {

	char *found;

	found=strstr(string,key);
	found=found+strlen(key);

	/* get rid of leading whitespace */
	while(1) {
		if ((*found==' ') || (*found=='\t')) found++;
		else break;
	}

	strcpy(output,found);

	/* remove trailing linefeed */
	if (strip_linefeed) output[strlen(output)-1]=0;

	return 0;

}

int main(int argc, char **argv) {

	char string[BUFSIZ];
	char *result;
	char ym_filename[BUFSIZ],lyrics_filename[BUFSIZ],*in_filename;
	char temp[BUFSIZ];
	FILE *ym_file,*lyrics_file,*in_file;
	int frames=0,digidrums=0;
	int attributes=0;
	int irq=50,loop=0;
	int header_length=0;
	int sp,i,j;
	fpos_t save;
	int line=0;

	char song_name[BUFSIZ];//="Still Alive";
	char author_name[BUFSIZ];//"Vince Weaver <vince@deater.net>";
	char comments[BUFSIZ];//="from Portal, Words and Music by Jonathan Coulton";
	char *comments_ptr=comments;

	unsigned char frame[16];

	/* Check command line arguments */
	if (argc<3) {
		printf("%s -- create a YM music file\n",argv[0]);
		printf("\n");
		printf("Usage:	%s INFILE OUTROOT\n\n",argv[0]);
		printf("\n");
		exit(1);
	}

	/* Open the input file */
	if (argv[1][0]=='-') {
		in_file=stdin;
	}
	else {
		in_filename=strdup(argv[1]);
		in_file=fopen(in_filename,"r");
		if (in_file==NULL) {
			fprintf(stderr,"Couldn't open %s\n",in_filename);
			return -1;
		}
	}

	/* Open the output/lyrics files */
	sprintf(ym_filename,"%s.ym",argv[2]);
	sprintf(lyrics_filename,"%s.lyrics",argv[2]);

	ym_file=fopen(ym_filename,"w");
	if (ym_file==NULL) {
		fprintf(stderr,"Couldn't open %s\n",ym_filename);
		return -1;
	}

	lyrics_file=fopen(lyrics_filename,"w");
	if (lyrics_file==NULL) {
		fprintf(stderr,"Couldn't open %s\n",lyrics_filename);
		return -1;
	}



	/* Get the info for the header */
	while(1) {
		result=fgets(string,BUFSIZ,in_file);
		if (result==NULL) break;
		line++;
		if (strstr(string,"ENDHEADER")) break;
		if (strstr(string,"TITLE:")) {
			get_string(string,"TITLE:",song_name,1);
		}
		if (strstr(string,"AUTHOR:")) {
			get_string(string,"AUTHOR:",author_name,1);
		}
		if (strstr(string,"COMMENTS:")) {
			get_string(string,"COMMENTS:",comments_ptr,0);
			comments_ptr=&comments[strlen(comments)];
		}
		if (strstr(string,"BPM:")) {
			get_string(string,"BPM:",temp,1);
			bpm=atoi(temp);
		}
		if (strstr(string,"TEMPO:")) {
			get_string(string,"TEMPO:",temp,1);
			tempo=atoi(temp);
		}
		if (strstr(string,"FREQ:")) {
			get_string(string,"FREQ:",temp,1);
			external_frequency=atoi(temp);
		}
		if (strstr(string,"IRQ:")) {
			get_string(string,"IRQ:",temp,1);
			irq=atoi(temp);
		}
		if (strstr(string,"LOOP:")) {
			get_string(string,"LOOP:",temp,1);
			loop=atoi(temp);
		}

	}

	if (bpm==120) {
		if (tempo==1) {
			baselen=96;	/* 120/min = 500ms, 50Hz=20ms 25*4=100 */
		}
		else if (tempo==3) {
			baselen=48;
		}
	}
	else if (bpm==136) {
		baselen=80;	/* 136/min = 440ms, 50Hz=20ms, 22*4 = 88 */
	}
	else if (bpm==160) {
		baselen=48;	/* 160/min = 266ms, 50Hz=20ms, 13*4 = 53 */
	}
	else {
		fprintf(stderr,"Warning!  Unusual BPM of %d\n",bpm);
		baselen=96;
	}
	/* 16 lines per frame in our text file (smallest note 16th note) */
	frames_per_line=baselen/16;

	/* Skip header, we'll fill in later */
	header_length=sizeof(struct ym_header)+
		strlen(song_name)+1+
		strlen(author_name)+1+
		strlen(comments)+1;

	fseek(ym_file, header_length, SEEK_SET);

	a.which='A';		b.which='B';		c.which='C';
	a.loud=15;		b.loud=15;		c.loud=15;
	a.arpeggio=0;		b.arpeggio=0;		c.arpeggio=0;
	a.portamento=0;		b.portamento=0;		c.portamento=0;
	a.vibrato=0;		b.vibrato=0;		c.vibrato=0;
	a.vibrato_count=0;	b.vibrato_count=0;	c.vibrato_count=0;
	a.sub_adjust=0;		b.sub_adjust=0;		c.sub_adjust=0;
	a.instrument=&instruments[0];			c.instrument=&instruments[0];
				b.instrument=&instruments[0];

	while(1) {
		result=fgets(string,BUFSIZ,in_file);
		if (result==NULL) break;
		line++;

		/* skip comments */
		if (string[0]=='\'') continue;
		if (string[0]=='-') continue;

		/* loudness */
		if (string[0]=='*') {
			get_directive(string);
			continue;
		}

		sp=0;

		/* Skip line number */
		while((string[sp]!=' ' && string[sp]!='\t')) sp++;

		sp=get_note(string,sp,&a,line);
		if (sp!=-1) sp=get_note(string,sp,&b,line);
		if (sp!=-1) sp=get_note(string,sp,&c,line);

		/* handle lyrics */
		if (sp!=-1) {
			while((string[sp]==' ' || string[sp]=='\t')) sp++;
			if ((string[sp]) && (string[sp]!='\n')) {
				fprintf(lyrics_file,"%d %s",
					frames,&string[sp]);
			}
		}

		for(j=0;j<frames_per_line;j++) {

			if (a.enabled) {
				update_note(&a);

				if ((a.arpeggio) || (a.portamento)) {
					if (j==0) {
						frame[0]=a.freq&0xff;
						frame[1]=(a.freq>>8)&0xf;
					}
					if (j==1) {
						frame[0]=a.freq2&0xff;
						frame[1]=(a.freq2>>8)&0xf;
					}
					if (j==2) {
						frame[0]=a.freq3&0xff;
						frame[1]=(a.freq3>>8)&0xf;
					}
				}
				else {
					frame[0]=a.freq&0xff;
					frame[1]=(a.freq>>8)&0xf;
				}

				frame[7]=0x38;

				if (a.instrument->noise) {
					frame[6]=calculate_noise(&a);
					frame[7]&=~enable_noise(&a,0);
				}

				frame[8]=calculate_amplitude(&a);
			}
			else {
				frame[0]=0x0;
				frame[1]=0x0;
				frame[7]|=0x8;
				frame[8]=0x0;
			}

			if (b.enabled) {
				update_note(&b);

				if ((b.arpeggio) || (b.portamento)) {
					if (j==0) {
						frame[2]=b.freq&0xff;
						frame[3]=(b.freq>>8)&0xf;
					}
					if (j==1) {
						frame[2]=b.freq2&0xff;
						frame[3]=(b.freq2>>8)&0xf;
					}
					if (j==2) {
						frame[2]=b.freq3&0xff;
						frame[3]=(b.freq3>>8)&0xf;
					}
				}
				else {
					frame[2]=b.freq&0xff;
					frame[3]=(b.freq>>8)&0xf;
				}
				frame[7]=0x38;
				frame[9]=calculate_amplitude(&b);

				if (b.instrument->noise) {
					frame[6]=calculate_noise(&b);
					frame[7]&=~enable_noise(&b,1);
				}
			}
			else {
				frame[2]=0x0;
				frame[3]=0x0;
				frame[7]|=0x10;
				frame[9]=0x0;
			}

			if (c.enabled) {
				update_note(&c);

				if ((c.arpeggio) || (c.portamento)) {
					if (j==0) {
						frame[4]=c.freq&0xff;
						frame[5]=(c.freq>>8)&0xf;
					}
					if (j==1) {
						frame[4]=c.freq2&0xff;
						frame[5]=(c.freq2>>8)&0xf;
					}
					if (j==2) {
						frame[4]=c.freq3&0xff;
						frame[5]=(c.freq3>>8)&0xf;
					}
				}
				else {
					frame[4]=c.freq&0xff;
					frame[5]=(c.freq>>8)&0xf;
				}
				frame[7]=0x38;
				frame[10]=calculate_amplitude(&c);

				if (c.instrument->noise) {
					frame[6]=calculate_noise(&c);
					frame[7]&=~enable_noise(&c,2);
				}
			}
			else {
				frame[4]=0x0;
				frame[5]=0x0;
				frame[7]|=0x20;
				frame[10]=0x0;
			}


			/* NOWRITE */
			frame[13]=0xff;

			for(i=0;i<16;i++) {
				fprintf(ym_file,"%c",frame[i]);
			}

			if (debug) {
				printf("%d\t",frames);
				for(i=0;i<16;i++) {
					printf("%4d",frame[i]);
				}
				printf("\n");
			}
			frames++;

			if (a.enabled) {
				a.left--;
				if (a.left<0) a.enabled=0;
			}

			if (b.enabled) {
				b.left--;
				if (b.left<0) b.enabled=0;
			}

			if (c.enabled) {
				c.left--;
				if (c.left<0) c.enabled=0;
			}

		}
		a.arpeggio=0;	b.arpeggio=0;	c.arpeggio=0;
		a.portamento=0;	b.portamento=0;	c.portamento=0;
		a.vibrato=0;	b.vibrato=0;	c.vibrato=0;
	}

	fgetpos(ym_file,&save);

	rewind(ym_file);

	strncpy(our_header.id,"YM5!",4);
	strncpy(our_header.check,"LeOnArD!",8);
	our_header.vbl=htonl(frames);
	our_header.song_attr=htonl(attributes);
	our_header.digidrum=htonl(digidrums);
	our_header.external_frequency=htonl(external_frequency);
	our_header.player_frequency=htons(irq);
	our_header.loop=htonl(loop);
	our_header.additional_data=htons(0);

	fwrite(&our_header,sizeof(struct ym_header),1,ym_file);

	fprintf(ym_file,"%s%c",song_name,0);
	fprintf(ym_file,"%s%c",author_name,0);
	fprintf(ym_file,"%s%c",comments,0);

	fsetpos(ym_file,&save);

	fprintf(ym_file,"End!");

	fclose(ym_file);
	fclose(lyrics_file);

	return 0;
}
