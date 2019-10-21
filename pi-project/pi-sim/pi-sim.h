#define XSIZE	640
#define YSIZE	480

extern unsigned char display[640*480*3];

extern unsigned char red_palette[256];
extern unsigned char blue_palette[256];
extern unsigned char green_palette[256];

int pisim_update(unsigned char *buffer);
int pisim_init(void);
int pisim_input(void);

void apple2_plot(int x, int y, int color, unsigned char *buffer);

void load_apple2_palette(void);

void hlin(int x1, int x2, int y, int color,unsigned char *buffer);
void plot(int x,int y, int color, unsigned char *buffer);
void vlin(int y1, int y2, int x, int color, unsigned char *buffer);

#define APPLE2_COLOR_BLACK		0
#define APPLE2_COLOR_DARKBLUE		2
#define APPLE2_COLOR_MEDIUMBLUE		9
#define APPLE2_COLOR_WHITE		15

void vmwTextXY(char *string,int x,int y,int color,int background,int overwrite,
        unsigned char font[256][16], unsigned char *buffer);
void vmwTextXYx2(char *string,int x,int y,int color,int background,int overwrite,
        unsigned char font[256][16], unsigned char *buffer);
int put_char(unsigned char c, int x, int y, int fg_color, int bg_color,
        int overwrite, unsigned char font[256][16], unsigned char *buffer);
int print_string(char *string, int x, int y, int color,unsigned char *buffer);

void vmwFadeToBlack(unsigned char *buffer);
