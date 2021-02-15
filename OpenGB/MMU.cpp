#include "MMU.h"
#include "GameBoy.h"

#include "Util.h"

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
		return gb->cart.read_cart(addr);
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		return vram[addr - 0x8000];
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		return gb->cart.read_cart(addr);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		return wram[addr - 0xC000];
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF)
	{
		return wram[addr - 0xE000];
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F)
	{
		return oam[addr - 0xFE00];
	}
	// TODO Special GPU addrs
	// TODO various IO mem addrs
	else if (addr == 0xFF00) return joypad_mem_state();
	else if (addr >= 0xFF00 && addr <= 0xFF7F)
	{
		return io[addr - 0xFF00];
	}	
	else if (addr >= 0xFF80 && addr <= 0xFFFF)
	{
		return hram[addr - 0xFF80];
	}

	return 0xFF; // In case we have an illegal address
}

uint16_t MMU::readShort(uint16_t addr)
{
	return readByte(addr) | (readByte(addr + 1) << 8);
}

void MMU::writeByte(uint16_t addr, uint8_t data, bool direct)
{
	if (addr <= 0x7FFF)
	{
		gb->cart.write_cart(addr, data);
	}
	if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		vram[addr - 0x8000] = data;
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		gb->cart.write_cart(addr, data);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		wram[addr - 0xC000] = data;
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF)
	{
		wram[addr - 0xE000] = data;
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F)
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
	else if (addr == 0xFF40 && (data & 0x80) == 0)
	{
		io[0x40] = data;
		writeByte(0xFF41, readByte(0xFF41) & 0xFC);
		io[0x44] = 0;
	}
	else if (addr == 0xFF40 && (data & 0x80) == 0x80 && (io[0x40] & 0x80) == 0)
	{
		io[0x40] = data;
		io[0x41] = (io[0x41] & 0xFC) | 0x02;
	}
	else if (addr == 0xFF44 && !direct) // LY
	{
		// Do nothing
		return;
	}
	else if (addr == 0xFF45) // LYC
	{
		io[0x45] = data;
		gb->ppu.check_lyc(); // Force an LY-LYC check
	}
	else if (addr == 0xFF46) // OAM DMA
	{
		uint16_t dma_addr = data * 0x100;
		for (int i = 0; i < 0xA0; ++i)
		{
			oam[i] = readByte(dma_addr + i);
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

uint8_t MMU::get_joypad_state()
{
	return joypad_state;
}

void MMU::set_joypad_state(const uint8_t key)
{
	set_bit(joypad_state, key);
}

void MMU::clear_joypad_state(const uint8_t key)
{
	clear_bit(joypad_state, key);
}

uint8_t MMU::joypad_mem_state()
{
	uint8_t res = io[0x00];

	res ^= 0xFF; // Flip all bits

	// Buttons
	if (!(test_bit(res, 4)))
	{
		uint8_t top_joypad = joypad_state >> 4;
		top_joypad |= 0xF0; // Set top nibble to 1s
		res &= top_joypad;
	}
	else if (!(test_bit(res, 5)))
	{
		uint8_t bottom_joypad = joypad_state & 0x0F;
		bottom_joypad |= 0xF0;
		res &= bottom_joypad;
	}
	return res;
}