#include "source/neslib_asm/neslib.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/menus/text_helpers.h"
#include "source/graphics/palettes.h"
#include "source/configuration/game_info.h"
#include "source/map/map.h"

#pragma code-name ("CODE")
#pragma rodata-name ("CODE")

#define selectedOption tempChara

// Rewrite to pull in custom data
void draw_title_screen() {
	set_vram_update(NULL);
    ppu_off();

	//clear_screen();
	oam_clear();

	vram_adr(0x2000);
	vram_write(&titleScreenData[0], 0x400);
    

	// put_str(NTADR_A(10, 16), "Press Start!");
	ppu_on_all();

	gameState = GAME_STATE_TITLE_INPUT;
}

void handle_title_input() {
	controllerState = pad_trigger(0);
	if (controllerState & PAD_START) {
		gameState = GAME_STATE_POST_TITLE;
	}
	
}