#include "PPU.h"
#include "GameBoy.h"
void PPU::draw_background(uint8_t scanline)
{
	uint16_t tiledata = 0;
	uint16_t background_memory = 0;
	bool is_unsigned = true;

	uint8_t scroll_x = gb->mmu.readByte(SCX);
	uint8_t scroll_y = gb->mmu.readByte(SCY);

	if (test_bit(gb->mmu.readByte(LCDC), 4)) tiledata = 0x8000;
	else
	{
		tiledata = 0x8800;
		is_unsigned = false;
	}
	if (test_bit(gb->mmu.readByte(LCDC), 3)) background_memory = 0x9C00;
	else background_memory = 0x9800;

	uint8_t y_pos = scroll_y + scanline;

	uint16_t tile_row = (y_pos / 8) * 32;
	for (int pixel = 0; pixel < 160; ++pixel)
	{
		uint8_t x_pos = pixel + scroll_x;
		
		uint16_t tile_col = (x_pos / 8);

		int16_t tile_no;
		uint16_t tile_addr = background_memory + tile_row + tile_col;

		if (is_unsigned)
			tile_no = static_cast<uint8_t>(gb->mmu.readByte(tile_addr));
		else
			tile_no = static_cast<int8_t>(gb->mmu.readByte(tile_addr));

		uint16_t tile_location = tiledata;

		if (is_unsigned)
			tile_location += (tile_no * 16);
		else
			tile_location += (tile_no * 128) * 16;

		uint8_t line = y_pos % 8;
		line *= 2;

		uint8_t low = gb->mmu.readByte(tile_location + line);
		uint8_t high = gb->mmu.readByte(tile_location + line + 1);

		int colour_bit = x_pos % 8;
		colour_bit -= 7;
		colour_bit *= -1;

		int colour_no = get_bit(high, colour_bit);
		colour_no <<= 1;
		colour_no |= get_bit(low, colour_bit);

		uint8_t palette = gb->mmu.readByte(BGP);
		int colour = (palette >> (colour_no * 2)) & 0x03;
		if (scanline >= 0 && scanline < 144 && pixel >= 0 && pixel < 160)
			lcd[scanline * 160 + pixel] = colour;
	}
}

void PPU::draw(uint8_t scanline)
{
	// Check if display is enabled
	if (gb->mmu.readByte(LCDC) & 0x80)
	{
		// Draw background
		if (gb->mmu.readByte(LCDC) & 0x01)
		{
			draw_background(scanline);
		}
		//// Render noise for now
		//for (int x = 0; x < 160; ++x)
		//{
		//	uint8_t color = rand() % 4;

		//	lcd[scanline * 160 + x] = color;
		//}
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
					if (scanline >= 144) // We're on the final scanline
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

	else
	{
		gb->mmu.writeByte(STAT, gb->mmu.readByte(STAT) & 0xFC);
		gb->mmu.writeByte(LY, 0x00);
	}
}