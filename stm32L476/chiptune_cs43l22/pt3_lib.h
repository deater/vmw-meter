#define PT3_HEADER_SIZE 0xCB
#define MAX_PT3_SIZE	65536
#define MAX_PT3_STRING	256


struct pt3_note_type {
	signed char which;
	unsigned char note;
	unsigned char original_note;
	unsigned char sample;
	unsigned char ornament;
	unsigned char volume;
	unsigned char spec_command;
	unsigned char spec_delay;
	unsigned char spec_hi;
	unsigned char spec_lo;

	unsigned char len;
	unsigned char len_count;

	unsigned char all_done;

	unsigned short ornament_pointer;
	unsigned char ornament_length;
	unsigned char ornament_loop;
	unsigned char ornament_position;

	unsigned short sample_pointer;
        unsigned char sample_length;
        unsigned char sample_loop;
        unsigned char sample_position;

        unsigned char envelope_enabled;

	signed char amplitude;
        signed char amplitude_sliding;
        signed char noise_sliding;
        signed char envelope_sliding;

        signed char tone_slide_count;
        signed short tone_sliding;		// error if less
	signed short tone_slide_step;
	signed char tone_slide_delay;
	signed short tone_delta;
	signed char slide_to_note;

	unsigned char simplegliss;

        signed short tone_accumulator;
        signed char onoff;
	signed char onoff_delay;
	signed char offon_delay;
	signed short tone;
	signed char enabled;

	unsigned char new_note;

	unsigned short addr;

};

struct pt3_song_t {
	int valid;

	char magic[13+1];
	char version;
	char name[32+1];
	char author[32+1];
	char which_frequency_table;
	char speed;
	char num_patterns;
	char loop;
	unsigned short pattern_loc;
	unsigned short sample_patterns[32];
	unsigned short ornament_patterns[16];
	unsigned short pattern_order;
	struct pt3_note_type a,b,c;
	struct pt3_note_type a_old,b_old,c_old;
	unsigned char music_len;
	unsigned char current_pattern;

	unsigned char envelope_type;
	unsigned char envelope_type_old;
	unsigned short envelope_period;
	unsigned short envelope_period_old;
	short envelope_slide;
	short envelope_slide_add;
	unsigned char envelope_add;
	unsigned char envelope_delay;
	unsigned char envelope_delay_orig;
	unsigned char mixer_value;

	unsigned char noise_period;
	unsigned char noise_add;

	unsigned short frequency_table[8*12];

	unsigned char *data;
};

struct pt3_image_t {
	int length;
	unsigned char *data;
};

void dump_header(struct pt3_song_t *pt3);
int pt3_load_song(struct pt3_image_t *pt3_image,
	struct pt3_song_t *pt3, struct pt3_song_t *pt3_2);
int pt3_load_song_from_disk(char *filename,struct pt3_image_t *pt3_image,
	struct pt3_song_t *pt3, struct pt3_song_t *pt3_2);
void pt3_make_frame(struct pt3_song_t *pt3, unsigned char *frame);
void pt3_print_tracker_line(struct pt3_song_t *pt3, int line);
int pt3_decode_line(struct pt3_song_t *pt3);
void pt3_set_pattern(int i, struct pt3_song_t *pt3);
char *pt3_current_note(int which, struct pt3_song_t *pt3);
void pt3_calc_frames(struct pt3_song_t *pt3, int *total, int *loop);
