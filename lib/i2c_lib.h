#define DISPLAY_LINES 8

#define HT16K33_ADDRESS1	0x70
#define HT16K33_ADDRESS2	0x71
#define WII_NUNCHUCK_ADDRESS	0x52

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
int init_display(int i2c_fd, int i2c_addr, int brightness);
int shutdown_display(int i2c_fd);
void reset_display(unsigned short *display_state);
int set_brightness(int i2c_fd, int i2c_addr,int value);
long long read_keypad(int i2c_fd, int i2c_addr);

int update_8x8_display_rotated(int i2c_fd, int i2c_addr, unsigned char *display_state,int degrees);

int update_display(int i2c_fd, int i2c_addr, unsigned short *display_state);

int init_nunchuck(int i2c_fd);
int read_nunchuck(int i2c_fd, struct nunchuck_data *data);
