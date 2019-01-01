#include "source/configuration/system_constants.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/map/map.h"
#include "source/globals.h"
#include "source/game_data/game_data.h"
#include "source/map/map_data.h"
#include "source/graphics/palettes.h"

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

void load_map_tiles_and_palette() {

    switch (currentGameData[GAME_DATA_OFFSET_TILESET_ID]) {
        case CHR_BANK_ARCADE:
            // Make sure we're looking at the right sprite and chr data, not the ones for the menu.
            set_chr_bank_0(CHR_BANK_ARCADE);
            set_chr_bank_1(CHR_BANK_SPRITES);

            // Also set the palettes to the in-game palettes.
            memcpy(palette, mainBgPalette, 16);
            memcpy((&(palette[12])), &(spritePalettes[currentGameData[GAME_DATA_OFFSET_SPRITE_ID]<<2]), 4);
            pal_bg(mainBgPalette);
            pal_spr(palette);
            memcpy(currentMapTileData, arcadeTileData, 32);

            break;
        case CHR_BANK_ZORIA_DESERT:
            set_chr_bank_0(CHR_BANK_ZORIA_DESERT);
            set_chr_bank_1(CHR_BANK_SPRITES);

            memcpy(palette, zoriaDesertBgPalette, 16);
            memcpy((&(palette[12])), &(spritePalettes[currentGameData[GAME_DATA_OFFSET_SPRITE_ID]<<2]), 4);


            // Also set the palettes to the in-game palettes.
            pal_bg(zoriaDesertBgPalette);
            pal_spr(palette);
            memcpy(currentMapTileData, zoriaDesertTileData, 32);
            break;
        
        case CHR_BANK_ZORIA:
        default:
            set_chr_bank_0(CHR_BANK_ZORIA);
            set_chr_bank_1(CHR_BANK_SPRITES);

            memcpy(palette, zoriaBgPalette, 16);
            memcpy((&(palette[12])), &(spritePalettes[currentGameData[GAME_DATA_OFFSET_SPRITE_ID]<<2]), 4);

            // Also set the palettes to the in-game palettes.
            pal_bg(zoriaBgPalette);
            pal_spr(palette);
            memcpy(currentMapTileData, zoriaTileData, 32);
            break;
        

    }

}
CODE_BANK_POP();

// Loads the map at the player's current position into the ram variable given. 
// Kept in a separate file, as this must remain in the primary bank so it can
// read data from another prg bank.
void load_map() {
    totalKeyCount = 0;
    totalCrateCount = 0;

    // FORMAT: 0: tileId, 1: palette, 2: collision type, 4: unused
    banked_call(PRG_BANK_MAP_LOGIC,  load_map_tiles_and_palette);

    
    // Need to switch to the bank that stores this map data.
    // TODO: This would be better as ASM - this is kinda inefficient
    UNPACK_6BIT_DATA((&(currentGameData[0]) + GAME_DATA_OFFSET_MAP + (currentLevelId*GAME_DATA_OFFSET_MAP_WORLD_LENGTH)), currentMap, 24);

    // Iterate a second time to bump all values up to their array index equivalents, to save us computation later.
    for (i = 0; i != 64; ++i) {
        currentMap[i] <<= 2;

        j = currentMapTileData[currentMap[i]+TILE_DATA_LOOKUP_OFFSET_COLLISION];
        if (j == TILE_COLLISION_COLLECTABLE) {
            currentMapOrig[i] = 0;
            ++totalKeyCount;
        } else if (j == TILE_COLLISION_CRATE) {
            currentMapOrig[i] = 0;
            ++totalCrateCount;
        } else if (j == TILE_COLLISION_GAP) {
            currentMapOrig[i] = 0;
        } else {
            currentMapOrig[i] = currentMap[i];
        }
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