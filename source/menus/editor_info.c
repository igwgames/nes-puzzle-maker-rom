#include "source/neslib_asm/neslib.h"
#include "source/menus/editor_info.h"
#include "source/menus/text_helpers.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/library/bank_helpers.h"
#include "source/globals.h"
#include "source/menus/input_helpers.h"
#include "source/graphics/palettes.h"
#include "source/menus/about_screen.rle.h"

void draw_editor_info() {
    ppu_off();
    //clear_screen_with_border();
    // We reuse the title palette here, though we have the option of making our own if needed.
    // ACtually we're using the game palette... because we need to show real tiles n stuff
    // FIXME: Also show player.

    vram_adr(0x2000);
    vram_unrle(about_screen);
    //pal_bg(titlePalette);
	//pal_spr(titlePalette);
    scroll(0, 0);

    // Actually, keep the same one
	//set_chr_bank_0(CHR_BANK_MENU);
    //set_chr_bank_1(CHR_BANK_MENU);

    // Just write "- Paused -" on the screen... there's plenty of nicer things you could do if you wanna spend time!
    //put_str(NTADR_A(11, 13), "- Paused -");



    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();
}

void handle_editor_info_input() {
    banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start);
    gameState = GAME_STATE_EDITOR;
}