#include <iostream>
#include <chrono>
#include <array>
#include <SDL.h>

#include "GameBoy.h"

const std::array<uint32_t, 4> colors = { 0xFFFFFFFF, 0xC0C0C0FF, 0x606060FF, 0x000000FF };

GameBoy gb;

void check_input(SDL_Event e)
{
	int key = -1;
	if (e.type == SDL_KEYDOWN)
	{
		switch (e.key.keysym.sym)
		{
			case SDLK_z:
				key = GameBoy::KEY_B;
				break;
			case SDLK_x:
				key = GameBoy::KEY_A;
				break;
			case SDLK_RETURN:
				key = GameBoy::KEY_START;
				break;
			case SDLK_RSHIFT:
				key = GameBoy::KEY_SELECT;
				break;
			case SDLK_UP:
				key = GameBoy::KEY_UP;
				break;
			case SDLK_DOWN:
				key = GameBoy::KEY_DOWN;
				break;
			case SDLK_LEFT:
				key = GameBoy::KEY_LEFT;
				break;
			case SDLK_RIGHT:
				key = GameBoy::KEY_RIGHT;
				break;
		}
		if (key != -1) gb.key_pressed(key);
	}
	else if (e.type == SDL_KEYUP)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_z:
			key = GameBoy::KEY_B;
			break;
		case SDLK_x:
			key = GameBoy::KEY_A;
			break;
		case SDLK_RETURN:
			key = GameBoy::KEY_START;
			break;
		case SDLK_RSHIFT:
			key = GameBoy::KEY_SELECT;
			break;
		case SDLK_UP:
			key = GameBoy::KEY_UP;
			break;
		case SDLK_DOWN:
			key = GameBoy::KEY_DOWN;
			break;
		case SDLK_LEFT:
			key = GameBoy::KEY_LEFT;
			break;
		case SDLK_RIGHT:
			key = GameBoy::KEY_RIGHT;
			break;
		}
		if (key != -1) gb.key_released(key);
	}
}

int main(int argc, char **argv)
{

	if (argc != 2)
	{
		std::cerr << "Usage: ./OpenGB <ROM FILE>" << std::endl;
		return 1;
	}

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

	std::string window_title = "OpenGB - ";
	std::string rom_title = argv[1];
	window_title += rom_title;

	SDL_SetWindowTitle(window, window_title.c_str());

	bool running = true;

	while (running)
	{
		int start = SDL_GetTicks();
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			check_input(e);
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

		int time = SDL_GetTicks() - start;
		if (time < 0) continue;
		int sleepTime = (1000.f / 60.f) - time;
		if (sleepTime > 0) SDL_Delay(sleepTime);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}