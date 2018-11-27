/* Decode a vortex-tracker PT3 file */
/* Vortex Tracker itself is written in Pascal and comments are in Russian */
/* which makes it a bit hard to understand */
/* This is based partly on the writeup here: */
/*   http://karoshi.auic.es/index.php?topic=397.msg4641#msg4641 */

/* PT3 Format */

/* Header */
// $00 - $0C : 13 bytes : Magic       : "ProTracker 3."
// $0D       :  1 byte  : Version     : '5' for Vortex Tracker II
// $0E - $1D : 16 bytes : String      : " compilation of "
// $1E - $3E : 32 bytes : Name        : Name of the module
// $3E - $41 :  4 bytes : String      : " by "
// $42 - $62 : 32 bytes : Author      : Author of the module.
// $63       :  1 byte  : Frequency table (from 1 to 4)
// $64       :  1 byte  : Speed/Delay
// $65       :  1 byte  : Number of patterns
// $66       :  1 byte  : LPosPtr     : Pattern loop/LPosPtr
// $67       :  2 bytes : PatsPtrs    : Position of patterns inside the module
// $69       : 64 bytes : SamPtrs[32] : Position of samples inside the module
// $A9       : 32 bytes : OrnPtrs[16] : Position of ornaments inside the module
// $C9       :  2 bytes : CrPsPtr     : Points to pattern order

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

struct header_info_t {
	char magic[13+1];
	char version;
	char name[32];
	char author[32];
	int frequency_table;
	int speed;
	int num_patterns;
	int loop;
	short pattern_loc;
	short sample_patterns[32];
	short ornament_patterns[16];
	short pattern_order;
} header;

#define HEADER_SIZE 0xCB

static char raw_header[HEADER_SIZE];

static int debug=1;


static int load_header(int fd) {

	memset(&raw_header,0,HEADER_SIZE);

	read(fd,raw_header,HEADER_SIZE);
	if (read<0) return -1;

	/* Magic */
	memcpy(&header.magic,&raw_header[0],13);
	if (memcmp(header.magic,"ProTracker 3.",13)) {
		fprintf(stderr,"Wrong magic %s != %s\n",
			header.magic,"ProTracker 3.");

		return -1;
	}

	/* version */
	header.version=raw_header[0xd];

	return 0;

}

int main(int argc, char **argv) {

	char filename[BUFSIZ];
	int fd;
	int result;

	if (argc>1) {
		strncpy(filename,argv[1],BUFSIZ-1);
	}
	else {
		strncpy(filename,"ea.pt3",BUFSIZ-1);
	}

	fd=open(filename,O_RDONLY);
	if (fd<0) {
		fprintf(stderr,"Error opening %s: %s\n",
			filename,strerror(errno));
		return -1;

	}

	result=load_header(fd);
	if (result) {
		fprintf(stderr,"Error loading!\n");
		close(fd);
		return -1;
	}

	close(fd);

	/* Print header info */
	if (debug) {
		printf("%s%c\n",header.magic,header.version);
	}


	return 0;
}
