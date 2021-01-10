#include "PPU.h"
#include "GameBoy.h"

void PPU::draw(uint8_t scanline)
{
	// Render noise for now
	for (int x = 0; x < 160; ++x)
	{
		uint8_t color = rand() % 4;

		lcd[scanline * 160 + x] = color;
	}
}

void PPU::check_lyc()
{
	bool equal = gb->mmu.readByte(LY) == gb->mmu.readByte(LYC);
	uint8_t status = gb->mmu.readByte(STAT) & 0xFB;
	if (equal) status |= 0x04;
	gb->mmu.writeByte(STAT, status);
	if (gb->mmu.readByte(STAT) & 0x40) gb->mmu.writeByte(IF, gb->mmu.readByte(IF) | 0x02);
}

void PPU::clock()
{
	cycles += (gb->cpu.get_cycles() * 4);
	bool req_int = false; // Should we request an LCD interrupt
	mode = gb->mmu.readByte(STAT) & 0x03;
	uint8_t current_mode = mode;
	uint8_t status = gb->mmu.readByte(STAT);

	if (gb->mmu.readByte(LCDC) & 0x80)
	{
		switch (mode)
		{
			case HBLANK:
				if (cycles >= HBLANK_CYCLES)
				{
					cycles = 0;
					uint8_t scanline = gb->mmu.readByte(LY);
					draw(scanline);
					++scanline;
					gb->mmu.writeByte(LY, scanline, true);
					if (scanline == 144) // We're on the final scanline
					{
						mode = VBLANK;
						// Set STAT register 
						status &= 0xFC;
						status |= 0x01;
						gb->mmu.writeByte(STAT, status);
						// Fire V-Blank interrupt
						gb->mmu.writeByte(IF, (gb->mmu.readByte(IF) | 0x01));
						// Check interrupt
						req_int = gb->mmu.readByte(STAT) & 0x10;
					}
					else
					{
						mode = OAM_READ;
						status &= 0xFC;
						status |= 0x02;
						gb->mmu.writeByte(STAT, status);
						req_int = gb->mmu.readByte(STAT) & 0x20;
					}
				}
				break;
			case VBLANK:
				if (cycles >= VBLANK_CYCLES)
				{
					cycles = 0;
					uint8_t scanline = gb->mmu.readByte(LY);
					++scanline;
					gb->mmu.writeByte(LY, scanline, true);
					if (scanline == 154) // End of frame
					{
						mode = OAM_READ;
						gb->mmu.writeByte(LY, 0x00);
						uint8_t status = gb->mmu.readByte(STAT);
						status &= 0xFC;
						status |= 0x02;
						gb->mmu.writeByte(STAT, status);
						req_int = gb->mmu.readByte(STAT) & 0x20;
					}
				}
				break;
			case OAM_READ:
				if (cycles >= OAM_READ_CYCLES)
				{
					cycles = 0;
					mode = VRAM_READ;
					uint8_t status = gb->mmu.readByte(STAT);
					status &= 0xFC;
					status |= 0x03;
					gb->mmu.writeByte(STAT, status);
				}
				break;
			case VRAM_READ: // VRAM
				if (cycles >= VRAM_READ_CYCLES)
				{
					cycles = 0;
					mode = HBLANK;
					req_int = gb->mmu.readByte(STAT) & 0x08;
					uint8_t status = gb->mmu.readByte(STAT);
					status &= 0xFC;
					status |= 0x00;
					gb->mmu.writeByte(STAT, status);
				}
				break;
			default:
				std::cerr << "Invalid PPU State!" << std::endl;
				break;
		}
		if (req_int && (mode != current_mode))
			gb->mmu.writeByte(IF, gb->mmu.readByte(IF) | 0x02);
		check_lyc();
	}
}