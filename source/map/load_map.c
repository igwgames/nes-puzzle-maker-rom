#include "source/configuration/system_constants.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/map/map.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/graphics/palettes.h"
#include "source/sprites/player.h"

unsigned char palette[16];


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