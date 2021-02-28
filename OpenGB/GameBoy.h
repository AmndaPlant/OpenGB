#pragma once

#include "MMU.h"
#include "Cartridge.h"
#include "CPU.h"
#include "PPU.h"
#include "APU.h"

class GameBoy
{
	public:
		GameBoy();

		enum
		{
			KEY_RIGHT,
			KEY_LEFT,
			KEY_UP,
			KEY_DOWN,
			KEY_A,
			KEY_B,
			KEY_SELECT,
			KEY_START,
		};

		MMU mmu;
		CPU cpu;
		PPU ppu;
		APU apu;
		Cartridge cart;

		bool readROM(const char* filename);

		void clock();
		void key_pressed(int key);
		void key_released(int key);
};

