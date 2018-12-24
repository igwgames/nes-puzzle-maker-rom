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
    put_str(NTADR_A(8, 5), "Congratulations!");

    put_str(NTADR_A(9, 12), "You have won! ");

    put_str(NTADR_A(5, 24), "Your time:       ");

    // 60 frames/second, so get down to seconds
    tempInt1 = (frameCount - gameTime) / 60;
    // Build it up in reverse...
    screenBuffer[4] = (tempInt1 % 60) % 10;
    screenBuffer[3] = (tempInt1 % 60) / 10;
    // Down to minutes
    tempInt1 /= 60;
    screenBuffer[2] = (tempInt1 % 100) % 10;
    screenBuffer[1] = (tempInt1 % 100) / 10;

    vram_put('0' + screenBuffer[1] + 0x60);
    vram_put('0' + screenBuffer[2] + 0x60);
    vram_put(':' + 0x60);
    vram_put('0' + screenBuffer[3] + 0x60);
    vram_put('0' + screenBuffer[4] + 0x60);


    switch (currentGameData[GAME_DATA_OFFSET_GAME_STYLE]) {
        case GAME_STYLE_MAZE:
            // Do nothing; nothing special to show
            break;
        case GAME_STYLE_COIN:
            put_str(NTADR_A(5, 22), "Coins collected:     ");

            vram_put('0' + (gameKeys / 10) + 0x60);
            vram_put('0' + (gameKeys % 10) + 0x60);
            break;
        case GAME_STYLE_CRATES:
            put_str(NTADR_A(5, 22), "Crates Removed:     ");
            vram_put('0' + (gameCrates / 10) + 0x60);
            vram_put('0' + (gameCrates % 10) + 0x60);
            break;

    }

    // Hide all existing sprites
    oam_clear();
    ppu_on_all();

}

void draw_credits_screen() {
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
    put_str(NTADR_A(10, 4), "  Credits  ");

    put_str(NTADR_A(4, 8), "Game Design");
    vram_adr(NTADR_A(5, 9));
    for (i = 0; i != GAME_DATA_OFFSET_AUTHOR_LENGTH; ++i) {
        vram_put(currentGameData[GAME_DATA_OFFSET_AUTHOR+i] + 0x60);
    }

    put_str(NTADR_A(4, 12), "Music");
    switch (currentGameData[GAME_DATA_OFFSET_SONG_ID]) {
        case SONG_OVERWORLD:
        case SONG_TITLE:
        case SONG_RPG_BATTLE:
        case SONG_CAVE:
            put_str(NTADR_A(5, 14), "Ted Kerr (Wolfgang)");
            put_str(NTADR_A(7, 16), "via OpenGameArt");
            break;

        case SONG_HEAD_IN_SAND:
            put_str(NTADR_A(5, 14), "congusbongus");
            put_str(NTADR_A(7, 16), "via OpenGameArt");
            break;

    }

    put_str(NTADR_A(4, 19), "Artwork");
    switch (currentGameData[GAME_DATA_OFFSET_TILESET_ID]) {
        case CHR_BANK_ARCADE:

            put_str(NTADR_A(5, 21), "RefreshGames");
            put_str(NTADR_A(7, 23), "via OpenGameArt");
            break;
        case CHR_BANK_ZORIA:
        case CHR_BANK_ZORIA_DESERT:
            put_str(NTADR_A(5, 21), "DragonDePlatino");
            put_str(NTADR_A(7, 24), "via OpenGameArt");
    }

    switch (currentGameData[GAME_DATA_OFFSET_SPRITE_ID]) {
        case GAME_SPRITE_ARCADE:
        case GAME_SPRITE_SLIME:
            if (currentGameData[GAME_DATA_OFFSET_TILESET_ID] != CHR_BANK_ARCADE) {
                put_str(NTADR_A(5, 22), "RefreshGames");
            }
            break;
        case GAME_SPRITE_DOG:
        case GAME_SPRITE_ROBITZ:
        case GAME_SPRITE_SPIDER:
        case GAME_SPRITE_ZORIA:
            if (currentGameData[GAME_DATA_OFFSET_TILESET_ID] != CHR_BANK_ZORIA && currentGameData[GAME_DATA_OFFSET_TILESET_ID] != CHR_BANK_ZORIA_DESERT) {
                put_str(NTADR_A(5, 22), "RefreshGames");
            }
            break;
    }


    // Hide all existing sprites
    oam_clear();
    ppu_on_all();
}