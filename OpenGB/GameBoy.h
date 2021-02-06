#pragma once

#include "MMU.h"
#include "Cartridge.h"
#include "CPU.h"
#include "PPU.h"

class GameBoy
{
	public:
		GameBoy();

		MMU mmu;
		CPU cpu;
		PPU ppu;
		Cartridge cart;

		bool readROM(const char* filename);

		void clock();
};

