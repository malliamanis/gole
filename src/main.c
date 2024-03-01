#include <stdio.h>
#include <stdlib.h>

#include "gole.h"

#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 200
#define DEFAULT_SCALE 3

int main(int argc, char **argv)
{
	if (argc < 4) {
		printf("not enough arguments, using default settings\n");
		gole_run(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_SCALE);
	}
	else
		gole_run(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

	return 0;
}
