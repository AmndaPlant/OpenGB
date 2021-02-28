#include "Util.h"

class GameBoy;

class APU
{
	private:
		GameBoy *gb = nullptr;
		int cycles = 0;

	public:
		void clock();
		void connectGB(GameBoy *g) { gb = g; }
};
