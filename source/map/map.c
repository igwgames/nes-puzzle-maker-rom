#include "source/map/map.h"
#include "source/map/load_map.h"
#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/game_states.h"
#include "source/globals.h"
#include "source/configuration/system_constants.h"
#include "source/graphics/palettes.h"
#include "source/graphics/hud.h"
#include "source/graphics/fade_animation.h"
#include "source/sprites/player.h"
#include "source/sprites/sprite_definitions.h"
#include "source/sprites/map_sprites.h"
#include "source/menus/error.h"

CODE_BANK(PRG_BANK_MAP_LOGIC);

ZEROPAGE_DEF(unsigned char, playerOverworldPosition);
ZEROPAGE_DEF(int, xScrollPosition);
ZEROPAGE_DEF(int, yScrollPosition);

unsigned char currentMap[120];
unsigned char currentMapOrig[120];

unsigned char assetTable[64];

unsigned char currentMapSpriteData[(16 * MAP_MAX_SPRITES)];
unsigned char currentMapTileData[32];

ZEROPAGE_DEF(unsigned char, currentGameStyle);

// unsigned char currentMapSpritePersistance[64];

unsigned char mapScreenBuffer[0x5c];


void init_map() {

    // Do some trickery to make the HUD show up at the top of the screen, with the map slightly below.
    scroll(0, 240-HUD_PIXEL_HEIGHT);
    set_mirroring(MIRROR_MODE_VERTICAL);

}

// Reusing a few temporary vars for the sprite function below.
#define currentValue tempInt1
#define spritePosition tempChar4
#define spriteDefinitionIndex tempChar5
#define mapSpriteDataIndex tempChar6
#define tempArrayIndex tempInt3
#define editorSelectedTileObject tempChar7
#define editorAttrX tempChar8
#define editorAttrY tempChar9
#define tempEditorTile tempChar2

// Load the sprites from the current map
void load_sprites() {
    // Do nothing; part of map.
}

// Clears the asset table. Set containsHud to 1 to set the HUD bytes to use palette 4 (will break the coloring logic if you use the
// last few rows for the map.)
void clear_asset_table(containsHud) {
    // Fill it with the border color, duplicating it to all 4 2bit pieces
    tempChara = tilePalettes[currentMapBorderTile >> 1];
    tempChara += (tempChara << 2);
    tempChara += (tempChara << 4);
    // Loop over assetTable to clear it out. 
    for (i = 0; i != sizeof(assetTable) - 8; ++i) {
        assetTable[i] = tempChara;
    }
    // The last row of the asset table uses the 4th palette to show the HUD correctly.
    for (; i != sizeof(assetTable); ++i) {
        assetTable[i] = containsHud == 0 ? tempChara : 0xff;
    }
}

// Loads the assets from assetTable (for the row *ending* with j) into mapScreenBuffer
// at tempArrayIndex. 
void load_palette_to_map_screen_buffer(int attributeTableAdr) {
    mapScreenBuffer[tempArrayIndex++] = MSB(attributeTableAdr + j - 7) | NT_UPD_HORZ;
    mapScreenBuffer[tempArrayIndex++] = LSB(attributeTableAdr + j - 7);
    mapScreenBuffer[tempArrayIndex++] = 8;

    // Using an unrolled loop to save a bit of RAM - not like we need it really.
    mapScreenBuffer[tempArrayIndex++] = assetTable[j-7];
    mapScreenBuffer[tempArrayIndex++] = assetTable[j-6];
    mapScreenBuffer[tempArrayIndex++] = assetTable[j-5];
    mapScreenBuffer[tempArrayIndex++] = assetTable[j-4];
    mapScreenBuffer[tempArrayIndex++] = assetTable[j-3];
    mapScreenBuffer[tempArrayIndex++] = assetTable[j-2];
    mapScreenBuffer[tempArrayIndex++] = assetTable[j-1];
    mapScreenBuffer[tempArrayIndex++] = assetTable[j];
    mapScreenBuffer[tempArrayIndex++] = NT_UPD_EOF;
}

// Now based on where we are in the map, shift them appropriately.
// This builds up the palette bytes - which comprise of 2 bits per 16x16 tile. It's a bit confusing...
void update_asset_table_based_on_current_value(unsigned char reverseAttributes) {
    if ((i & 0x01) == 0) {
        // Even/left
        if (((i >> 3) & 0x01) == reverseAttributes) {
            // top
            currentValue >>= 6;
        } else {
            //bottom
            currentValue >>= 2;
        }
    } else {
        // Odd/right
        if (((i >> 3) & 0x01) == reverseAttributes) {
            // Top
            currentValue >>= 4;
        } else {
            // Bottom 
            currentValue >>= 0;
        }
    }
    assetTable[j] += currentValue;
}


// We need to reuse some variables here to save on memory usage. So, use #define to give them readable names.
// Note that this is ONLY a rename; if something relies on the original variable, that impacts this one too.
#define currentMemoryLocation tempInt2
// NOTE: tempChar1-tempChar3 are in use by update_player_sprite, which we call here. (Confusing, I know...)
#define bufferIndex tempChar8 
#define otherLoopIndex tempChar9


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
    clear_asset_table(0);
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
            update_asset_table_based_on_i_j();
            if (tempChar3 < 8) {
                tempChar3 <<= 1;
            } else {
                tempChar3 -= 8;
                tempChar3 <<= 1;
                tempChar3 += 32;
            }
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

// A quick, low-tech glamour-free way to transition between screens.
void do_fade_screen_transition() {
    load_map();
    load_sprites();
    clear_asset_table(1);
    fade_out_fast();

/*    
    // Now that the screen is clear, migrate the player's sprite a bit..
    if (playerDirection == SPRITE_DIRECTION_LEFT) {
        playerXPosition = (SCREEN_EDGE_RIGHT << PLAYER_POSITION_SHIFT);
    } else if (playerDirection == SPRITE_DIRECTION_RIGHT) {
        playerXPosition = (SCREEN_EDGE_LEFT << PLAYER_POSITION_SHIFT);
    } else if (playerDirection == SPRITE_DIRECTION_UP) {
        playerYPosition = (SCREEN_EDGE_BOTTOM << PLAYER_POSITION_SHIFT);
    } else if (playerDirection == SPRITE_DIRECTION_DOWN) {
        playerYPosition = (SCREEN_EDGE_TOP << PLAYER_POSITION_SHIFT);
    }
    */
    // Actually move the sprite too, since otherwise this won't happen until after we un-blank the screen.
    banked_call(PRG_BANK_PLAYER_SPRITE, update_player_sprite);

    // Draw the updated map to the screen...
    ppu_off();
    draw_current_map_to_a_inline();
    ppu_on_all();
    
    // Update sprites once to make sure we don't show a flash of the old sprite positions.
    banked_call(PRG_BANK_MAP_SPRITES, update_map_sprites);
    fade_in_fast();
    // Aand we're back!
    gameState = GAME_STATE_RUNNING;
}


// TODO: May want to move this into the kernel; reproduced in a couple places now
void put_map_str(unsigned int adr, const char* str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)+0x60);//-0x20 because ASCII code 0x20 is placed in tile 80 of the CHR
	}
}