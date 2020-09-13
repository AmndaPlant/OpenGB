#include <iostream>

#include "GameBoy.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./OpenGB <ROM FILE>" << std::endl;
		return 1;
	}

	GameBoy gb;
	if (gb.readROM(argv[1]))
	{
		std::cout << "Succesfully loaded ROM!" << std::endl;
		while (!gb.cpu.is_halted())
		{
			gb.cpu.cpu_step();
		}
	}
	else
	{
		return 1;
	}

	return 0;
}