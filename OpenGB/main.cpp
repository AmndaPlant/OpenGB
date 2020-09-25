#include <iostream>
#include <SDL.h>

#include "GameBoy.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./OpenGB <ROM FILE>" << std::endl;
		return 1;
	}

	GameBoy gb;
	if (gb.readROM(argv[1]))
	{
		std::cout << "Succesfully loaded ROM!" << std::endl;
	}
	else
	{
		return 1;
	}

	// Create SDL Window
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Failed to initialize SDL" << std::endl;
		std::cout << "SDL2 Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_CreateWindowAndRenderer(160, 144, SDL_WINDOW_SHOWN, &window, &renderer);
	SDL_SetWindowTitle(window, "OpenGB");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_SetWindowResizable(window, SDL_TRUE);

	if (!window)
	{
		std::cout << "Failed to create window" << std::endl;
		std::cout << "SDL2 Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Texture* screen = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		256, 240);

	bool running = true;

	while (running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
					running = false;
					break;
			}
		}
		gb.cpu.cpu_frame();

		SDL_RenderCopy(renderer, screen, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(screen);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}