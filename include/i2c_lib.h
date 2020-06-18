#define DISPLAY_LINES 8

#define HT16K33_ADDRESS0	0x70
#define HT16K33_ADDRESS1	0x71
#define HT16K33_ADDRESS2	0x72
#define HT16K33_ADDRESS3	0x73
#define HT16K33_ADDRESS4	0x74
#define HT16K33_ADDRESS5	0x75
#define HT16K33_ADDRESS6	0x76
#define HT16K33_ADDRESS7	0x77


#define WII_NUNCHUCK_ADDRESS	0x52

#define SAA1064_ADDRESS1	0x38
#define SAA1064_ADDRESS2	0x39
#define SAA1064_ADDRESS3	0x3A
#define SAA1064_ADDRESS4	0x3B

#define MCP4725_ADDRESS1	0x62

#define NOT_BROKEN		0
#define BROKEN			1

struct nunchuck_data {
        int joy_x;
        int joy_y;
        int acc_x;
        int acc_y;
        int acc_z;
        int z_pressed;
        int c_pressed;
};

int init_i2c(char *device);

int init_dac(int i2c_fd, int i2c_addr, int powerdown);
int set_dac(int i2c_fd, int i2c_addr, int powerdown, int value);



int init_display(int i2c_fd, int i2c_addr, int brightness);
int shutdown_display(int i2c_fd);
void reset_display(unsigned short *display_state);
int set_brightness(int i2c_fd, int i2c_addr,int value);
long long read_keypad(int i2c_fd, int i2c_addr);

int update_8x8_display_rotated(int i2c_fd, int i2c_addr,
		unsigned char *display_state,int degrees,
		int bug_workaround);

int update_8x8_bicolor_display_rotated(int i2c_fd, int i2c_addr,
		unsigned char *display_state_green,
		unsigned char *display_state_red,
		int degrees);

int update_8x16_display(int i2c_fd, int i2c_addr,
		unsigned short *display_state,int degrees,
		int bug_workaround);


int update_display(int i2c_fd, int i2c_addr, unsigned short *display_state);

int init_nunchuck(int i2c_fd);
int read_nunchuck(int i2c_fd, struct nunchuck_data *data);

int emulate_8x8_display(unsigned char *display_state);
int emulate_8x16_display(unsigned char *display_state);

int emulate_4x7seg_display(unsigned short *display_state);

int init_keyboard(void);
int read_keyboard(void);
int reset_keyboard(void);

#define KEYBOARD_UNKNOWN	0
#define KEYBOARD_UP	1
#define KEYBOARD_DOWN	2
#define KEYBOARD_RIGHT	3
#define KEYBOARD_LEFT	4


int init_saa1064_display(int i2c_fd, int i2c_addr);
int update_saa1064_display(int i2c_fd, int i2c_addr, unsigned short *in_state);
int update_saa1064_ascii(unsigned short *in_state);


#define SAA1064_CONTROL_STATIC    0x01
#define SAA1064_CONTROL_UNBLANK13 0x02
#define SAA1064_CONTROL_UNBLANK24 0x04
#define SAA1064_CONTROL_OUTTEST   0x08
#define SAA1064_CONTROL_PLUS3MA   0x10
#define SAA1064_CONTROL_PLUS6MA   0x20
#define SAA1064_CONTROL_PLUS12MA  0x40

#define SAA1064_SEGMENT_A  0x8000
#define SAA1064_SEGMENT_B  0x4000
#define SAA1064_SEGMENT_C  0x2000
#define SAA1064_SEGMENT_D  0x1000
#define SAA1064_SEGMENT_E  0x0800
#define SAA1064_SEGMENT_F  0x0400
#define SAA1064_SEGMENT_G  0x0200
#define SAA1064_SEGMENT_H  0x0100
#define SAA1064_SEGMENT_J  0x0080
#define SAA1064_SEGMENT_K  0x0040
#define SAA1064_SEGMENT_L  0x0020
#define SAA1064_SEGMENT_M  0x0010
#define SAA1064_SEGMENT_N  0x0008
#define SAA1064_SEGMENT_P  0x0004
#define SAA1064_SEGMENT_DP 0x0002
#define SAA1064_SEGMENT_EX 0x0001

