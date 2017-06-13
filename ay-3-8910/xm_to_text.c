#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "xm_lib.h"

static int print_usage(char *file) {

	printf("Usage: %s -h -c X,Y,Z in.xm out.txt\n\n",file);
	return 0;
}

int main(int argc, char **argv) {

	struct xm_info_struct xm;
	int result;
	char *filename;
	char *outname;
	FILE *outfile;
	int c;
	int ch0=0,ch1=1,ch2=2;

	while ((c = getopt(argc, argv, "hvc:"))!=-1) {
		switch(c) {
			case 'h':
				print_usage(argv[0]);
				exit(0);
				break;
			case 'c':
				/* Lazy Hack */
				ch0=optarg[0]-'0';
				ch1=optarg[2]-'0';
				ch2=optarg[4]-'0';
				printf("Using channels %d,%d,%d\n",ch0,ch1,ch2);
				break;
			default:
				printf("Unknown argument %c\n",c);
		}
	}

	if (optind<argc) {
		filename=argv[optind];
	}
	else {
		filename=strdup("stillalive.xm");
	}

	optind++;

	if (optind<argc) {
		outname=argv[optind];
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
		xm_to_text(outfile,&xm,ch0,ch1,ch2);
	}

	fclose(outfile);

	return 0;
}
