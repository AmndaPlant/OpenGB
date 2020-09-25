#include "GameBoy.h"

GameBoy::GameBoy()
{
	mmu.connectGB(this);
	cpu.connectGB(this);
	ppu.connectGB(this);
}

bool GameBoy::readROM(const char* filename)
{
	std::string name;
	rom_type type;
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
	stream.read((char*) file_contents, position);
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

	/*if (type != ROM_PLAIN)
	{
		std::cerr << "Only 32k games with no mappers are supported!" << std::endl;
		return false;
	}*/

	rom_size = file_contents[ROM_OFFSET_ROM_SIZE];

	if ((rom_size & 0xF0) == 0x50) rom_size = (int)pow(2.0, (double)(((0x52) & 0xF) + 1)) + 64;
	else rom_size = (int)pow(2.0, (double)(rom_size + 1));

	std::cout << "ROM Size: " << rom_size * 16 << "K" << std::endl;

	if (rom_size * 16 != 32)
	{
		std::cerr << "Only 32k games with no mappers are supported!" << std::endl;
		return false;
	}

	if (filesize != rom_size * 16 * 1024)
	{
		std::cerr << "Filesize does not equal ROM size" << std::endl;
		return false;
	}

	ram_size = file_contents[ROM_OFFSET_RAM_SIZE];
	ram_size = (int)pow(4.0, (double)(ram_size)) / 2;

	std::cout << "RAM Size: " << ram_size << "K" << std::endl;

	ram_size = ceil(ram_size / 8.f);

	for (int i = 0; i < filesize; ++i)
	{
		mmu.set_rom(file_contents);
	}

	return 1;
}

void GameBoy::unloadROM()
{
	uint8_t zeros[0x8000] = { 0 };
	mmu.set_rom(zeros);
}

void GameBoy::clock()
{
	cpu.cpu_step();
	ppu.clock();
}
