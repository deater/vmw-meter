#define FORE_BLACK	0x0
#define FORE_BLUE	0x1
#define FORE_GREEN	0x2
#define FORE_CYAN	0x3
#define FORE_RED	0x4
#define FORE_PURPLE	0x5
#define FORE_BROWN	0x6
#define FORE_GREY	0x7
#define FORE_DGREY	0x8
#define FORE_LBLUE	0x9
#define FORE_LGREEN	0xa
#define FORE_LCYAN	0xb
#define FORE_LRED	0xc
#define FORE_PINK	0xd
#define FORE_YELLOW	0xe
#define FORE_WHITE	0xf

#define BACK_BLACK	0
#define BACK_BLUE	1
#define BACK_GREEN	2
#define BACK_CYAN	3
#define BACK_RED	4
#define BACK_PURPLE	5
#define BACK_BROWN	6
#define BACK_GREY	7



int framebuffer_console_putchar(int fore_color, int back_color,
			int ch, int x, int y);
int framebuffer_console_write(const char *buffer, int length);

int framebuffer_console_init(void);
int framebuffer_console_clear(void);
int framebuffer_console_home(void);

void framebuffer_console_setfont(int which);
int framebuffer_console_push(void);
int framebuffer_tb1(void);
