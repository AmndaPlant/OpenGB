#include "GameBoy.h"

GameBoy::GameBoy()
{
	mmu.connectGB(this);
	cpu.connectGB(this);
	ppu.connectGB(this);
	cart.connectGB(this);
	apu.connectGB(this);

	mmu.writeByte(P1, 0xFF); // Joypad
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
	apu.clock();
}

void GameBoy::key_pressed(int key)
{
	bool prev_unset = false;
	if (test_bit(mmu.get_joypad_state(), key) == false)
	{
		prev_unset = true;
	}

	uint8_t key_req = mmu.readByte(P1);
	mmu.clear_joypad_state(key);
	bool req_int = false;

	if (!(key == KEY_RIGHT || key == KEY_LEFT || key == KEY_UP || key == KEY_DOWN) && !test_bit(key_req, 5))
		req_int = true;
	else if ((key == KEY_RIGHT || key == KEY_LEFT || key == KEY_UP || key == KEY_DOWN) && test_bit(key_req, 4))
		req_int = true;

	if (req_int && prev_unset) cpu.request_interrupt(joypad);
}

void GameBoy::key_released(int key)
{
	mmu.set_joypad_state(key);
}
