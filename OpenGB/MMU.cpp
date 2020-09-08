#include "MMU.h"

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
	// TODO interrupt addrs
	else if (addr >= 0xFF00 && addr <= 0xFF7F)
	{
		return io[addr - 0xFF00];
	}	
	else if (addr >= 0xFF80 && addr <= 0xFFFE)
	{
		return hram[addr - 0xFF80];
	}

	return 0; // In case we have an illegal address
}

uint16_t MMU::readShort(uint16_t addr)
{
	return readByte(addr) | (readByte(addr + 1) << 8);
}

void MMU::writeByte(uint16_t addr, uint8_t data)
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
	// TODO interrupt addrs
	else if (addr >= 0xFF00 && addr <= 0xFF7F)
	{
		io[addr - 0xFF00] = data;
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE)
	{
		hram[addr - 0xFF80] = data;
	}
}

void MMU::writeShort(uint16_t addr, uint16_t data)
{
	writeByte(addr, (uint8_t)(data & 0x00FF));
	writeByte(addr + 1, (uint8_t)((data & 0xFF00) >> 8));
}