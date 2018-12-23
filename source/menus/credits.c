#include "source/menus/credits.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"
#include "source/configuration/game_info.h"
#include "source/game_data/game_data.h"

CODE_BANK(PRG_BANK_CREDITS_MENU);

void draw_win_screen() {
    ppu_off();
    clear_screen_with_border();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

    vram_adr(NTADR_A(3,2));
    vram_put(' ' + 0x60);

    for (i = 0; i != GAME_DATA_OFFSET_TITLE_LENGTH; ++i) {
        if (currentGameData[GAME_DATA_OFFSET_TITLE+i] == ' ' || currentGameData[GAME_DATA_OFFSET_TITLE+i] == 0) {
            // If all that's left is spaces, don't keep printing
            for (j = i; j != GAME_DATA_OFFSET_TITLE_LENGTH; ++j) {
                if (currentGameData[GAME_DATA_OFFSET_TITLE+j] != ' ' && currentGameData[GAME_DATA_OFFSET_TITLE+j] != 0) {
                    goto keep_going;
                }
            }
            break;
        }
        keep_going:
        vram_put(currentGameData[GAME_DATA_OFFSET_TITLE+i] + 0x60);
    }
    vram_put(' ' + 0x60);

    // Add whatever you want here; NTADR_A just picks a position on the screen for you. Your options are 0, 0 to 32, 30
    put_str(NTADR_A(7, 10), "  Congratulations  ");

    put_str(NTADR_A(4, 15), "You did the thing, and");
    put_str(NTADR_A(4, 16), "thus won the game!");

    // Hide all existing sprites
    oam_clear();
    ppu_on_all();

}

void draw_credits_screen() {
    // FIXME: this is so very wrong
    ppu_off();
    clear_screen_with_border();
    // We reuse the title palette here, though we have the option of making our own if needed.
    pal_bg(titlePalette);
	pal_spr(titlePalette);
    scroll(0, 0);

	set_chr_bank_0(CHR_BANK_MENU);
    set_chr_bank_1(CHR_BANK_MENU);

    vram_adr(NTADR_A(3,2));
    vram_put(' ' + 0x60);

    for (i = 0; i != GAME_DATA_OFFSET_TITLE_LENGTH; ++i) {
        if (currentGameData[GAME_DATA_OFFSET_TITLE+i] == ' ' || currentGameData[GAME_DATA_OFFSET_TITLE+i] == 0) {
            // If all that's left is spaces, don't keep printing
            for (j = i; j != GAME_DATA_OFFSET_TITLE_LENGTH; ++j) {
                if (currentGameData[GAME_DATA_OFFSET_TITLE+j] != ' ' && currentGameData[GAME_DATA_OFFSET_TITLE+j] != 0) {
                    goto keep_going;
                }
            }
            break;
        }
        keep_going:
        vram_put(currentGameData[GAME_DATA_OFFSET_TITLE+i] + 0x60);
    }
    vram_put(' ' + 0x60);


    // Add whatever you want here; NTADR_A just picks a position on the screen for you. Your options are 0, 0 to 32, 30
    put_str(NTADR_A(11, 4), "  Credits  ");

    put_str(NTADR_A(2, 6), "Game Design and Logic");
    put_str(NTADR_A(4, 8), gameAuthor);

    put_str(NTADR_A(2, 11), "Music");
    put_str(NTADR_A(4, 13), "Wolfgang (OpenGameArt)");

    put_str(NTADR_A(2, 16), "Artwork");
    put_str(NTADR_A(4, 18), "Refresh Games (OpenGameArt)");

    put_str(NTADR_A(6, 24), "Created in");
    put_str(NTADR_A(17, 24), currentYear);
    put_str(NTADR_A(22, 24), "by");

    put_str(NTADR_A(8, 26), gameAuthor);


    // Hide all existing sprites
    oam_clear();
    ppu_on_all();
}