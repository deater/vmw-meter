#include <stdint.h>

#include "num_to_string.h"

/* FIXME: really inefficien code */

int num_to_string(char *string,unsigned int value) {

	int len=0,q,r,nonzero=0;

	if (value==0) {
		string[len]='0';
		len++;
		goto done_num_to_string;
	}

	q=value/1000000;
	r=value%1000000;
	/* too big */
	if (q>10) return -1;

	if (q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}

	q=r;
	r=q%100000;
	q/=100000;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
	r=q%10000;
	q/=10000;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
	r=q%1000;
	q/=1000;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
	r=q%100;
	q/=100;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
	r=q%10;
	q/=10;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}


	q=r;
//	r=q%1;
	q/=1;
	if (nonzero || q!=0) {
		string[len]=q+'0';
		len++;
		nonzero=1;
	}




done_num_to_string:

	string[len+1]=0;

	return len;
}

