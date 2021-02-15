#pragma once

#include <cstdint>
#include <iostream>


//	#define LOGGING

#define P1 0xFF00

class GameBoy;

class MMU {
	private:
		uint8_t cart[0x8000] = { 0 };
		uint8_t sram[0x2000] = { 0 };
		uint8_t io[0x100] = { 0 };
		uint8_t vram[0x2000] = { 0 };
		uint8_t oam[0xA0] = { 0 };
		uint8_t wram[0x2000] = { 0 };
		uint8_t hram[0x80] = { 0 };

		GameBoy *gb = nullptr;

		uint8_t joypad_state;

		uint8_t joypad_mem_state();

	public:
		MMU() { joypad_state = 0xFF; }
		void copy(uint16_t source, uint16_t dest, size_t length);

		uint8_t readByte(uint16_t addr);
		uint16_t readShort(uint16_t addr);

		void writeByte(uint16_t addr, uint8_t data, bool direct = false);
		void writeShort(uint16_t addr, uint16_t data, bool direct = false);

		void connectGB(GameBoy *g) { gb = g; }

		uint8_t get_joypad_state();
		void set_joypad_state(const uint8_t key);
		void clear_joypad_state(const uint8_t key);
};

