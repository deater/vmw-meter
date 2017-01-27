#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>

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

#define TWELTH_TWO 1.059463094359

// http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html
static double note_to_freq(char note, int flat, int sharp, int octave) {

	double freq=0.0;
	int step=0;

	switch(note) {
		case 'B': step=2; break;
		case 'A': step=0; break;
		case 'G': step=-2; break;
		case 'F': step=-4; break;
		case 'E': step=-5; break;
		case 'D': step=-7; break;
		case 'C': step=-9; break;
		default:
			fprintf(stderr,"Unknown note %c\n",note);
	}
	if (flat) step++;
	if (sharp) step--;

	step-=(4-octave)*12;

	freq=440.0*pow(TWELTH_TWO,step);

	return freq;
}

#define FRAMES_PER_LINE	12

int main(int argc, char **argv) {

	char string[BUFSIZ];
	char *result;
	char *outfile;
	FILE *fff;
	int frames=0,digidrums=0,external_frequency=1000000;
	int frequency=50,attributes=0;
	int loop=0;
	int header_length=0;
	int i,j,a_freq=0;
	fpos_t save;
	double freq;
	int line=0;
	unsigned char note;
	int sharp,flat,length,octave;

	char song_name[]="Still Alive";
	char author_name[]="Vince Weaver <vince@deater.net>";
	char comments[]="from Portal, Words and Music by Jonathan Coulton";

	unsigned char frame[16];

	outfile=strdup("out.ym");

	fff=fopen(outfile,"w");
	if (fff==NULL) {
		fprintf(stderr,"Couldn't open %s\n",outfile);
		return -1;
	}

	/* Skip header, we'll fill in later */
	header_length=sizeof(struct ym_header)+
		strlen(song_name)+1+
		strlen(author_name)+1+
		strlen(comments)+1;

	fseek(fff, header_length, SEEK_SET);

	while(1) {
		result=fgets(string,BUFSIZ,stdin);
		if (result==NULL) break;
		line++;

		/* skip comments */
		if (string[0]=='\'') continue;
		if (string[0]=='-') continue;

		i=0;
		sharp=0;
		flat=0;
		/* Skip line number */
		while((string[i]!=' ' && string[i]!='\t')) i++;

		/* Skip white space */
		while((string[i]==' ' || string[i]=='\t')) i++;

		note=string[i];
		i++;
		if (string[i]=='#') sharp=1;
		if (string[i]=='-') flat=1;
		i++;
		octave=string[i]-'0';
		i++;
		i++;
		length=string[i]-'0';

		if (note!='-') {

			freq=note_to_freq(note,sharp,flat,octave);

			printf("%d: %c%c L=%d O=%d f=%lf\n",
				line,
				note,
				sharp?'#':' ',
				length,
				octave,
				freq);

			a_freq=external_frequency/(16.0*freq);
		}
		else {
			a_freq=0;
		}

		frame[0]=a_freq&0xff;
		frame[1]=(a_freq>>8)&0xf;
		frame[7]=0x3e;
		frame[8]=0x0f;	// amp A

		for(j=0;j<FRAMES_PER_LINE;j++) {
			for(i=0;i<16;i++) {
				fprintf(fff,"%c",frame[i]);
			}
			frames++;
		}
	}

	fgetpos(fff,&save);

	rewind(fff);

	strncpy(our_header.id,"YM5!",4);
	strncpy(our_header.check,"LeOnArD!",8);
	our_header.vbl=htonl(frames);
	our_header.song_attr=htonl(attributes);
	our_header.digidrum=htonl(digidrums);
	our_header.external_frequency=htonl(external_frequency);
	our_header.player_frequency=htons(frequency);
	our_header.loop=htonl(loop);
	our_header.additional_data=htons(0);

	fwrite(&our_header,sizeof(struct ym_header),1,fff);

	fprintf(fff,"%s%c",song_name,0);
	fprintf(fff,"%s%c",author_name,0);
	fprintf(fff,"%s%c",comments,0);

	fsetpos(fff,&save);

	fprintf(fff,"End!");

	fclose(fff);

	return 0;
}
