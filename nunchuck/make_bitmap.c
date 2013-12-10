#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv) {

	char line[8][BUFSIZ];
	int i=0,x,y;
	char *ptr;
	int overall_max_length=0;
	int max_length[8];
	int value;

	while(1) {
		if (i==8) break;

		ptr=fgets(line[i],BUFSIZ,stdin);
		if (ptr==NULL) break;

		i++;

	}

	for(x=0;x<i;x++) {
		printf("%s",line[x]);
		max_length[x]=strlen(line[x]);
		if (strlen(line[x])>overall_max_length) {
			overall_max_length=strlen(line[x]);
		}
	}

	printf("Max length: %d\n",overall_max_length);

	for(x=overall_max_length;x>=0;x--) {
		value=0;
		for(y=0;y<i;y++) {
			if (x<max_length[y]) value|=(!isspace(line[y][x]))<<(7-y);
		}
		printf("\t0x%x,	/* ",value);
		for(y=0;y<i;y++) {
			if ((x<max_length[y]) && (line[y][x]!='\n')) {
				printf("%c",line[y][x]);
			}
			else {
				printf(" ");
			}
		}
		printf(" */\n");
	}

	return 0;
}
