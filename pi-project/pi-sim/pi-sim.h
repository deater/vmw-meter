extern unsigned char display[640*480*3];
extern unsigned char red[640*480];
extern unsigned char blue[640*480];
extern unsigned char green[640*480];

int pisim_update(void);
int pisim_init(void);
int pisim_input(void);

void apple2_plot(int x, int y, int color);
