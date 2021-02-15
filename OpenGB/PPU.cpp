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
	for (int pixel = 0; pixel < LCD_WIDTH; ++pixel)
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
			tile_location += (tile_no + 128) * 16;

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
		if (scanline >= 0 && scanline < LCD_HEIGHT && pixel >= 0 && pixel < LCD_WIDTH)
		{
			lcd[scanline * LCD_WIDTH + pixel] = colour;
			bg_colours[scanline * LCD_WIDTH + pixel] = colour_no;
		}
			
	}
}

void PPU::draw_window(uint8_t scanline)
{
	uint16_t tiledata = 0;
	uint16_t window_memory = 0;
	bool is_unsigned = true;

	uint8_t window_x = gb->mmu.readByte(WX) - 7;
	uint8_t window_y = gb->mmu.readByte(WY);

	if (window_x < 0 || window_x > 159 || window_y < 0 || window_y > 143)
		return;

	// Really hacky fix that may break some games
	if (scanline < window_y) return;

	if (test_bit(gb->mmu.readByte(LCDC), 4))
		tiledata = 0x8000;
	else
	{
		tiledata = 0x8800;
		is_unsigned = false;
	}

	if (test_bit(gb->mmu.readByte(LCDC), 6))
		window_memory = 0x9C00;
	else
		window_memory = 0x9800;

	uint8_t y_pos = window_counter;

	uint16_t tile_row = y_pos / 8 * 32;
	for (int pixel = window_x; pixel < LCD_WIDTH; ++pixel)
	{
		uint8_t x_pos = 8;
		if (pixel > window_x);
			x_pos = pixel - window_x;

		uint16_t tile_col = (x_pos / 8);

		int16_t tile_no;
		uint16_t tile_addr = window_memory + tile_row + tile_col;

		if (is_unsigned)
			tile_no = static_cast<uint8_t>(gb->mmu.readByte(tile_addr));
		else
			tile_no = static_cast<int8_t>(gb->mmu.readByte(tile_addr));

		uint16_t tile_location = tiledata;

		if (is_unsigned)
			tile_location += (tile_no * 16);
		else
			tile_location += (tile_no + 128) * 16;

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
		if (scanline >= 0 && scanline < LCD_HEIGHT && pixel >= 0 && pixel < LCD_WIDTH)
		{
			lcd[scanline * LCD_WIDTH + pixel] = colour;
			bg_colours[scanline * LCD_WIDTH + pixel] = colour_no;
		}
	}
	++window_counter;
}

void PPU::draw_sprites(uint8_t scanline)
{
	bool use_8x16 = test_bit(gb->mmu.readByte(LCDC), 2);
	int sprite_count = 0;
	for (int sprite = 0; sprite < 40; ++sprite)
	{
		if (sprite_count >= 10) break;
		uint8_t sprite_x_coords[10]; // Really hacky fix for DMG acid'
		bool skip_sprite = false;
		uint8_t index = sprite * 4;
		uint8_t y_pos = gb->mmu.readByte(OAM + index) - 16;
		uint8_t x_pos = gb->mmu.readByte(OAM + index + 1) - 8;
		uint8_t tile_location = gb->mmu.readByte(OAM + index + 2);
		uint8_t attr = gb->mmu.readByte(OAM + index + 3);

		/*for (int i = 0; i < 10; ++i)
		{
			if (sprite_x_coords[i] == x_pos) skip_sprite = true;
		}*/

		if (skip_sprite) continue;

		bool bg_priority = test_bit(attr, 7);
		bool y_flip = test_bit(attr, 6);
		bool x_flip = test_bit(attr, 5);

		uint8_t y_size = 8;
		if (use_8x16)
		{
			y_size = 16;
			clear_bit(tile_location, 0);
		}

		if ((scanline >= y_pos) && (scanline < (y_pos + y_size)))
		{
			int line = scanline - y_pos;

			sprite_x_coords[sprite_count] = x_pos;
			++sprite_count;

			if (y_flip)
			{
				line -= (y_size - 1);
				line *= -1;
			}

			line *= 2;
			uint16_t data_addr = (VRAM_ADDR + tile_location * 16) + line;
			uint8_t low = gb->mmu.readByte(data_addr);
			uint8_t high = gb->mmu.readByte(data_addr + 1);

			for (int tile_pixel = 0; tile_pixel < 8; ++tile_pixel)
			{
				int colour_bit = tile_pixel;

				if (x_flip)
				{
					colour_bit -= 7;
					colour_bit *= -1;
				}

				int colour_no = get_bit(high, colour_bit);
				colour_no <<= 1;
				colour_no |= get_bit(low, colour_bit);

				if (colour_no == 0) continue;

				uint16_t colour_addr = test_bit(attr, 4) ? OBP1 : OBP0;
				uint8_t palette = gb->mmu.readByte(colour_addr);
				int colour = (palette >> (colour_no * 2)) & 0x03;

				int x_pix = 7 - tile_pixel;
				//x_pix += 7;
				int pixel = x_pos + x_pix;

				/*if (scanline < 0 || scanline >= LCD_HEIGHT || pixel < 0 || pixel >= LCD_WIDTH)
				{
					continue;
				}*/

				if (bg_priority && bg_colours[scanline * LCD_WIDTH + pixel] != 0)
				{
					continue;
				}

				if (scanline >= 0 && scanline < LCD_HEIGHT && pixel >= 0 && pixel < LCD_WIDTH)
				lcd[scanline * LCD_WIDTH + pixel] = colour;
			}
		}
	}
}

void PPU::draw(uint8_t scanline)
{
	// Check if display is enabled
	if (test_bit(gb->mmu.readByte(LCDC), 7))
	{
		// Draw background and window
		if (test_bit(gb->mmu.readByte(LCDC), 0))
		{
			draw_background(scanline);

			if (test_bit(gb->mmu.readByte(LCDC), 5))
				draw_window(scanline);
		}
		/*else
		{
			for (uint8_t pixel = 0; pixel < LCD_WIDTH; ++pixel)
			{
				if (scanline >= 0 && scanline < LCD_HEIGHT)
					lcd[scanline * LCD_WIDTH + pixel] = 0;
			}
		}*/

		// Draw sprites
		if (test_bit(gb->mmu.readByte(LCDC), 1))
			draw_sprites(scanline);
	}
}

void PPU::check_lyc()
{
	uint8_t scanline = gb->mmu.readByte(LY);
	uint8_t lyc = gb->mmu.readByte(LYC);
	bool equal = scanline == lyc;
	uint8_t status = gb->mmu.readByte(STAT);
	if (equal) set_bit(status, 2);
	else clear_bit(status, 2);
	gb->mmu.writeByte(STAT, status);
	if (test_bit(status, 6) && equal) gb->cpu.request_interrupt(lcd_status);
}

void PPU::clock()
{
	bool req_int = false; // Should we request an LCD interrupt
	mode = gb->mmu.readByte(STAT) & 0x03;
 	uint8_t current_mode = mode;
	uint8_t status = gb->mmu.readByte(STAT);

	if (test_bit(gb->mmu.readByte(LCDC), 7))
	{
		cycles += (gb->cpu.get_cycles() * 4);
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
					check_lyc();
					if (scanline >= LCD_HEIGHT) // We're on the final scanline
					{
						mode = VBLANK;
						// Set STAT register 
						status &= 0xFC;
						status |= 0x01;
						gb->mmu.writeByte(STAT, status);
						// Fire V-Blank interrupt
						gb->cpu.request_interrupt(vblank);
						// Check interrupt
						req_int = test_bit(status, 4);
					}
					else
					{
						mode = OAM_READ;
						status &= 0xFC;
						status |= 0x02;
						gb->mmu.writeByte(STAT, status);
						req_int = test_bit(status, 5);
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
					check_lyc();
					if (scanline == MAX_SCANLINES) // End of frame
					{
						mode = OAM_READ;
						gb->mmu.writeByte(LY, 0x00, true);
						window_counter = 0;
						uint8_t status = gb->mmu.readByte(STAT);
						status &= 0xFC;
						status |= 0x02;
						gb->mmu.writeByte(STAT, status);
						req_int = test_bit(status, 5);
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
					req_int = test_bit(status, 3);
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
			gb->cpu.request_interrupt(lcd_status);

	}
	else
	{
		lcd = { 0 };
		cycles = 0;
	}
}