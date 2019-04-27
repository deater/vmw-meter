#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <endian.h>

#include "sine_table.h"

#include "freq_lookup.h"
#include "fighting.h"

static int calculate_sine(int degrees) {
        int x;
        x=degrees%360;

        if (x<90) return sine_lookup[x];
        if (x<180) return sine_lookup[180-x];
        if (x<270) return 4096-sine_lookup[x-180];
        return 4096-sine_lookup[360-x];
}


struct asdr_type {
	int a;
	int d;
	int s;
	int r;
};

int calc_asdr(int len, struct asdr_type *asdr) {

	asdr->a=(len*9)/10;
	asdr->d=(len*8)/10;
	asdr->s=len/10;

//	printf("%d, %d %d %d\n",len,asdr->a,asdr->d,asdr->s);

	return 0;
}

int scale_output(int value,int scale) {

	int temp;

	temp=value-0x800;

	temp*=scale;
	temp/=16;

	temp+=0x800;

//	printf("Orig: %x Scale: %x Result: %x\n",value,scale,temp);

	return temp;
}

#define FIXED_CONSTANT 1000

/* http://soundfile.sapp.org/doc/WaveFormat/ */

int main(int argc, char **argv) {

	int fd;
	char filename[]="out.wav";
	int temp_int;
	short temp_short;
	int datasize;
	int i,samples;
	int f0,newstep0=0,stepsize0=0,angle0=0,l0=0,scale0=16;
	int f1,newstep1=0,stepsize1=0,angle1=0,l1=0,scale1=16;
	int f2,newstep2=0,stepsize2=0,angle2=0,l2=0,scale2=16;
	int c0_done=1,c1_done=1,c2_done=1;
	int which;
	struct asdr_type asdr0={0,0,0,0},asdr1={0,0,0,0},asdr2={0,0,0,0};

	fd=open(filename,O_WRONLY|O_CREAT,0666);
	if (fd<0) {
		printf("Error opening %s: %s\n",filename,strerror(errno));
		exit(1);
	}

	/* Assuming 120BPM */
	/* record SONG_LENGTH*4s of audio */
	/* mono, 44.1kHz, so 441,000 samples of 16-bits */

//	samples=SONG_LENGTH*4*44100;

	// force only 10s of sound

	samples=4*44100*20;

	printf("Creating %dMB of data\n",2*(samples/1024/1024));


	datasize=samples*2;

	/* RIFF header */
	write(fd,"RIFF",4);
	temp_int=htole32(36+datasize);
	write(fd,&temp_int,4);
	write(fd,"WAVE",4);

	/* subchunk1 */
	write(fd,"fmt ",4);
	temp_int=htole32(16);	// 16 = size of rest of subchunk
	write(fd,&temp_int,4);
	temp_short=htole16(1);	// PCM = 1
	write(fd,&temp_short,2);
	temp_short=htole16(1);	// Channels = 1
	write(fd,&temp_short,2);
	temp_int=htole32(44100);// 44100 = sample size
	write(fd,&temp_int,4);
	temp_int=htole32(88200);// 88200 = byte rate
	write(fd,&temp_int,4);
	temp_short=htole16(2);	// Block Align = 2
	write(fd,&temp_short,2);
	temp_short=htole16(16);	// Bits per sample = 2
	write(fd,&temp_short,2);

	/* subchunk2 */
	write(fd,"data",4);
	temp_int=htole32(datasize);	// subchunk2size
	write(fd,&temp_int,4);

#define TEMPO	3200

	for(i=0;i<samples;i++) {

		if (i%TEMPO==0) {
		which=i/TEMPO;

		/* Set next note if applicable */
		if (fighting_channel0[which]!=0) {
			f0=44300000/freq_lookup[fighting_channel0[which]];
			newstep0=360*FIXED_CONSTANT*FIXED_CONSTANT/f0;

			l0=fighting_len0[which]*(TEMPO/6);
			c0_done=1;
//			printf("New note: %d %d\n",newstep0,l0);
		}

		if (fighting_channel1[which]!=0) {
			f1=44300000/freq_lookup[fighting_channel1[which]];
			newstep1=360*FIXED_CONSTANT*FIXED_CONSTANT/f1;
			l1=fighting_len1[which]*(TEMPO/6);
			c1_done=1;
		}

		if (fighting_channel2[which]!=0) {
			f2=44300000/freq_lookup[fighting_channel2[which]];
			newstep2=360*FIXED_CONSTANT*FIXED_CONSTANT/f2;
			l2=fighting_len2[which]*(TEMPO/6);
			c2_done=1;
		}

		}

		/* count down length */
		if (l0>0) {
			l0--;
			if (l0==0) {
				newstep0=0;
				c0_done=1;
			}
		}

		if (l1>0) {
			l1--;
			if (l1==0) {
				newstep1=0;
				c1_done=1;
			}
		}

		if (l2>0) {
			l2--;
			if (l2==0) {
				newstep2=0;
				c2_done=1;
			}
		}


//		printf("%d %d %d\n",angle0,stepsize0,l0);

		/* If zero crossing, and a new note, start new note */

		if ((angle0+stepsize0>=360*FIXED_CONSTANT) || (stepsize0==0)) {
			if (c0_done) {
				stepsize0=newstep0;
				if (stepsize0==0) angle0=0;
//				printf("Angle0 is %d/%d Changing stepsize to %d\n",
//					angle0,stepsize0,newstep0);
				calc_asdr(l0,&asdr0);
				c0_done=0;
			}

			if (l0>asdr0.a) {
				scale0=15;
			}
			else if (l0>asdr0.d) {
				scale0=16;
			}
			else if (l0>asdr0.s) {
				scale0=15;
			}
			else {
				scale0=10;
			}

		}

                if (angle0+stepsize0>=360*FIXED_CONSTANT) {
                        angle0-=360*FIXED_CONSTANT;
                }
		angle0+=stepsize0;


		if ((angle1+stepsize1>=360*FIXED_CONSTANT) || (stepsize1==0)) {
			if (c1_done) {
				stepsize1=newstep1;
				if (stepsize1==0) angle1=0;
//				printf("Angle0 is %d/%d Changing stepsize to %d\n",
//					angle0,stepsize0,newstep0);
				calc_asdr(l1,&asdr1);
				c1_done=0;
			}
			if (l1>asdr1.a) {
				scale1=15;
			}
			else if (l1>asdr1.d) {
				scale1=16;
			}
			else if (l1>asdr1.s) {
				scale1=15;
			}
			else {
				scale1=10;
			}
		}

                if (angle1+stepsize1>=360*FIXED_CONSTANT) {
                        angle1-=360*FIXED_CONSTANT;
                }
		angle1+=stepsize1;


		if ((angle2+stepsize2>=360*FIXED_CONSTANT) || (stepsize2==0)) {
			if (c2_done) {
				stepsize2=newstep2;
				if (stepsize2==0) angle2=0;
//				printf("Angle0 is %d/%d Changing stepsize to %d\n",
//					angle0,stepsize0,newstep0);
				calc_asdr(l2,&asdr2);
				c2_done=0;
			}
			if (l2>asdr2.a) {
				scale2=15;
			}
			else if (l2>asdr2.d) {
				scale2=16;
			}
			else if (l2>asdr2.s) {
				scale2=15;
			}
			else {
				scale2=10;
			}

		}

                if (angle2+stepsize2>=360*FIXED_CONSTANT) {
                        angle2-=360*FIXED_CONSTANT;
                }
		angle2+=stepsize2;







		int output1,output2,output3;

//		printf("ANGLE0=%d\n",angle0);
//		printf("ANGLE1=%d (%x)\n",angle1,calculate_sine(angle1/1000));

		output1=calculate_sine(angle0/FIXED_CONSTANT);
		output1=scale_output(output1,scale0);

		output2=calculate_sine(angle1/FIXED_CONSTANT);
		output2=scale_output(output2,scale1);

		output3=calculate_sine(angle2/FIXED_CONSTANT);
		output3=scale_output(output3,scale2);

		temp_short=htole16(((output1+
			output2+
			output3)*2)-(12*1024));
//		printf("%d\n",temp_short);

		write(fd,&temp_short,2);
	}


	close(fd);

	return 0;
}
