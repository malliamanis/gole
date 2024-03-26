#include <SDL2/SDL.h>

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "gole.h"

#define TITLE "GOLe"

#define WHITE 0xFFFFFFFF
#define BLACK 0

void gole_run(ui32 width, ui32 height, ui32 scale)
{
	// init

	srand(time(NULL));

	const ui32 size = width * height;

	bool quit = false;
	bool paused = true;

	// need two so that it can apply the rule while reading [cells] and modifying [pixels]
	ui32 *cells = calloc(size, sizeof(ui32));
	ui32 *pixels = calloc(size, sizeof(ui32));

	bool modified = false;

	SDL_Init(SDL_INIT_VIDEO);


	SDL_Window *window =
		SDL_CreateWindow(
			TITLE,
			SDL_WINDOWPOS_CENTERED_DISPLAY(0),
			SDL_WINDOWPOS_CENTERED_DISPLAY(0),
			width * scale,
			height * scale,
			SDL_WINDOW_ALLOW_HIGHDPI);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	// SDL_RenderSetScale(renderer, scale, scale);

	SDL_Texture *screen =
		SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			width,
			height);


	ui64 ticks = 25;
	ui64 delta_time = 1000 / ticks;

	ui64 current_time = SDL_GetTicks64();
	ui64 new_time;
	ui64 accumulator = 0;

	while (!quit) {
		// update

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					quit = true;
				case SDL_KEYDOWN:
					if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
						ticks += 1;
						delta_time = 1000 / ticks;
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
						if (ticks > 1)
							ticks -= 1;
						delta_time = 1000 / ticks;
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_R) {
						memset(pixels, BLACK, size * sizeof(ui32));

						for (int i = 0; i < size; ++i) {
							if (rand() % 3 == 0)
								pixels[i] = WHITE;
						}

						modified = true;
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_C) {
						memset(pixels, BLACK, size * sizeof(ui32));

						modified = true;
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
						paused = !paused;
					else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
						quit = true;

					break;
				case SDL_MOUSEBUTTONDOWN:
					{
						i32 x, y;

						SDL_GetMouseState(&x, &y);

						if (x <= 0 || x >= width * scale || y <= 0 || y >= height * scale) {
							printf("%d %d\n", x, y);

							break;
						}

						ui32 coords = (x / scale) + (y / scale) * width;

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

			printf("\n");
			printf("TICKS PER SECOND: %lu\n", ticks);
			if (paused) {
				printf("PAUSED\n");
				continue;
			}
			else
				printf("RUNNING\n");

			// tick

			memcpy(cells, pixels, size * sizeof(ui32));

			modified = true;

			// apply rule for each cell
			for (ui32 y = 0; y < height; ++y) {
				for (ui32 x = 0; x < width; ++x) {
					int pixels_index = x + y * width;

					// if it's on the screen border then it can't get the 3x3 region around the cell
					if (x <= 0 || y <= 0 || x == width - 1 || y == height - 1) {
						pixels[pixels_index] = BLACK;
						continue;
					}

					ui32 live_neighbours = 0;
					// loop through the cells in the 3x3 region around the cell at [i]
					for (ui32 k = y - 1; k <= y + 1; ++k) {
						for (ui32 j = x - 1; j <= x + 1; ++j) {
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

	SDL_DestroyTexture(screen);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	free(pixels);

	SDL_Quit();
}
