#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "display.h"
#include "i2c_lib.h"

#include "14seg_font.h"

#define MAX_LYRICS	2048

struct lyric_line {
	int frame;
	char *text;
};

struct lyric_type {
	int num;
	struct lyric_line *l;
};

static int i2c_fd;

static void print_ascii_art(int which);

static int y_line=0;

static int clear_things(int side_too) {

	int i;
	char string[BUFSIZ];

	if (side_too) {
		write(1,"\033[2J",4);
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

	char buffer1[17],buffer2[17];
	int i,ch;

	buffer1[0]=0;
	buffer2[0]=0;

	for(i=0;i<4;i++) {
		ch=led_string[i+4];
		buffer1[(i*2)+1]=adafruit_lookup[ch]>>8;
		buffer1[(i*2)+2]=adafruit_lookup[ch]&0xff;
	}

	for(i=0;i<4;i++) {
		ch=led_string[i];
		buffer2[(i*2)+1]=adafruit_lookup[ch]>>8;
		buffer2[(i*2)+2]=adafruit_lookup[ch]&0xff;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS3) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS3);
		return -1;
	}

	if ( (write(i2c_fd, buffer1, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
	}

	if (ioctl(i2c_fd, I2C_SLAVE, HT16K33_ADDRESS7) < 0) {
		fprintf(stderr,"Bargraph error setting i2c address %x\n",
			HT16K33_ADDRESS7);
		return -1;
	}

	if ( (write(i2c_fd, buffer2, 17)) !=17) {
		fprintf(stderr,"Error writing display %s!\n",
			strerror(errno));
		return -1;
	}

	return 0;
}


static int lyrics_play(struct lyric_type *l, int i2c_display) {

	int frame=0,lnum=0,sub=0;
	char led_string[8],ch;
	int i;
	char string[BUFSIZ];

	clear_things(1);

	while(1) {
		frame++;

		if (frame>=l->l[lnum].frame) {

			sub=0;
			while(1) {
				if (l->l[lnum].text[sub]==0) break;

				if (l->l[lnum].text[sub]=='\\') {
					sub++;
					ch=l->l[lnum].text[sub];
					if (ch=='n') {
						y_line++;
						sprintf(string,"\n\033[%d;2H",y_line);
						write(1,string,strlen(string));
					}
					if ((ch>='1')&&(ch<=':')) {
						print_ascii_art(ch-'1');
					}
					if (ch=='f') {
						clear_things(0);
						y_line=2;
					}
				}

				else {
					ch=l->l[lnum].text[sub];
					write(1,&ch,1);

					for(i=1;i<8;i++) {
						led_string[i-1]=led_string[i];
					}
					if (isalpha(ch)) {
						led_string[7]=toupper(ch);
					}
					else {
						led_string[7]=ch;
					}
				}

				if (i2c_display) display_string(led_string);

				sub++;

				usleep(20000);
				usleep(20000);
				frame+=2;


			}
			lnum++;
		}
		usleep(20000);
		if (lnum>=l->num) break;
	}

	return 0;

}

static int load_lyrics(char *filename, struct lyric_type *l) {

	char *result;
	char string[BUFSIZ];

	int num=0,line=0;
	int sp;
	int frame_num;
	char *start;

	l->l=calloc(sizeof(struct lyric_type),MAX_LYRICS);
	if (l->l==NULL) {
		fprintf(stderr,"MEM ERROR\n");
		return -1;
	}

	while(1) {
		result=fgets(string,BUFSIZ,stdin);
		if (result==NULL) {
			break;
		}
		line++;

//		printf("%s",string);

		sp=0;

		/* Ignore comments */
		if (string[0]=='#') continue;

		/* skip whitespace */
		while((string[sp]==' ' || string[sp]=='\t')) sp++;
		if (string[sp]=='\n') continue;

		/* get number */
		frame_num=0;
		while((string[sp]!=' ' && string[sp]!='\t' &&
			string[sp]!='\n')) {
			frame_num*=10;
			frame_num+=string[sp]-'0';
			sp++;
		}

		l->l[num].frame=frame_num;

		/* skip whitespace */
		while((string[sp]==' ' || string[sp]=='\t')) sp++;
		if (string[sp]=='\n') continue;

		if (string[sp]!='\"') {
			fprintf(stderr,"Unknown string line %d\n",line);
			continue;
		}
		sp++;

		start=&string[sp];

		while((string[sp]!='\"' && string[sp]!='\n')) {
			sp++;
		}
		string[sp]='\0';

		l->l[num].text=strdup(start);


//		printf("%d %s\n",num,start);
//		fflush(stdout);
//		printf("%d %s\n",l[num].frame,l[num].text);

		num++;
		if (num>MAX_LYRICS) {
			fprintf(stderr,"Too many lyrics, line %d!\n",line);
			return -1;
		}
	}

	l->num=num;

	return 0;
}

static int destroy_lyrics(struct lyric_type *l) {

	free(l->l);

	return 0;
}

int main(int argc, char **argv) {

	int visualize=1;

	struct lyric_type l;

	/* Initialize the display */
	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		visualize=0;
	}

	if (visualize) {

		/* Init display */
		if (init_display(i2c_fd,HT16K33_ADDRESS3,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}
		if (init_display(i2c_fd,HT16K33_ADDRESS7,10)) {
			fprintf(stderr,"Error opening display\n");
			return -1;
		}
	}

	translate_to_adafruit();

	load_lyrics(NULL,&l);

	lyrics_play(&l,visualize);

	destroy_lyrics(&l);

	close(i2c_fd);

	return 0;
}

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
/* 4 = heart */
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
