#define DISPLAY_LINES 8

void shutdown_display(void);
void reset_display(unsigned short *display_state);
int update_display(unsigned char *display_state);
int init_display(char *device,int brightness);
int set_brightness(int value);
long long read_keypad(void);
int update_display_raw(unsigned short *display_state);
