#define MAX_YM_STRING	256

struct ym_song_t {
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
        int data_begin;
        char song_name[MAX_YM_STRING];
        char author[MAX_YM_STRING];
        char comment[MAX_YM_STRING];
};

int load_ym_song(
        char *filename,
        struct ym_song_t *ym_song );
