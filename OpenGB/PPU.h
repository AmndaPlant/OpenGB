#pragma once

#include <cstdint>
#include <array>

#include "Util.h"

#define HBLANK 0
#define VBLANK 1
#define OAM_READ 2
#define VRAM_READ 3

#define HBLANK_CYCLES 204
#define VBLANK_CYCLES 456
#define OAM_READ_CYCLES 80
#define VRAM_READ_CYCLES 172

#define VRAM_ADDR 0x8000
#define OAM 0xFE00
#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define BGP 0xFF47
#define OBP0 0xFF48
#define OBP1 0xFF49
#define WY 0xFF4A
#define WX 0xFF4B

#define LCD_WIDTH 160
#define LCD_HEIGHT 144
#define MAX_SCANLINES 154

class GameBoy;

class PPU
{
	private:
		GameBoy *gb = nullptr;
		std::array<uint8_t, 160 * 144> lcd;
		std::array<uint8_t, 160 * 144> bg_colours;
		int cycles = 0;

		uint8_t mode;
		uint8_t window_counter;

		void draw(uint8_t scanline);
		void draw_background(uint8_t scanline);
		void draw_window(uint8_t scanline);
		void draw_sprites(uint8_t scanline);

	public:
		void clock();
		void check_lyc();

		void connectGB(GameBoy *g) { gb = g; }
		const std::array<uint8_t, 160 * 144> &get_lcd() const { return lcd; }
};

