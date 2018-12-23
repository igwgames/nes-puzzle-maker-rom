// Saving and loading game data, parsing it out of large structures...
// Also handles the temporary storage for the editor.
#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/map/map_data.h"
#include "source/library/bank_helpers.h"

unsigned char currentGameData[256];

ZEROPAGE_DEF(unsigned char, __extraBits);

SRAM_ARRAY_DEF(unsigned char, savedGameData, 2048);


void unload_game() {
    memfill(currentGameData, 0x00, 256);
}

void load_game() {
    if (selectedGameId < 128) {
        bank_push(PRG_BANK_MAP_DATA);

        memcpy(currentGameData, (&(builtInMapData[0]) + (selectedGameId<<8)), 256);
        bank_pop();
    } else {
        // FIXME: This should load from SRAM
        memcpy(currentGameData, (&(savedGameData[(selectedGameId-128)<<8])), 256);
    }
}

void save_editor_game(unsigned char* address) {
    memcpy(address, currentGameData, 256);
}
