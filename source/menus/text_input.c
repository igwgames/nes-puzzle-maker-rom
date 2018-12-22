#include "source/menus/text_input.h"
#include "source/menus/text_helpers.h"
#include "source/library/bank_helpers.h"
#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/graphics/fade_animation.h"
#include "source/graphics/palettes.h"


unsigned char inputText[32];
unsigned char inputSelection, lastInputSelection;
unsigned int lastFullPosition, currentFullPosition;
unsigned char currentTextPosition;

const unsigned char inputLookupTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]@ !\"#$%&'()";

// Solicits the user for text input. Reads/writes inputText
void _do_text_input(const unsigned char* name, const unsigned char maxLength);
void do_text_input(const unsigned char* name, const unsigned char maxLength) {
    bank_push(PRG_BANK_TEXT_INPUT);
    _do_text_input(name, maxLength);
    bank_pop();
}

CODE_BANK(PRG_BANK_TEXT_INPUT);

void draw_string(const char* name) {
    i = 0;
    while (1) {
        inputSelection = name[i];
        if (inputSelection == 0) {
            break;
        }
        vram_put(inputSelection + 0x60);
        ++i;
    }
}

void draw_string_len(const char* name, unsigned char length) {
    for (i = 0; i != length; ++i) {
        vram_put(name[i] + 0x60);
    }
}

#define COPY_STRING_TO_INPUT \
    for (i = 0; i != maxLength; ++i) { \
        screenBuffer[9+i] = inputText[i] + 0x60; \
    }

void _do_text_input(const unsigned char* name, const unsigned char maxLength) {
    fade_out();
    ppu_off();

    clear_screen_with_border();

    vram_adr(NTADR_A(4, 2));
    vram_put(' ' + 0x60);

    draw_string(name);
    vram_put(' ' + 0x60);

    vram_adr(NTADR_A(4, 4));
    draw_string("Please enter a ");
    draw_string(name);

    vram_adr(NTADR_A(5, 6));
    draw_string_len(inputText, maxLength);

    // Next, draw the available alphabet
    vram_adr(NTADR_A(6, 12));

    for (i = 0; i != 10; ++i) {
        vram_put(('0' + 0x60) + i);
        vram_put(' ' + 0x60);
    }

    vram_adr(NTADR_A(6, 14));
    for (i = 0; i != 10; ++i) {
        vram_put(('A' + 0x60) + i);
        vram_put(' ' + 0x60);
    }

    vram_adr(NTADR_A(6, 16));
    for (i = 0; i != 10; ++i) {
        vram_put(('K' + 0x60) + i);
        vram_put(' ' + 0x60);
    }

    vram_adr(NTADR_A(6, 18));
    for (i = 0; i != 9; ++i) {
        vram_put(('U' + 0x60) + i);
        vram_put(' ' + 0x60);
    }
    // Replace the carat with an @, since people like twitter and the like
    vram_put('@' + 0x60);

    vram_adr(NTADR_A(6, 20));
    for (i = 0; i != 10; ++i) {
        vram_put((' ' + 0x60) + i);
        vram_put(' ' + 0x60);
    }

    ppu_on_bg();
    fade_in();

    lastFullPosition = NTADR_A(6, 13);
    currentFullPosition = lastFullPosition;
    screenBuffer[0] = MSB(lastFullPosition);
    screenBuffer[1] = LSB(lastFullPosition);
    screenBuffer[2] = ' ' + 0x60;
    screenBuffer[3] = MSB(currentFullPosition);
    screenBuffer[4] = LSB(currentFullPosition);
    screenBuffer[5] = 0xe3; // FIXME: Constant.
    screenBuffer[6] = MSB(NTADR_A(5, 6)) | NT_UPD_HORZ;
    screenBuffer[7] = LSB(NTADR_A(5, 6));
    screenBuffer[8] = maxLength;
    screenBuffer[maxLength+9] = NT_UPD_EOF;
    COPY_STRING_TO_INPUT;
    set_vram_update(screenBuffer);

    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);
        COPY_STRING_TO_INPUT;

        if (frameCount & 0x20) {
            screenBuffer[9+currentTextPosition] = 0xbf;
        }

        lastFullPosition = currentFullPosition;

        currentFullPosition = NTADR_A(6 + ((inputSelection%10)<<1), 13 + ((inputSelection/10)<<1));
        screenBuffer[0] = MSB(lastFullPosition);
        screenBuffer[1] = LSB(lastFullPosition);
        screenBuffer[3] = MSB(currentFullPosition);
        screenBuffer[4] = LSB(currentFullPosition);


        if (controllerState & PAD_RIGHT && !(lastControllerState & PAD_RIGHT)) {
            ++inputSelection;
            if (inputSelection % 10 == 0) {
                inputSelection -= 10;
            }
        }

        if (controllerState & PAD_LEFT && !(lastControllerState & PAD_LEFT)) {
            --inputSelection;
            if (inputSelection % 10 == 9 || inputSelection == 255) {
                inputSelection += 10;
            }
        }

        if (controllerState & PAD_UP && !(lastControllerState & PAD_UP)) {
            inputSelection -= 10;
            // If we rolled over, bump things back
            if (inputSelection > 200) {
                inputSelection += 50;
            }
        }

        if (controllerState & PAD_DOWN && !(lastControllerState & PAD_DOWN)) {
            inputSelection += 10;
            if (inputSelection >= 50) {
                inputSelection -= 50;
            }
        }

        if (controllerState & PAD_B && !(lastControllerState & PAD_B)) {
            --currentTextPosition;
            if (currentTextPosition == 255) {
                currentTextPosition = maxLength-1;
            }
        }
        if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
            inputText[currentTextPosition] = inputLookupTable[inputSelection];
            ++currentTextPosition;
            if (currentTextPosition == maxLength) {
                currentTextPosition = 0;
            }
        }
        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            break;
        }
        ppu_wait_nmi();
    }
    set_vram_update(NULL);
}