#include <SDL2/SDL.h>

#include <SDL2/SDL_mouse.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "gole.h"

#define TITLE "GOLE"

#define WHITE 0xFFFFFFFF
#define BLACK 0

void gole_run(ui32 width, ui32 height, ui32 scale)
{
	srand(time(NULL));

	// init

	SDL_Init(SDL_INIT_VIDEO);

	bool quit = false;
	bool paused = true;

	// need two so that it can apply the rule while reading [cells] and modifying [pixels]
	ui32 *cells = calloc(width * height, sizeof(ui32));
	ui32 *pixels = calloc(width * height, sizeof(ui32));

	bool modified = false;

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_CreateWindowAndRenderer(width * scale, height * scale, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
	SDL_RenderSetScale(renderer, scale, scale);

	SDL_SetWindowTitle(window, TITLE);

	SDL_Texture *screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

	SDL_UpdateTexture(screen, NULL, pixels, width * scale);


	ui64 ticks = 25;
	ui64 delta_time = 1000 / ticks;

	ui64 currentTime = SDL_GetTicks64();
	ui64 newTime;
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
						memset(pixels, BLACK, width * height * sizeof(ui32));

						for (int i = 0; i < width * height; ++i) {
							if (rand() % 5 == 0)
								pixels[i] = WHITE;
						}

						modified = true;
					}
					else if (event.key.keysym.scancode == SDL_SCANCODE_C) {
						memset(pixels, BLACK, width * height * sizeof(ui32));

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

		newTime = SDL_GetTicks64();

		accumulator += newTime - currentTime;
		currentTime = newTime;

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

			memcpy(cells, pixels, width * height * sizeof(ui32));

			modified = true;

			// apply rule for each cell
			for (ui32 i = 0; i < width * height; ++i) {
				i32 y = (int)(i / width);
				i32 x = i - y * width;

				// if it's on the screen border then it can't get the 3x3 region around the cell
				if (x <= 0 || y <= 0 || x == width - 1 || y == height - 1) {
					pixels[i] = BLACK;
					continue;
				}

				ui32 live_neighbours = 0;
				// loop through the cells in the 3x3 region around the cell at [i]
				for (ui32 j = x - 1; j <= x + 1; ++j) {
					for (ui32 k = y - 1; k <= y + 1; ++k) {
						// center cell in 3x3 region
						if (j == x && k == y)
							continue;

						if (cells[j + k * width] == WHITE)
							++live_neighbours;
					}
				}

				// game of life rules
				if (live_neighbours < 2 || live_neighbours > 3)
					pixels[i] = BLACK;
				else if (live_neighbours == 3)
					pixels[i] = WHITE;
			}

		}

		// render

		if (modified) {
			modified = false;

			SDL_UpdateTexture(screen, NULL, pixels, width * scale);
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
