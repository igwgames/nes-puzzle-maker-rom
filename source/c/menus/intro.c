#include "source/c/menus/intro.h"
#include "source/c/configuration/system_constants.h"
#include "source/c/globals.h"
#include "source/c/neslib.h"
#include "source/c/configuration/game_states.h"
#include "source/c/menus/text_helpers.h"
#include "source/c/menus/input_helpers.h"
#include "source/c/map/map.h"

#pragma code-name ("CODE")
#pragma rodata-name ("CODE")

#define titleLen tempChar1

#define crateTile tempChar2
#define holeTile tempChar3
#define coinTile tempChar4
#define goalTile tempChar5
#define tempTile tempChar6

// Load the intro screen from user data
void draw_intro_screen() {
    ppu_off();
    scroll(0, 0);


    vram_adr(0x2000);
    vram_write(&introScreenData[0], 0x400);


    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();
}

void handle_intro_input() {
    wait_for_start();

}