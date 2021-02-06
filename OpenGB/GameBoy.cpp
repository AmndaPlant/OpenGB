#include "GameBoy.h"

GameBoy::GameBoy()
{
	mmu.connectGB(this);
	cpu.connectGB(this);
	ppu.connectGB(this);
	cart.connectGB(this);

	mmu.writeByte(0xFF00, 0xFF); // Joypad
	mmu.writeByte(0xFF05, 0x00); // TIMA
	mmu.writeByte(0xFF06, 0x00); // TMA
	mmu.writeByte(0xFF07, 0x00); // TAC
	mmu.writeByte(0xFF40, 0x91); // LCDC
	mmu.writeByte(0xFF41, 0x81); // STAT
	mmu.writeByte(0xFF44, 0x8F, true); // LY
	mmu.writeByte(0xFF47, 0xFC); // BGP
	mmu.writeByte(0xFF4D, 0xFF); // Apparently some games use this to detect CGB
}

bool GameBoy::readROM(const char* filename)
{
	return cart.load_cart(filename);
}

void GameBoy::clock()
{
	cpu.cpu_step();
	ppu.clock();
}
