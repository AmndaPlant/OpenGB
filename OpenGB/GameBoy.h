#pragma once

#include <fstream>
#include <iostream>
#include <vector>

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

		bool readROM(const char* filename);
		void unloadROM();

		void clock();
};

