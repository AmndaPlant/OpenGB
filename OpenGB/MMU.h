#pragma once

#include <stdint.h>

class MMU {
	private:
		uint8_t cart[0x8000] = { 0 };
		uint8_t sram[0x2000] = { 0 };
		uint8_t io[0x100] = { 0 };
		uint8_t vram[0x2000] = { 0 };
		uint8_t oam[0x100] = { 0 };
		uint8_t wram[0x2000] = { 0 };
		uint8_t hram[0x80] = { 0 };

	public:
		void copy(uint16_t source, uint16_t dest, size_t length);

		uint8_t readByte(uint16_t addr);
		uint16_t readShort(uint16_t addr);

		void writeByte(uint16_t addr, uint8_t data);
		void writeShort(uint16_t addr, uint16_t data);
};

