#include "source/c/neslib.h"
#include "source/c/menus/title.h"
#include "source/c/globals.h"
#include "source/c/configuration/game_states.h"
#include "source/c/configuration/system_constants.h"
#include "source/c/menus/text_helpers.h"
#include "source/c/map/map.h"

#pragma code-name ("CODE")
#pragma rodata-name ("CODE")

#define selectedOption tempChara

// Draws custom data from the patched area onto the screen, then moves on.
void draw_title_screen() {
	set_vram_update(NULL);
    ppu_off();

	oam_clear();

	vram_adr(0x2000);
	vram_write(&titleScreenData[0], 0x400);
    

	ppu_on_all();

	gameState = GAME_STATE_TITLE_INPUT;
}
