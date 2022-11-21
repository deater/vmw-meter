#define XSIZE	640
#define YSIZE	480

//extern unsigned char display[640*480];

extern unsigned char red_palette[256];
extern unsigned char blue_palette[256];
extern unsigned char green_palette[256];

int pisim_update(unsigned char *buffer);
int pisim_init(void);
int pisim_input(void);

/* Font Routines */
void vmwTextXY(char *string,int x,int y,int color,int background,int overwrite,
        unsigned char font[256][16], unsigned char *buffer);
void vmwTextXYx2(char *string,int x,int y,int color,int background,int overwrite,
        unsigned char font[256][16], unsigned char *buffer);
int put_char(unsigned char c, int x, int y, int fg_color, int bg_color,
        int overwrite, unsigned char font[256][16], unsigned char *buffer);
int print_string(char *string, int x, int y, int color,unsigned char *buffer);

/* Palette */
void vmwFadeToBlack(unsigned char *buffer);
void vmwFadeFromBlack(unsigned char *buffer, unsigned char *pal);
void vmwSetAllBlackPalette(void);

/* Clear Screen */
void vmwClearScreen(int color, unsigned char *buffer);

/* Line Drawing */
void vmwHlin(int x1, int x2, int y, int color,unsigned char *buffer);
void vmwPlot(int x,int y, int color, unsigned char *buffer);
void vmwVlin(int y1, int y2, int x, int color, unsigned char *buffer);


/* Apple2 Compatible */
void apple2_plot(int x, int y, int color, unsigned char *buffer);
void apple2_load_palette(void);

#define APPLE2_COLOR_BLACK		0
#define APPLE2_COLOR_DARKBLUE		2
#define APPLE2_COLOR_MEDIUMBLUE		9
#define APPLE2_COLOR_WHITE		15
