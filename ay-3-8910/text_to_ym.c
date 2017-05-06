#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <math.h>

#include "notes.h"


#define MAX_INSTRUMENTS	7

#define DEFAULT	0

struct instrument_type {
	int attack[16];
	int attack_size;
	int decay[16];
	int decay_size;
	int sustain;
	int release[16];
	int release_size;
        char *name;
};


struct instrument_type instruments[MAX_INSTRUMENTS] = {
	{
	.name="raw",
	.attack_size=0,
	.decay_size=0,
	.release_size=1,
	.release={0},
	.sustain=15,
	},
	{
	.name="silence",
	.attack_size=0,
	.decay_size=0,
	.release_size=1,
	.release={0},
	.sustain=0,
	},
	{
	.name="piano",
	.attack={14,15},
	.attack_size=2,
	.decay={14},
	.decay_size=1,
	.sustain=13,
	.release={10,5},
	.release_size=2,
	},
	{
	.name="piano2",
	.attack={13,14,15},
	.attack_size=3,
	.decay={14,13,12},
	.decay_size=3,
	.sustain=11,
	.release={10},
	.release_size=1,

	},
};


static int debug=0;

static int external_frequency=1000000;
static int bpm=120;
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
		default:
			fprintf(stderr,"Unknown length %d\n",length);
	}

	return len;
}

struct note_type {
	unsigned char which;
	unsigned char note;
	int sharp,flat;
	int octave;
	int len;
	int enabled;
	int freq;
	int length;
	int left;
};



static int get_note(char *string, int sp, struct note_type *n, int line) {

	double freq;
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


	if (n->note!='-') {

		freq=note_to_freq(n->note,n->sharp,n->flat,n->octave);

		if (debug) printf("(%c) %c%c L=%d O=%d f=%lf\n",
				n->which,
				n->note,
				n->sharp?'#':' ',
				n->len,
				n->octave,
				freq);

		n->freq=external_frequency/(16.0*freq);
		n->enabled=1;
		n->length=note_to_length(n->len);
		n->left=n->length-1;

		if (n->length<=0) {
			printf("Error line %d\n",line);
			exit(-1);
		}
	}
	else {
		n->freq=0;
	}

	return sp;
}

static int calculate_amplitude(struct note_type *n,
				struct instrument_type *i,
				int debug) {

	int result=0;

/*
 A  A  A  D  D  D                    R R
16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0

length=17
*/
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

	if (debug) printf("%d %d %d\n",n->length,n->left,result);
//		((n->length-n->left)*16)/n->length,
//		instruments[DEFAULT].envelope[
//			((n->length-n->left)*16)/n->length
//		]);

	return result;
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
	struct note_type a,b,c;

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
		baselen=96;	/* 120/min = 500ms, 50Hz=20ms 25*4=100 */
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

	a.which='A';	b.which='B';	c.which='C';

	while(1) {
		result=fgets(string,BUFSIZ,in_file);
		if (result==NULL) break;
		line++;

		/* skip comments */
		if (string[0]=='\'') continue;
		if (string[0]=='-') continue;

		sp=0;

		/* Skip line number */
		while((string[sp]!=' ' && string[sp]!='\t')) sp++;

		sp=get_note(string,sp,&a,line);
		if (sp!=-1) sp=get_note(string,sp,&b,line);
		if (sp!=-1) sp=get_note(string,sp,&c,line);

		/* handle lyrics */
		if (sp!=-1) {
			while((string[sp]==' ' || string[sp]=='\t')) sp++;
			if (string[sp]!='\n') {
				fprintf(lyrics_file,"%d %s",frames,&string[sp]);
			}
		}

		for(j=0;j<frames_per_line;j++) {

			if (a.enabled) {
				frame[0]=a.freq&0xff;
				frame[1]=(a.freq>>8)&0xf;
				frame[7]=0x38;
				//frame[8]=0x0f;	// amp A
				frame[8]=calculate_amplitude(&a,
					&instruments[DEFAULT],1);
			}
			else {
				frame[0]=0x0;
				frame[1]=0x0;
				frame[8]=0x0;
			}

			if (b.enabled) {
				frame[2]=b.freq&0xff;
				frame[3]=(b.freq>>8)&0xf;
				frame[7]=0x38;
				//frame[9]=0x0f;	// amp B
				frame[9]=calculate_amplitude(&b,
					&instruments[DEFAULT],0);
			}
			else {
				frame[2]=0x0;
				frame[3]=0x0;
				frame[9]=0x0;
			}

			if (c.enabled) {
				frame[4]=c.freq&0xff;
				frame[5]=(c.freq>>8)&0xf;
				frame[7]=0x38;
				//frame[10]=0x0f;	// amp C
				frame[10]=calculate_amplitude(&c,
					&instruments[DEFAULT],0);
//				frame[10]=instruments[1].envelope[(c.length-c.left)/16];
			}
			else {
				frame[4]=0x0;
				frame[5]=0x0;
				frame[10]=0x0;
			}


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

			if (a.left) a.left--;
			if (a.left<0) a.enabled=0;

//		printf("a.length=%d a.enabled=%d\n",a.length,a.enabled);

			if (b.left) b.left--;
			if (b.left<0) b.enabled=0;

			if (c.left) c.left--;
			if (c.left<0) c.enabled=0;

		}
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

