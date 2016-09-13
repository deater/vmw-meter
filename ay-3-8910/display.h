#define DISPLAY_I2C	0x1
#define DISPLAY_TEXT	0x2
#define DISPLAY_BOTH	0x3

#define MODE_TITLE	0x1
#define MODE_VISUAL	0x2
#define MODE_NAME	0x3
#define MODE_TIME	0x4
#define MODE_VOLUME	0x5



int display_init(int type);
int display_update(int type, int a1, int b1, int c1,
				int a2, int b2, int c2);
int display_shutdown(int type);

