#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "xm_lib.h"

int main(int argc, char **argv) {

	struct xm_info_struct xm;
	int result;
	char *filename;
	char *outname;
	FILE *outfile;

	if (argc>1) {
		filename=argv[1];
	}
	else {
		filename=strdup("stillalive.xm");
	}

	if (argc>2) {
		outname=argv[2];
	}
	else {
		outname=strdup("stillalive.txt");
	}

	result=load_xm_file(filename,&xm);

	if (!strcmp(outname,"-")) {
		outfile=stdout;
	}
	else {
		outfile=fopen(outname,"w");
		if (outfile==NULL) {
			fprintf(stderr,"Could not open %s\n",outname);
			return -1;
		}

	}

	if (result<0) {

	}
	else {
		xm_to_text(outfile,&xm);
	}

	fclose(outfile);

	return 0;
}
