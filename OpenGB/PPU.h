#pragma once

#include <cstdint>
#include <SDL.h>

#define HBLANK 0
#define VBLANK 1
#define OAM_READ 2
#define VRAM_READ 3

#define HBLANK_CYCLES 204
#define VBLANK_CYCLES 456
#define OAM_READ_CYCLES 80
#define VRAM_READ_CYCLES 172

class GameBoy;

class PPU
{
	private:
		GameBoy *gb = nullptr;
		SDL_Renderer *renderer;

		uint8_t frame_buffer[160 * 144 * 4] = { 0 };

		int cycles = 0;

		void read_lcd_control();

		void handle_ppu_modes();
		void render_scanline();

		void render_frame();

		uint8_t bg_display_enable;
		uint8_t sprite_display_enable;
		uint8_t sprite_size;
		uint8_t bg_tile_map_region;
		uint8_t bg_and_window_tile_map_region;
		uint8_t window_display_enable;
		uint8_t window_tile_map_region;
		uint8_t lcd_display_enable;

		uint8_t ppu_mode;
		uint8_t scanline;
		uint8_t tile;
		uint8_t colorval;
		uint8_t color;

		const uint8_t COLORS[16] =
		{
			0xFF, 0xFF, 0xFF,
			0xC0, 0xC0, 0xC0,
			0x60, 0x60, 0x60,
			0x00, 0x00, 0x00,
		};

	public:
		void clock();

		void connectGB(GameBoy *g) { gb = g; }
		void setRenderer(SDL_Renderer *r) { renderer = r; }
		uint8_t* get_frame_buffer() { return frame_buffer; }

};

