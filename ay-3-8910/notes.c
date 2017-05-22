#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define TWELTH_TWO 1.059463094359

// http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html
double note_to_freq(char note, int flat, int sharp, int octave) {

	double freq=0.0;
	int step=0;

	switch(note) {
		case 'B': step=2; break;
		case 'A': step=0; break;
		case 'G': step=-2; break;
		case 'F': step=-4; break;
		case 'E': step=-5; break;
		case 'D': step=-7; break;
		case 'C': step=-9; break;
		default:
			fprintf(stderr,"Unknown note %c\n",note);
	}
	if (flat) step+=flat;
	if (sharp) step-=sharp;

	step-=(4-octave)*12;

	freq=440.0*pow(TWELTH_TWO,step);

	return freq;
}

/* https://sourceforge.net/p/ed2midi/code/ */
/* From a spreadsheet from ed2midi */
/*      Octave  1       2       3       4       5
        A       255     128     64      32      16
        A#/B-   240     120     60      30      15
        B       228     114     57      28      14
        C       216     108     54      27      13
        C#/D-   204     102     51      25      12
        D       192      96     48      24      12
        D#/E-   180      90     45      22      11
        E       172      86     43      21      10
        F       160      80     40      20      10
        F#/G-   152      76     38      19      9
        G       144      72     36      18      9
        G#/A-   136      68     34      17      8
*/

int note_to_ed(char note, int flat, int sharp, int octave) {

	int value=0;

	switch(note) {
		case 'A':
			if (flat==1) value=272;
			else if (sharp==1) value=240;
			else value=255;
			break;
		case 'B':
			if (flat==1) value=240;
			else if (sharp==1) value=216;
			else value=228;
			break;
		case 'C':
			if (flat==1) value=228;
			else if (sharp==1) value=204;
			else value=216;
			break;
		case 'D':
			if (flat==1) value=204;
			else if (sharp==1) value=180;
			else value=192;
			break;
		case 'E':
			if (flat==1) value=180;
			else if (sharp==1) value=160;
			else value=172;
			break;
		case 'F':
			if (flat==1) value=172;
			else if (sharp==1) value=152;
			else value=160;
			break;
		case 'G':
			if (flat==1) value=152;
			else if (sharp==1) value=136;
			else value=144;
			break;
		default:
			fprintf(stderr,"Unknown note %c\n",note);
	}

	int octave_divider[5]={1,2,4,8,16};

	if ((octave<1) || (octave>5)) {
		fprintf(stderr,"Invalid octave %d\n",octave);
		return -1;
	}

	return value/octave_divider[octave-1];
}
