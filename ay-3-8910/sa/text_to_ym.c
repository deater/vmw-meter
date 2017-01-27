#include <stdio.h>

int main(int argc, char **argv) {

	char string[BUFSIZ];
	char *result;

	while(1) {
		result=fgets(string,BUFSIZ,stdin);
		if (result==NULL) break;

		/* skip comments */
		if (string[0]=='\'') continue;
		if (string[0]=='-') continue;

		printf("%s",string);
	}

	return 0;
}
