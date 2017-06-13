#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "xm_lib.h"

/* too big to allocate on stack */
static struct xm_info_struct xm;

int main(int argc, char **argv) {


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
