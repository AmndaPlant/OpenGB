#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "MMU.h"
#include "Cartridge.h"

class GameBoy
{
	private:
		MMU mmu;
	public:
		bool readROM(const char* filename);
		void unloadROM();
};

