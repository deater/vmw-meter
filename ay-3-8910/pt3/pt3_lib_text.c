/* Routines used when dumping the files */
/* Not needed in a pure player */


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
#include "pt3_lib_text.h"

static char note_names[96][4]={
	"C-1","C#1","D-1","D#1","E-1","F-1","F#1","G-1", // 50
	"G#1","A-1","A#1","B-1","C-2","C#2","D-2","D#2", // 58
	"E-2","F-2","F#2","G-2","G#2","A-2","A#2","B-2", // 60
	"C-3","C#3","D-3","D#3","E-3","F-3","F#3","G-3", // 68
	"G#3","A-3","A#3","B-3","C-4","C#4","D-4","D#4", // 70
	"E-4","F-4","F#4","G-4","G#4","A-4","A#4","B-4", // 78
	"C-5","C#5","D-5","D#5","E-5","F-5","F#5","G-5", // 80
	"G#5","A-5","A#5","B-5","C-6","C#6","D-6","D#6", // 88
	"E-6","F-6","F#6","G-6","G#6","A-6","A#6","B-6", // 90
	"C-7","C#7","D-7","D#7","E-7","F-7","F#7","G-7", // 98
	"G#7","A-7","A#7","B-7","C-8","C#8","D-8","D#8", // a0
	"E-8","F-8","F#8","G-8","G#8","A-8","A#8","B-8", // a8
};



static void print_note(int which, struct pt3_song_t *pt3,int line) {

	struct pt3_note_type *a, *a_old;

	if (which=='A') {
		a=&(pt3->a);
		a_old=&(pt3->a_old);
	}
	else if (which=='B') {
		a=&(pt3->b);
		a_old=&(pt3->b_old);
	}
	else if (which=='C') {
		a=&(pt3->c);
		a_old=&(pt3->c_old);
	}
	else {
		fprintf(stderr,"ERROR unknown note %c\n",which);
		exit(1);
	}




	/* A note */
	if (!a->new_note) printf("---");
//        if (a->note==a_old->note)
	else if (a->note==0xff) printf("R--");
	else printf("%s",note_names[a->original_note]);
	printf(" ");

	/* A sample */
	if (a->sample==a_old->sample) printf(".");
	else if (a->sample<16) printf("%X",a->sample);
	else printf("%c",(a->sample-10)+'A');

	/* A envelope */
//	if ((envelope_period_l==0) || (a->envelope==0)) printf(".");
//	else printf("%X",a->envelope);

//	if ((envelope_period_l==0) || (a->envelope==0)) printf(".");
	if ((a->new_note) && (a->envelope_enabled)) printf("%X",pt3->envelope_type);
	else printf(".");

	/* A ornament */
	if (a->ornament==0) printf(".");
	else if (a->ornament==a_old->ornament) printf(".");
	else printf("%X",a->ornament);

	/* A volume */
	if (a->volume==0) printf(".");
	else if (a->volume==a_old->volume) printf(".");
	else printf("%X",a->volume);

	/* Special */
	printf(" ");
	if (a->spec_command==0) printf(".");
	else printf("%X",a->spec_command);

	if (a->spec_delay==0) printf(".");
	else printf("%X",a->spec_delay);

	if ((a->spec_lo&0xf0)==0) printf(".");
	else printf("%X",(a->spec_lo>>4)&0xf);

	if (a->spec_lo==0) printf(".");
	else printf("%X",(a->spec_lo&0xf));

	printf("|");
}

static char *decode_freqtable_name(int freq, int version) {

	if (freq==0) {
		if (version <= 3) {
			return "PT_33_34r";
		}
                else {
			return "PT_34_35";
		}
	}
	else if (freq==1) {
			return "ST";
	}
	else if (freq==2) {
		if (version <= 3) {
			return "ASM_34r";
		}
		else {
			return "ASM_34_35";
		}
	}
	else {
		if (version <= 3) {
			return "REAL_34r";
		}
		else {
			return "REAL_34_35";
		}
	}

}

void dump_header(struct pt3_song_t *pt3) {

	int i,j,addr,loop,len;

	printf("\tPT Version: %d\n",pt3->version);
	printf("\tNAME: %s\n",pt3->name);
	printf("\tBY  : %s\n",pt3->author);
	printf("\tFreqTable: %d (%s) Speed: %d  Patterns: %d Loop: %d\n",
			pt3->which_frequency_table,
			decode_freqtable_name(pt3->which_frequency_table,
						pt3->version),
			pt3->speed,
			pt3->num_patterns,
			pt3->loop);

	/**************************/
	/* Print pattern location */
	/**************************/
	printf("\tPattern Location Offset: %04x\n",pt3->pattern_loc);


	/**************************/
	/* Print Sample addresses */
	/**************************/
	printf("\tSample pattern addresses:");
	for(i=0;i<32;i++) {
		if (i%8==0) printf("\n\t\t");
		printf("%04x ",pt3->sample_patterns[i]);
	}
	printf("\n");

	/****************************/
	/* Print Ornamemt addresses */
	/****************************/
	printf("\tOrnament addresses:");
	for(i=0;i<16;i++) {
		if (i%8==0) printf("\n\t\t");
		printf("%04x ",pt3->ornament_patterns[i]);
	}
	printf("\n");
//	printf("\tPattern order @%04x\n",pt3->pattern_order);

	/**************************/
	/* Print Pattern Order    */
	/**************************/
	i=0;
	printf("\tPattern order:");
	while(1) {
		if (i%16==0) printf("\n\t\t");
		if (pt3->data[0xc9+i]==0xff) break;
		printf("%02d ",pt3->data[0xc9+i]/3);
		i++;
	}
	printf("\n");

	/***************************/
	/* Print Pattern addresses */
	/***************************/
	printf("\tPattern Locations:\n");
	for(i=0;i<pt3->num_patterns;i++) {
		printf("\t\t%d (%4x):\t",i,(i*6)+pt3->pattern_loc);

		addr=pt3->data[(i*6)+0+pt3->pattern_loc] |
			(pt3->data[(i*6)+1+pt3->pattern_loc]<<8);
		printf("A: %04x ",addr);

		addr=pt3->data[(i*6)+2+pt3->pattern_loc] |
			(pt3->data[(i*6)+3+pt3->pattern_loc]<<8);
		printf("B: %04x ",addr);

		addr=pt3->data[(i*6)+4+pt3->pattern_loc] |
			(pt3->data[(i*6)+5+pt3->pattern_loc]<<8);
		printf("C: %04x ",addr);

		printf("\n");
	}

	/**************************/
	/* Print Sample Data      */
	/**************************/
	printf("\tSample Dump:\n");
	for(i=0;i<32;i++) {
		printf("\t\t%i: ",i);
		if (pt3->sample_patterns[i]==0) printf("N/A\n");
		else {
			loop=pt3->data[0+pt3->sample_patterns[i]];
			len=pt3->data[1+pt3->sample_patterns[i]];
			printf("Loop: %d Length: %d\n",loop,len);
			for(j=0;j<len;j++) {
				printf("\t\t\t%02x %02x %02x %02x\n",
					pt3->data[2+(j*4)+
						pt3->sample_patterns[i]],
					pt3->data[3+(j*4)+
						pt3->sample_patterns[i]],
					pt3->data[4+(j*4)+
						pt3->sample_patterns[i]],
					pt3->data[5+(j*4)+
						pt3->sample_patterns[i]]);
			}
		}
	}


	/**************************/
	/* Print Ornament Data    */
	/**************************/
	printf("\tOrnament Dump:\n");
	for(i=0;i<16;i++) {
		printf("\t\t%i: ",i);
		if (pt3->ornament_patterns[i]==0) printf("N/A\n");
		else {
			loop=pt3->data[0+	pt3->ornament_patterns[i]];
			len=pt3->data[1+pt3->ornament_patterns[i]];
			printf("Loop: %d Length: %d\n\t\t\t",loop,len);
			for(j=0;j<len;j++) {
				printf("%02x ",
					pt3->data[2+j+
					pt3->ornament_patterns[i]]);
			}
			printf("\n");
		}
	}

}

void pt3_print_tracker_line(struct pt3_song_t *pt3, int line) {

	/* Print line of tracker */

	/* line */
	printf("%02x|",line);

	/* envelope */
	if ((pt3->envelope_period>>8)==0) printf("..");
	else printf("%02X",pt3->envelope_period>>8);
	if ((pt3->envelope_period&0xff)==0) printf("..");
	else printf("%02X",pt3->envelope_period&0xff);

	/* noise */
	printf("|");
	if (pt3->noise_period==0) printf("..");
	else printf("%02X",pt3->noise_period);
	printf("|");

	print_note('A',pt3,line);
	print_note('B',pt3,line);
	print_note('C',pt3,line);

	memcpy(&pt3->a_old,&pt3->a,sizeof(struct pt3_note_type));
	memcpy(&pt3->b_old,&pt3->b,sizeof(struct pt3_note_type));
	memcpy(&pt3->c_old,&pt3->c,sizeof(struct pt3_note_type));
	pt3->envelope_period_old=(pt3->envelope_period);
}

static char empty_note[]="---";

char *pt3_current_note(int which, struct pt3_song_t *pt3) {

	if (which=='A') {
		if (pt3->a.volume) return note_names[pt3->a.note];
		else return empty_note;
	}

	if (which=='B') {
		if (pt3->b.volume) return note_names[pt3->b.note];
		else return empty_note;
	}

	if (which=='C') {
		if (pt3->c.volume) return note_names[pt3->c.note];
		else return empty_note;
	}

	return empty_note;
}


