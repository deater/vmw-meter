/* assumes two ht16k33 displays hooked up on the first i2c bus	*/
/* first one is custom with 3 (eventually 6?) 10 seg bargraphs	*/
/* second one is an adafruit 8x16 led matrix backpack		*/

/* The bargraphs are on i2c addr 0x70, the matrix on 0x72	*/

/* Be sure to modprobe i2c-dev */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


#include "i2c_lib.h"

#include "display.h"

#include "font.h"

static int current_mode=MODE_TITLE;
static int kiosk_mode=0;

static unsigned char display_buffer[DISPLAY_LINES];

static int i2c_fd=-1;

static int bargraph_i2c(int a, int b, int c) {

	int i;

	char buffer[17];

	buffer[0]=0;

	for(i=0;i<16;i++) buffer[i+1]=0x0;

	/* a */
	if (a>0) {
		a--;
		buffer[1]|=(2<<a)-1;
		if (a>7) buffer[2]|=(2<<(a-8))-1;
	}

	/* b */
	if (b>0) {
		b--;
		buffer[3]|=(2<<b)-1;
		if (b>7) buffer[4]|=(2<<(b-8))-1;
	}

	/* c */
	if (c>0) {
		c--;
		buffer[5]|=(2<<c)-1;
		if (c>7) buffer[6]|=(2<<(c-8))-1;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS0) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS0);
		return -1;
	}

	if ( (write(i2c_fd, buffer, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
        }

	return 0;
}

static int bargraph_text(int a, int b, int c) {

	int i,j,value;

	for(j=0;j<3;j++) {
		if (j==0) value=a;
		if (j==1) value=b;
		if (j==2) value=c;

		if (value>10) value=10;

		printf("[");
		for(i=0;i<value;i++) printf("*");
		for(i=value;i<10;i++) printf(" ");
		printf("]\n");
	}
	return 0;
}


static int bargraph(int type, int a, int b, int c) {

	if (type&DISPLAY_I2C) {
		bargraph_i2c(a,b,c);
	}

	if (type&DISPLAY_TEXT) {
		bargraph_text(a,b,c);
	}

	return 0;
}


static int close_bargraph(int type) {

	if (type&DISPLAY_I2C) {
		bargraph_i2c(0,0,0);
	}

	return 0;
}

static int freq_max[16];
static int freq_matrix[16][8];

static int put_8x16display(int display_type, int refresh_i2c) {

	char buffer[17];
	int i,x,y;

	if ((display_type&DISPLAY_I2C) && (refresh_i2c)) {

		buffer[0]=0;

		for(i=0;i<16;i++) buffer[i+1]=0x0;

		for(i=0;i<16;i++) {
			for(x=0;x<8;x++) {
				buffer[i+1]|=(freq_matrix[x+(8*(i%2))][i/2]<<x);
			}
		}

		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS2) < 0) {
			fprintf(stderr,"8x16 Error setting i2c address %x\n",
				HT16K33_ADDRESS2);
			return -1;
		}


		if ( (write(i2c_fd, buffer, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
				strerror(errno));
			return -1;
        	}
	}

	if (display_type&DISPLAY_TEXT) {
		for(y=0;y<8;y++) {
			for(x=0;x<16;x++) {
				if (freq_matrix[x][y]) printf("*");
				else printf(" ");
			}
			printf("\n");
		}
	}
	return 0;
}





#define UPDATE_DIVIDER	5

static int divider=0;

static int freq_display(int display_type, int a, int b, int c) {

	int x,y;

	if (a>=0) {
		if (a>15) {
//			printf("A too big %d\n",a);
			a=15;
		}
		freq_max[a]=1;
		for(y=0;y<8;y++) freq_matrix[a][y]=1;
	}

	if (b>=0) {
		if (b>15) {
//			printf("B too big %d\n",b);
			b=15;
		}
		freq_max[b]=1;
		for(y=0;y<8;y++) freq_matrix[b][y]=1;
	}

	if (c>=0) {
		if (c>15) {
//			printf("C too big %d\n",c);
			c=15;
		}
		freq_max[c]=1;
		for(y=0;y<8;y++) freq_matrix[c][y]=1;
	}


	for(x=0;x<16;x++) {
		freq_matrix[x][freq_max[x]]=7;
		if (divider==0)
		if (freq_max[x]<7) freq_max[x]++;
	}

	if (divider==0) {
		put_8x16display(display_type,1);
	} else {
		put_8x16display(display_type,0);
	}

	divider++;
	if (divider>UPDATE_DIVIDER) {
		divider=0;
		/* Clear background */

		for(y=0;y<8;y++) {
			for(x=0;x<16;x++) {
				freq_matrix[x][y]=0;
			}
		}
	}

	return 0;
}



static int close_freq_display(int display_type) {

	int x,y;
	int i;

	char buffer[17];

	/* Clear background */
	for(y=0;y<8;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y]=0;
		}
	}

	if (display_type&DISPLAY_I2C) {

		buffer[0]=0;
		for(i=0;i<16;i++) buffer[i+1]=0x0;

		for(i=0;i<16;i++) {
			for(x=0;x<8;x++) {
				buffer[i+1]|=(freq_matrix[x+(8*(i%2))][i/2]<<x);
			}
		}

		if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS2) < 0) {
			fprintf(stderr,"Close Error setting i2c address %x\n",
				HT16K33_ADDRESS2);
			return -1;
		}


		if ( (write(i2c_fd, buffer, 17)) !=17) {
			fprintf(stderr,"Error writing display %s!\n",
				strerror(errno));
			return -1;
        	}
	}

	return 0;
}

static int put_number(int which, int x, int y) {

	int a,b;

	for(a=0;a<3;a++) {
		for(b=0;b<5;b++) {
			freq_matrix[a+x][b+y]=number_font[which][b][a];
		}
	}

	return 0;
}

static int time_display(int display_type, int current_frame, int total_frames) {

	int x,y;
	int total_s,current_s,display_m,display_s,bar_length;

	/* Only update a few times a second? */
	/* Should do more for responsiveness? */
	if (current_frame%16!=0) {
		put_8x16display(display_type,0);
		return 0;
	}

	/* clear display */
	for(x=0;x<16;x++) {
		for(y=0;y<8;y++) {
			freq_matrix[x][y]=0;
		}
	}

	total_s=total_frames/50;
	current_s=current_frame/50;

	if (total_s==0) bar_length=0;
	else bar_length=(current_s*14)/total_s;
//	printf("Bar length=%d/14\n",bar_length);

	for(x=0;x<bar_length;x++) {
		freq_matrix[x+1][6]=1;
	}

	display_m=current_s/60;
	display_s=current_s%60;
//	printf("%d:%d\n",display_m,display_s);

	/* Print tens char */
	/* Don't handle files >= 20 minutes well */
	if (display_m>9) {
		for(y=0;y<5;y++) freq_matrix[1][y]=1;
	}

	put_number(display_m,3,0);

	/* Draw Colon */
	freq_matrix[7][1]=1;
	freq_matrix[7][3]=1;

	put_number(display_s/10,9,0);
	put_number(display_s%10,13,0);

	put_8x16display(display_type,1);

	return 0;
}

static int title_display(int display_type) {

	int x,y;
	static int count=0,offset=0,direction=+1;
	static int toggles=0;

	/* clear display */
	for(x=0;x<16;x++) {
		for(y=0;y<8;y++) {
			freq_matrix[x][y]=0;
		}
	}

	for(y=0;y<3;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y]=!!(vmw[y]&(1<<(31-x)));
		}
	}

	for(y=0;y<3;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y+4]=!!(chiptune[y]&(1<<(31-x-offset)));
		}
	}

	put_8x16display(display_type,1);

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
	if ((!kiosk_mode) && (toggles>0)) {
		current_mode=MODE_VISUAL;
	}

	return 0;
}

static int scroll_text(int display_type, char *string, int new_string) {

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

	for(y=0;y<8;y++) {
		for(x=0;x<16;x++) {
			freq_matrix[x][y]=matrix[x+scroll][y];
		}
	}

	put_8x16display(display_type,1);

	return 0;
}

int display_update(int display_type,
		struct display_stats *ds,
		int current_frame, int num_frames,
		char *filename, int new_filename) {


	bargraph(display_type, ds->a_bar, ds->b_bar, ds->c_bar);

	switch(current_mode) {
		case MODE_TITLE:
			title_display(display_type);
			break;
		case MODE_VISUAL:
			freq_display(display_type, ds->a_freq, ds->b_freq, ds->c_freq);
			break;
		case MODE_NAME:
			scroll_text(display_type, filename, new_filename);
			break;
		case MODE_TIME:
			time_display(display_type, current_frame, num_frames);
			break;
		case MODE_VOLUME:
		default:
			//printf("Unknown visual mode!\n");
			break;
	}

	return 0;
}


int display_read_keypad(int display_type) {

	unsigned char ch;
	int result;
	long long keypad;
	static int old_keypad=0;
	static int keypad_skip;

	/* Read from Keyboard */
	result=read(0,&ch,1);

	/* Read from keypad */
	if ((display_type&DISPLAY_I2C) && (keypad_skip==2)) {
		keypad=read_keypad(i2c_fd,HT16K33_ADDRESS0);
		if (keypad!=old_keypad) {
			printf("KEY: %lld\n",keypad);
			old_keypad=keypad;
			if (keypad!=0) {
				result=0;
				if (keypad&256) ch=',';
				if (keypad&512) ch=' ';
				if (keypad&1024) ch='.';
				if (keypad&2048) ch='m';
				if (keypad&4096) ch='q';
			}
		}
		keypad_skip=0;
	}
	keypad_skip++;

	if (result<0) { //printf("Error %s\n",strerror(errno));
	}
	else {
		switch(ch) {
			case 'a':
				return CMD_MUTE_A;
				break;
			case 'b':
				return CMD_MUTE_B;
				break;
			case 'c':
				return CMD_MUTE_C;
				break;
			case 'n':
				return CMD_MUTE_N;
				break;
			case ' ': /* pause/play */
				return CMD_PAUSE;
				break;
			case '<': /* back */
				return CMD_BACK;
				break;
			case '>': /* forward */
				return CMD_FWD;
				break;
			case ',': /* rewind */
				return CMD_RW;
				break;
			case '.': /* fast-forward */
				return CMD_FF;
				break;
			case 'l': /* toggle loop */
				return CMD_LOOP;
				break;
			case 'm': /* mode */
				current_mode++;
				if (current_mode==MODE_MAX) {
					current_mode=0;
				}
				break;
			case '-': /* volume down */
				return CMD_VOL_DOWN;
				break;
			case '=':
			case '+':
				return CMD_VOL_UP;
				break;
			case '1':
				return CMD_HEADPHONE_IN;
				break;
			case '2':
				return CMD_HEADPHONE_OUT;
				break;
			case 'q':
			case 'Q':
			case 27:
				/* quit */
				return CMD_EXIT_PROGRAM;
				break;

		}
	}



	return 0;
}

static struct termios saved_tty;

int display_shutdown(int display_type) {

	/* read any lingering keypad presses */
	if (display_type&DISPLAY_I2C) {
		read_keypad(i2c_fd,HT16K33_ADDRESS0);
	}

	close_freq_display(display_type);
	close_bargraph(display_type);

	/* restore keyboard */
	tcsetattr (0, TCSANOW, &saved_tty);

	return 0;
}



int display_init(int type) {

	int result=0;
	int i;

	struct termios new_tty;

	/* Save currenty term settings */
	tcgetattr (0, &saved_tty);
	tcgetattr (0, &new_tty);

	/* Put term in raw keryboard mode */
	new_tty.c_lflag &= ~ICANON;
	new_tty.c_cc[VMIN] = 1;
	new_tty.c_lflag &= ~ECHO;
	tcsetattr (0, TCSANOW, &new_tty);
	/* Make it nonblocking too */
	fcntl (0, F_SETFL, fcntl (0, F_GETFL) | O_NONBLOCK);


	if (type&DISPLAY_I2C) {

		i2c_fd=init_i2c("/dev/i2c-1");
		if (i2c_fd < 0) {
			fprintf(stderr,"Error opening device!\n");
			return -1;
 		}

		/* Init display */
		if (init_display(i2c_fd,HT16K33_ADDRESS0,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}

		/* Init display */
		if (init_display(i2c_fd,HT16K33_ADDRESS2,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}

		for(i=0;i<DISPLAY_LINES;i++) display_buffer[i]=0;
	}

	return result;
}
