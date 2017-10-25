#define MODE_TITLE	0x0	/* Prints VMW CHIPTUNE */
#define MODE_VISUAL	0x1	/* Waterfall visualization */
#define MODE_NAME	0x2	/* Scrolls filename */
#define MODE_TIME	0x3	/* Shows elapsed time */
#define MODE_PIANO	0x4	/* Shows note matrix */
#define MODE_NOTEMATRIX	0x5	/* Shows note as being played */
#define MODE_MAX	0x5

int display_8x16_freq(int display_type, struct display_stats *ds);
int display_8x16_time(int display_type, int current_frame, int total_frames);
int display_8x16_title(int display_type);
int display_8x16_scroll_text(int display_type, char *string, int new_string);
int display_8x16_piano(int display_type, struct display_stats *ds);
int display_8x16_notematrix(int display_type, struct display_stats *ds);

int display_update(int type,
			struct display_stats *ds,
			int current_frame, int num_frames,
			char *filename, int new_filename,
			int current_mode);
