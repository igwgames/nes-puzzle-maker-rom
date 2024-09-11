#include "source/c/menus/credits.h"
#include "source/c/mapper.h"
#include "source/c/library/user_data.h"
#include "source/c/configuration/system_constants.h"
#include "source/c/globals.h"
#include "source/c/neslib.h"
#include "source/c/configuration/game_states.h"
#include "source/c/menus/text_helpers.h"
#include "source/c/map/map.h"
#include "source/c/graphics/fade_animation.h"
#include "source/c/menus/input_helpers.h"
#include "source/c/library/user_data.h"

#pragma code-name ("MENUS")
#pragma rodata-name ("MENUS")

void draw_win_screen() {
    ppu_off();
    clear_screen_with_border();
    scroll(0, 0);

    if (disableStatsScreen) {
        ppu_on_all();
        return;
    }


    // Add whatever you want here; NTADR_A picks a position on the screen for you. Your options are 0, 0 to 32, 30
    put_str(NTADR_A(8, 5), "Congratulations!");

    put_str(NTADR_A(9, 12), "You have won! ");

    put_str(NTADR_A(5, 24), "Your time:       ");

    // Figure out how long the player was playing.
    // 60 frames/second, so get down to seconds
    tempInt1 = (frameCount - gameTime) / 60;
    // Build the string version up in reverse...
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

    tempChar1 = 0;
    switch (currentGameStyle) {
        case GAME_STYLE_MAZE:
            // Do nothing; nothing special to show
            tempChar1 = 1;
            break;
        case GAME_STYLE_COIN:
            load_coinsCollectedText_to_buffer(BANK_MENUS);
            put_str(NTADR_A(5, 22), userDataBuffer);
            tempInt1 = gameCollectableCount;
            break;
        case GAME_STYLE_CRATES:
            load_cratesRemovedText_to_buffer(BANK_MENUS);
            put_str(NTADR_A(5, 22), userDataBuffer);
            tempInt1 = gameCrates;
            break;
    }

    // NOTE: If you need space back, all of the % 10 and /10 likely takes up a bit of space and cpu time.
    if (tempChar1 == 0) {
        vram_put('0' + ((tempInt1 / 100) % 10) + 0x60);
        vram_put('0' +((tempInt1 / 10) % 10) + 0x60);
        vram_put('0' + ((tempInt1 % 10)) + 0x60);
    }

    // Hide all existing sprites
    oam_clear();
    ppu_on_all();

    fade_in();
    wait_for_start();
    fade_out();

}

// Forward-definition of this method, so we can call it to load data. 
void load_credits_user_data();

// Draw the credits screen from user data.
void draw_credits_screen() {
    ppu_off();
    scroll(0, 0);


    vram_adr(0x2000);
    load_credits_user_data();

    // Hide all existing sprites
    oam_clear();
    ppu_on_all();

    fade_in();
    wait_for_start();
    fade_out();
}

// Switch back to the main prg bank for this load method, to fetch things from user_data in another bank.
#pragma code-name ("CODE")
#pragma rodata-name ("CODE")
void load_credits_user_data() {
    unrom_set_prg_bank(BANK_USER_DATA);
    vram_write((unsigned char*)&(user_creditsScreenData[0]), 0x400);
    unrom_set_prg_bank(BANK_MENUS);
}

