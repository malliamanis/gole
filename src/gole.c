#include <SDL2/SDL.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "gole.h"

#define TITLE "GOLE"

void gole_run(ui32 width, ui32 height, ui32 scale)
{
	// init

	SDL_Init(SDL_INIT_VIDEO);

	bool quit = false;
	ui32 *pixels = calloc(width * height, sizeof(ui32));

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_CreateWindowAndRenderer(width * scale, height * scale, SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
	SDL_RenderSetScale(renderer, scale, scale);

	SDL_SetWindowTitle(window, TITLE);

	SDL_Texture *screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);


	// loop
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
					if (event.key.keysym.sym == SDLK_ESCAPE)
						quit = true;
					break;
			}
		}


		// render

		memset(pixels, 0, width * height);

		pixels[(width >> 1) + (height >> 1) * width] = 0xFFFFFFFF;

		SDL_UpdateTexture(screen, NULL, pixels, width * scale);
		SDL_RenderCopyEx(renderer, screen, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);

		SDL_RenderPresent(renderer);
	}

	
	// quit

	SDL_DestroyTexture(screen);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	free(pixels);

	SDL_Quit();
}
