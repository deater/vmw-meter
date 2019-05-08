/* Some code based on Formats.pas in Bulba's ay_emul */

/* Convert pt3 file to ym file */


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


struct ym_header {
        char id[4];                             // 0  -> 4
        char check[8];                          // 4  -> 12
        uint32_t vbl;                           // 12 -> 16
        uint32_t song_attr;                     // 16 -> 20
        uint16_t digidrum;                      // 20 -> 22
        uint32_t external_frequency;            // 22 -> 26
        uint16_t player_frequency;              // 26 -> 28
        uint32_t loop;                          // 28 -> 32
        uint16_t additional_data;               // 32 -> 34
}  __attribute__((packed)) our_header;







int main(int argc, char **argv) {

	char filename[BUFSIZ];
	char out_filename[BUFSIZ];
	int out_fd;
	int result;
	struct pt3_song_t pt3;

	int ym5_header_length;
	char ym5_comment[]="VMW pt3_to_ym5";
	int ym5_digidrums=0;
	int ym5_attributes=0;
	int ym5_irq=50;
	unsigned char frame[16];
	int frames=0,loop,total_frames;

	int i,j,f;

	int external_frequency=1773400;

	/******************************/
	/* Get command line arguments */
	/******************************/

	if (argc>1) {
		strncpy(filename,argv[1],BUFSIZ-1);
	}
	else {
		strncpy(filename,"ea.pt3",BUFSIZ-1);
	}

	if (argc>2) {
		strncpy(out_filename,argv[2],BUFSIZ-1);
	}
	else {
		strncpy(out_filename,"out.ym",BUFSIZ-1);
	}

	/* Load song */
	result=pt3_load_song(filename, &pt3);
	if (result<0) {
		fprintf(stderr,"Error opening file %s\n",filename);
		return -1;
	}

	pt3_calc_frames(&pt3, &total_frames, &loop);

	/* Open output file */
	out_fd=open(out_filename,O_WRONLY|O_CREAT,0666);
	if (out_fd<0) {
		fprintf(stderr,"Error opening %s\n",out_filename);
		return -1;
	}

	/* Start filling in the ym file */

	/* Skip header, we'll fill in later */
	ym5_header_length=sizeof(struct ym_header)+
		strlen(pt3.name)+1+
		strlen(pt3.author)+1+
		strlen(ym5_comment)+1;
	lseek(out_fd, ym5_header_length, SEEK_SET);




	for(i=0;i < pt3.music_len;i++) {

		pt3_set_pattern(i,&pt3);

		printf("Chunk %d/%d, 00:00/00:00, Pattern #%d\n",
			i,pt3.music_len-1,pt3.current_pattern);
		printf("a_addr: %04x, b_addr: %04x, c_addr: %04x\n",
				pt3.a_addr,pt3.b_addr,pt3.c_addr);

		printf("Cdata: ");
		for(j=0;j<32;j++) printf("%02x ",pt3.data[pt3.c_addr+j]);
		printf("\n");

		for(j=0;j<64;j++) {

			printf("VMW frame: %d\n",frames);

			/* decode line. 1 if done early */
			if (pt3_decode_line(&pt3)) break;

			/* Print line of tracker */
			pt3_print_tracker_line(&pt3,j);

//			memcpy(&pt3.a_old,&pt3.a,sizeof(struct note_type));
//			memcpy(&pt3.b_old,&pt3.b,sizeof(struct note_type));
//			memcpy(&pt3.c_old,&pt3.c,sizeof(struct note_type));
//			pt3.envelope_period_old=(pt3.envelope_period);

			/* Dump out subframes of line */
			for(f=0;f<pt3.speed;f++) {
				pt3_make_frame(&pt3,frame);

				write(out_fd,frame,16);
				frames++;
			}

			printf("\n");
		}
	}


	/* Finish out ym5 file */

	/* Print End! marker */
	write(out_fd,"End!",4);

	/* Go back and print header */

	lseek(out_fd,0,SEEK_SET);

	strncpy(our_header.id,"YM5!",5);
	strncpy(our_header.check,"LeOnArD!",9);
	our_header.vbl=htonl(frames);
	our_header.song_attr=htonl(ym5_attributes);
	our_header.digidrum=htonl(ym5_digidrums);
	our_header.external_frequency=htonl(external_frequency);
	our_header.player_frequency=htons(ym5_irq);
	our_header.loop=htonl(loop);
	our_header.additional_data=htons(0);

        write(out_fd,&our_header,sizeof(struct ym_header));

        write(out_fd,pt3.name,strlen(pt3.name));
	write(out_fd,"\0",1);
        write(out_fd,pt3.author,strlen(pt3.author));
	write(out_fd,"\0",1);
        write(out_fd,ym5_comment,strlen(ym5_comment));
	write(out_fd,"\0",1);


	close(out_fd);

	return 0;

}
