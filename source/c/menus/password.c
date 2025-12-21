// Various things having to do with the password system - translating them, showing them, entering them, etc.
#include "../globals.h"
#include "graphics/password_rle.h"
#include "source/c/neslib.h"
#include "source/c/configuration/system_constants.h"
#include "source/c/library/user_data.h"

const unsigned char vocabulary[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 
    'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 
    'U', 'V', 'W', 'X', 'Y', 'Z', '2', '3', '4',
    '5', '6', '7', '8', '9'
};

// Password construction: 
//   coins, 7 bits (0-99)
//   crates: 7 bits (0-99)
//   checksum: 4 bits (0-16)
//   level: 7 bits (0-127)
// Total: 25 bits. 5 bits/char for vocab 
// 00000 00000 00000 00000 00000
// CCCCC CCKKK KKKKS SSSLL LLLLL
unsigned char checksum, bees, boops, carak, currentPassword[5];

void write_state_to_password(void) {
    // Calculate the checksum of the data we're going to store
    checksum = (gameCollectableCount + gameCrates + currentLevelId + 7) & 0x0f;
    // Divvy it up between the 5 microbytes 
    currentPassword[0] = 0b00011111 & gameCollectableCount;
    currentPassword[1] = (0b00011000 & (gameCollectableCount >> 2));
    currentPassword[1] |= (0b00000111 & gameCrates);
    currentPassword[2] = (0b00011110 & (gameCrates >> 2));
    currentPassword[2] |= (0b00000001) & checksum;
    currentPassword[3] = (0b00011100) & (checksum << 1);
    currentPassword[3] |= (0b00000011) & (currentLevelId);
    currentPassword[4] = (0b00011111) & (currentLevelId >> 2);

    // Translate to ASCII characters
    for (i = 0; i != 5; ++i) {
        currentPassword[i] = vocabulary[currentPassword[i]];
    }
}

// NOTE, whether successful or not, this will DESTROY current state. 
// If not successful, it must be rewritten.
// 1 = success, 0 = failure
unsigned char read_password_to_state(void) {
    // Translate from ASCII back to raw numbers
    for (i = 0; i != 5; ++i) {
        for (j = 0; j < 32; ++j) {
            if (vocabulary[j] == currentPassword[i]) {
                currentPassword[i] = j; 
                break;
            }
        }
    }

    // And pull the data back out.
    gameCollectableCount = currentPassword[0] & 0b11111; // Efectively taking everything
    gameCollectableCount |= (currentPassword[1] & 0b00011000) << 2;
    gameCrates = currentPassword[1] & 0b00000111;
    gameCrates |= (currentPassword[2] & 0b00011110) << 2;
    checksum = currentPassword[2] & 0b1;
    checksum |= (currentPassword[3] & 0b00011100) >> 1;
    // Unvalidated checksum, need to validate after this
    currentLevelId = currentPassword[3] & 0b00000011;
    currentLevelId |= (currentPassword[4] & 0b00011111) << 2;

    if (currentLevelId > totalGameLevels) {
        return 0;
    }

    return (checksum == ((gameCollectableCount + gameCrates + currentLevelId + 7) & 0x0f));
}  

void draw_password_screen(void) {
    ppu_off();
    vram_adr(0x2000);
    vram_unrle(password_rle);
    ppu_on_all();
}

#define cursorPos tempChar1
#define lastCursorPos tempChar2
#define cursorPosX tempChar3
#define cursorPosY tempChar4
#define passwordPosition tempChar5
#define timeToExit tempChar6
#define tempNametablePos tempInt1

void test_password(void) {
    if (read_password_to_state()) {
        sfx_play(SFX_WIN, SFX_CHANNEL_1);
        timeToExit = 1;

    } else {
        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);
    }
}

void do_password_input(void) {
    for (i = 0; i != 5; ++i) {
        currentPassword[i] = 0;
    }
    cursorPos = 0;
    lastCursorPos = 1; // Any tile other than the one we're on so we don't overwrite it.
    passwordPosition = 0;
    timeToExit = 0;

    // Set up screenBuffer. 
    // First two slots are the old and new cursors.
    screenBuffer[0] = 0x20;
    screenBuffer[1] = 00;
    screenBuffer[2] = 0xe2;
    screenBuffer[3] = 0x20;
    screenBuffer[4] = 00;
    screenBuffer[5] = ' ' + 0x60;
    screenBuffer[6] = 0x21 | NT_UPD_HORZ;
    screenBuffer[7] = 0x09;
    screenBuffer[8] = 13;
    for (i = 9; i < (9+13); ++i) {
        screenBuffer[i] = ' ' + 0x60;
    }
    screenBuffer[9+13] = NT_UPD_EOF;

    while (!timeToExit) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);
        lastCursorPos = cursorPos;

        if (controllerState & PAD_LEFT && !(lastControllerState & PAD_LEFT)) {
            --cursorPos;
            if (cursorPos == 33) { cursorPos = 32; }
            if (cursorPos > 33) { cursorPos = 34; }
        }
        if (controllerState & PAD_RIGHT && !(lastControllerState & PAD_RIGHT)) {
            ++cursorPos;
            if (cursorPos == 33) { cursorPos = 34; }
            if (cursorPos > 34) { cursorPos = 0; }
        }
        if (controllerState & PAD_UP && !(lastControllerState & PAD_UP)) {
            cursorPos -= 9;
            while (cursorPos > 33) { cursorPos -= 9; }
        }
        if (controllerState & PAD_DOWN && !(lastControllerState & PAD_DOWN)) {
            cursorPos += 9;
            if (cursorPos > 32) { cursorPos = 34; }
        }

        if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
            if (cursorPos < 32) {
                currentPassword[passwordPosition] = cursorPos;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_2);
                if (passwordPosition < 4) { ++passwordPosition; }
            } else if (cursorPos == 32) {
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_2);
                if (passwordPosition > 0) { 
                    --passwordPosition; 
                } else {
                    timeToExit = 1;
                }
            } else if (cursorPos == 34) {
                test_password();
            }
        }

        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            test_password();
        }

        if (controllerState & PAD_B && !(lastControllerState & PAD_B)) {
            sfx_play(SFX_MENU_BOP, SFX_CHANNEL_2);
            if (passwordPosition > 0) { --passwordPosition; }
        }

        if (cursorPos == lastCursorPos) {
            lastCursorPos = cursorPos - 1;
        } else {
            sfx_play(SFX_MENU_BOP, SFX_CHANNEL_3);
        }

        cursorPosX = (cursorPos % 9)*2;
        cursorPosY = (cursorPos / 9)*2;

        tempNametablePos = NTADR_A(cursorPosX+6, cursorPosY+16);
        screenBuffer[0] = MSB(tempNametablePos);
        screenBuffer[1] = LSB(tempNametablePos);

        cursorPosX = (lastCursorPos % 9) * 2;
        cursorPosY = (lastCursorPos / 9) * 2;

        tempNametablePos = NTADR_A(cursorPosX+6, cursorPosY+16);
        screenBuffer[3] = MSB(tempNametablePos);
        screenBuffer[4] = LSB(tempNametablePos);

        for (i = 0; i < 5; ++i) {
            screenBuffer[9 + i*3] = (vocabulary[currentPassword[i]]) + 0x60;
        }

        // Blink current char
        if ((frameCount & 0x04) == 0x04) {
            screenBuffer[9 + passwordPosition*3] = ' ' + 0x60;
        }

        ppu_wait_nmi();
        set_vram_update(screenBuffer);

    }

    set_vram_update(NULL);

    if (!read_password_to_state()) {
        reset();
    }
    // Rest of setup is handled in main.c

}
