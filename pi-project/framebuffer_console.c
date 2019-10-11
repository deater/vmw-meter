#include <stddef.h>
#include <stdint.h>

#include "printk.h"
#include "mmio.h"

#include "framebuffer.h"
#include "framebuffer_console.h"

#include "cfnt.h"

#include "string.h"

#define ANSI_BLACK	0
#define ANSI_RED	1
#define ANSI_GREEN	2
#define ANSI_BLUE	4
#define ANSI_GREY	7


static int font_ysize=16;
static unsigned char *current_font=(unsigned char *)default_font;

#define CONSOLE_X 80
#define CONSOLE_Y 35

static unsigned char text_console[CONSOLE_X*CONSOLE_Y];
static unsigned char text_color[CONSOLE_X*CONSOLE_Y];
static int console_x=0;
static int console_y=0;
static int console_fore_color=0xffffff;
static int console_back_color=0;

static unsigned int ansi_colors[16]={
	0x000000, /* black */
        0xaa0000, /* red */
        0x00aa00, /* green */
        0xaa5522, /* brown */
        0x0000aa, /* blue  */
        0xaa00aa, /* purple */
        0x00aaaa, /* cyan */
        0xaaaaaa, /* grey */
        0x7d7d7d, /* dark grey */
        0xff7d7d, /* bright red */
        0x00ff00, /* bright green */
        0xffff00, /* yellow */
        0x0000ff, /* bright blue */
        0xff00ff, /* pink */
        0x00ffff, /* bright_cyan */
        0xffffff, /* white */
};

void framebuffer_console_setfont(int which) {

	current_font=(unsigned char *)default_font;
	font_ysize=16;
	printk("Using default font\n");

	framebuffer_clear_screen(0);

}


int framebuffer_console_putchar(int fore_color, int back_color,
			int ch, int x, int y) {

	int xx,yy;

	for(yy=0;yy<font_ysize;yy++) {
		for(xx=0;xx<8;xx++) {
			if (current_font[(ch*font_ysize)+yy] & (1<<(7-xx))) {
				framebuffer_putpixel(fore_color,x+xx,y+yy);
			}
			else {
				/* transparency */
				if (back_color!=0) {
					framebuffer_putpixel(back_color,x+xx,y+yy);
				}
			}
		}
	}
	return 0;

}

int framebuffer_console_clear(void) {

	int x,y;

	framebuffer_clear_screen(0);

	for(y=0;y<CONSOLE_Y;y++) {
		for(x=0;x<CONSOLE_X;x++) {
			text_console[x+(y*CONSOLE_X)]=' ';
			text_color[x+(y*CONSOLE_X)]=FORE_GREY|BACK_BLACK;
		}
	}

	return 0;
}

int framebuffer_console_home(void) {

	console_x=0;
	console_y=0;

	return 0;
}

int framebuffer_console_init(void) {

	framebuffer_console_clear();
	framebuffer_console_home();

	return 0;
}



int framebuffer_console_push(void) {

	int x,y;

	if (!framebuffer_ready()) return -1;

	for(x=0;x<CONSOLE_X;x++) {
		for(y=0;y<CONSOLE_Y;y++) {
			framebuffer_console_putchar(
				ansi_colors[text_color[x+(y*CONSOLE_X)]&0xf],
				ansi_colors[(text_color[x+(y*CONSOLE_X)]>>4)&0xf],
				text_console[x+(y*CONSOLE_X)],x*8,y*font_ysize);
		}
	}

	return 0;
}

static int isalpha(int ch) {

	if ((ch>='A') && (ch<='Z')) return 1;
	if ((ch>='a') && (ch<='z')) return 1;

	return 0;

}

static void ansi_parse_color(char *code) {

	int p=0;
	int number=0;

//	printk("Parsing: %s\n",code);

	while(code[p]!=0) {

		if ((code[p]==';') || (code[p]=='m')) {

//			printk("Setting color %d\n",number);

			if (number==0) {
				console_fore_color=ANSI_GREY;
				console_back_color=ANSI_BLACK;
			}

			/* Foreground Colors */
			if ((number>=30)&&(number<=37)) {
				console_fore_color=number-30;
			}

			/* FIXME Color 38 used for 24-bit color support */

			/* Background Colors */
			if ((number>=40)&&(number<=47)) {
				console_back_color=number-40;
			}
			number=0;
		}
		else {
			number*=10;
			number+=(code[p]-'0');
		}

		p++;
	}
}



int framebuffer_console_write(const char *buffer, int length) {

	char escape_code[20];

	int i,e;

	for(i=0;i<length;i++) {

		if (buffer[i]=='\r') {
			console_y++;
		} else if (buffer[i]=='\n') {
			console_x=0;
			console_y++;
		} else if (buffer[i]=='\t') {
			console_x=(console_x+8)&(~0x7);
		} else if (buffer[i]=='\b') {
			console_x--;
			if (console_x<0) console_x=0;
		} else if (buffer[i]==27) {
			/* escape */
			i++;
			if (buffer[i]=='[') {
				i++;

				/* Read in the command */
				e=0;

				while (!isalpha(buffer[i])) {
					escape_code[e]=buffer[i];
					e++;
					i++;
				}

				escape_code[e]=buffer[i];
				escape_code[e+1]=0;

//				printk("ESCAPE! %s\n",escape_code);

				/* clear screen */
				if (escape_code[e]=='J') {
					framebuffer_console_clear();
					framebuffer_console_home();
				}
				/* colors */
				if (escape_code[e]=='m') {
//					printk("Parsing %s\n",escape_code);
					ansi_parse_color(escape_code);
				}

			}
		} else {
			text_console[console_x+(console_y*CONSOLE_X)]=buffer[i];
			text_color[console_x+(console_y*CONSOLE_X)]=
				(console_back_color<<4 |
				(console_fore_color&0xf));
			console_x++;
		}

		if (console_x>=CONSOLE_X) {
			console_x=0;
			console_y++;
		}

		if (console_y>=CONSOLE_Y) {
			int i;
			/* scroll up a line */

			framebuffer_clear_screen(0);
			memcpy(&text_console[0],&text_console[CONSOLE_X],
				(CONSOLE_Y-1)*CONSOLE_X*sizeof(unsigned char));
			memcpy(&text_color[0],&text_color[CONSOLE_X],
				(CONSOLE_Y-1)*CONSOLE_X*sizeof(unsigned char));

			for(i=0;i<CONSOLE_X;i++) {
				text_console[i+(CONSOLE_Y-1)*CONSOLE_X]=' ';
				text_color[i+(CONSOLE_Y-1)*CONSOLE_X]=FORE_GREY|BACK_BLACK;

			}
			console_y--;
		}
	}


	framebuffer_console_push();

	return 0;
}

int framebuffer_console_val(int x, int y) {

	if (x>=CONSOLE_X) return -1;
	if (y>=CONSOLE_Y) return -1;

	return text_console[x+(y*CONSOLE_X)];

}

