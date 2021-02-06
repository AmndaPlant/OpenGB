#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

#include "Cartridge.h"

bool Cartridge::load_cart(const char* filename)
{
	std::string name;
	int rom_size;
	int ram_size;

	std::ifstream stream(filename, std::ios::binary | std::ios::ate);

	if (!stream.good())
	{
		std::cerr << "Can't open file " << filename << std::endl;
		stream.close();
		return false;
	}

	std::ifstream::pos_type position = stream.tellg();
	size_t filesize = static_cast<size_t>(position);

	if (filesize < 0x180)
	{
		std::cerr << "File too small" << std::endl;
		stream.close();
		return false;
	}

	unsigned char* file_contents = new unsigned char[filesize];

	stream.seekg(0, std::ios::beg);
	stream.read((char*)file_contents, position);
	stream.close();

	for (int i = ROM_OFFSET_NAME; i < ROM_OFFSET_NAME + 16; ++i)
	{
		name += file_contents[i];
	}

	std::cout << "Internal Title: " << name << std::endl;

	type = static_cast<rom_type>(file_contents[ROM_OFFSET_TYPE]);

	auto it = rom_type_names.find(type);

	if (it == rom_type_names.end())
	{
		std::cerr << "Invalid ROM type" << std::endl;
		return false;
	}

	std::cout << "ROM Type: " << it->second << std::endl;

	if (type != ROM_PLAIN && 
		type != ROM_MBC1 &&
		type != ROM_MBC1_RAM &&
		type != ROM_MBC1_RAM_BATT)
	{
		std::cerr << "Unsupported ROM Type!" << std::endl;
		return false;
	}

	rom_size = file_contents[ROM_OFFSET_ROM_SIZE];

	if ((rom_size & 0xF0) == 0x50) rom_size = (int)pow(2.0, (double)(((0x52) & 0xF) + 1)) + 64;
	else rom_size = (int)pow(2.0, (double)(rom_size + 1));

	std::cout << "ROM Size: " << rom_size * 16 << "K" << std::endl;

	if (filesize != rom_size * 16 * 1024)
	{
		std::cerr << "Filesize does not equal ROM size" << std::endl;
		return false;
	}

	ram_size = file_contents[ROM_OFFSET_RAM_SIZE];
	ram_size = (int)pow(4.0, (double)(ram_size)) / 2;

	std::cout << "RAM Size: " << ram_size << "K" << std::endl;

	ram_size *= 1024;

	rom = new uint8_t[filesize];
	ram = new uint8_t[ram_size];

	std::memcpy(rom, file_contents, filesize);

	return 1;
}

uint8_t Cartridge::read_cart(uint16_t addr)
{
	switch (type)
	{
		case ROM_PLAIN:
			if (addr <= 0x7FFF) return rom[addr];
			else if (addr >= 0xA000 && addr <= 0xBFFF) return ram[addr - 0xA000];
			else return 0xFF;
			break;
		case ROM_MBC1:
		case ROM_MBC1_RAM:
		case ROM_MBC1_RAM_BATT:
		{
			if (addr <= 0x3FFF) return rom[addr];
			else if (addr >= 0x4000 && addr <= 0x7FFF)
			{
				uint32_t target = (rom_bank * 0x4000) + (addr - 0x4000);
				return rom[target];
			}
			else if (addr >= 0xA000 && addr <= 0xBFFF && ram_enabled)
			{
				uint32_t target = (ram_bank * 0x2000) + (addr - 0x2000);
				return ram[target];
			}
			else return 0xFF;
			break;
		}
		default:
			return 0xFF;
			break;
	}
}

void Cartridge::write_cart(uint16_t addr, uint8_t data)
{
	switch (type)
	{
		case ROM_PLAIN:
			if (addr >= 0xA000 && addr <= 0xBFFF) ram[addr - 0xA000] = data;
			break;
		case ROM_MBC1:
		case ROM_MBC1_RAM:
		case ROM_MBC1_RAM_BATT:
			if (addr <= 0x1FFF)
			{
				ram_enabled = data > 0;
			}
			else if (addr <= 0x3FFF)
			{
				rom_bank = data & 0x1F;
				if (data == 0x00 || data == 0x20 || data == 0x40 || data == 0x60)
					rom_bank = (data & 0x1F) + 1;
			}
			else if (addr <= 0x5FFF)
			{
				if (rom_mode == 0) rom_bank |= (data & 3) << 5;
				else ram_bank = data & 3;
			}
			else if (addr <= 0x8000)
				rom_mode = data > 0;
			else if (addr >= 0xA000 && addr <= 0xBFFF && ram_enabled) 
			{
				uint32_t target = (ram_bank * 0x2000) + (addr - 0x2000);
				ram[target] = data;
			}
			break;
		default:
			break;
	}
}