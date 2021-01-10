#include <iostream>
#include <chrono>
#include <array>
#include <SDL.h>

#include "GameBoy.h"

const std::array<uint32_t, 4> colors = { 0xFFFFFFFF, 0xC0C0C0FF, 0x606060FF, 0x000000FF };

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
	SDL_CreateWindowAndRenderer(160 * 4, 144 * 4, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160 , 144);

	std::array<uint32_t, 160 * 144> pixels;

	if (!window)
	{
		std::cout << "Failed to create window" << std::endl;
		std::cout << "SDL2 Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_SetWindowTitle(window, "OpenGB");

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

		// Generate frame texture and draw it
		const std::array<uint8_t, 160 * 144> &lcd = gb.ppu.get_lcd();
		for (int i = 0; i < 160 * 144; ++i)
		{
			//std::cout << (int)lcd[i] << std::endl;
			pixels[i] = colors[lcd[i]];
		}
		SDL_UpdateTexture(texture, nullptr, &pixels[0], 160 * 4);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}