#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "MMU.h"
#include "Cartridge.h"
#include "CPU.h"

class GameBoy
{
	public:
		GameBoy();

		MMU mmu;
		CPU cpu;

		bool readROM(const char* filename);
		void unloadROM();
};

