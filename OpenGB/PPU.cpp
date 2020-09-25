#include "PPU.h"
#include "GameBoy.h"

void PPU::clock()
{
	read_lcd_control();

	if (!lcd_display_enable)
	{
		// LCD is off
		return;
	}

	cycles += (gb->cpu.get_cycles() * 4);

	handle_ppu_modes();
}

void PPU::read_lcd_control()
{
	uint8_t lcd_control = gb->mmu.readByte(0xFF40);
	bg_display_enable = lcd_control & 0x01;
	sprite_display_enable = lcd_control & 0x02;
	sprite_size = lcd_control & 0x04;
	bg_tile_map_region = lcd_control & 0x08;
	bg_and_window_tile_map_region = lcd_control & 0x10;
	window_display_enable = lcd_control & 0x20;
	window_tile_map_region = lcd_control & 0x40;
	lcd_display_enable = lcd_control & 0x80;
}

void PPU::render_scanline()
{
	read_lcd_control();

	scanline = gb->mmu.readByte(0xFF44);
}

void PPU::render_frame()
{
	// Just draw noise for now
	for (int x = 0; x < 144; ++x)
	{
		for (int y = 0; y < 160; ++y)
		{
			uint8_t color = rand() % 255;
			uint8_t r = color;
			uint8_t g = color;
			uint8_t b = color;
			uint8_t a = 0xFF;

			SDL_SetRenderDrawColor(renderer, r, g, b, a);
			SDL_RenderDrawPoint(renderer, y, x);
		}
	}

	SDL_RenderPresent(renderer);
}

void PPU::handle_ppu_modes()
{
	switch (ppu_mode)
	{
		case OAM_READ:
			if (cycles >= OAM_READ_CYCLES)
			{
				cycles = 0;
				ppu_mode = VRAM_READ;
			}
			break;
		case VRAM_READ:
			if (cycles >= VRAM_READ_CYCLES)
			{
				cycles = 0;
				ppu_mode = HBLANK;
				render_scanline();
			}
			break;
		case HBLANK:
			if (cycles >= HBLANK_CYCLES)
			{
				cycles = 0;
				gb->mmu.writeByte(0xFF44, gb->mmu.readByte(0xFF44) + 1, true);
				scanline = gb->mmu.readByte(0xFF44);

				// Check if on last scanline
				if (scanline == 144)
				{
					ppu_mode = VBLANK;
					gb->mmu.writeByte(0xFF0F, gb->mmu.readByte(0xFF0F) | 0x01); // Fire VBlank interrupt
					render_frame();
				}
				else
				{
					ppu_mode = OAM_READ;
				}
			}
			break;
		case VBLANK:
			if (cycles >= VBLANK_CYCLES)
			{
				cycles = 0;
				gb->mmu.writeByte(0xFF44, gb->mmu.readByte(0xFF44) + 1, true);
				scanline = gb->mmu.readByte(0xFF44);

				if (scanline > 153)
				{
					ppu_mode = OAM_READ;
					gb->mmu.writeByte(0xFF44, 0);
				}
			}
			break;
	}
}
