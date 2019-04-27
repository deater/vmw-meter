#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <endian.h>

#include "sine_table.h"


static int calculate_sine(int degrees) {
        int x;
        x=degrees%360;

        if (x<90) return sine_lookup[x];
        if (x<180) return sine_lookup[180-x];
        if (x<270) return 4096-sine_lookup[x-180];
        return 4096-sine_lookup[360-x];
}


/* http://soundfile.sapp.org/doc/WaveFormat/ */

int main(int argc, char **argv) {

	int fd;
	char filename[]="out.wav";
	int temp_int;
	short temp_short;
	int datasize;
	int i,samples;
	int f,stepsize,angle=0;

	fd=open(filename,O_WRONLY|O_CREAT,0666);
	if (fd<0) {
		printf("Error opening %s: %s\n",filename,strerror(errno));
		exit(1);
	}

	/* record 10s of audio */
	/* mono, 44.1kHz, so 441,000 samples of 16-bits */

	samples=441000;

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

	for(i=0;i<samples;i++) {

		f=44300000/440;
                stepsize=360*1000000/f;

                angle+=stepsize;
                if (angle>360000) {
                        angle-=360000;
                }

		temp_short=htole16(calculate_sine(angle/1000));

		write(fd,&temp_short,2);
	}


	close(fd);

	return 0;
}
