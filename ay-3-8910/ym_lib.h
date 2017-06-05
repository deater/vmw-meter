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
	unsigned char *frame_data;
	char song_name[MAX_YM_STRING];
	char author[MAX_YM_STRING];
	char comment[MAX_YM_STRING];
};

int load_ym_song(
        char *filename,
        struct ym_song_t *ym_song );

struct frame_stats {
	int left_a_bar,left_b_bar,left_c_bar;
	int left_a_freq,left_b_freq,left_c_freq;
	int right_a_bar,right_b_bar,right_c_bar;
	int right_a_freq,right_b_freq,right_c_freq;
};


int ym_play_frame(struct ym_song_t *ym_song, int frame_num, int shift_size,
                        struct frame_stats *ds,
			int diff_mode,
			int play_music,
			int mute_channel);


int ym_dump_frame(struct ym_song_t *ym_song, int frame_num,
		int raw, int diff_mode);
