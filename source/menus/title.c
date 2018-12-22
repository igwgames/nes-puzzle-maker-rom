#include "source/neslib_asm/neslib.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/menus/text_helpers.h"
#include "source/graphics/palettes.h"
#include "source/configuration/game_info.h"

CODE_BANK(PRG_BANK_TITLE);

#define selectedOption tempChara

void draw_title_screen() {
	set_vram_update(NULL);
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);
	clear_screen();
	oam_clear();

    
    put_str(NTADR_A(8, 6), gameName);
	
	put_str(NTADR_A(2, 26), gameAuthorContact);
	
	put_str(NTADR_A(2, 28), "Copyright");
	put_str(NTADR_A(12, 28), currentYear);
	put_str(NTADR_A(17, 28), gameAuthor);

	put_str(NTADR_A(10, 14), "Play A Game");
	put_str(NTADR_A(10, 16), "Game Editor");

	// put_str(NTADR_A(10, 16), "Press Start!");
	ppu_on_all();

	gameState = GAME_STATE_TITLE_INPUT;
	selectedOption = 0;

	screenBuffer[0] = MSB(NTADR_A(8, 14));
	screenBuffer[1] = LSB(NTADR_A(8, 14));
	screenBuffer[2] = 0xe2;
	screenBuffer[3] = MSB(NTADR_A(8, 16));
	screenBuffer[4] = LSB(NTADR_A(8, 16));
	screenBuffer[5] = ' ' + 0x60;
	screenBuffer[6] = NT_UPD_EOF;
	set_vram_update(screenBuffer);
}

void handle_title_input() {
	controllerState = pad_trigger(0);
	if (controllerState & PAD_START) {
		if (selectedOption == 0) {
			gameState = GAME_STATE_POST_TITLE;
		} else {
			gameState = GAME_STATE_EDITOR_INIT;
		}
	} else if (controllerState & PAD_UP) {
		selectedOption = 0;
		screenBuffer[2] = 0xe2;
		screenBuffer[5] = ' ' + 0x60;
	} else if (controllerState & PAD_DOWN) {
		selectedOption = 1;
		screenBuffer[2] = ' ' + 0x60;
		screenBuffer[5] = 0xe2;
	}
	
}