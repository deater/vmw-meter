#define COLOR_BLACK	0
#define COLOR_WHITE	1
#define COLOR_RED	2
#define COLOR_ORANGE	3
#define COLOR_YELLOW	4
#define COLOR_GREEN	5
#define COLOR_BLUE	6
#define COLOR_INDIGO	7
#define COLOR_VIOLET	8
#define COLOR_CYAN	9

#define MAX_COLORS 10

extern char color_names[MAX_COLORS][15];

struct color_type {
	int r;
	int g;
	int b;
};

extern struct color_type colors[MAX_COLORS];

int get_color(char *name,int *r,int *g,int *b);

int get_random_color_noblack(void);

