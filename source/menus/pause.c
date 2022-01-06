#include "source/menus/pause.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"
#include "source/menus/input_helpers.h"

CODE_BANK(PRG_BANK_PAUSE_MENU);

unsigned char editorSelectedPosition;

void draw_pause_screen() {
    ppu_off();
    clear_screen_with_border_b();
    // We reuse the title palette here, though we have the option of making our own if needed.
    scroll(256, 0);

	// set_chr_bank_0(CHR_BANK_MENU);
    // set_chr_bank_1(CHR_BANK_MENU);

    // Just write "- Paused -" on the screen... there's plenty of nicer things you could do if you wanna spend time!
    put_str(NTADR_B(11, 7), "- Paused -");

    put_str(NTADR_B(12, 16), "Continue");
    put_str(NTADR_B(12, 18), "Restart");


/*    vram_adr(NTADR_B(3,2));
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
*/

    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();
}

void handle_pause_input() {
    editorSelectedPosition = 0;
    screenBuffer[0] = MSB(NTADR_B(10, 16));
    screenBuffer[1] = LSB(NTADR_B(10, 16));
    screenBuffer[2] = 0xe2; // FIXME: Constant
    screenBuffer[3] = MSB(NTADR_B(10, 18));
    screenBuffer[4] = LSB(NTADR_B(10, 18));
    screenBuffer[5] = ' ' + 0x60;
    screenBuffer[6] = NT_UPD_EOF;
    set_vram_update(screenBuffer);
    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        // If Start is pressed now, and was not pressed before...
        if ((controllerState & PAD_START && !(lastControllerState & PAD_START)) || (controllerState & PAD_A && !(lastControllerState & PAD_A))) {
            break;
        }

        if (controllerState & PAD_UP && !(lastControllerState & PAD_UP)) {
            sfx_play(SFX_MENU_BOP, SFX_CHANNEL_2);
            editorSelectedPosition = 0;
            screenBuffer[2] = 0xe2;
            screenBuffer[5] = ' ' + 0x60;
        }

        if (controllerState & PAD_DOWN && !(lastControllerState & PAD_DOWN)) {
            sfx_play(SFX_MENU_BOP, SFX_CHANNEL_2);
            editorSelectedPosition = 1;
            screenBuffer[5] = 0xe2;
            screenBuffer[2] = ' ' + 0x60;
        }


        ppu_wait_nmi();

    }
    set_vram_update(NULL);
    if (editorSelectedPosition == 0) {
        gameState = GAME_STATE_RUNNING;
    } else {
        gameState = GAME_STATE_LOAD_LEVEL;
    }
}