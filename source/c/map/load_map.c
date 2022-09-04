#include "source/c/configuration/system_constants.h"
#include "source/c/neslib.h"
#include "source/c/library/bank_helpers.h"
#include "source/c/map/map.h"
#include "source/c/globals.h"
#include "source/c/map/map.h"
#include "source/c/sprites/player.h"

unsigned char palette[16];

// Replace any "sprite-ish" things, so that when we move them they don't duplicate
void update_map_replace_spriteish(void) {
    if (tempChar1 == TILE_COLLISION_COLLECTABLE) {
        currentMapOrig[j] = 0;
        ++totalCollectableCount;
    } else if (tempChar1 == TILE_COLLISION_CRATE || tempChar1 == TILE_COLLISION_LOCK || tempChar1 == TILE_COLLISION_KEY) {
        currentMapOrig[j] = 0;
    } else if (tempChar1 == TILE_COLLISION_GAP) {
        currentMapOrig[j] = 0;
        ++totalCrateCount;
    } else {
        currentMapOrig[j] = currentMap[j];
    }
}

// Loads the map at the player's current position into the ram variable for the map. 
void load_map() {
    totalCollectableCount = 0;
    totalCrateCount = 0;

    // Each map is 64 bytes in total, so find the index to start looking at
    tempInt1 = currentLevelId << 6;

    // Pull data out of the data we have available (see patchable_data.asm for where this comes from)
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


    // Iterate over the map data and expand it into a full map. Each byte in the data we store actually holds
    // 2 tiles - each nybble (half-byte) is a tile id, left then right. Thus each row is 6 bytes. If you're editing
    // in assembly, the left tile is 0xN_ and the right is 0x_N.
    for (i = 0, j = 0; i != 60; ++i) {
        j = i<<1;
        currentMap[j] = (gameLevelData[i + tempInt1] & 0xf0) >> 4;

        tempChar1 = tileCollisionTypes[currentMap[j]];
        update_map_replace_spriteish();

        ++j;
        currentMap[j] = (gameLevelData[i + tempInt1] & 0x0f);

        tempChar1 = tileCollisionTypes[currentMap[j]];
        update_map_replace_spriteish();
    }
    playerGridPositionX = gameLevelData[tempInt1 + 62] & 0x0f;
    playerGridPositionY = gameLevelData[tempInt1 + 62] >> 4;
    currentGameStyle = gameLevelData[tempInt1 + 61];

}