#include "source/c/neslib.h"
#include "source/c/graphics/static_screens.h"
#include "source/c/mapper.h"
#include "source/c/graphics/fade_animation.h"
#include "source/c/menus/input_helpers.h"
#include "source/c/globals.h"
#include "source/c/library/music.h"
#include "source/c/library/user_data.h"
#include "source/c/graphics/static_screens.h"

// Purposely left in main bank
void do_show_screen(unsigned char screen) {
    fade_out();
    ppu_off();
    set_vram_update(NULL);
    ppu_wait_nmi();
    
    scroll(0, 0);
    vram_adr(0x2000);
    vram_write((unsigned char*)&(user_staticScreens[screen << 10]), 1024);
    ppu_on_all();
    fade_in();


    unrom_set_prg_bank(BANK_MENUS);
    wait_for_start();
    fade_out();
}

void show_relevant_screen(unsigned char position) {
    recur:
    unrom_set_prg_bank(BANK_STATIC_SC);

    tempChar1 = 255;
    for (i = 0; i < 16; ++i) {
        if (user_staticScreenTypes[i] == position) {
            tempChar1 = i;
            break;
        }
    }
    if (tempChar1 != 255) {
        unrom_set_prg_bank(BANK_SOUND);
        playSongForEvent(MUSIC_TIME_ON_SCREEN + tempChar1);
        unrom_set_prg_bank(BANK_STATIC_SC);
        do_show_screen(tempChar1);
        
        // Recur without adding to stack, since we're tripping over the max
        position = STATIC_SCREEN_AFTER_SCREEN + tempChar1;
        goto recur;
    }
}