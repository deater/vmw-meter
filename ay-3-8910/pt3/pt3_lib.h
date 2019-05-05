#define PT3_HEADER_SIZE 0xCB
#define MAX_PT3_SIZE	65536
#define MAX_PT3_STRING	256


struct pt3_note_type {
	char which;
	int note;
	int sample;
	int ornament;
	int volume;
	int spec_command;
	int spec_delay;
	int spec_hi;
	int spec_lo;

	int len;
	int len_count;

	int all_done;

        int ornament_pointer;
        int ornament_length;
        int ornament_loop;
        int ornament_position;

        int sample_pointer;
        int sample_length;
        int sample_loop;
        int sample_position;

        int envelope_enabled;

	int amplitude;
        int amplitude_sliding;
        int noise_sliding;
        int envelope_sliding;

        int tone_slide_count;
        int tone_sliding;
	int tone_slide_step;
	int tone_slide_delay;
	int tone_delta;
	int slide_to_note;

	int simplegliss;

        int tone_accumulator;
        int onoff;
	int onoff_delay;
	int offon_delay;
	int tone;
        int enabled;
};

struct pt3_song_t {
	char magic[13+1];
	char version;
	char name[32+1];
	char author[32+1];
	int frequency_table;
	int speed;
	int num_patterns;
	int loop;
	unsigned short pattern_loc;
	unsigned short sample_patterns[32];
	unsigned short ornament_patterns[16];
	unsigned short pattern_order;
	unsigned short a_addr,b_addr,c_addr;
	struct pt3_note_type a,b,c;
	struct pt3_note_type a_old,b_old,c_old;
	int music_len;
	int current_pattern;

	int envelope_type;
	int envelope_type_old;
	int envelope_period;
	int envelope_period_old;
	int envelope_slide;
	int envelope_slide_add;
	int envelope_add;
	int envelope_delay;
	int envelope_delay_orig;
	int mixer_value;

	int noise_period;
	int noise_add;

	unsigned char data[MAX_PT3_SIZE];
};

void dump_header(struct pt3_song_t *pt3);
int pt3_load_song(char *filename, struct pt3_song_t *pt3);
void pt3_make_frame(struct pt3_song_t *pt3, unsigned char *frame);
void pt3_print_tracker_line(struct pt3_song_t *pt3, int line);
int pt3_decode_line(struct pt3_song_t *pt3);
void pt3_set_pattern(int i, struct pt3_song_t *pt3);
char *pt3_current_note(int which, struct pt3_song_t *pt3);
