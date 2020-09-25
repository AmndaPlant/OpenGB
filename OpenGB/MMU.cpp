#include "MMU.h"
#include "GameBoy.h"

void MMU::copy(uint16_t source, uint16_t dest, size_t length) 
{
	for (int i = 0; i < length; ++i)
	{
		writeByte(dest + i, readByte(source + i));
	}
}

uint8_t MMU::readByte(uint16_t addr)
{
	if (addr <= 0x7FFF)
	{
		return cart[addr];
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		return vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		return sram[addr - 0xA000];
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		return wram[addr - 0xC000];
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF)
	{
		return wram[addr - 0xE000];
	}
	else if (addr >= 0xFE00 && addr <= 0xFEFF)
	{
		return oam[addr - 0xFE00];
	}
	// TODO Special GPU addrs
	// TODO various IO mem addrs
	else if (addr >= 0xFF00 && addr <= 0xFF7F)
	{
		return io[addr - 0xFF00];
	}	
	else if (addr >= 0xFF80 && addr <= 0xFFFF)
	{
		return hram[addr - 0xFF80];
	}

	return 0; // In case we have an illegal address
}

uint16_t MMU::readShort(uint16_t addr)
{
	return readByte(addr) | (readByte(addr + 1) << 8);
}

void MMU::writeByte(uint16_t addr, uint8_t data, bool direct)
{
	if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		vram[addr - 0x8000] = data;
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		sram[addr - 0xA000] = data;
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		wram[addr - 0xC000] = data;
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF)
	{
		wram[addr - 0xE000] = data;
	}
	else if (addr >= 0xFE00 && addr <= 0xFEFF)
	{
		oam[addr - 0xFE00] = data;
	}
	// TODO Special GPU addrs
	// TODO various IO mem addrs
	// Logging for blargg's tests as we don't have a GPU
	#ifndef LOGGING
		else if (addr == 0xFF02 && data == 0x81)
		{
			std::cout << static_cast<char>(readByte(0xFF01));
		}
	#endif // LOGGING
	else if (addr == 0xFF04 && !direct) // DIV
	{
		io[0x04] = 0x00;
		gb->cpu.resetTimerDiv();
	}
	else if (addr == 0xFF07) // TAC
	{
		io[0x07] = data;

		switch (data & 0x03)
		{
			case 0:
				gb->cpu.setClockSpeed(1024);
				break;
			case 1:
				gb->cpu.setClockSpeed(16);
				break;
			case 2:
				gb->cpu.setClockSpeed(64);
				break;
			case 3:
				gb->cpu.setClockSpeed(256);
				break;
			default: break;
		}
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F)
	{
		io[addr - 0xFF00] = data;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFF)
	{
		hram[addr - 0xFF80] = data;
	}
}

void MMU::writeShort(uint16_t addr, uint16_t data, bool direct)
{
	writeByte(addr, (uint8_t)(data & 0x00FF), direct);
	writeByte(addr + 1, (uint8_t)((data & 0xFF00) >> 8), direct);
}

void MMU::set_rom(uint8_t *rom_data)
{
	std::memcpy(cart, rom_data, sizeof(cart));
}