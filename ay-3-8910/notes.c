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

