#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define OCTAVE_UP 0

#define MAXSIZE	4096

static int channel0[MAXSIZE],channel1[MAXSIZE],channel2[MAXSIZE];
static int len0[MAXSIZE],len1[MAXSIZE],len2[MAXSIZE];

static int note_lookup[8]={
	/* A  B  C  D  E  F  G */
	   9,11, 0, 2, 4, 5, 7,
};

static int text_line=0;

static int len_lookup(int length) {

	int len=0;
	int frames_per_whole=96;

	switch(length) {
		case 0: len=(frames_per_whole*5)/2; break;      // 0 = 2.5
		case 1: len=frames_per_whole; break;            // 1 =   1 whole
		case 2: len=frames_per_whole/2; break;          // 2 = 1/2 half
		case 3: len=(frames_per_whole*3)/8; break;      // 3 = 3/8 dotted quarter
                case 4: len=frames_per_whole/4; break;          // 4 = 1/4 quarter
                case 5: len=(frames_per_whole*5)/8; break;      // 5 = 5/8 ?
                case 8: len=frames_per_whole/8; break;          // 8 = 1/8 eighth
                case 9: len=(frames_per_whole*3)/16; break;     // 9 = 3/16 dotted eighth
                case 6: len=(frames_per_whole)/16; break;       // 6 = 1/16 sixteenth
                case 10: len=(frames_per_whole*3)/4; break;     // : = 3/4 dotted half
                case 11: len=(frames_per_whole*9)/8; break;     // ; = 9/8 dotted half + dotted quarter
                case 12: len=(frames_per_whole*3)/2; break;     // < = 3/2 dotted whole
                case 13: len=(frames_per_whole*2); break;       // = = 2   double whole
		case 14: len=(frames_per_whole/3); break;       // > = 1/3 triple note
                case 15: len=255;     break;          // ? = forever
                default:
                        fprintf(stderr,"Unknown length %d line %d\n",
                                length,text_line);
        }

        return len;
}



static char song_name[BUFSIZ]="highwind";
static char song_name_cap[BUFSIZ]="HIGHWIND";


int main(int argc, char **argv) {

	char line[BUFSIZ];
	char *result;
	int count=0;
	char *split;
	int i;
	int note,sharp,octave,len,p;

	if (argc>1) {
		strncpy(song_name,argv[1],BUFSIZ-1);
		strncpy(song_name_cap,argv[1],BUFSIZ-1);
		for(i=0;i<strlen(song_name_cap);i++) {
			song_name_cap[i]=toupper(song_name_cap[i]);
		}
	}

	while(1) {
		result=fgets(line,BUFSIZ,stdin);
		text_line++;
		if (result==NULL) break;
		if (result[0]=='\'') continue;

		/* skip first line */
		split=strtok(line," \t");

		/* Get next */
		split=strtok(NULL,"\t");

		if (split==NULL) {
//			fprintf(stderr,"ERRROR line %d!\n",count);
			continue;
		}

		if (split[0]=='-') {
			channel0[count]=0;
		}
		else {
			note=split[0];
			sharp=split[1];
			octave=split[2];
			len=split[4];

			p=note_lookup[(note-'A')];
			if (sharp=='#') p++;
			if (sharp=='-') p--;

			// scale up an octave
			if (OCTAVE_UP) {
				octave++;
			}

			p+=((octave-'0')+1)*12;

			channel0[count]=p;
			len0[count]=len_lookup(len-'0');

//			printf("%d %c%c%c %c\n",p,note,sharp,octave,len);
		}

		/* Get next */
		split=strtok(NULL,"\t\n");

		if (split[0]=='-') {
			channel1[count]=0;
		}
		else {
			note=split[0];
			sharp=split[1];
			octave=split[2];
			len=split[4];

			p=note_lookup[(note-'A')];
			if (sharp=='#') p++;
			if (sharp=='-') p--;

			if (OCTAVE_UP) {
				octave++;
			}
			p+=((octave-'0')+1)*12;

			channel1[count]=p;
			len1[count]=len_lookup(len-'0');

//			printf("%d %c%c%c %c\n",p,note,sharp,octave,len);
		}

		/* Get next */
		split=strtok(NULL,"\t\n");

		if (split[0]=='-') {
			channel2[count]=0;
		}
		else {
			note=split[0];
			sharp=split[1];
			octave=split[2];
			len=split[4];

			p=note_lookup[(note-'A')];
			if (sharp=='#') p++;
			if (sharp=='-') p--;

			if (OCTAVE_UP) {
				octave++;
			}

			p+=((octave-'0')+1)*12;

			channel2[count]=p;
			len2[count]=len_lookup(len-'0');

//			printf("%d %c%c%c %c\n",p,note,sharp,octave,len);
		}



		count++;
	}



	printf("#define %s_SONG_LENGTH %d\n",song_name_cap,count);

	printf("unsigned char %s_channel0[%s_SONG_LENGTH]={",song_name,song_name_cap);
	for(i=0;i<count;i++) {
		if(i%8==0) printf("\n\t");
		printf("0x%02X,",channel0[i]);
	}
	printf("\n};\n");

	printf("unsigned char %s_len0[%s_SONG_LENGTH]={",song_name,song_name_cap);
	for(i=0;i<count;i++) {
		if(i%8==0) printf("\n\t");
		printf("0x%02X,",len0[i]);
	}
	printf("\n};\n");

	printf("unsigned char %s_channel1[%s_SONG_LENGTH]={",song_name,song_name_cap);
	for(i=0;i<count;i++) {
		if(i%8==0) printf("\n\t");
		printf("0x%02X,",channel1[i]);
	}
	printf("\n};\n");

	printf("unsigned char %s_len1[%s_SONG_LENGTH]={",song_name,song_name_cap);
	for(i=0;i<count;i++) {
		if(i%8==0) printf("\n\t");
		printf("0x%02X,",len1[i]);
	}
	printf("\n};\n");

	printf("unsigned char %s_channel2[%s_SONG_LENGTH]={",song_name,song_name_cap);
	for(i=0;i<count;i++) {
		if(i%8==0) printf("\n\t");
		printf("0x%02X,",channel2[i]);
	}
	printf("\n};\n");

	printf("unsigned char %s_len2[%s_SONG_LENGTH]={",song_name,song_name_cap);
	for(i=0;i<count;i++) {
		if(i%8==0) printf("\n\t");
		printf("0x%02X,",len2[i]);
	}
	printf("\n};\n");

	return 0;
}
