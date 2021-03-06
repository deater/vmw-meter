/* Convert ym5 file to a file that can be played */
/* On an Apple II Mockingboard card		*/
/* The Apple II code can be found in my dos33fsutils git tree */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <math.h>
#include <sys/resource.h>

#include "stats.h"
#include "ym_lib.h"

#define VERSION "0.7"

static void print_help(int just_version, char *exec_name) {

	printf("\nym5_to_mockingboard version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This converts ym5 files to a form playable on Apple II Mockkingboard\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-b] [-d] [-r] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-b: write out binary (not assembly) results\n");
	printf("\t-d: print debug messages\n");
	printf("\t-r: raw uncompressed data\n");

	exit(0);
}

static int dump_song(char *filename, int debug, int binary) {

	int result;
	int length_seconds,total;

	int frame_num=0;
	int i;
	int byte_count=0;
	int frame_different;
	int which1,which2;

	struct ym_song_t ym_song;

	static unsigned char oldframe[YM5_FRAME_SIZE];
	static int lastframe=-1;

	unsigned char frame[YM5_FRAME_SIZE];

	if (!binary) {
		printf("\n; Using ym5_to_mockingboard to convert song %s\n",
			filename);
	}

	result=load_ym_song(filename,&ym_song);
	if (result<0) {
		return -1;
	}

	/**********************/
	/* Print song summary */
	/**********************/

	if (!binary) {
		printf(";\tYM%d",ym_song.type);
		printf("\tSong attributes (%d) : ",ym_song.attributes);
		printf("Interleaved=%s\n",ym_song.interleaved?"yes":"no");
		if (ym_song.num_digidrum>0) {
			printf("; Num digidrum samples: %d\n",
				ym_song.num_digidrum);
		}
		printf(";\tFrames: %d, ",ym_song.num_frames);
		printf("Chip clock: %d Hz, ",ym_song.master_clock);
		printf("Frame rate: %d Hz, ",ym_song.frame_rate);
		if (ym_song.frame_rate!=50) {
			fprintf(stderr,"FIX ME framerate %d\n",
				ym_song.frame_rate);
			exit(1);
		}
		length_seconds=ym_song.num_frames/ym_song.frame_rate;
		printf("Length=%d:%02d\n",length_seconds/60,length_seconds%60);
		printf(";\tLoop frame: %d, ",ym_song.loop_frame);
		printf("Extra data size: %d\n",ym_song.extra_data);
		printf(";\tSong name: %s\n",ym_song.song_name);
		printf(";\tAuthor name: %s\n",ym_song.author);
		printf(";\tComment: %s\n",ym_song.comment);
	}

	frame_num=0;
	while(1) {

		ym_return_frame(&ym_song,frame_num,frame,NULL);

		frame_different=0;
		which1=0; which2=0;
		total=0;

		for(i=0;i<14;i++) {
			if ((i==13) && (frame[i]==0xff)) {
			}
			else {
				if (frame[i]!=oldframe[i]) {
					if (i<8) {
						which1|=1<<i;
					}
					else {
						which2|=1<<(i-8);
					}
					frame_different++;
				}
			}
		}

		if (frame_different) {
			if (frame_num-lastframe>254) {
				fprintf(stderr,"ERROR frame diff too big %d!\n",
					frame_num-lastframe);
				exit(1);
			}

			if (!binary) {
				printf(".byte\t$%02X,$%02X,",
					frame_num-lastframe,which1);
			}
			else {
				printf("%c%c",frame_num-lastframe,which1);
			}

			byte_count+=3;
			for(i=0;i<14;i++) {
				if (i==8) {
					if (!binary) {
						if (which2) printf("$%02X,",which2);
						else printf(",$%02X",which2);
					}
					else {
						printf("%c",which2);
					}
				}
				if ((i==13)&&(frame[i]==0xff)) {

				}
				else {
					if (frame[i]!=oldframe[i]) {
						if (!binary) {
							printf("$%02X",frame[i]);
						}
						else {
							printf("%c",frame[i]);
						}
						byte_count+=1;
						total++;
						if (!binary) {
							if (total!=frame_different) {
								printf(",");
							}
						}
					}
				}
			}
			if (!binary) {
				for(i=0;i<5-(frame_different*4)/8;i++) {
					printf("\t");
				}
				printf("; %05d\n",frame_num);
			}
			lastframe=frame_num;
		}

		memcpy(oldframe,frame,YM5_FRAME_SIZE);

		frame_num++;

		/* Check to see if done with file */
		if (frame_num>=ym_song.num_frames) {
			if (!binary) {
				printf(".byte\t$%02X,$00,$00,$ff"
					"\t\t\t\t\t; %05d\n",
					frame_num-lastframe,frame_num);
			}
			else {
				printf("%c%c%c%c",
					frame_num-lastframe,0x0,0x0,0xff);
			}
			byte_count+=4;
			break;
		}
	}

	if (!binary) {
		printf("; Raw size = %d bytes\n",frame_num*14);
		printf("; Updated size = %d bytes\n",byte_count);
	}
	else {
		fprintf(stderr,"; Raw size = %d bytes\n",frame_num*14);
		fprintf(stderr,"; Updated size = %d bytes\n",byte_count);
	}

	/* Free the ym file */
	free(ym_song.file_data);

	return 0;
}

static int dump_song_raw(char *filename, int debug, int binary) {

	int result;
	int length_seconds;

	int frame_num=0;
	int m,s;

	struct ym_song_t ym_song;

	printf("\nDumping song %s\n",filename);

	result=load_ym_song(filename,&ym_song);
	if (result<0) {
		return -1;
	}

	/**********************/
	/* Print song summary */
	/**********************/

	printf("\tYM%d",ym_song.type);
	printf("\tSong attributes (%d) : ",ym_song.attributes);
	printf("Interleaved=%s\n",ym_song.interleaved?"yes":"no");
	if (ym_song.num_digidrum>0) {
		printf("Num digidrum samples: %d\n",ym_song.num_digidrum);
	}
	printf("\tFrames: %d, ",ym_song.num_frames);
	printf("Chip clock: %d Hz, ",ym_song.master_clock);
	printf("Frame rate: %d Hz, ",ym_song.frame_rate);
	if (ym_song.frame_rate!=50) {
		fprintf(stderr,"FIX ME framerate %d\n",ym_song.frame_rate);
		exit(1);
	}
	length_seconds=ym_song.num_frames/ym_song.frame_rate;
	printf("Length=%d:%02d\n",length_seconds/60,length_seconds%60);
	printf("\tLoop frame: %d, ",ym_song.loop_frame);
	printf("Extra data size: %d\n",ym_song.extra_data);
	printf("\tSong name: %s\n",ym_song.song_name);
	printf("\tAuthor name: %s\n",ym_song.author);
	printf("\tComment: %s\n",ym_song.comment);

	/******************/
	/* Play the song! */
	/******************/

	frame_num=0;
	while(1) {
		s=frame_num/50;
		m=s/60;
		s=s%60;

		if (frame_num%96==0) {
			//        0   1   2   3   4   5   6   7
			//        8   9  10  11  12  13
			printf("; %02d:%02d "
				"AL  AH  BL  BH  CL  CH   N  NT  "
				"VA  VB  VC  EL  EH  ET\n",m,s);
		}

		ym_dump_frame_raw(&ym_song,frame_num);

		frame_num++;

		/* Check to see if done with file */
		if (frame_num>=ym_song.num_frames) {
			break;
		}
	}

	printf("; Total size = %d bytes\n",frame_num*14);

	/* Free the ym file */
	free(ym_song.file_data);

	return 0;
}


int main(int argc, char **argv) {

	char filename[BUFSIZ]="intro2.ym";

	int c,debug=0;
	int first_song;
	int raw=0,binary=0;

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dhbrv"))!=-1) {
		switch (c) {
			case 'd':
				/* Debug messages */
				printf("Debug enabled\n");
				debug=1;
				break;
			case 'h':
				/* help */
				print_help(0,argv[0]);
				break;
			case 'b':
				/* binary */
				binary=1;
				break;
			case 'r':
				/* raw */
				raw=1;
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	first_song=optind;

	if (argv[first_song]!=NULL) {
		strcpy(filename,argv[first_song]);
	}

	/* Dump the song */
	if (raw) {
		dump_song_raw(filename,debug,binary);
	}
	else {
		dump_song(filename,debug,binary);
	}

	return 0;
}
