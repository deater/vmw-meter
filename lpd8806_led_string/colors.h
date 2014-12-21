#define COLOR_BLACK	0
#define COLOR_WHITE	1
#define COLOR_RED	2
#define COLOR_ORANGE	3
#define COLOR_YELLOW	4
#define COLOR_GREEN	5
#define COLOR_BLUE	6
#define COLOR_INDIGO	7
#define COLOR_VIOLET	8

#define MAX_COLORS 9

struct color_type {
	int r;
	int g;
	int b;
} colors[MAX_COLORS] = {
	{0,0,0},	/* black */
	{63,63,63},	/* white */
	{63,0,0},	/* red */
	{63,25,0},	/* orange */
	{63,63,0},	/* yellow */
	{0,63,0},	/* green */
	{0,0,63},	/* blue */
	{19,0,32},	/* indigo */
	{25,0,25},	/* violet */

};
