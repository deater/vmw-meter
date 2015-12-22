#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"

char color_names[MAX_COLORS][15] = {
	{"black"},
	{"white"},
	{"red"},
	{"orange"},
	{"yellow"},
	{"green"},
	{"blue"},
	{"indigo"},
	{"violet"},
	{"cyan"},
	{"purple"},
	{"pink"},
	{"limegreen"},
};

struct color_type colors[MAX_COLORS] = {
	{0,0,0},	/* black */
	{63,63,63},	/* white */
	{63,0,0},	/* red */
	{63,25,0},	/* orange */
	{63,63,0},	/* yellow */
	{0,63,0},	/* green */
	{0,0,63},	/* blue */
	{19,0,32},	/* indigo */
	{25,0,25},	/* violet */
	{0,63,63},	/* cyan */
	{38,0,38},	/* purple */
	{63,10,40},	/* pink */
	{25,30,12},	/* limegreen */
};



int get_color(char *name,int *r,int *g,int *b) {

	int i;

	for(i=0;i<MAX_COLORS;i++) {
		if (!strcmp(name,color_names[i])) {

			*r=colors[i].r;
			*g=colors[i].g;
			*b=colors[i].b;


			return 0;
		}
	}
	*r=10;
	*g=10;
	*b=10;
	printf("Unknown color %s\n",name);

	return 0;
}

int get_random_color_noblack(void) {
	return (random()%(MAX_COLORS-1))+1;
}

