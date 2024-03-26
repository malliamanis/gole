#include <stdio.h>
#include <stdlib.h>

#include "gole.h"

#define DEFAULT_SCALE 5
#define DEFAULT_WIDTH (1280 / DEFAULT_SCALE)
#define DEFAULT_HEIGHT (DEFAULT_WIDTH * 9 / 16)

int main(int argc, char **argv)
{
	int width, height, scale;

	if (argc < 4) {
		printf("not enough arguments, using default settings\n");

		width = DEFAULT_WIDTH;
		height = DEFAULT_HEIGHT;
		scale = DEFAULT_SCALE;
	}
	else {
		scale = atoi(argv[3]);
		width = atoi(argv[1]) / scale;
		height = atoi(argv[2]) / scale;
	}

	gole_run(width, height, scale);

	return 0;
}
