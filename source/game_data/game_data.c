// Saving and loading game data, parsing it out of large structures...
// Also handles the temporary storage for the editor.
#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/map/map_data.h"

unsigned char currentGameData[256];

ZEROPAGE_DEF(unsigned char, __extraBits);

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
    }
}

void save_editor_game(unsigned char* address) {
    memcpy(address, currentGameData, 256);
}

void list_games() {
    // FIXME: Implement.. also what does this even do?
}