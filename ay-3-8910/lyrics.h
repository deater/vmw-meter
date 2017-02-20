#define MAX_LYRICS	2048

struct lyric_line {
	int frame;
	char *text;
};

struct lyric_type {
	int num;
	struct lyric_line *l;
};

int load_lyrics(char *filename, struct lyric_type *l);
int destroy_lyrics(struct lyric_type *l);
