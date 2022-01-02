#include "source/neslib_asm/neslib.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/menus/text_helpers.h"
#include "source/graphics/palettes.h"
#include "source/configuration/game_info.h"
#include "source/map/map.h"

CODE_BANK(PRG_BANK_TITLE);


#define selectedOption tempChara

// Rewrite to pull in custom data
void draw_title_screen() {
	set_vram_update(NULL);
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);
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