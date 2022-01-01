#include "source/neslib_asm/neslib.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/menus/text_helpers.h"
#include "source/graphics/palettes.h"
#include "source/configuration/game_info.h"

CODE_BANK(PRG_BANK_TITLE);

extern unsigned char PATCH_GAME_NAME[];
// const unsigned char gameName[] =   "     Retro Puzzle Maker     ";
const unsigned char gameAuthor[] = "             You                                                ";
const unsigned char currentYear[] = "2022";


#define selectedOption tempChara

// FIXME: This is dead, kill it or repurpose.
void draw_title_screen() {
	set_vram_update(NULL);
    ppu_off();
	pal_bg(titlePalette);
	pal_spr(titlePalette);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);
	clear_screen();
	oam_clear();

    
    put_str(NTADR_A(2, 6), PATCH_GAME_NAME);
	
	
	put_str(NTADR_A(3, 26), "Copyright");
	put_str(NTADR_A(13, 26), currentYear);
	put_str(NTADR_A(2, 27), gameAuthor);

	put_str(NTADR_A(10, 14), "Play");

	put_str(NTADR_A(28, 28), romVersion);

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
		gameState = GAME_STATE_POST_TITLE;
	}
	
}