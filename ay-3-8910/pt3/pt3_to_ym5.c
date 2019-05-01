/* Convert pt3 file to ym file */

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


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

/* Is this standard for pt3?  I guess on spectrum files? */
static int external_frequency=1773400;
//static int bpm=120;
//static int tempo=6;
//static int frames_per_line=6;   /* frames per tracker line */
//static int frames_per_whole=96; /* numer of frames per whole note */

static int frames=0;

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



struct pt3_header {
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

unsigned char *aptr,*bptr,*cptr;
unsigned short a_addr,b_addr,c_addr;

static int music_len=0,current_pattern=0;

static int load_header(void) {

	int i;

	/* Magic */
	memcpy(&header.magic,&raw_header[0],13);
	if (!memcmp(header.magic,"ProTracker 3.",13)) {
		printf("Found ProTracker 3.");
	}
	else if (!memcmp(header.magic,"Vortex Tracke",13)) {
		printf("Found Vortex Tracke");
	} else {
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
	header.num_patterns=raw_header[0x65]+1;

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

struct note_type {
	char which;
	int note;
	int sample;
	int envelope;
	int ornament;
	int prev_ornament;
	int volume;
	int spec_command;
	int spec_delay;
	int spec_hi;
	int spec_lo;

	int len;
	int len_count;

	int all_done;
};

static int envelope_period_h=0;
static int envelope_period_l=0;
static int envelope_period_h_old=0;
static int envelope_period_l_old=0;
static int noise_period=0;

//static int delay=0;

static void decode_note(struct note_type *a,
			unsigned short *addr) {

	int a_done=0;
	int current_val;

	a->spec_command=0;
	a->spec_delay=0;
	a->spec_lo=0;
	a->note=0;
	a->envelope=0;

	/* Skip decode if note still running */
	if (a->len_count>1) {
		a->len_count--;
		return;
	}

	while(1) {
		a->len_count=a->len;

		current_val=pt3_data[*addr];
		//printf("%02X\n",current_val);

		switch((current_val>>4)&0xf) {
			case 0:
				if (current_val==0x0) {
					printf("ALL DONE %c\n",a->which);
					a->all_done=1;
					a_done=1;
				}
				else if (current_val==0x1) {
					/* tone down */
					a->spec_command=0x1;
				}
				else if (current_val==0x2) {
					/* port */
					a->spec_command=0x3;
				}
				else if (current_val==0x8) {
					a->spec_command=0x9;
				}
				else if (current_val==0x9) {
					a->spec_command=0xb;
				}
				else printf("%c UNKNOWN %02X\n",a->which,current_val);
				break;
			case 1:
				if ((current_val&0xf)==0x0) {
					//printf("UNKNOWN %02X ",current_val);
					a->envelope=0xf; // (disable)
				}
				else {
					a->envelope=(current_val&0xf);

					current_val=pt3_data[*addr+1];
					envelope_period_h=current_val;
					//printf("%02X ",current_val);
					(*addr)++;

					current_val=pt3_data[(*addr)+1];
					envelope_period_l=current_val;
					//printf("%02X ",current_val);
					(*addr)++;
				}
				current_val=pt3_data[(*addr)+1];
				a->ornament=a->prev_ornament;
				a->sample=(current_val/2);
				//printf("%02X\n",current_val);
				(*addr)++;

				break;
			case 2:
				/* Reset noise? */
				noise_period=(current_val&0xf);
//				if (current_val==0x20) {
//					noise_period=0;
//				}
//				else {
//					printf("UNKNOWN %02X\n",current_val);
//				}
				break;
			case 3:
				noise_period=(current_val&0xf)+0x10;
				break;
			case 4:
				if (a->envelope==0) a->envelope=0xf;
				a->ornament=(current_val&0xf);
				a->prev_ornament=a->ornament;

				//printf("%x envelope=%x\n",current_val,
				//	a->envelope);
				break;
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 0xa:
				a->note=current_val;
				a_done=1;
				break;
			case 0xb:
				/* Set noise? */
				if (current_val==0xb0) {
					a->envelope=0xf;
					a->ornament=a->prev_ornament;
					//current_val=pt3_data[(*addr)+1];
					//noise_period=(current_val&0xf);
					(*addr)++;
				}
				/* set len */
				else if (current_val==0xb1) {
					current_val=pt3_data[(*addr)+1];
					a->len=current_val;
					a->len_count=a->len;
					(*addr)++;
				}
				else {
					a->envelope=(current_val&0xf)-1;
					a->ornament=a->prev_ornament;
					current_val=pt3_data[(*addr)+1];
					(*addr)++;
					envelope_period_h=current_val;
					current_val=pt3_data[(*addr)+1];
					(*addr)++;
					envelope_period_l=current_val;
				}
				break;
			case 0xc:	/* volume */
				a->volume=current_val&0xf;
				if (a->volume==0) {
					a->note=0xff;
					a_done=1;
				}
				break;
			case 0xd:
				if (current_val==0xd0) {
					a->note=0;
					a_done=1;
				}
				else {
					a->sample=(current_val&0xf);
				}
				break;
			case 0xe:
				a->sample=(current_val-0xd0);
				break;
			case 0xf:
//               Envelope=15, Ornament=low byte, Sample=arg1/2
				a->envelope=0xf;
				a->ornament=(current_val&0xf);
				a->prev_ornament=(current_val&0xf);
				current_val=pt3_data[(*addr)+1];
				a->sample=current_val/2;
				(*addr)++;
				break;
		}

		(*addr)++;
		if (a_done) {
			if (a->spec_command==0xb) {
				current_val=pt3_data[(*addr)];
				a->spec_lo=current_val;
				(*addr)++;
			}
			if (a->spec_command==0x1) {
				current_val=pt3_data[(*addr)];
				a->spec_delay=current_val;
				(*addr)++;

				current_val=pt3_data[(*addr)];
				a->spec_lo=(current_val);
				(*addr)++;

				current_val=pt3_data[(*addr)];
				a->spec_hi=(current_val);
				(*addr)++;
			}
			if (a->spec_command==0x3) {
				current_val=pt3_data[(*addr)];
				a->spec_delay=current_val;
				(*addr)++;

				current_val=pt3_data[(*addr)];
//				a->spec_lo=(current_val)&0xf;
				(*addr)++;

				current_val=pt3_data[(*addr)];
//				a->spec_lo=(current_val)&0xf;
				(*addr)++;
				current_val=pt3_data[(*addr)];
				a->spec_lo=(current_val)&0xf;
				(*addr)++;
				current_val=pt3_data[(*addr)];
//				a->spec_lo=(current_val)&0xf;
				(*addr)++;

			}
			if (a->spec_command==0x9) {

				/* delay? */
				current_val=pt3_data[(*addr)];
				a->spec_delay=current_val&0xf;
				(*addr)++;

				/* Low? */
				current_val=pt3_data[(*addr)];
				a->spec_lo=current_val&0xf;
				(*addr)++;

				/* High? */
				current_val=pt3_data[(*addr)];
				a->spec_hi=current_val&0xf;
				(*addr)++;
			}
			break;
		}
	}

}

static void print_note(struct note_type *a, struct note_type *a_old) {
	/* A note */
	if (a->note==0xff) printf("R--");
	else if (a->note==0) printf("---");
	else printf("%s",note_names[a->note-0x50]);
	printf(" ");

	/* A sample */
	if (a->sample==a_old->sample) printf(".");
	else if (a->sample<16) printf("%X",a->sample);
	else printf("%c",(a->sample-10)+'A');

	/* A envelope */
//	if ((envelope_period_l==0) || (a->envelope==0)) printf(".");
//	else printf("%X",a->envelope);

//	if ((envelope_period_l==0) || (a->envelope==0)) printf(".");
	if (a->envelope!=0) printf("%X",a->envelope);
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



int main(int argc, char **argv) {

	char filename[BUFSIZ];
	int fd,out,i,j,addr;
	int result,sample_loop,sample_len;
	int digidrums=0;
	int attributes=0;
	int irq=50,loop=0;
	char comments[]="VMW pt3_to_ym5";
	int header_length;

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

	out=open("out.ym",O_WRONLY|O_CREAT,0666);
	if (out<0) {
		fprintf(stderr,"Error opening \n");
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



	printf("\tNAME: %s\n",header.name);
	printf("\tBY  : %s\n",header.author);
	printf("\tFreqTable: %d Speed: %d  Patterns: %d Loop: %d\n",
			header.frequency_table,
			header.speed,
			header.num_patterns,
			header.loop);
	printf("\tPattern Location Offset: %04x\n",header.pattern_loc);

	/* Skip header, we'll fill in later */
	header_length=sizeof(struct ym_header)+
		strlen(header.name)+1+
		strlen(header.author)+1+
		strlen(comments)+1;

	lseek(fd, header_length, SEEK_SET);


#if 0
	printf("\tSample pattern addresses:");
		for(i=0;i<32;i++) {
			if (i%8==0) printf("\n\t\t");
			printf("%04x ",header.sample_patterns[i]);
		}
		printf("\n");
		printf("\tOrnament addresses:");
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
			music_len++;
		}
		printf("\n");

		printf("\tPattern Locations:\n");
		for(i=0;i<header.num_patterns;i++) {
			printf("\t\t%d (%4x):\t",i,(i*6)+header.pattern_loc);

			addr=pt3_data[(i*6)+0+header.pattern_loc] |
				(pt3_data[(i*6)+1+header.pattern_loc]<<8);
			printf("A: %04x ",addr);

			addr=pt3_data[(i*6)+2+header.pattern_loc] |
				(pt3_data[(i*6)+3+header.pattern_loc]<<8);
			printf("B: %04x ",addr);

			addr=pt3_data[(i*6)+4+header.pattern_loc] |
				(pt3_data[(i*6)+5+header.pattern_loc]<<8);
			printf("C: %04x ",addr);

			printf("\n");
		}

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


		printf("\tOrnament Dump:\n");
		for(i=0;i<16;i++) {
			printf("\t\t%i: ",i);
			if (header.ornament_patterns[i]==0) printf("N/A\n");
			else {
				sample_loop=pt3_data[0+
						header.ornament_patterns[i]];
				sample_len=pt3_data[1+
						header.ornament_patterns[i]];
				printf("Loop: %d Length: %d\n\t\t\t",
						sample_loop,
						sample_len);
				for(j=0;j<sample_len;j++) {
					printf("%02x ",
						pt3_data[2+j+
						header.sample_patterns[i]]);
				}
				printf("\n");
			}
		}
#endif


	for(i=0;i<music_len;i++) {
		current_pattern=pt3_data[0xc9+i]/3;
		printf("Chunk %d/%d, 00:00/00:00, Pattern #%d\n",
			i,music_len-1,current_pattern);

		a_addr=pt3_data[(current_pattern*6)+0+header.pattern_loc] |
			(pt3_data[(current_pattern*6)+1+header.pattern_loc]<<8);

		b_addr=pt3_data[(current_pattern*6)+2+header.pattern_loc] |
			(pt3_data[(current_pattern*6)+3+header.pattern_loc]<<8);

		c_addr=pt3_data[(current_pattern*6)+4+header.pattern_loc] |
			(pt3_data[(current_pattern*6)+5+header.pattern_loc]<<8);

		printf("a_addr: %04x, b_addr: %04x, c_addr: %04x\n",
				a_addr,b_addr,c_addr);

		aptr=&pt3_data[a_addr];
		bptr=&pt3_data[b_addr];
		cptr=&pt3_data[c_addr];

		struct note_type a,b,c;
		struct note_type a_old,b_old,c_old;

		memset(&a,0,sizeof(struct note_type));
		memset(&b,0,sizeof(struct note_type));
		memset(&c,0,sizeof(struct note_type));
		a.which='A';
		b.which='B';
		c.which='C';

		memset(&a_old,0,sizeof(struct note_type));
		memset(&b_old,0,sizeof(struct note_type));
		memset(&c_old,0,sizeof(struct note_type));

		noise_period=0;

		j=0;

		for(j=0;j<64;j++) {

			envelope_period_h=0;
			envelope_period_l=0;

			decode_note(&a,&a_addr);
			decode_note(&b,&b_addr);
			decode_note(&c,&c_addr);


			if (a.all_done && b.all_done && c.all_done) {
				break;
			}


			/* Print line of tracker */

			/* line */
			printf("%02x|",j);

			/* envelope */
				if (envelope_period_h==0) printf("..");
				else printf("%02X",envelope_period_h);
				if (envelope_period_l==0) printf("..");
				else printf("%02X",envelope_period_l);

				/* noise */
				printf("|");
				if (noise_period==0) printf("..");
				else printf("%02X",noise_period);
				printf("|");

				print_note(&a,&a_old);
				print_note(&b,&b_old);
				print_note(&c,&c_old);

				memcpy(&a_old,&a,sizeof(struct note_type));
				memcpy(&b_old,&b,sizeof(struct note_type));
				memcpy(&c_old,&c,sizeof(struct note_type));
				envelope_period_h_old=envelope_period_h;
				envelope_period_l_old=envelope_period_l;

				printf("\n");
			}
		}



	/* Print End! marker */
	write(fd,"End!",4);

	/* Go back and print header */

	lseek(fd,0,SEEK_SET);

	strncpy(our_header.id,"YM5!",5);
        strncpy(our_header.check,"LeOnArD!",9);
        our_header.vbl=htonl(frames);
        our_header.song_attr=htonl(attributes);
        our_header.digidrum=htonl(digidrums);
        our_header.external_frequency=htonl(external_frequency);
        our_header.player_frequency=htons(irq);
        our_header.loop=htonl(loop);
        our_header.additional_data=htons(0);

        write(out,&our_header,sizeof(struct ym_header));

        write(out,header.name,strlen(header.name));
	write(out,"\0",1);
        write(out,header.author,strlen(header.author));
	write(out,"\0",1);
        write(out,comments,strlen(comments));
	write(out,"\0",1);


	close(out);

	return 0;
}
