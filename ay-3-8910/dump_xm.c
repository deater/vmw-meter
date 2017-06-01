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

	if (argc>1) {
		filename=argv[1];
	}
	else {
		filename=strdup("stillalive.xm");
	}

	result=load_xm_file(filename,&xm);

	if (result<0) {

	}
	else {
		dump_xm_file(&xm);
	}

	return 0;
}
