#include <iostream>
#include <chrono>
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
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_CreateWindowAndRenderer(320, 288, SDL_WINDOW_SHOWN, &window, &renderer);
	SDL_SetWindowTitle(window, "OpenGB");
    SDL_SetWindowResizable(window, SDL_TRUE);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, 160, 144);

	if (!window)
	{
		std::cout << "Failed to create window" << std::endl;
		std::cout << "SDL2 Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	gb.ppu.setRenderer(renderer);

	bool running = true;

	float timeRemaining = 0.0f;

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

		do { gb.clock(); } while (!gb.cpu.frame_complete);
		gb.cpu.frame_complete = false;
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}