#include "source/c/neslib.h"
#include "source/c/menus/text_helpers.h"
#include "source/c/globals.h"

#pragma code-name ("MENUS")
#pragma rodata-name ("MENUS")

// Rather than have a full ASCII table, we skip the first 32 characters so we have a little room for extra 
// data later in the bank. We have to subtract this constantly though, so we made a constant.
#define TEXT_ASCII_SKIPPED_CHARACTERS 0x60

// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++) + TEXT_ASCII_SKIPPED_CHARACTERS);
	}
}

// Clear the screen and put a nice border around it.
void _clear_screen_with_border() {
	set_vram_update(NULL);
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
	vram_fill(0x55, 64);

}

void clear_screen_with_border() {
	vram_adr(0x2000);
	_clear_screen_with_border();
}

void clear_screen_with_border_b() {
	vram_adr(NAMETABLE_B);
	_clear_screen_with_border();
}