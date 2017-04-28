#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <math.h>

#include "notes.h"

static int debug=0;

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

	int len=0;
	int baselen=96;  /* 120/minute, 50Hz, should really be 100 */

	switch(length) {
		case 1:	len=baselen; break;
		case 2: len=baselen/2; break;
		case 3: len=(baselen*3)/8; break;
		case 4: len=baselen/4; break;
		case 5: len=(baselen*5)/8; break;
		case 8: len=baselen/8; break;
		case 9: len=(baselen*3)/16; break;
		case 6: len=baselen/16; break;
		default:
			fprintf(stderr,"Unknown length %d\n",length);
	}

	return len-2;
}

#define FRAMES_PER_LINE	6


struct note_type {
	unsigned char which;
	unsigned char note;
	int sharp,flat;
	int octave;
	int len;
	int enabled;
	int freq;
	int length;
};

static int external_frequency=1000000;


int get_note(char *string, int sp, struct note_type *n) {

	double freq;

	n->sharp=0;
	n->flat=0;

	/* Skip white space */
	while((string[sp]==' ' || string[sp]=='\t')) sp++;

	if (string[sp]=='\n') return -1;

	/* get note info */
	n->note=string[sp];
	sp++;
	if (string[sp]=='#') n->sharp=1;
	if (string[sp]=='-') n->flat=1;
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
	}
	else {
		n->freq=0;
	}


	return sp;
}

int main(int argc, char **argv) {

	char string[BUFSIZ];
	char *result;
	char *ym_filename,*lyrics_filename,*in_filename;
	FILE *ym_file,*lyrics_file,*in_file;
	int frames=0,digidrums=0;
	int frequency=50,attributes=0;
	int loop=0;
	int header_length=0;
	int sp,i,j;
	fpos_t save;
	int line=0;
	struct note_type a,b,c;

	char song_name[]="Still Alive";
	char author_name[]="Vince Weaver <vince@deater.net>";
	char comments[]="from Portal, Words and Music by Jonathan Coulton";

	unsigned char frame[16];



	if (argc<2) {
		printf("%s -- create a YM music file\n",argv[0]);
		printf("\n");
		printf("Usage:	%s INFILE OUTROOT\n\n",argv[0]);
		printf("\n");
		exit(1);
	}

	in_filename=strdup("sa.txt");
	ym_filename=strdup("sa.ym");
	lyrics_filename=strdup("sa.lyrics");

	in_file=fopen(in_filename,"r");
	if (in_file==NULL) {
		fprintf(stderr,"Couldn't open %s\n",in_filename);
		return -1;
	}

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

		sp=get_note(string,sp,&a);
		if (sp!=-1) sp=get_note(string,sp,&b);
		if (sp!=-1) sp=get_note(string,sp,&c);

		/* handle lyrics */
		if (sp!=-1) {
			while((string[sp]==' ' || string[sp]=='\t')) sp++;
			if (string[sp]!='\n') {
				fprintf(lyrics_file,"%d %s",frames,&string[sp]);
			}
		}

		for(j=0;j<FRAMES_PER_LINE;j++) {

			if (a.enabled) {
				frame[0]=a.freq&0xff;
				frame[1]=(a.freq>>8)&0xf;
				frame[7]=0x38;
				frame[8]=0x0f;	// amp A
			}
			else {
				frame[8]=0x0;
			}

			if (b.enabled) {
				frame[2]=b.freq&0xff;
				frame[3]=(b.freq>>8)&0xf;
				frame[7]=0x38;
				frame[9]=0x0a;	// amp B
			}
			else {
				frame[9]=0x0;
			}

			if (c.enabled) {
				frame[4]=c.freq&0xff;
				frame[5]=(c.freq>>8)&0xf;
				frame[7]=0x38;
				frame[10]=0x0a;	// amp C
			}
			else {
				frame[10]=0x0;
			}

			for(i=0;i<16;i++) {
				fprintf(ym_file,"%c",frame[i]);
			}
			frames++;

			if (a.length) a.length--;
			if (a.length==0) a.enabled=0;

			if (b.length) b.length--;
			if (b.length==0) b.enabled=0;

			if (c.length) c.length--;
			if (c.length==0) c.enabled=0;

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
	our_header.player_frequency=htons(frequency);
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

