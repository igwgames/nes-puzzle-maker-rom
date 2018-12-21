#include "source/configuration/system_constants.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/map/map.h"
#include "source/globals.h"
#include "source/game_data/game_data.h"
#include "source/map/map_data.h"

// FIXME: Not in the fixed bank.. not like this
const unsigned char dummyMap[] = {
    0x01, 0x13, 0x25,
    0xc1, 0x53, 0xe5,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c
};

// FIXME: Not like this 
const unsigned char dummyTileData[] = {
    0x02, 0x00, TILE_COLLISION_WALKABLE, 0,
    0x0a, 0x01, TILE_COLLISION_SOLID, 0,
    0x42, 0x01, TILE_COLLISION_SOLID, 0,
    0x28, 0x03, TILE_COLLISION_GAP, 0,
    0x60, 0x02, TILE_COLLISION_CRATE, 0,
    0x00, 0x00, TILE_COLLISION_UNUSED, 0,
    0x00, 0x00, TILE_COLLISION_UNUSED, 0,
    0x4a, 0x00, TILE_COLLISION_LEVEL_END, 0
};


// Loads the map at the player's current position into the ram variable given. 
// Kept in a separate file, as this must remain in the primary bank so it can
// read data from another prg bank.
void load_map() {

    // FIXME: should probably be looking this up.
    // FORMAT: 0: tileId, 1: palette, 2: collision type, 4: unused
    memcpy(currentMapTileData, dummyTileData, 32);

    
    // Need to switch to the bank that stores this map data.
    // TODO: This would be better as ASM - this is kinda inefficient
    UNPACK_6BIT_DATA((&(currentGameData[0]) + GAME_DATA_OFFSET_MAP + (currentLevelId*GAME_DATA_OFFSET_MAP_WORLD_LENGTH)), currentMap, 24);

    // Iterate a second time to bump all values up to their array index equivalents, to save us computation later.
    for (i = 0; i != 64; ++i) {
        currentMap[i] <<= 2;
    }

}

// Saves the map at the player's current position from the ram variable given.
// I'm not sure why I'm keeping this in the primary bank, but for now I am.
void save_map() {
    for (i = 0; i != 64; ++i) {
        currentMap[i] >>= 2;
    }
    PACK_6BIT_DATA(currentMap, (&(currentGameData[0]) + GAME_DATA_OFFSET_MAP + (currentLevelId*GAME_DATA_OFFSET_MAP_WORLD_LENGTH)), 64);

    for (i = 0; i != 64; ++i) {
        currentMap[i] <<= 2;
    }
}