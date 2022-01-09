#include "source/configuration/system_constants.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/map/map.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/graphics/palettes.h"
#include "source/sprites/player.h"

CODE_BANK(PRG_BANK_MAP_LOGIC);
const unsigned char arcadeTileData[] = {
    0x00, 0x00, TILE_COLLISION_WALKABLE, 0,
    0x02, 0x01, TILE_COLLISION_WALKABLE, 0,
    0x04, 0x01, TILE_COLLISION_SOLID, 0,
    0x06, 0x01, TILE_COLLISION_SOLID, 0,
    0x08, 0x02, TILE_COLLISION_CRATE, 0,
    0x0a, 0x02, TILE_COLLISION_GAP, 0,
    0x0c, 0x02, TILE_COLLISION_COLLECTABLE, 0,
    0x0e, 0x02, TILE_COLLISION_LEVEL_END, 0
};

const unsigned char zoriaDesertTileData[] = {
    0x00, 0x00, TILE_COLLISION_WALKABLE, 0,
    0x02, 0x00, TILE_COLLISION_WALKABLE, 0,
    0x04, 0x00, TILE_COLLISION_SOLID, 0,
    0x06, 0x01, TILE_COLLISION_SOLID, 0,
    0x08, 0x00, TILE_COLLISION_CRATE, 0,
    0x0a, 0x00, TILE_COLLISION_GAP, 0,
    0x0c, 0x02, TILE_COLLISION_COLLECTABLE, 0,
    0x0e, 0x02, TILE_COLLISION_LEVEL_END, 0
};

const unsigned char zoriaTileData[] = {
    0x00, 0x00, TILE_COLLISION_WALKABLE, 0,
    0x02, 0x00, TILE_COLLISION_WALKABLE, 0,
    0x04, 0x00, TILE_COLLISION_SOLID, 0,
    0x06, 0x00, TILE_COLLISION_SOLID, 0,
    0x08, 0x01, TILE_COLLISION_CRATE, 0,
    0x0a, 0x00, TILE_COLLISION_GAP, 0,
    0x0c, 0x02, TILE_COLLISION_COLLECTABLE, 0,
    0x0e, 0x02, TILE_COLLISION_LEVEL_END, 0
};

const unsigned char spritePalettes[] = {
    // One set per sprite, in order they appear in chr
    0x0f, 0x01, 0x21, 0x36,
    0x0f, 0x09, 0x1a, 0x27,
    0x0f, 0x16, 0x26, 0x37,
    0x0f, 0x06, 0x16, 0x30,
    0x0f, 0x06, 0x16, 0x26,
    0x0f, 0x06, 0x16, 0x26
};

unsigned char palette[16];

CODE_BANK_POP();

// Loads the map at the player's current position into the ram variable given. 
// Kept in a separate file, as this must remain in the primary bank so it can
// read data from another prg bank.
void load_map() {
    totalKeyCount = 0;
    totalCrateCount = 0;

    
    tempInt1 = currentLevelId << 6;

    currentMapBorderTile = gameLevelData[60 + tempInt1];
    currentMapBorderAsset = tilePalettes[currentMapBorderTile];
    currentMapBorderAsset += (currentMapBorderAsset << 2);
    currentMapBorderAsset += (currentMapBorderAsset << 4);
    if (currentMapBorderTile < 8) {
        currentMapBorderTile <<= 1;
    } else {
        currentMapBorderTile -= 8;
        currentMapBorderTile <<= 1;
        currentMapBorderTile += 32;
    }


    // Iterate a second time to bump all values up to their array index equivalents, to save us computation later.
    for (i = 0, j = 0; i != 60; ++i) {
        j = i<<1;
        currentMap[j] = (gameLevelData[i + tempInt1] & 0xf0) >> 4;

        // NOTE: I don't like repeating this twice, cleaning that up might help save some space
        tempChar1 = tileCollisionTypes[currentMap[j]];
        if (tempChar1 == TILE_COLLISION_COLLECTABLE) {
            currentMapOrig[j] = 0;
            ++totalKeyCount;
        } else if (tempChar1 == TILE_COLLISION_CRATE) {
            currentMapOrig[j] = 0;
            ++totalCrateCount;
        } else if (tempChar1 == TILE_COLLISION_GAP) {
            currentMapOrig[j] = 0;
        } else {
            currentMapOrig[j] = currentMap[j];
        }

        ++j;
        currentMap[j] = (gameLevelData[i + tempInt1] & 0x0f);

        tempChar1 = tileCollisionTypes[currentMap[j]];
        if (tempChar1 == TILE_COLLISION_COLLECTABLE) {
            currentMapOrig[j] = 0;
            ++totalKeyCount;
        } else if (tempChar1 == TILE_COLLISION_CRATE) {
            currentMapOrig[j] = 0;
            ++totalCrateCount;
        } else if (tempChar1 == TILE_COLLISION_GAP) {
            currentMapOrig[j] = 0;
        } else {
            currentMapOrig[j] = currentMap[j];
        }
    }
    playerGridPositionX = gameLevelData[tempInt1 + 62] & 0x0f;
    playerGridPositionY = gameLevelData[tempInt1 + 62] >> 4;
    currentGameStyle = gameLevelData[tempInt1 + 61];

}