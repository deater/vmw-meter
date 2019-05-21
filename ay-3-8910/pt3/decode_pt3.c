/* Dump a pt3 file, much like a tracker would */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "pt3_lib.h"

int main(int argc, char **argv) {

	char filename[BUFSIZ];
	int result,current_time=0;
	struct pt3_song_t pt3,pt3_2;
	struct pt3_image_t pt3_image;
	int total_time,loop;

	int i,j;

	/******************************/
	/* Get command line arguments */
	/******************************/

	if (argc>1) {
		strncpy(filename,argv[1],BUFSIZ-1);
	}
	else {
		strncpy(filename,"ea.pt3",BUFSIZ-1);
	}


	/* Load song */
	result=pt3_load_song(filename, &pt3_image, &pt3, &pt3_2);
	if (result<0) {
		fprintf(stderr,"Error opening file %s\n",filename);
		return -1;
	}

	pt3_calc_frames(&pt3, &total_time, &loop);

	for(i=0;i < pt3.music_len;i++) {

		pt3_set_pattern(i,&pt3);

		printf("Chunk %d/%d, %02d:%02d/%02d:%02d, Pattern #%d\n",
			i,pt3.music_len-1,
			(current_time/50/60),(current_time/50)%60,
			(total_time/50/60),(total_time/50)%60,
			pt3.current_pattern);
		printf("a_addr: %04x, b_addr: %04x, c_addr: %04x\n",
				pt3.a.addr,pt3.b.addr,pt3.c.addr);

//		printf("Cdata: ");
//		for(j=0;j<32;j++) printf("%02x ",pt3.data[pt3.c_addr+j]);
//		printf("\n");

		for(j=0;j<64;j++) {

//			printf("VMW frame: %d\n",frames);

			/* decode line. 1 if done early */
			if (pt3_decode_line(&pt3)) break;

			/* Print line of tracker */
			pt3_print_tracker_line(&pt3,j);
			current_time+=pt3.speed;

			printf("\n");
		}
	}

	return 0;

}
