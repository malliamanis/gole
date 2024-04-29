#include <SDL2/SDL.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "gole.h"

#define TITLE "GOLe"

#define WHITE 0xFFFFFFFF
#define BLACK 0

void gole_run(uint32_t width, uint32_t height, uint32_t scale)
{
	// init

	srand(time(NULL));

	const uint32_t size = width * height;

	bool quit = false;
	bool paused = true;
	bool status_changed = true;

	// need two so that it can apply the rule while reading [cells] and modifying [pixels]
	uint32_t *cells = calloc(size, sizeof *cells);
	uint32_t *pixels = calloc(size, sizeof *pixels);

	bool modified = false;

	SDL_Init(SDL_INIT_VIDEO);


	SDL_Window *window =
		SDL_CreateWindow(
			TITLE,
			SDL_WINDOWPOS_CENTERED_DISPLAY(0),
			SDL_WINDOWPOS_CENTERED_DISPLAY(0),
			width * scale,
			height * scale,
			SDL_WINDOW_ALLOW_HIGHDPI
		);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	// SDL_RenderSetScale(renderer, scale, scale);

	SDL_Texture *screen =
		SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			width,
			height
		);


	uint64_t ticks = 25;
	uint64_t delta_time = 1000 / ticks;

	uint64_t current_time = SDL_GetTicks64();
	uint64_t new_time;
	uint64_t accumulator = 0;

	while (!quit) {
		// update

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = true;
				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode) {
						case SDL_SCANCODE_ESCAPE:
							quit = true;
							break;
						case SDL_SCANCODE_SPACE:
							status_changed = true;
							paused = !paused;
							break;
						case SDL_SCANCODE_UP:
							status_changed = true;

							ticks += 1;
							delta_time = 1000 / ticks;
							break;
						case SDL_SCANCODE_DOWN:
							status_changed = true;

							if (ticks > 1)
								ticks -= 1;
							delta_time = 1000 / ticks;
							break;
						case SDL_SCANCODE_R:
							memset(pixels, BLACK, size * sizeof(uint32_t));

							for (int i = 0; i < size; ++i) {
								if (rand() % 4 == 0)
									pixels[i] = WHITE;
							}

							modified = true;
							break;
						case SDL_SCANCODE_C:
							memset(pixels, BLACK, size * sizeof(uint32_t));

							modified = true;
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					{
						int32_t x, y;

						SDL_GetMouseState(&x, &y);

						if (x <= 0 || x >= width * scale || y <= 0 || y >= height * scale) {
							printf("%d %d\n", x, y);

							break;
						}

						uint32_t coords = (x / scale) + (y / scale) * width;

						if (pixels[coords] == BLACK)
							pixels[coords] = WHITE;
						else
							pixels[coords] = BLACK;

						modified = true;
					}

					break;
			}
		}

		new_time = SDL_GetTicks64();

		accumulator += new_time - current_time;
		current_time = new_time;

		while (accumulator >= delta_time) {
			accumulator -= delta_time;

			if (status_changed) {
				printf("\33[2K\r");
				if (paused)
					printf("PAUSED");
				else
					printf("RUNNING");
				printf(" | TICKS PER SECOND: %lu", ticks);

				fflush(stdout);

				status_changed = false;
			}

			if (paused)
				continue;

			// tick

			memcpy(cells, pixels, size * sizeof(uint32_t));

			modified = true;

			// apply rule for each cell
			for (uint32_t y = 0; y < height; ++y) {
				for (uint32_t x = 0; x < width; ++x) {
					int pixels_index = x + y * width;

					// if it's on the screen border then it can't get the 3x3 region around the cell
					if (x <= 0 || y <= 0 || x == width - 1 || y == height - 1) {
						pixels[pixels_index] = BLACK;
						continue;
					}

					uint32_t live_neighbours = 0;
					// loop through the cells in the 3x3 region around the cell at [i]
					for (uint32_t k = y - 1; k <= y + 1; ++k) {
						for (uint32_t j = x - 1; j <= x + 1; ++j) {
							// center cell in 3x3 region
							if (j == x && k == y)
								continue;

							if (cells[j + k * width] == WHITE)
								++live_neighbours;
						}
					}

					// game of life rules
					if (live_neighbours < 2 || live_neighbours > 3)
						pixels[pixels_index] = BLACK;
					else if (live_neighbours == 3)
						pixels[pixels_index] = WHITE;
				}
			}
		}

		// render

		if (modified) {
			modified = false;
			SDL_UpdateTexture(screen, NULL, pixels, width * (sizeof *pixels));
		}

		SDL_RenderCopyEx(renderer, screen, NULL, NULL, 0.0, NULL, 0);
		SDL_RenderPresent(renderer);
	}

	
	// quit

	printf("\n");

	SDL_DestroyTexture(screen);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	free(pixels);

	SDL_Quit();
}
