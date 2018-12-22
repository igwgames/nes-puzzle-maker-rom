#include "source/neslib_asm/neslib.h"
#include "source/menus/text_helpers.h"
#include "source/globals.h"
#include "source/graphics/game_text.h"

// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++) + TEXT_ASCII_SKIPPED_CHARACTERS);
	}
}

// Blank the screen - uses the 'space' character 
void clear_screen() {
	vram_adr(0x2000);
	vram_fill(' ' + TEXT_ASCII_SKIPPED_CHARACTERS, 0x03c0);
	vram_fill(0, 0x30);
}

// Clear the screen and put a nice border around it.
void clear_screen_with_border() {
	set_vram_update(NULL);
	vram_adr(0x2000);
	vram_fill(' ' + TEXT_ASCII_SKIPPED_CHARACTERS, 64);

	vram_put(' ' + TEXT_ASCII_SKIPPED_CHARACTERS);
	vram_put(ASCII_TILE_TL);
	vram_fill(ASCII_TILE_HORZ, 28);
	vram_put(ASCII_TILE_TR);
	vram_put(' ' + TEXT_ASCII_SKIPPED_CHARACTERS);

	for (i = 0; i != 24; ++i) {
		vram_put(' ' + TEXT_ASCII_SKIPPED_CHARACTERS);
		vram_put(ASCII_TILE_VERT);
		vram_fill(' ' + TEXT_ASCII_SKIPPED_CHARACTERS, 28);
		vram_put(ASCII_TILE_VERT);
		vram_put(' ' + TEXT_ASCII_SKIPPED_CHARACTERS);
	}
	vram_put(' ' + TEXT_ASCII_SKIPPED_CHARACTERS);
	vram_put(ASCII_TILE_BL);
	vram_fill(ASCII_TILE_HORZ, 28);
	vram_put(ASCII_TILE_BR);
	vram_put(' ' + TEXT_ASCII_SKIPPED_CHARACTERS);

	vram_fill(' ' + TEXT_ASCII_SKIPPED_CHARACTERS, 64);

	// Attribute table
	vram_fill(0, 64);

}