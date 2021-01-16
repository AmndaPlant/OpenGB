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

#define LCDC 0xFF40
#define STAT 0xFF41
#define SCY 0xFF42
#define SCX 0xFF43
#define LY 0xFF44
#define LYC 0xFF45
#define BGP 0xFF47

#define LCD_WIDTH 160
#define LCD_HEIGHT 144
#define MAX_SCANLINES 154

class GameBoy;

class PPU
{
	private:
		GameBoy *gb = nullptr;
		std::array<uint8_t, 160 * 144> lcd;
		int cycles = 0;

		uint8_t mode;

		void draw(uint8_t scanline);
		void draw_background(uint8_t scanline);
		void check_lyc();

	public:
		void clock();

		void connectGB(GameBoy *g) { gb = g; }
		const std::array<uint8_t, 160 * 144> &get_lcd() const { return lcd; }
};

