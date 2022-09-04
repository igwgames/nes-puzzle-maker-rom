#include "source/c/menus/pause.h"
#include "source/c/configuration/system_constants.h"
#include "source/c/globals.h"
#include "source/c/neslib.h"
#include "source/c/configuration/game_states.h"
#include "source/c/menus/text_helpers.h"
#include "source/c/menus/input_helpers.h"

#pragma code-name ("CODE")
#pragma rodata-name ("CODE")

unsigned char editorSelectedPosition;

void draw_pause_screen() {
    ppu_off();
    clear_screen_with_border_b();
    scroll(256, 0);


    // Just write "- Paused -" on the screen... there's plenty of nicer things you could do if you wanna spend time!
    put_str(NTADR_B(11, 7), "- Paused -");

    put_str(NTADR_B(12, 16), "Continue");
    put_str(NTADR_B(12, 18), "Restart");

    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();
}

void handle_pause_input() {
    // Show a few options, and regularly update the tiles to show where the user has selected
    editorSelectedPosition = 0;
    screenBuffer[0] = MSB(NTADR_B(10, 16));
    screenBuffer[1] = LSB(NTADR_B(10, 16));
    screenBuffer[2] = 0xe2;
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