struct pattern_element_struct {
	unsigned char note,instrument,volume,effect,param;
};

struct pattern_struct {
	int num_rows;
	int packing_type;
	struct pattern_element_struct p[256][4];
};

// FIXME: bigger channels, up to 16 or more?
// Or calloc it based on num_channels?

struct xm_info_struct {
	char module_name[21];
	char tracker_name[21];
	int version_high,version_low;
	int song_length;
	int restart_position;
	int number_of_channels;
	int number_of_patterns;
	int number_of_instruments;
	int flags;
	int default_tempo;
	int default_bpm;
	unsigned char pattern_order[256];
	struct pattern_struct pattern[256];
};

int dump_xm_file(struct xm_info_struct *xm);
int load_xm_file(char *filename, struct xm_info_struct *xm);
int xm_to_text(FILE *fff, struct xm_info_struct *xm);
