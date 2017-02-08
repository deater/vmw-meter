#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LYRICS	2048

struct lyric_type {
	int frame;
	char *text;
};

int lyrics_play(struct lyric_type *l, int num) {

	int frame=0,lnum=0;

	while(1) {
		frame++;

		if (frame==l[lnum].frame) {
			printf("%s",l[lnum].text);
			fflush(stdout);
			lnum++;
		}
		usleep(20000);
		if (lnum==num) break;
	}

	return 0;

}

int main(int argc, char **argv) {

	char *result;
	char string[BUFSIZ];

	struct lyric_type *l;
	int num=0,line=0;
	int sp;
	int frame_num;
	char *start;

	l=calloc(sizeof(struct lyric_type),MAX_LYRICS);
	if (l==NULL) {
		fprintf(stderr,"MEM ERROR\n");
		return -1;
	}

	while(1) {
		result=fgets(string,BUFSIZ,stdin);
		if (result==NULL) break;
		line++;

//		printf("%s",string);

		sp=0;

		/* Ignore comments */
		if (string[0]=='#') continue;

		/* skip whitespace */
		while((string[sp]==' ' || string[sp]=='\t')) sp++;
		if (string[sp]=='\n') continue;

		/* get number */
		frame_num=0;
		while((string[sp]!=' ' && string[sp]!='\t' &&
			string[sp]!='\n')) {
			frame_num*=10;
			frame_num+=string[sp]-'0';
			sp++;
		}

		l[num].frame=frame_num;

		/* skip whitespace */
		while((string[sp]==' ' || string[sp]=='\t')) sp++;
		if (string[sp]=='\n') continue;

		if (string[sp]!='\"') {
			fprintf(stderr,"Unknown string line %d\n",line);
			continue;
		}
		sp++;

		start=&string[sp];

		while((string[sp]!='\"' && string[sp]!='\n')) {
			sp++;
		}
		string[sp]='\0';

		l[num].text=strdup(start);


//		printf("%d %s\n",l[num].frame,l[num].text);

		num++;
		if (num>MAX_LYRICS) {
			fprintf(stderr,"Too many lyrics, line %d!\n",line);
			return -1;
		}
	}

	lyrics_play(l,num);

	free(l);

	return 0;
}
