#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

#include <sys/time.h>

#include <sys/ioctl.h>
#include <sys/resource.h>
#include <linux/i2c-dev.h>

#include "display.h"
#include "i2c_lib.h"

#include "ay-3-8910.h"
#include "ym_lib.h"

#include <bcm2835.h>

#include "14seg_font.h"

#include "lyrics.h"

static int i2c_display=1;
static int play_music=1;
static int shift_size=16;

static int i2c_fd;

static void print_ascii_art(int which);
static int display_led_art(int which);

static int y_line=0;


static void quiet_and_exit(int sig) {

	if (play_music) {
		quiet_ay_3_8910(shift_size);
		close_ay_3_8910();
	}

//	display_shutdown(display_type);

	printf("Quieting and exiting\n");
	_exit(0);

}


static int clear_things(int side_too) {

	int i;
	char string[BUFSIZ];

	if (side_too) {
		/* clear screen */
		write(1,"\033[2J",4);

		/* clear 16x8 display */
		display_led_art(1024);
	}

	write(1,"\033[1;1H--------------------------------------",44);
	for(i=2;i<24;i++) {
		sprintf(string,"\033[%d;1H|                                    |",i);
		write(1,string,strlen(string));
	}
	write(1,"\033[24;1H--------------------------------------",45);
	write(1,"\033[2;2H",6);
	y_line=2;
	return 0;
}


int display_string(char *led_string) {

	char buffer1[17],buffer2[17],buffer3[17];
	int i,ch;

	buffer1[0]=0;
	buffer2[0]=0;
	buffer3[0]=0;

	for(i=0;i<4;i++) {
		ch=led_string[i];
		buffer1[(i*2)+1]=adafruit_lookup[ch]>>8;
		buffer1[(i*2)+2]=adafruit_lookup[ch]&0xff;
	}

	for(i=0;i<4;i++) {
		ch=led_string[i+4];
		buffer2[(i*2)+1]=adafruit_lookup[ch]>>8;
		buffer2[(i*2)+2]=adafruit_lookup[ch]&0xff;
	}

	for(i=0;i<4;i++) {
		ch=led_string[i+8];
		buffer3[(i*2)+1]=adafruit_lookup[ch]>>8;
		buffer3[(i*2)+2]=adafruit_lookup[ch]&0xff;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS5) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS5);
		return -1;
	}

	if ( (write(i2c_fd, buffer1, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS3) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS3);
		return -1;
	}

	if ( (write(i2c_fd, buffer2, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS7) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS7);
		return -1;
	}

	if ( (write(i2c_fd, buffer3, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
	}
	return 0;

}


#define NUM_ALPHANUM	12

/* Currently we are 8 50Hz interrupts per 16th note */
#define MAX_LYRIC_LEN	8


static int ignore_led=0;

static int parse_lyric(struct lyric_type *l, int lnum, char *string) {

	int length=0,sub=0;
	int ch;

	while(1) {

		ch=l->l[lnum].text[sub];

		if (ch==0) break;

		/* Handle special escape characters */
		if (ch=='\\') {
			sub++;
			ch=l->l[lnum].text[sub];

			/* \i means don't write text to LED display */
			if (ch=='i') {
				ignore_led=1;
			}

			/* \n is special, we delay updating LED */
			if (ch=='n') {
				string[length]='\n';
				length++;
				if (length>MAX_LYRIC_LEN) break;
			}

			/* \1 - \: (i.e. 1-10) */
			/* Update ASCII art on screen and 8x16 panel */
			if ((ch>='1')&&(ch<=':')) {
				print_ascii_art(ch-'1');
				display_led_art(ch-'1');
			}

			/* \f means clear screen */
			if (ch=='f') {
				string[length]='\f';
				length++;
				if (length>MAX_LYRIC_LEN) break;
			}
		}

		else {
			string[length]=ch;
			length++;
			if (length>MAX_LYRIC_LEN) break;
		}

		sub++;

	}

	if (length>MAX_LYRIC_LEN) {
		fprintf(stderr,"ERROR! LYRIC TOO LONG!\n");
		return -1;
	}

	/* NUL terminate */
	string[length]=0;

	return length;
}

static int lyrics_play(struct lyric_type *l) {

	int frame=0,lnum=0,sub=0;
	char led_string[NUM_ALPHANUM],ch;
	int i;
	char string[BUFSIZ];
	int result;
	int led_offset=0,clear_next=0;

	struct timeval start,next;
	double s,n,diff;

	struct ym_song_t ym_song;

	int lyric_active=0;
	char current_lyric[MAX_LYRIC_LEN+1];

	result=load_ym_song("sa/sa.ym5",&ym_song);
	if (result<0) {
		return -1;
	}

	clear_things(1);

        gettimeofday(&start,NULL);

	frame=0;
	while(1) {

		/* Play the music for this frame */
		ym_play_frame(&ym_song,frame,shift_size,
				NULL,0,play_music);


		/* Parse any lyric updates for this frame */

		/* We cross a lyric threshold, start a lyric */
		if (frame==l->l[lnum].frame) {
			lyric_active=1;
			sub=0;
			ignore_led=0;
			parse_lyric(l,lnum,current_lyric);
		}

		if (lyric_active) {
			ch=current_lyric[sub];
			if (ch==0) {
				lyric_active=0;
				lnum++;
			}
			else if (ch=='\n') {
				y_line++;
				sprintf(string,"\n\033[%d;2H",y_line);
				write(1,string,strlen(string));

				if (!ignore_led) clear_next=1;
			}
			else if (ch=='\f') {
				clear_things(0);
				y_line=2;

				for(i=0;i<NUM_ALPHANUM;i++) {
					led_string[i]=' ';
				}
				led_offset=0;
			}
			else {
				write(1,&ch,1);

				if (!ignore_led) {
					if (clear_next) {
						for(i=0;i<NUM_ALPHANUM;i++) {
							led_string[i]=' ';
						}
						led_offset=0;
						clear_next=0;
					}

					if (led_offset<NUM_ALPHANUM) {

					}
					else {
						for(i=1;i<NUM_ALPHANUM;i++) {
							led_string[i-1]=led_string[i];
						}
						led_offset=NUM_ALPHANUM-1;
					}

					if (isalpha(ch)) {
						led_string[led_offset]=toupper(ch);
					}
					else {
						led_string[led_offset]=ch;
					}
					led_offset++;
				}
			}

			if (i2c_display) display_string(led_string);

			sub++;
		}



		/* Calculate time we were busy this frame */
		gettimeofday(&next,NULL);
		s=start.tv_sec+(start.tv_usec/1000000.0);
		n=next.tv_sec+(next.tv_usec/1000000.0);
		diff=(n-s)*1000000.0;

		start.tv_sec=next.tv_sec;
		start.tv_usec=next.tv_usec;

		/* Delay until time for next update (often 50Hz) */
                if (play_music) {
                        if (diff>0) bcm2835_delayMicroseconds(20000-diff);
                        /* often 50Hz = 20000 */
                        /* TODO: calculate correctly */
                }
                else {
                        if (1) usleep(1000000/ym_song.frame_rate);
                }


		frame++;

		/* If hit the end of the song, then stop */
		if (frame>ym_song.num_frames) break;
	}

	return 0;

}

int main(int argc, char **argv) {

	int result;

	struct lyric_type l;

	/* Setup control-C handler to quiet the music   */
	/* otherwise if you force quit it keeps playing */
	/* the last tones */
	signal(SIGINT, quiet_and_exit);

	/* Set to have highest possible priority */
	setpriority(PRIO_PROCESS, 0, -20);

	if (play_music) {
		result=initialize_ay_3_8910(1);
		if (result<0) {
			printf("Error starting music!\n");
			play_music=0;
		}
	}

	/* Initialize the display */
	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		i2c_display=0;
	}

	if (i2c_display) {

		/* Init displays */

		/* ALPHANUM #1 */
		if (init_display(i2c_fd,HT16K33_ADDRESS3,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}

		/* ALPHANUM #2 */
		if (init_display(i2c_fd,HT16K33_ADDRESS7,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}

		/* ALPHANUM #3 */ /* ? */
		if (init_display(i2c_fd,HT16K33_ADDRESS5,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}

		/* 8x16 */
		if (init_display(i2c_fd,HT16K33_ADDRESS2,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}


	}

	translate_to_adafruit();

//	display_led_art(6);

	load_lyrics("sa/sa.lyrics",&l);

	lyrics_play(&l);

	destroy_lyrics(&l);

	if (i2c_display) {
		close(i2c_fd);
	}

	return 0;
}

/* Inspired a bit by evi1wombat */
/* https://www.youtube.com/watch?v=hfmGnLMlKvs */

static short led_art[10][8]={
/* 1 = aperture */
	{
	0x03c0,		/*       ****       */
	0x0560,		/*      * * **      */
	0x0c10,		/*     **     *     */
	0x0830,		/*     *     **     */
	0x0c10,		/*     **     *     */
	0x0830,		/*     *     **     */
	0x06a0,		/*      ** * *      */
	0x03c0,		/*       ****       */
	},
	{
/* 2 = radiation */
	0x01c0,		/*        ***      */
	0x01c0,		/*        ***      */
	0x0080,		/*         *       */
	0x0000,		/*                 */
	0x0080,		/*         *       */
	0x0e38,		/*     ***   ***   */
	0x0630,		/*      **   **    */
	0x0220,		/*       *   *     */
	},
/* 3 = atom */
	{
	0x0180,		/*        **       */
	0x1a58,		/*    ** *  * **   */
	0x15a8,		/*    * * ** * *   */
	0x0a50,		/*     * *  * *    */
	0x0a50,		/*     * *  * *    */
	0x15a8,		/*    * * ** * *   */
	0x1a58,		/*    ** *  * **   */
	0x0180,		/*        **       */
	},
/* 4 = broken heart */
	{
	0x0c30,		/*     **    **    */
	0x1e78,		/*    ****  ****   */
	0x1ce8,		/*    ***  *** *   */
	0x1e78,		/*    ****  ****   */
	0x0cf0,		/*     **  ****    */
	0x0660,		/*      **  **     */
	0x03c0,		/*       ****      */
	0x0180,		/*        **       */
	},
/* 5 = explosion */
	{
	0x0200,		/*       *        */
	0x0310,		/*       **   *   */
	0x11a0,		/*    *   ** *    */
	0x0fe8,		/*     ******* *  */
	0x03f0,		/*       ******   */
	0x07e0,		/*      ******    */
	0x0db0,		/*     ** ** **   */
	0x1118,		/*    *   *   **  */
	},
/* 6 = fire */
	{
	0x0290,		/*       * *  *   */
	0x08c0,		/*     *   **     */
	0x01c0,		/*        ***     */
	0x03e0,		/*       *****    */
	0x0760,		/*      *** **    */
	0x0660,		/*      **  **    */
	0x06c0,		/*      ** **     */
	0x0280,		/*       * *      */
	},
/* 7 = check */
	{
	0x001c,		/*          ***   */
	0x003c,		/*         ****   */
	0x0078,		/*        ****    */
	0x18f0,		/*  **   ****     */
	0x3de0,		/* **** ****      */
	0x1fc0,		/*  *******       */
	0x0f80,		/*   *****        */
	0x0700,		/*    ***         */
	},
/* 8 = black mesa */
	{
	0x03c0,		/*      ****      */
	0x0420,		/*     *    *     */
	0x0810,		/*    *      *    */
	0x09d0,		/*    *  *** *    */
	0x09f0,		/*    *  *****    */
	0x09f0,		/*    *  *****    */
	0x07e0,		/*     ******     */
	0x03c0,		/*      ****      */
	},
/* 9 = cake, delicious and moist */
	{
	0x0df8,		/*    ** ******  */
	0x1f80,		/*   ******      */
	0x1c18,		/*   ***     **  */
	0x1c60,		/*   ***   **    */
	0x1f80,		/*   ******      */
	0x1c18,		/*   ***     **  */
	0x0c60,		/*    **   **    */
	0x0780,		/*     ****      */
	},
/* : = GLaDOS */
	{
	0x18f8,		/*    **   *****  */
	0x3124,		/*   **   *  *  * */
	0x11c4,		/*    *   ***   * */
	0x3224,		/*   **  *   *  * */
	0x12a4,		/*    *  * * *  * */
	0x32a4,		/*   **  * * *  * */
	0x3224,		/*   **  *   *  * */
	0x19c8,		/*    **  ***  *  */
	},
};

static char ascii_art[10][21][41]={
{
/* 1 = aperture */
	{"              .,-:;//;:=,"},
	{"          . :H@@@MM@M#H/.,+%;,"},
	{"       ,/X+ +M@@M@MM%=,-%HMMM@X/,"},
	{"     -+@MM; $M@@MH+-,;XMMMM@MMMM@+-"},
	{"    ;@M@@M- XM@X;. -+XXXXXHHH@M@M#@/."},
	{"  ,%MM@@MH ,@%=            .---=-=:=,."},
	{"  =@#@@@MX .,              -%HX$$%%%+;"},
	{" =-./@M@M$                  .;@MMMM@MM:"},
	{" X@/ -$MM/                    .+MM@@@M$"},
	{",@M@H: :@:                    . =X#@@@@-"},
	{",@@@MMX, .                    /H- ;@M@M="},
	{".H@@@@M@+,                    %MM+..%#$."},
	{" /MMMM@MMH/.                  XM@MH; =;"},
	{"  /%+%$XHH@$=              , .H@@@@MX,"},
	{"   .=--------.           -%H.,@@@@@MX,"},
	{"   .%MM@@@HHHXX$$$%+- .:$MMX =M@@MM%."},
	{"     =XMMM@MM@MM#H;,-+HMM@M+ /MMMX="},
	{"       =%@M@M#@$-.=$@MM@@@M; %M%="},
	{"         ,:+$+-,/H#MMMMMMM@= =,"},
	{"               =++%%%%+/:-."},
},
{
/* 2 = radiation */
	{"             =+$HM####@H%;,"},
	{"          /H###############M$,"},
	{"          ,@################+"},
	{"           .H##############+"},
	{"             X############/"},
	{"              $##########/"},
	{"               %########/"},
	{"                /X/;;+X/"},
	{" "},
	{"                 -XHHX-"},
	{"                ,######,"},
	{"#############X  .M####M.  X#############"},
	{"##############-   -//-   -##############"},
	{"X##############%,      ,+##############X"},
	{"-##############X        X##############-"},
	{" %############%          %############%"},
	{"  %##########;            ;##########%"},
	{"   ;#######M=              =M#######;"},
	{"    .+M###@,                ,@###M+."},
	{"       :XH.                  .HX:"},
},
{
/* 3 = atom */
	{"                 =/;;/-"},
	{"                +:    //"},
	{"               /;      /;"},
	{"              -X        H."},
	{".//;;;:;;-,   X=        :+   .-;:=;:;%;."},
	{"M-       ,=;;;#:,      ,:#;;:=,       ,@"},
	{":%           :%.=/++++/=.$=           %="},
	{" ,%;         %/:+/;,,/++:+/         ;+."},
	{"   ,+/.    ,;@+,        ,%H;,    ,/+,"},
	{"      ;+;;/= @.  .H##X   -X :///+;"},
	{"      ;+=;;;.@,  .XM@$.  =X.//;=%/."},
	{"   ,;:      :@%=        =$H:     .+%-"},
	{" ,%=         %;-///==///-//         =%,"},
	{";+           :%-;;;:;;;;-X-           +:"},
	{"@-      .-;;;;M-        =M/;;;-.      -X"},
	{" :;;::;;-.    %-        :+    ,-;;-;:=="},
	{"              ,X        H."},
	{"               ;/      %="},
	{"                //    +;"},
	{"                 ,////,"},
},
{
/* 4 = broken heart */
	{"                          .,---."},
	{"                        ,/XM#MMMX;,"},
	{"                      -%##########M%,"},
	{"                     -@######%  $###@="},
	{"      .,--,         -H#######$   $###M:"},
	{"   ,;$M###MMX;     .;##########$;HM###X="},
	{" ,/@##########H=      ;################+"},
	{"-+#############M/,      %##############+"},
	{"%M###############=      /##############:"},
	{"H################      .M#############;."},
	{"@###############M      ,@###########M:."},
	{"X################,      -$=X#######@:"},
	{"/@##################%-     +######$-"},
	{".;##################X     .X#####+,"},
	{" .;H################/     -X####+."},
	{"   ,;X##############,       .MM/"},
	{"      ,:+$H@M#######M#$-    .$$="},
	{"           .,-=;+$@###X:    ;/=."},
	{"                  .,/X$;   .::,"},
	{"                      .,    .."},
},
{
/* 5 = explosion */
	{"            .+"},
	{"             /M;"},
	{"              H#@:              ;,"},
	{"              -###H-          -@/"},
	{"               %####$.  -;  .%#X"},
	{"                M#####+;#H :M#M."},
	{"..          .+/;%#########X###-"},
	{" -/%H%+;-,    +##############/"},
	{"    .:$M###MH$%+############X  ,--=;-"},
	{"        -/H#####################H+=."},
	{"           .+#################X."},
	{"         =%M####################H;."},
	{"            /@###############+;;/%%;,"},
	{"         -%###################$."},
	{"       ;H######################M="},
	{"    ,%#####MH$%;+#####M###-/@####%"},
	{"  :$H%+;=-      -####X.,H#   -+M##@-"},
	{" .              ,###;    ;      =$##+"},
	{"                .#H,               :XH,"},
	{"                 +                   .;-"},
},
{
/* 6 = fire */
	{"                     -$-"},
	{"                    .H##H,"},
	{"                   +######+"},
	{"                .+#########H."},
	{"              -$############@."},
	{"            =H###############@  -X:"},
	{"          .$##################:  @#@-"},
	{"     ,;  .M###################;  H###;"},
	{"   ;@#:  @###################@  ,#####:"},
	{" -M###.  M#################@.  ;######H"},
	{" M####-  +###############$   =@#######X"},
	{" H####$   -M###########+   :#########M,"},
	{"  /####X-   =########%   :M########@/."},
	{"    ,;%H@X;   .$###X   :##MM@%+;:-"},
	{"                 .."},
	{"  -/;:-,.              ,,-==+M########H"},
	{" -##################@HX%%+%%$%%%+:,,"},
	{"    .-/H%%%+%%$H@###############M@+=:/+:"},
	{"/XHX%:#####MH%=    ,---:;;;;/%%XHM,:###$"},
	{"$@#MX %+;-                           ."},
},
{
/* 7 = check */
	{"                                     :X-"},
	{"                                  :X###"},
	{"                                ;@####@"},
	{"                              ;M######X"},
	{"                            -@########$"},
	{"                          .$##########@"},
	{"                         =M############-"},
	{"                        +##############$"},
	{"                      .H############$=."},
	{"         ,/:         ,M##########M;."},
	{"      -+@###;       =##########M;"},
	{"   =%M#######;     :#########M/"},
	{"-$M###########;   :#########/"},
	{" ,;X###########; =########$."},
	{"     ;H#########+#######M="},
	{"       ,+##############+"},
	{"          /M#########@-"},
	{"            ;M######%"},
	{"              +####:"},
	{"               ,$M-"},
},
{
/* 8 = black mesa */
	{"           .-;+$XHHHHHHX$+;-."},
	{"        ,;X@@X%/;=----=:/%X@@X/,"},
	{"      =$@@%=.              .=+H@X:"},
	{"    -XMX:                      =XMX="},
	{"   /@@:                          =H@+"},
	{"  %@X,                            .$@$"},
	{" +@X.                               $@%"},
	{"-@@,                                .@@="},
	{"%@%                                  +@$"},
	{"H@:                                  :@H"},
	{"H@:         :HHHHHHHHHHHHHHHHHHX,    =@H"},
	{"%@%         ;@M@@@@@@@@@@@@@@@@@H-   +@$"},
	{"=@@,        :@@@@@@@@@@@@@@@@@@@@@= .@@:"},
	{" +@X        :@@@@@@@@@@@@@@@M@@@@@@:%@%"},
	{"  $@$,      ;@@@@@@@@@@@@@@@@@M@@@@@@$."},
	{"   +@@HHHHHHH@@@@@@@@@@@@@@@@@@@@@@@+"},
	{"    =X@@@@@@@@@@@@@@@@@@@@@@@@@@@@X="},
	{"      :$@@@@@@@@@@@@@@@@@@@M@@@@$:"},
	{"        ,;$@@@@@@@@@@@@@@@@@@X/-"},
	{"           .-;+$XXHHHHHX$+;-."},
},
{
/* 9 = cake, delicious and moist */
	{"            ,:/+/-"},
	{"            /M/              .,-=;//;-"},
	{"       .:/= ;MH/,    ,=/+%$XH@MM#@:"},
	{"      -$##@+$###@H@MMM#######H:.    -/H#"},
	{" .,H@H@ X######@ -H#####@+-     -+H###@X"},
	{"  .,@##H;      +XM##M/,     =%@###@X;-"},
	{"X%-  :M##########$.    .:%M###@%:"},
	{"M##H,   +H@@@$/-.  ,;$M###@%,          -"},
	{"M####M=,,---,.-%%H####M$:          ,+@##"},
	{"@##################@/.         :%H##@$-"},
	{"M###############H,         ;HM##M$="},
	{"#################.    .=$M##M$="},
	{"################H..;XM##M$=          .:+"},
	{"M###################@%=           =+@MH%"},
	{"@################M/.          =+H#X%="},
	{"=+M##############M,       -/X#X+;."},
	{"  .;XM##########H=    ,/X#H+:,"},
	{"     .=+HM######M+/+HM@+=."},
	{"         ,:/%XM####H/."},
	{"              ,.:=-."},
},
{
/* : = GLaDOS */
	{"       #+ @      # #              M#@"},
	{" .    .X  X.%##@;# #   +@#######X. @#%"},
	{"   ,==.   ,######M+  -#####%M####M-    #"},
	{"  :H##M%:=##+ .M##M,;#####/+#######% ,M#"},
	{" .M########=  =@#@.=#####M=M#######=  X#"},
	{" :@@MMM##M.  -##M.,#######M#######. =  M"},
	{"             @##..###:.    .H####. @@ X,"},
	{"   ############: ###,/####;  /##= @#. M"},
	{"           ,M## ;##,@#M;/M#M  @# X#% X#"},
	{".%=   ######M## ##.M#:   ./#M ,M #M ,#$"},
	{"##/         $## #+;#: #### ;#/ M M- @# :"},
	{"#+ #M@MM###M-;M #:$#-##$H# .#X @ + $#. #"},
	{"      ######/.: #%=# M#:MM./#.-#  @#: H#"},
	{"+,.=   @###: /@ %#,@  ##@X #,-#@.##% .@#"},
	{"#####+;/##/ @##  @#,+       /#M    . X,"},
	{"   ;###M#@ M###H .#M-     ,##M  ;@@; ###"},
	{"   .M#M##H ;####X ,@#######M/ -M###$  -H"},
	{"    .M###%  X####H  .@@MM@;  ;@#M@"},
	{"      H#M    /@####/      ,++.  / ==-,"},
	{"               ,=/:, .+X@MMH@#H  #####$="},
}
};

static void print_ascii_art(int which) {
	int i;
	char string[BUFSIZ];

	/* save cursor position */
	write(1,"\033[s",3);

	for(i=0;i<21;i++) {
		sprintf(string,"\033[%d;40H",i+2);
		write(1,string,strlen(string));
		write(1,ascii_art[which][i],strlen(&ascii_art[which][i][0]));
		write(1," \033[K",4);
	}

	/* restore cursor position */
	write(1,"\033[u",3);

	return;
}

int reverse_bits(int b) {

	int out;

	out = ((b * 0x0802LU & 0x22110LU) |
		(b * 0x8020LU & 0x88440LU)) *
		0x10101LU >> 16;
	return out&0xff;
}

static int display_led_art(int which) {

	int i;
	char buffer[17];

	if (!i2c_display) return 0;

	buffer[0]=0;

	/* clear buffer */
	for(i=0;i<16;i++) buffer[i+1]=0x0;

	if (which==1024) {
		/* special case, clear screen */
	} else {

		for(i=0;i<8;i++) {
			buffer[i*2+1]=reverse_bits((led_art[which][i]>>8));
			buffer[i*2+2]=reverse_bits(led_art[which][i]&0xff);
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

	return 0;
}
