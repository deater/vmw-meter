#define DISPLAY_I2C	0x1
#define DISPLAY_TEXT	0x2
#define DISPLAY_BOTH	0x3

int bargraph(int type, int a, int b, int c);
int display_init(int type);
int freq_display(int type, int a, int b, int c);

int close_freq_display(int type);
int close_bargraph(int type);
