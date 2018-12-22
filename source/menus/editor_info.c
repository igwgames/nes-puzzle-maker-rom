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
#include "source/game_data/game_data.h"
#include "source/menus/text_input.h"
#include "source/graphics/fade_animation.h"

// FIXME: Bank this. It's currently stored in 0, and banked with the pause menu in main.c (Man, I'm gettin sloppy...)
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

    // Now draw all the user-generated stuff onto the screen
    vram_adr(NTADR_A(5, 5));
    for (i = 0; i != GAME_DATA_OFFSET_TITLE_LENGTH; ++i) {
        vram_put(currentGameData[GAME_DATA_OFFSET_TITLE+i] + 0x60);
    }

    vram_adr(NTADR_A(5, 21));
    for (i = 0; i != GAME_DATA_OFFSET_AUTHOR_LENGTH; ++i) {
        vram_put(currentGameData[GAME_DATA_OFFSET_AUTHOR+i] + 0x60);
    }



    scroll(0, 0);

    // Actually, keep the same one
	//set_chr_bank_0(CHR_BANK_MENU);
    //set_chr_bank_1(CHR_BANK_MENU);

    // Just write "- Paused -" on the screen... there's plenty of nicer things you could do if you wanna spend time!
    //put_str(NTADR_A(11, 13), "- Paused -");



    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    set_vram_update(NULL);
    ppu_on_bg();
}

#define editorInfoPosition tempChar1
#define editorInfoPositionFull tempChar2
#define redraw tempChar9 // Using a higher one, so it's unlikely our called functions steal it.
#define editorInfoTempInt tempInt1

void handle_editor_info_input() {
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
    editorInfoPosition = 0;
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
            if (editorInfoPosition != 0) {
                --editorInfoPosition;
            }
        }

        if (controllerState & PAD_DOWN && !(lastControllerState & PAD_DOWN)) {
            if (editorInfoPosition != 6) {
                ++editorInfoPosition;
            }
        }

        if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
            if (editorInfoPosition == 0) {
                memcpy(inputText, &(currentGameData[GAME_DATA_OFFSET_TITLE]), 12);
                do_text_input("game name", 12);
                memcpy(&(currentGameData[GAME_DATA_OFFSET_TITLE]), inputText, 12);
                redraw = 1;
                break;
            } else if (editorInfoPosition == 5) {
                memcpy(inputText, &(currentGameData[GAME_DATA_OFFSET_AUTHOR]), 12);
                do_text_input("author", 12);
                memcpy(&(currentGameData[GAME_DATA_OFFSET_AUTHOR]), inputText, 12);
                redraw = 1;
                break;
            }

        }

        switch (editorInfoPosition) {
            case 0:
                editorInfoPositionFull = 4;
                break;
            case 1:
                editorInfoPositionFull = 7;
                break;
            case 2:
                editorInfoPositionFull = 11;
                break;
            default:
                editorInfoPositionFull = 11 + ((editorInfoPosition-2)*3);
                break;
        }
        editorInfoTempInt = NTADR_A(3, editorInfoPositionFull);
        screenBuffer[23] = MSB(editorInfoTempInt);
        screenBuffer[24] = LSB(editorInfoTempInt);

        ppu_wait_nmi();

    }
    if (redraw) {
        fade_out();
        draw_editor_info();
        fade_in();
        goto do_redraw;
    }
    gameState = GAME_STATE_EDITOR;
}