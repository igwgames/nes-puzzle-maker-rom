#include "source/menus/text_input.h"
#include "source/menus/text_helpers.h"
#include "source/library/bank_helpers.h"
#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/graphics/fade_animation.h"


unsigned char inputText[32];
#define inputSelection tempChar1

// Solicits the user for text input. Reads/writes inputText
void _do_text_input(const unsigned char* name);

void do_text_input(const unsigned char* name) {
    bank_push(PRG_BANK_TEXT_INPUT);
    _do_text_input(name);
    bank_pop();
}

CODE_BANK(PRG_BANK_TEXT_INPUT);

void _do_text_input(const unsigned char* name) {
    fade_out();
    ppu_off();

    clear_screen_with_border();

    vram_adr(NTADR_A(4, 2));
    vram_put(' ' + 0x60);

    i = 0;
    while (1) {
        inputSelection = name[i];
        if (inputSelection == 0) {
            break;
        }
        vram_put(inputSelection + 0x60);
        ++i;
    }
    vram_put(' ' + 0x60);

    ppu_on_bg();
    fade_in();

    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            break;
        }
    }
}