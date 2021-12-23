// Saving and loading game data, parsing it out of large structures...
// Also handles the temporary storage for the editor.
#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/map/map_data.h"
#include "source/library/bank_helpers.h"
#include "source/menus/error.h"
#include "source/game_data/game_data.h"
#include "source/configuration/system_constants.h"

unsigned char currentGameData[256];

ZEROPAGE_DEF(unsigned char, __extraBits);

SRAM_ARRAY_DEF(unsigned char, savedGameData, 2048);
SRAM_ARRAY_DEF(unsigned char, currentSramGameData, 256);


void load_game() {
    bank_push(PRG_BANK_MAP_DATA);

    memcpy(currentGameData, (&(builtInMapData[0]) + (selectedGameId<<8)), 256);
    bank_pop();
}