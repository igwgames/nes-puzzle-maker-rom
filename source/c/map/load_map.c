#include "source/c/configuration/system_constants.h"
#include "source/c/neslib.h"
#include "source/c/mapper.h"
#include "source/c/library/bank_helpers.h"
#include "source/c/library/user_data.h"
#include "source/c/map/map.h"
#include "source/c/globals.h"
#include "source/c/map/map.h"
#include "source/c/sprites/player.h"

unsigned char palette[16];

unsigned char bitplaneBytes[3];

// NOTE: This file is in the primary prg since it loads from patchable_data, which lives in a bank.

// Replace any "sprite-ish" things from the "orig" copy of the map, so that when we move them they don't duplicate
void update_map_replace_spriteish(void) {
    if (tempChar1 == TILE_COLLISION_COLLECTABLE) {
        currentMapOrig[j] = 0;
        ++totalCollectableCount;
    } else if (tempChar1 == TILE_COLLISION_CRATE) {
        currentMapOrig[j] = 0;
        ++tempChar3;
    } else if (tempChar1 == TILE_COLLISION_LOCK || tempChar1 == TILE_COLLISION_KEY) {
        currentMapOrig[j] = 0;
    } else if (tempChar1 == TILE_COLLISION_GAP || tempChar1 == TILE_COLLISION_GAP_PASSABLE || tempChar1 == TILE_COLLISION_COLLAPSIBLE) {
        currentMapOrig[j] = 0;
        ++tempChar2;
    } else {
        currentMapOrig[j] = currentMap[j];
    }
}

// Loads the map at the player's current position into the ram variable for the map. 
void load_map() {
    // WARNING: Changes prg bank and does not change it back
    unrom_set_prg_bank(BANK_USER_DATA);
    totalCollectableCount = 0;
    totalCrateCount = 0;

    // Each map is 78 bytes in total, so find the index to start looking at
    tempInt1 = currentLevelId * 78;

    // Pull data out of the data we have available (see patchable_data.asm for where this comes from)
    currentMapBorderTile = user_gameLevelData[75 + tempInt1];
    currentMapBorderAsset = tilePalettes[currentMapBorderTile];
    currentMapBorderAsset += (currentMapBorderAsset << 2);
    currentMapBorderAsset += (currentMapBorderAsset << 4);
    // Expand to a full map tile
    tempChar2 = currentMapBorderTile;
    tempChar3 = tempChar2;
    tempChar2 &= 0x07;
    tempChar3 >>= 3;
    tempChar2 <<= 1;
    tempChar3 <<= 5;
    currentMapBorderTile = tempChar2 + tempChar3;


    // Iterate over the map data and expand it into a full array. Each byte in the data we store actually holds
    // data for 2 tiles - one in the lower 5 bits, then a bitplane of the last 3 bytes, 6=<<5, 7=<<6, 8=<<7

    // Track whether we have more crates, or more holes. Use this to determine how to finish level.
    tempChar2 = 0;
    tempChar3 = 0;

    // i = index on gameLevelData, j = index on currentMap
    for (i = 0, j = 0; i != 75; ++i) {
        
        tempChar4 = user_gameLevelData[i + tempInt1];
        tempChar5 = tempChar4 & 0x1f;
        currentMap[j] = tempChar5;

        tempChar1 = tileCollisionTypes[tempChar5];
        update_map_replace_spriteish();

        tempChar6 = i % 5; // bit within bitlaneBytes
        bitplaneBytes[0] += (!!(tempChar4 & 0x20)) << tempChar6;
        bitplaneBytes[1] += (!!(tempChar4 & 0x40)) << tempChar6;
        bitplaneBytes[2] += (!!(tempChar4 & 0x80)) << tempChar6;
        if (tempChar6 == 4) {
            // End of a chunk, use bitplane values
            ++j;
            tempChar7 = bitplaneBytes[0];
            currentMap[j] = tempChar7;
            tempChar1 = tileCollisionTypes[tempChar7];
            update_map_replace_spriteish();
            ++j;
            tempChar7 = bitplaneBytes[1];
            currentMap[j] = tempChar7;
            tempChar1 = tileCollisionTypes[tempChar7];
            update_map_replace_spriteish();
            ++j;
            tempChar7 = bitplaneBytes[2];
            currentMap[j] = tempChar7;
            tempChar1 = tileCollisionTypes[tempChar7];
            update_map_replace_spriteish();

            bitplaneBytes[0] = 0;
            bitplaneBytes[1] = 0;
            bitplaneBytes[2] = 0;
        }

        ++j;

    }
    if (tempChar2 < tempChar3) {
        totalCrateCount = tempChar2;
    } else {
        totalCrateCount = tempChar3;
    }
    playerGridPositionX = user_gameLevelData[tempInt1 + 77] & 0x0f;
    playerGridPositionY = user_gameLevelData[tempInt1 + 77] >> 4;
    currentGameStyle = user_gameLevelData[tempInt1 + 76];
}
