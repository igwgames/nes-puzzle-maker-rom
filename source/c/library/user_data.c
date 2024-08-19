#include "source/c/library/bank_helpers.h"
#include "source/c/mapper.h"
#include "source/c/neslib.h"
#include "source/c/globals.h"

ZEROPAGE_DEF(unsigned char, totalGameLevels);
ZEROPAGE_DEF(unsigned char, movementSpeed);
ZEROPAGE_DEF(unsigned char, enableUndo);

unsigned char tileCollisionTypes[32];
unsigned char tilePalettes[32];
unsigned char gamePaletteData[16];
unsigned char spritePalette[4];
unsigned char singleLevelOverride;
unsigned char enableLevelShow;
unsigned char enableKeyCount;
unsigned char showGameTitle;
unsigned char showGoal;
unsigned char animateBlockMovement;
unsigned char customSongShowTimes[16];

unsigned char userDataBuffer[16];


// Stuff from patchable_data.asm
extern const unsigned char user_gamePaletteData[16];
extern const unsigned char user_spritePalette[4];

extern const unsigned char user_movementSpeed;
extern const unsigned char user_coinsCollectedText[];
extern const unsigned char user_cratesRemovedText[];

extern const unsigned char user_enableUndo;
extern const unsigned char user_enableLevelShow;
extern const unsigned char user_enableKeyCount;

extern const unsigned char user_gameName[];
extern const unsigned char user_tileCollisionTypes[];
extern const unsigned char user_tilePalettes[];

extern const unsigned char user_gameLevelData[];
extern const unsigned char user_totalGameLevels;
extern const unsigned char user_unusedByte1;
extern const unsigned char user_singleLevelOverride;
extern const unsigned char user_titleScreenData[];

extern const unsigned char user_showGameTitle;
extern const unsigned char user_showGoal;
extern const unsigned char user_animateBlockMovement;
extern const unsigned char user_hudData[];
extern const unsigned char user_hudDataAttrs[];
extern const unsigned char user_customSongShowTimes[];



// This is in the kernel so it can get data out of the user data bank
#pragma code-name ("USER_DATA")
#pragma rodata-name ("USER_DATA")
void load_user_data(void) {
    totalGameLevels = user_totalGameLevels;
    movementSpeed = user_movementSpeed;
    enableUndo = user_enableUndo;
    singleLevelOverride = user_singleLevelOverride;
    enableLevelShow = user_enableLevelShow;
    enableKeyCount = user_enableKeyCount;
    showGameTitle = user_showGameTitle;
    showGoal = user_showGoal;
    animateBlockMovement = user_animateBlockMovement;

    memcpy(&tileCollisionTypes[0], &user_tileCollisionTypes[0], 32);
    memcpy(&tilePalettes[0], &user_tilePalettes[0], 32);
    memcpy(&gamePaletteData[0], &user_gamePaletteData[0], 16);
    memcpy(&spritePalette[0], &user_spritePalette[0], 4);
    memcpy(&customSongShowTimes[0], &user_customSongShowTimes[0], 16);
}

#pragma code-name ("CODE")
#pragma rodata-name ("CODE")

void load_gamename_to_buffer(unsigned char bankToReturnTo) {
    unrom_set_prg_bank(BANK_USER_DATA);
    memcpy(&userDataBuffer[0], &user_gameName[0], 28);
    unrom_set_prg_bank(bankToReturnTo);
}

void load_coinsCollectedText_to_buffer(unsigned char bankToReturnTo) {
    unrom_set_prg_bank(BANK_USER_DATA);
    memcpy(&userDataBuffer[0], &user_coinsCollectedText[0], 28);
    unrom_set_prg_bank(bankToReturnTo);
}

void load_cratesRemovedText_to_buffer(unsigned char bankToReturnTo) {
    unrom_set_prg_bank(BANK_USER_DATA);
    memcpy(&userDataBuffer[0], &user_cratesRemovedText[0], 28);
    unrom_set_prg_bank(bankToReturnTo);
}

void load_hud_vram(unsigned char bankToReturnTo) {
    unrom_set_prg_bank(BANK_USER_DATA);
    vram_adr(0x2300);
    vram_write((unsigned char*)&(user_hudData[0]), 192);
    vram_adr(0x23f0);
    vram_write((unsigned char*)&(user_hudDataAttrs[0]), 16);
    unrom_set_prg_bank(bankToReturnTo);
}

unsigned char user_get_hud_palette_for_goal(unsigned char bankToReturnTo) {
    unrom_set_prg_bank(BANK_USER_DATA);
    tempChara = user_hudDataAttrs[6];
    unrom_set_prg_bank(bankToReturnTo);
    return tempChara;
}