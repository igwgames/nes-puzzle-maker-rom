#include "source/menus/intro.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"
#include "source/menus/input_helpers.h"
#include "source/map/map.h"

CODE_BANK(PRG_BANK_INTRO_SCREEN);

#define titleLen tempChar1

#define crateTile tempChar2
#define holeTile tempChar3
#define coinTile tempChar4
#define goalTile tempChar5
#define tempTile tempChar6

void draw_intro_screen() {
    ppu_off();
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

    vram_adr(0x2000);
    vram_write(&introScreenData[0], 0x400);


    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();
}

void handle_intro_input() {
    banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start);

}