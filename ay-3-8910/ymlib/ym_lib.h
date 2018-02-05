#define YM4_HEADER_SIZE 26
#define YM5_HEADER_SIZE 34
#define YM5_FRAME_SIZE  16
#define YM3_FRAME_SIZE  16

#define MAX_YM_STRING	256

struct ym_song_t {
	int channels;
	int type;
	int file_size;
	int num_frames;
	int attributes;
	int interleaved;
	int num_digidrum;
	int drum_size;
	int master_clock;
	int frame_rate;
	int extra_data;
	int loop_frame;
	int frame_size;
	unsigned char *file_data;
	unsigned char *file_data2;
	unsigned char *frame_data;
	unsigned char *frame_data2;
	char song_name[MAX_YM_STRING];
	char author[MAX_YM_STRING];
	char comment[MAX_YM_STRING];
};

int load_ym_song(
        char *filename,
        struct ym_song_t *ym_song );


int ym_play_frame(struct ym_song_t *ym_song, int frame_num, int shift_size,
                        struct display_stats *ds,
			int diff_mode,
			int play_music,
			int mute_channel);


int ym_play_frame_effects(struct ym_song_t *ym_song,
			int frame_num, int shift_size,
			struct display_stats *ds,
			int play_music,
			unsigned char *frame2);

int ym_dump_frame(struct ym_song_t *ym_song, int frame_num,
		int raw, int diff_mode);

int ym_make_frame(struct ym_song_t *ym_song,
			unsigned char *frame_data,
			int frame_num,
			unsigned char *frame);

int ym_dump_frame_raw(struct ym_song_t *ym_song, int frame_num);

int ym_return_frame(struct ym_song_t *ym_song, int frame_num,
        unsigned char *frame,
	unsigned char *frame2);

