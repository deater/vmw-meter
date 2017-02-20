#define DISPLAY_I2C	0x1
#define DISPLAY_TEXT	0x2
#define DISPLAY_BOTH	0x3

#define MODE_TITLE	0x0	/* Prints VMW CHIPTUNE */
#define MODE_VISUAL	0x1	/* Waterfall visualization */
#define MODE_NAME	0x2	/* Scrolls filename */
#define MODE_TIME	0x3	/* Shows elapsed time */
#define MODE_VOLUME	0x4	/* Allows setting the volume */
#define MODE_MAX	0x5

#define CMD_PAUSE		0x1
#define CMD_BACK		0x2
#define CMD_FWD			0x3
#define CMD_EXIT_PROGRAM	0x4
#define CMD_FF			0x5
#define CMD_RW			0x6
#define CMD_LOOP		0x7

struct display_stats {
        int a_bar,b_bar,c_bar;
        int a_freq,b_freq,c_freq;
};


int display_init(int type);
int display_update(int type,
			struct display_stats *ds,
			int current_frame, int num_frames,
			char *filename, int new_filename);
int display_shutdown(int type);

