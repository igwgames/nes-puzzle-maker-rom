#include "source/c/library/bank_helpers.h"

ZEROPAGE_EXTERN(unsigned char, totalGameLevels);
ZEROPAGE_EXTERN(unsigned char, movementSpeed);
ZEROPAGE_EXTERN(unsigned char, enableUndo);

extern unsigned char tileCollisionTypes[16];
extern unsigned char tilePalettes[16];
extern unsigned char gamePaletteData[16];
extern unsigned char spritePalette[4];
extern unsigned char unusedByte1;
extern unsigned char singleLevelOverride;
extern unsigned char titleSong;
extern unsigned char gameplaySong;
extern unsigned char creditsSong;
extern unsigned char enableLevelShow;
extern unsigned char enableKeyCount;
extern unsigned char showGoal;
extern unsigned char showGameTitle;
extern unsigned char customSongShowTimes[16];

extern unsigned char userDataBuffer[16];

// These ones are only in the bank, not copied. Make sure to swap to the USER_DATA bank!
extern const unsigned char user_gameLevelData[4096];
extern const unsigned char user_titleScreenData[1024];
extern const unsigned char user_creditsScreenData[1024];




void load_user_data(void);
void load_gamename_to_buffer(unsigned char bankToReturnTo);
void load_coinsCollectedText_to_buffer(unsigned char bankToReturnTo);
void load_cratesRemovedText_to_buffer(unsigned char bankToReturnTo);
void load_hud_vram(unsigned char bankToReturnTo);
unsigned char user_get_hud_palette_for_goal(unsigned char bankToReturnTo);