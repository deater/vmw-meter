#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

#include "lyrics.h"

int load_lyrics(char *filename, struct lyric_type *l) {

	char *result;
	char string[BUFSIZ];

	int num=0,line=0;
	int sp;
	int frame_num;
	char *start;

	l->l=calloc(sizeof(struct lyric_type),MAX_LYRICS);
	if (l->l==NULL) {
		fprintf(stderr,"MEM ERROR\n");
		return -1;
	}

	while(1) {
		result=fgets(string,BUFSIZ,stdin);
		if (result==NULL) {
			break;
		}
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

		l->l[num].frame=frame_num;

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

		l->l[num].text=strdup(start);


//		printf("%d %s\n",num,start);
//		fflush(stdout);
//		printf("%d %s\n",l[num].frame,l[num].text);

		num++;
		if (num>MAX_LYRICS) {
			fprintf(stderr,"Too many lyrics, line %d!\n",line);
			return -1;
		}
	}

	l->num=num;

	return 0;
}

int destroy_lyrics(struct lyric_type *l) {

	free(l->l);

	return 0;
}
