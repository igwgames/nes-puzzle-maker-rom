#include "source/menus/list_games.h"
#include "source/library/bank_helpers.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/graphics/fade_animation.h"
#include "source/menus/text_helpers.h"
#include "source/game_data/game_data.h"
#include "source/configuration/game_states.h"
// NOTE: We're stealing the input variable from this for a temp location to store stuff.
#include "source/menus/text_input.h"

void bank_draw_list_games(unsigned char isSave) {
    bank_push(PRG_BANK_GAME_LIST);
    draw_list_games(isSave);
    bank_pop();
}

void bank_do_list_game_input(unsigned char isSave) {
    bank_push(PRG_BANK_GAME_LIST);
    do_list_game_input(isSave);
    bank_pop();
}

CODE_BANK(PRG_BANK_GAME_LIST);

#define listGamesTempInt tempInt1
#define redraw tempChar9 // Using a higher one, so it's unlikely our called functions steal it.

#define builtInGameCount tempChar1
#define customGameCount tempChar2
#define tempSelGameHolder tempChar3
#define shownBuiltInGameCount tempChar4

void draw_list_games(unsigned char isSave) {
    tempSelGameHolder = selectedGameId;
    builtInGameCount = 0;
    customGameCount = 0;
    ppu_off();
    clear_screen_with_border();
    // We reuse the title palette here, though we have the option of making our own if needed.
    // pal_bg(titlePalette);
	// pal_spr(titlePalette);
    scroll(0, 0);

    if (isSave) {
        put_str(NTADR_A(3, 2), " Save Game ");
        put_str(NTADR_A(5, 4), "Choose a slot to save to");
    } else {
        put_str(NTADR_A(3, 2), " Load Game ");

        put_str(NTADR_A(5, 4), "Choose a game to load.");
    }

    for (i = 0; i != 8; ++i) {
        selectedGameId = i;
        load_game_name_to_input();
        if (inputText[0] != NULL) {
            ++builtInGameCount;
            if (!isSave) {
                vram_adr(NTADR_A(5, 6+i));
                vram_put('0' + i + 1 + 0x60);
                vram_put('.' + 0x60);
                for (j = 0; j != GAME_DATA_OFFSET_TITLE_LENGTH; ++j) {
                    vram_put(inputText[j] + 0x60);
                }
            }
        } else {
            break;
        }
    }

    if (isSave) {
        shownBuiltInGameCount = 0;
    } else {
        shownBuiltInGameCount = builtInGameCount;
    }

    for (i = 0; i != 8; ++i) {
        selectedGameId = 128 + i;
        load_game_name_to_input();
        if (inputText[0] != NULL) {
            ++customGameCount;
            vram_adr(NTADR_A(5, 6+(shownBuiltInGameCount+i)));
            if ((shownBuiltInGameCount + i + 1) < 10) {
                vram_put('0' + (shownBuiltInGameCount + i) + 1 + 0x60);
            } else {
                vram_put('A' + (shownBuiltInGameCount + i) + 1 - 10 + 0x60);
            }
            vram_put('.' + 0x60);

            for (j = 0; j != GAME_DATA_OFFSET_TITLE_LENGTH; ++j) {
                vram_put(inputText[j] + 0x60);
            }
        } else {
            break;
        }
    }


    if ((isSave && customGameCount < 8) || gameState == GAME_STATE_EDITOR_INIT) {
        put_str(NTADR_A(5, 6+(shownBuiltInGameCount + customGameCount)), "- New Game -");
    }

    if (tempSelGameHolder >= 128) {
        selectedGameId = tempSelGameHolder - 128 + shownBuiltInGameCount;
    } else {
        selectedGameId = tempSelGameHolder;
    }

    if (selectedGameId > (shownBuiltInGameCount + customGameCount)) {
        selectedGameId = 0;
    }

    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();
}

void do_list_game_input(unsigned char isSave) {

    do_redraw:
    screenBuffer[0] = MSB(NTADR_A(3, 4)) | NT_UPD_VERT;
    screenBuffer[1] = LSB(NTADR_A(3, 4));
    screenBuffer[2] = 20;
    for (i = 3; i != 23; ++i) {
        // FIXME: Constant
        screenBuffer[i] = 0xee;
    }
    screenBuffer[23] = MSB(NTADR_A(3, 4));
    screenBuffer[24] = LSB(NTADR_A(3, 4));
    // FIXME: Constant
    screenBuffer[25] = 0xe2;
    // NOTE: This is just a random tile to update and keep black - this should be updated when we need a second tile.
    screenBuffer[26] = MSB(NTADR_A(8, 3));
    screenBuffer[27] = LSB(NTADR_A(8, 3));
    screenBuffer[28] = 0xee; // AGAIN, FIXME: Constant
    screenBuffer[29] = NT_UPD_EOF;
    set_vram_update(screenBuffer);
    redraw = 0;
    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        // If Start is pressed now, and was not pressed before...
        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            break;
        }

        if (controllerState & PAD_UP && !(lastControllerState & PAD_UP)) {
            if (selectedGameId != 0) {
                --selectedGameId;
            }
        }

        if (controllerState & PAD_DOWN && !(lastControllerState & PAD_DOWN)) {
            if (selectedGameId != (shownBuiltInGameCount + customGameCount + ((isSave && customGameCount < 8) || gameState == GAME_STATE_EDITOR_INIT ? 0 : -1))) {
                ++selectedGameId;
            }
        }

        if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
            break;
        }

        listGamesTempInt = NTADR_A(3, selectedGameId + 6);
        screenBuffer[23] = MSB(listGamesTempInt);
        screenBuffer[24] = LSB(listGamesTempInt);

        ppu_wait_nmi();

    }
    if (redraw) {
        fade_out();
        draw_list_games(isSave);
        fade_in();
        goto do_redraw;
    }

    if (selectedGameId >= shownBuiltInGameCount) {
        selectedGameId = 128 + (selectedGameId - shownBuiltInGameCount);
    }

}