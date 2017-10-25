/* + An adafruit 8x16 led matrix backpack (0x72) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include "stats.h"
#include "display.h"
#include "visualizations.h"

#include "font.h"

#include "../noteslib/notes.h"

#define UPDATE_DIVIDER	5

static int divider=0;




static int intlog2(int value) {

	int logged;

	logged= (32-__builtin_clzl(value))-1;

//	printf("Log2 of %d is %d\n",value,logged);

	return logged;
}



int display_8x16_freq(int display_type, struct display_stats *ds) {

	int i;
	int x;

	static int initialized=0;
	static int freq_max[16];
	static unsigned char freq_matrix[16];

	if (!initialized) {
		for(x=0;x<16;x++) freq_max[x]=0x80;
		initialized=1;
	}

	/* the incoming values are log2 of the frequency */
	/* so likely in the range 4-14 or so */

	/* set the max values, also draw solid line if currently that freq */
	for(i=0;i<3;i++) {
		ds->right_freq[i]=intlog2(ds->right_freq[i]);
		if (ds->right_freq[i]>0) {
			if (ds->right_freq[i]>15) ds->right_freq[i]=15;
			freq_max[ds->right_freq[i]]=0x80;
			freq_matrix[ds->right_freq[i]]=0xff;
		}

		ds->left_freq[i]=intlog2(ds->left_freq[i]);
		if (ds->left_freq[i]>0) {
			if (ds->left_freq[i]>15) ds->left_freq[i]=15;
			freq_max[ds->left_freq[i]]=0x80;
			freq_matrix[ds->left_freq[i]]=0xff;
		}
	}

	/* Set the display to the max values */
	for(x=0;x<16;x++) {
		freq_matrix[x]|=freq_max[x];
		if ((divider==0) && (freq_max[x]>1)) freq_max[x]>>=1;
	}

//	if (divider==0) {
	//	freq_8x16display(display_type,1);
//	} else {
	//	freq_8x16display(display_type,0);
//	}

	if (divider==0) {
		display_8x16_vertical(display_type,freq_matrix);
	}

	divider++;
	if (divider>UPDATE_DIVIDER) {
		divider=0;
		/* Clear background */
		for(x=0;x<16;x++) {
			freq_matrix[x]=0;
		}
	}

	return 0;
}


int display_8x16_notematrix(int display_type, struct display_stats *ds) {

	int i;
	int x;
	int note;

	static unsigned char buffer[16];
	static int count=0;

	if (count==0) {
		display_8x16_vertical(display_type,buffer);
		for(x=0;x<16;x++) buffer[x]=0x0;
	}

	for(i=0;i<3;i++) {
		note=freq_to_note(ds->right_freq[i]);
		buffer[(note%12)+2]|=1<<(note/12);

		note=freq_to_note(ds->left_freq[i]);
		buffer[(note%12)+2]|=1<<(note/12);
	}

	count++;
	if (count>3) count=0;



	return 0;
}

static void piano_halfkey(int note, unsigned char *buffer) {

	int offset;

	offset=1+note*2;

	buffer[offset]|=0xff;
	buffer[offset+1]|=0x0f;

}

static void piano_sharpkey(int note, unsigned char *buffer) {

	int offset;

	offset=1+note*2;

	buffer[offset+1]|=0xf0;

}


static void piano_fullkey(int note, unsigned char *buffer) {

	int offset;

	offset=1+note*2;

	buffer[offset]|=0xff;
	buffer[offset+1]|=0xff;

}


int display_8x16_piano(int display_type, struct display_stats *ds) {

	int i;
	int x;
	int note;

	static unsigned char buffer[16];
	static int count=0;

	if (count==0) {
		display_8x16_vertical(display_type,buffer);
		for(x=0;x<16;x++) buffer[x]=0x0;
	}

	for(i=0;i<3;i++) {
		note=freq_to_note(ds->right_freq[i]);
		note=note%12;

		switch(note) {
			case 0:	// C
				piano_halfkey(0,buffer);	break;
			case 1:	// C#
				piano_sharpkey(0,buffer);	break;
			case 2:	// D
				piano_halfkey(1,buffer);	break;
			case 3:	// D#
				piano_sharpkey(1,buffer);	break;
			case 4:	// E
				piano_fullkey(2,buffer);	break;
			case 5:	// F
				piano_halfkey(3,buffer);	break;
			case 6:	// F#
				piano_sharpkey(3,buffer);	break;
			case 7:	// G
				piano_halfkey(4,buffer);	break;
			case 8:	// G#
				piano_sharpkey(4,buffer);	break;
			case 9:	// A
				piano_halfkey(5,buffer);	break;
			case 10:// A#
				piano_sharpkey(5,buffer);	break;
			case 11:// B#
				piano_fullkey(6,buffer);	break;
			default:
				printf("Unknown note %d\n",note);
		}


	}

	count++;
	if (count>3) count=0;



	return 0;
}



static int put_number(unsigned char *buffer,int which, int x, int y) {

	int a,b;

	for(a=0;a<3;a++) {
		buffer[a+x]&=~0x1f;
		for(b=0;b<5;b++) {
			buffer[a+x]|=number_font[which][b][a]<<(4-b);
		}
	}

	return 0;
}

int display_8x16_time(int display_type, int current_frame, int total_frames) {

	int x,y;
	int total_s,current_s,display_m,display_s,bar_length;

	unsigned char freq_matrix[16];

	/* Only update a few times a second? */
	/* Should do more for responsiveness? */
	if (current_frame%16!=0) {
//		display_8x16_vertical(display_type,freq_matrix);
		//freq_8x16display(display_type,0);
		return 0;
	}

	/* clear display */
	for(x=0;x<16;x++) {
		freq_matrix[x]=0;
	}

	total_s=total_frames/50;
	current_s=current_frame/50;

	if (total_s==0) bar_length=0;
	else bar_length=(current_s*14)/total_s;
//	printf("Bar length=%d/14\n",bar_length);

	for(x=0;x<bar_length;x++) {
		freq_matrix[x+1]|=1<<6;
	}

	display_m=current_s/60;
	display_s=current_s%60;
//	printf("%d:%d\n",display_m,display_s);

	/* Print tens char */
	/* Don't handle files >= 20 minutes well */
	if (display_m>9) {
		for(y=0;y<5;y++) freq_matrix[1]|=1<<y;
	}

	put_number(freq_matrix,display_m,3,0);

	/* Draw Colon */
	freq_matrix[7]|=1<<1;
	freq_matrix[7]|=1<<3;

	put_number(freq_matrix,display_s/10,9,0);
	put_number(freq_matrix,display_s%10,13,0);

//	freq_8x16display(display_type,1);
	display_8x16_vertical(display_type,freq_matrix);

	return 0;
}

int display_8x16_title(int display_type) {

	int x,y;
	static int count=0,offset=0,direction=+1;
	static int toggles=0;

	unsigned char freq_matrix[16];

	/* clear display */
	for(x=0;x<16;x++) {
		freq_matrix[x]=0;
	}

	for(y=0;y<3;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x]|=(!!(vmw[y]&(1<<(31-x))))<<(7-y);
		}
	}

	for(y=0;y<3;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x]|=(!!(chiptune[y]&(1<<(31-x-offset))))<<(3-y);
		}
	}

	display_8x16_vertical(display_type,freq_matrix);

	/* Only scroll at 1/6 of update time */
	count++;
	if (count>6) {
		count=0;
		offset+=direction;

		/* If scroll off end, change direction */
		if (offset>15) {
			offset=15;
			direction=-direction;
		}

		/* if scoll back to beginning, change direction */
		if (offset<0) {
			offset=0;
			direction=-direction;
			toggles++;
//			printf("Toggle %d\n",toggles);
		}
	}

	/* If not in kiosk mode only display title briefly */
	if (toggles) {
		return 1;
	}

	return 0;
}


int display_8x16_scroll_text(int display_type, char *string, int new_string) {

	unsigned char freq_matrix[16];

	/* 5x7 font, really 6x7 if you include space */
	/* 6*16 = 96 */
	char ascii[]=	" !\"#$%&\'()*+,-./"
			"0123456789:;<=>?"
			"@ABCDEFGHIJKLMNO"
			"PQRSTUVWXYZ[\\]^_"
			"`abcdefghijklmno"
			"pqrstuvwxyz{|}~ ";
//	char buf[16]="ABCDEFGHIJKLMNOP";
	char matrix[96][8];

	int scroll=0,x,y,length;
	static int i;
	static int frames=0;

	length=strlen(ascii);

	frames++;
	if (frames<50) {
		return 0;
	}
	else {
		frames=0;
		i++;
		if (i>length) i=0;
	}

	printf("Trying to print %c\n",ascii[i]);

	for(y=0;y<8;y++) {
		for(x=0;x<5;x++) {
			matrix[x][y]=!!(fbs_font[(int)ascii[i]][y]&1<<(4-x));
		}
		matrix[x][y]=0;
	}

	for(x=0;x<16;x++) {
		freq_matrix[x]=0;
		for(y=0;y<8;y++) {
			freq_matrix[x]|=matrix[x+scroll][y]<<(7-y);
		}
	}
	display_8x16_vertical(display_type,freq_matrix);

	return 0;
}
