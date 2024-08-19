#include "source/c/map/map.h"
#include "source/c/map/load_map.h"
#include "source/c/neslib.h"
#include "source/c/library/user_data.h"
#include "source/c/library/bank_helpers.h"
#include "source/c/configuration/game_states.h"
#include "source/c/globals.h"
#include "source/c/configuration/system_constants.h"
#include "source/c/graphics/hud.h"
#include "source/c/graphics/fade_animation.h"
#include "source/c/sprites/player.h"

#pragma code-name ("PLAYER")
#pragma rodata-name ("PLAYER")

unsigned char currentMap[120];
unsigned char currentMapOrig[120];

unsigned char assetTable[64];

ZEROPAGE_DEF(unsigned char, currentGameStyle);

unsigned char mapScreenBuffer[0x5c];


void init_map() {

    // Do some trickery to make the HUD show up at the top of the screen, with the map slightly below.
    scroll(0, 240-HUD_PIXEL_HEIGHT);

}

// Reusing a few temporary vars for the sprite functions below.
#define tempArrayIndex tempInt3

// Clears the asset table. Set containsHud to 1 to set the HUD bytes to use palette 4 (will break the coloring logic if you use the
// last few rows for the map.)
void clear_asset_table() {
    // Fill it with the border color, duplicating it to all 4 2bit pieces
    for (i = 0; i != sizeof(assetTable); ++i) {
        assetTable[i] = currentMapBorderAsset;
    }
}

// This is an ascii space
#define BLANK_TILE 0x80
void fill_border_line() {
    for (j = 0; j != 12; ++j) {
        mapScreenBuffer[0x04 + (j<<1)] = tempChar1;
        mapScreenBuffer[0x04 + (j<<1) + 1] = tempChar1 + 1;
        mapScreenBuffer[0x24 + (j<<1)] = tempChar1 + 16;
        mapScreenBuffer[0x24 + (j<<1) + 1] = tempChar1 + 17;

    }
}

void update_asset_table_based_on_i_j() {
    tempChar9 = tilePalettes[tempChar3];
    // Row id
    tempChar6 = (((i + 1) >> 1) << 3) + ((j + 2) >> 1);
    if (((j + 2) & 0x01) == 0) {
        // Even/left
        if (((i + 1) & 0x01) == 0) {
            // top
            // tempChar9 <<= 0;
            assetTable[tempChar6] &= 0xfc;
        } else {
            //bottom
            tempChar9 <<= 4;
            assetTable[tempChar6] &= 0xcf;
        }
    } else {
        // Odd/right
        if (((i + 1) & 0x01) == 0) {
            // Top
            tempChar9 <<= 2;
            assetTable[tempChar6] &= 0xf3;
        } else {
            // Bottom 
            tempChar9 <<= 6;
            assetTable[tempChar6] &= 0x3f;
        }
    }
    assetTable[tempChar6] += tempChar9;
}


void draw_current_map_to_a_inline() {

    // Prepare to draw on the first nametable
    set_vram_update(NULL);
    clear_asset_table();
    // Make some tweaks for text areas outside the normal map

    // Border tile 
    tempChar1 = currentMapBorderTile;
    // Offset for current row (x12)
    tempChar4 = 0;

    // Set these outside the loop and leave em alone
    mapScreenBuffer[0x00] = BLANK_TILE;
    mapScreenBuffer[0x01] = BLANK_TILE;
    mapScreenBuffer[0x1e] = BLANK_TILE;
    mapScreenBuffer[0x1f] = BLANK_TILE;

    mapScreenBuffer[0x20] = BLANK_TILE;
    mapScreenBuffer[0x21] = BLANK_TILE;
    mapScreenBuffer[0x3e] = BLANK_TILE;
    mapScreenBuffer[0x3f] = BLANK_TILE;

    mapScreenBuffer[0x02] = tempChar1;
    mapScreenBuffer[0x03] = tempChar1+1;
    mapScreenBuffer[0x1c] = tempChar1;
    mapScreenBuffer[0x1d] = tempChar1 + 1;

    mapScreenBuffer[0x22] = tempChar1 + 16;
    mapScreenBuffer[0x23] = tempChar1 + 17;
    mapScreenBuffer[0x3c] = tempChar1 + 16;
    mapScreenBuffer[0x3d] = tempChar1 + 17;


    for (i = 0; i != 10; ++i) {

        for (j = 0; j != 12; ++j) {
            // position in the grid
            tempChar2 = tempChar4 + j;
            // value from the grid, expanded up to a "real" tile id
            tempChar3 = currentMap[tempChar2];
            tempChara = tempChar3;
            update_asset_table_based_on_i_j();
            tempChar3 &= 0x07;
            tempChara >>= 3;
            tempChar3 <<= 1;
            tempChara <<= 5;
            tempChar3 += tempChara;
            mapScreenBuffer[0x04 + (j<<1)] = tempChar3;
            mapScreenBuffer[0x04 + (j<<1) + 1] = tempChar3 + 1;
            mapScreenBuffer[0x24 + (j<<1)] = tempChar3 + 16;
            mapScreenBuffer[0x24 + (j<<1) + 1] = tempChar3 + 17;
            
        }
        tempChar4 += 12;

        vram_adr(0x2000 + 0x40 + (i<<6));
        vram_write(&mapScreenBuffer[0], 64);
    }

    vram_adr(0x2000);
    fill_border_line();
    vram_write(&mapScreenBuffer[0], 64);
    vram_adr(0x22c0);
    vram_write(&mapScreenBuffer[0], 64);

    vram_adr(0x23c0);
    vram_write(&assetTable[0], 64);
}