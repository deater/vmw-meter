/* Decode a vortex-tracker PT3 file */
/* Vortex Tracker itself is written in Pascal and comments are in Russian */
/* which makes it a bit hard to understand */
/* This is based partly on the writeup here: */
/*   http://karoshi.auic.es/index.php?topic=397.msg4641#msg4641 */

/* PT3 Format */

/* Header */
/* Note: 16-bit values are little-endian */

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
// $67 - $68 :  2 bytes : PatsPtrs    : Position of patterns inside the module
// $69 - $A8 : 64 bytes : SamPtrs[32] : Position of samples inside the module
// $A9 - $C8 : 32 bytes : OrnPtrs[16] : Position of ornaments inside the module
// ***************** this is wrong? $C9 - $CA :  2 bytes : CrPsPtr     : Points to pattern order
// pattern order follows starting at $C9, $ff terminated
// each pattern number is multiplied by 3

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

struct header_info_t {
	char magic[13+1];
	char version;
	char name[32+1];
	char author[32+1];
	int frequency_table;
	int speed;
	int num_patterns;
	int loop;
	unsigned short pattern_loc;
	unsigned short sample_patterns[32];
	unsigned short ornament_patterns[16];
	unsigned short pattern_order;
} header;

#define HEADER_SIZE 0xCB
#define MAX_PT3_SIZE	65536

static unsigned char raw_header[HEADER_SIZE];
static unsigned char pt3_data[MAX_PT3_SIZE];

static int debug=1;


static int load_header(void) {

	int i;

	/* Magic */
	memcpy(&header.magic,&raw_header[0],13);
	if (memcmp(header.magic,"ProTracker 3.",13)) {
		fprintf(stderr,"Wrong magic %s != %s\n",
			header.magic,"ProTracker 3.");

		return -1;
	}

	/* version */
	header.version=raw_header[0xd];

	/* Name */
	memcpy(&header.name,&raw_header[0x1e],32);

	/* Author */
	memcpy(&header.author,&raw_header[0x42],32);

	/* Frequency Table */
	header.frequency_table=raw_header[0x63];

	/* Speed */
	header.speed=raw_header[0x64];

	/* Number of Patterns */
	header.num_patterns=raw_header[0x65];

	/* Loop Pointer */
	header.loop=raw_header[0x66];

	/* Pattern Position */
	header.pattern_loc=(raw_header[0x68]<<8)|raw_header[0x67];

	/* Sample positions */
	for(i=0;i<32;i++) {
		header.sample_patterns[i]=
			(raw_header[0x6a+(i*2)]<<8)|raw_header[0x69+(i*2)];
	}

	/* Ornament Positions */
	for(i=0;i<16;i++) {
		header.ornament_patterns[i]=
			(raw_header[0xaa+(i*2)]<<8)|raw_header[0xa9+(i*2)];
	}

	/* Pattern Order */
	header.pattern_order=(raw_header[0xca]<<8)|raw_header[0xc9];

	return 0;

}

int main(int argc, char **argv) {

	char filename[BUFSIZ];
	int fd,i,j;
	int result,sample_loop,sample_len;

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


	memset(&pt3_data,0,MAX_PT3_SIZE);

	result=read(fd,pt3_data,MAX_PT3_SIZE);
	if (result<0) {
		fprintf(stderr,"Error reading file: %s\n",
			strerror(errno));
		return -1;
	}

	close(fd);

	memcpy(&raw_header,&pt3_data,HEADER_SIZE);
	result=load_header();
	if (result) {
		fprintf(stderr,"Error decoding header!\n");
		return -1;
	}



	/* Print header info */
	if (debug) {
		printf("%s%c\n",header.magic,header.version);
		printf("\tNAME: %s\n",header.name);
		printf("\tBY  : %s\n",header.author);
		printf("\tFreqTable: %d Speed: %d  Patterns: %d Loop: %d\n",
			header.frequency_table,
			header.speed,
			header.num_patterns,
			header.loop);
		printf("\tPattern Location Offset: %04x\n",header.pattern_loc);
		printf("\tSample pattern addresses:");
		for(i=0;i<32;i++) {
			if (i%8==0) printf("\n\t\t");
			printf("%04x ",header.sample_patterns[i]);
		}
		printf("\n");
		printf("\tOrnament patterns:");
		for(i=0;i<16;i++) {
			if (i%8==0) printf("\n\t\t");
			printf("%04x ",header.ornament_patterns[i]);
		}
		printf("\n");
//		printf("\tPattern order @%04x\n",header.pattern_order);

		i=0;
		printf("\tPattern order:");
		while(1) {
			if (i%16==0) printf("\n\t\t");
			if (pt3_data[0xc9+i]==0xff) break;
			printf("%02d ",pt3_data[0xc9+i]/3);
			i++;
		}
		printf("\n");

		printf("\tSample Dump:\n");
		for(i=0;i<32;i++) {
			printf("\t\t%i: ",i);
			if (header.sample_patterns[i]==0) printf("N/A\n");
			else {
				sample_loop=pt3_data[0+
						header.sample_patterns[i]];
				sample_len=pt3_data[1+
						header.sample_patterns[i]];
				printf("Loop: %d Length: %d\n",
						sample_loop,
						sample_len);
				for(j=0;j<sample_len;j++) {
					printf("\t\t\t%02x %02x %02x %02x\n",
						pt3_data[2+(j*4)+
						header.sample_patterns[i]],
						pt3_data[3+(j*4)+
						header.sample_patterns[i]],
						pt3_data[4+(j*4)+
						header.sample_patterns[i]],
						pt3_data[5+(j*4)+
						header.sample_patterns[i]]);
				}
			}
		}



	}

	return 0;
}
