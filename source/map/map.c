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

unsigned char currentMap[64];

unsigned char assetTable[0x38];

unsigned char currentMapSpriteData[(16 * MAP_MAX_SPRITES)];
unsigned char currentMapTileData[32];

// unsigned char currentMapSpritePersistance[64];

unsigned char mapScreenBuffer[0x55];


void init_map() {
    // Make sure we're looking at the right sprite and chr data, not the ones for the menu.
    set_chr_bank_0(CHR_BANK_TILES);
    set_chr_bank_1(CHR_BANK_SPRITES);

    // Also set the palettes to the in-game palettes.
    pal_bg(mainBgPalette);
    pal_spr(mainSpritePalette);

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

// Load the sprites from the current map
void load_sprites() {
    // Do nothing; part of map.
}

// Clears the asset table. Set containsHud to 1 to set the HUD bytes to use palette 4 (will break the coloring logic if you use the
// last few rows for the map.)
void clear_asset_table(containsHud) {
    // Loop over assetTable to clear it out. 
    for (i = 0; i != sizeof(assetTable) - 8; ++i) {
        assetTable[i] = 0x00;
    }
    // The last row of the asset table uses the 4th palette to show the HUD correctly.
    for (; i != sizeof(assetTable); ++i) {
        assetTable[i] = containsHud == 0 ? 0x00 : 0xff;
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


// reverseAttributes: If set to 1, this will flip which bits are used for the top and the bottom palette in the attribute table.
//                    This allows us to correctly draw starting on an odd-numbered row (such as at the start of our HUD.) 
void draw_current_map_to_nametable(int nametableAdr, int attributeTableAdr, unsigned char reverseAttributes) {

    // Prepare to draw on the first nametable
    set_vram_update(NULL);
    clear_asset_table(0);
    // Make some tweaks for text areas outside the normal map
    for (i = 0; i != 7; ++i) {
        assetTable[i] = 0xff;
        assetTable[i+0x28] = 0xff;
    }

    bufferIndex = 0;
    for (i = 0; i != 0x55; ++i) {
        // FIXME: Probably wanna make this a constant, and put it in a known place.
        mapScreenBuffer[i] = 0xe2;
    }

    if (!reverseAttributes) {
        j = 9;
    } else {
        j = 7;
    }
    tempArrayIndex = NAMETABLE_UPDATE_PREFIX_LENGTH;
    for (i = 0; i != 64; ++i) {
        // Look up tile id from our lookup table based on the id in the map - the data stored in currentMap is aleady an array index, so just shift to what we wanna look up.
        currentValue = currentMapTileData[currentMap[i]+TILE_DATA_LOOKUP_OFFSET_ID];

        if (bufferIndex == 0) {
            // FIXME: Constant would be good here (8 is the number of tiles over from the lefthand side, 64 is one 16x16 row of tiles)
            currentMemoryLocation = nametableAdr + 8 + 128 + ((i & 0x38) << 3) + ((i & 0x07) << 1);
        }

        mapScreenBuffer[tempArrayIndex] = currentValue;
        mapScreenBuffer[tempArrayIndex + 1] = currentValue + 1;
        mapScreenBuffer[tempArrayIndex + 32] = currentValue + 16;
        mapScreenBuffer[tempArrayIndex + 33] = currentValue + 17;

        // okay, now we have to update the byte for palettes. This is going to look a bit messy...
        // Start with the top 2 bytes
        currentValue = (currentMapTileData[currentMap[i] + TILE_DATA_LOOKUP_OFFSET_PALETTE]) << 6;

        // Update where we are going to update with the palette data, which we store in the buffer.
        // Flip it every other row, since attribute tables are 32x32, not 16x16
        if ((i % 16) == 8) 
			j -= 4;
        else if ((i % 8) == 0 && i != 0) 
            j += 4;
		if ((i & 0x01) == 0) 
			j++;

        // Now based on where we are in the map, shift them appropriately.
        // This builds up the palette bytes - which comprise of 2 bits per 16x16 tile. It's a bit confusing...
        update_asset_table_based_on_current_value(reverseAttributes);

        // Every 16 frames, write the buffered data to the screen and start anew.
        ++bufferIndex;
        tempArrayIndex += 2;
        if (bufferIndex == 8) {
            bufferIndex = 0;
            tempArrayIndex = NAMETABLE_UPDATE_PREFIX_LENGTH;
            // Bunch of messy-looking stuff that tells neslib where to write this to the nametable, and how.
            mapScreenBuffer[0] = MSB(currentMemoryLocation) | NT_UPD_HORZ;
            mapScreenBuffer[1] = LSB(currentMemoryLocation);
            mapScreenBuffer[2] = 65;
            mapScreenBuffer[64 + NAMETABLE_UPDATE_PREFIX_LENGTH + 1] = NT_UPD_EOF;
            set_vram_update(mapScreenBuffer);
            ppu_wait_nmi();
            if (xScrollPosition != -1) {
                split_y(xScrollPosition, yScrollPosition);
            }
            set_vram_update(NULL);

        }
    }
    // Clear the rest of the screen... one row at a time.
    // 64 tiles in one row.
    for (j = 0; j != 3; ++j) {
        currentMemoryLocation += 64;
        for (i = 0; i != 0x55; ++i) {
            // FIXME: Probably wanna make this a constant, and put it in a known place.
            mapScreenBuffer[i] = 0xe2;
        }
        mapScreenBuffer[0] = MSB(currentMemoryLocation) | NT_UPD_HORZ;
        mapScreenBuffer[1] = LSB(currentMemoryLocation);
        mapScreenBuffer[2] = 65;
        mapScreenBuffer[64 + NAMETABLE_UPDATE_PREFIX_LENGTH + 1] = NT_UPD_EOF;
        set_vram_update(mapScreenBuffer);
        ppu_wait_nmi();
        if (xScrollPosition != -1) {
            split_y(xScrollPosition, yScrollPosition);
        }
        set_vram_update(NULL);
    }

    // Very first rows too
    // TODO: This code is duplicated a lot... probably makes sense to toss to a function.
    currentMemoryLocation = nametableAdr;
    for (j = 0; j != 2; ++j) {

        mapScreenBuffer[0] = MSB(currentMemoryLocation) | NT_UPD_HORZ;
        mapScreenBuffer[1] = LSB(currentMemoryLocation);
        mapScreenBuffer[2] = 65;
        mapScreenBuffer[64 + NAMETABLE_UPDATE_PREFIX_LENGTH + 1] = NT_UPD_EOF;
        set_vram_update(mapScreenBuffer);
        ppu_wait_nmi();
        if (xScrollPosition != -1) {
            split_y(xScrollPosition, yScrollPosition);
        }
        set_vram_update(NULL);
        currentMemoryLocation += 64;
    }

    // Okay, one last blip for right before the map starts.
    mapScreenBuffer[0] = MSB(currentMemoryLocation) | NT_UPD_HORZ;
    mapScreenBuffer[1] = LSB(currentMemoryLocation);
    mapScreenBuffer[2] = 8;
    mapScreenBuffer[7 + NAMETABLE_UPDATE_PREFIX_LENGTH + 1] = NT_UPD_EOF;
    set_vram_update(mapScreenBuffer);
    ppu_wait_nmi();
    if (xScrollPosition != -1) {
        split_y(xScrollPosition, yScrollPosition);
    }
    set_vram_update(NULL);



    // Draw the palette that we built up above.
    // Start by copying it into mapScreenBuffer, so we can tell neslib where this lives.
    for (i = 0; i != 0x38; ++i) {
        mapScreenBuffer[NAMETABLE_UPDATE_PREFIX_LENGTH + i] = assetTable[i];
    }
    mapScreenBuffer[0] = MSB(attributeTableAdr) | NT_UPD_HORZ;
    mapScreenBuffer[1] = LSB(attributeTableAdr);
    mapScreenBuffer[2] = 0x38;
    mapScreenBuffer[0x3b] = NT_UPD_EOF;
    set_vram_update(mapScreenBuffer);
    ppu_wait_nmi();
    if (xScrollPosition != -1) {
        split_y(xScrollPosition, yScrollPosition);
    }

    set_vram_update(NULL);
    
}

void draw_current_map_to_a() {
    clear_asset_table(1);
    xScrollPosition = -1;
    yScrollPosition = 0;
    draw_current_map_to_nametable(NAMETABLE_A, NAMETABLE_A_ATTRS, 0);
}

void draw_current_map_to_b() {
    clear_asset_table(0);
    xScrollPosition = -1;
    yScrollPosition = 0;
    draw_current_map_to_nametable(NAMETABLE_B, NAMETABLE_B_ATTRS, 0);
}

void draw_current_map_to_c() {
    clear_asset_table(0);
    xScrollPosition = -1;
    yScrollPosition = 0;
    draw_current_map_to_nametable(NAMETABLE_C, NAMETABLE_C_ATTRS, 0);
}

void draw_current_map_to_d() {
    clear_asset_table(0);
    xScrollPosition = -1;
    yScrollPosition = 0;
    draw_current_map_to_nametable(NAMETABLE_D, NAMETABLE_D_ATTRS, 0);
}

// A quick, low-tech glamour-free way to transition between screens.
void do_fade_screen_transition() {
    load_map();
    load_sprites();
    clear_asset_table(1);
    fade_out_fast();
    
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
    // Actually move the sprite too, since otherwise this won't happen until after we un-blank the screen.
    banked_call(PRG_BANK_PLAYER_SPRITE, update_player_sprite);

    // Draw the updated map to the screen...
    draw_current_map_to_nametable(NAMETABLE_A, NAMETABLE_A_ATTRS, 0);
    
    // Update sprites once to make sure we don't show a flash of the old sprite positions.
    banked_call(PRG_BANK_MAP_SPRITES, update_map_sprites);
    fade_in_fast();
    // Aand we're back!
    gameState = GAME_STATE_RUNNING;
}

void update_editor_map_tile() {
    // Tile id is editorSelectedTileId, 0-7 or known constant
    // map is currentMapData
    // Need to update nametable too, which is of course the finnicky bit
    if (editorSelectedTileId < 8) {
        // Reusing rawX
        currentMap[playerGridPosition] = editorSelectedTileId;
        currentValue = NTADR_A(((playerGridPosition & 0x07)<<1) + 8, ((playerGridPosition & 0x38) >> 2) + 4);
        editorSelectedTileObject = currentMapTileData[(editorSelectedTileId << 2) + TILE_DATA_LOOKUP_OFFSET_ID];

        screenBuffer[0] = MSB(currentValue);
        screenBuffer[1] = LSB(currentValue);
        screenBuffer[2] = editorSelectedTileObject;
        ++currentValue;
        screenBuffer[3] = MSB(currentValue);
        screenBuffer[4] = LSB(currentValue);
        screenBuffer[5] = editorSelectedTileObject+1;
        currentValue += 31;
        screenBuffer[6] = MSB(currentValue);
        screenBuffer[7] = LSB(currentValue);
        screenBuffer[8] = editorSelectedTileObject+16;
        ++currentValue;
        screenBuffer[9] = MSB(currentValue);
        screenBuffer[10] = LSB(currentValue);
        screenBuffer[11] = editorSelectedTileObject+17;

        // Next, figure out the palette bits...
        editorSelectedTileObject = currentMapTileData[(editorSelectedTileId << 2) + TILE_DATA_LOOKUP_OFFSET_PALETTE];

        // Raw X / Y positions on-screen
        editorAttrX = (playerGridPosition & 0x07) + 4;
        editorAttrY = ((playerGridPosition & 0x38) >> 3) + 2;

        // Calculate raw attr table address
        currentValue = ((editorAttrY >> 1) << 3) + (editorAttrX >> 1);

        if (editorAttrX & 0x01) {
            if (editorAttrY & 0x01) {
                assetTable[currentValue] &= 0x3f;
                assetTable[currentValue] |= (editorSelectedTileObject) << 6;
            } else {
                assetTable[currentValue] &= 0xf3;
                assetTable[currentValue] |= (editorSelectedTileObject) << 2;
            }
        } else {
            if (editorAttrY & 0x01) {
                assetTable[currentValue] &= 0xcf;
                assetTable[currentValue] |= (editorSelectedTileObject) << 4;
            } else {
                assetTable[currentValue] &= 0xfc;
                assetTable[currentValue] |= (editorSelectedTileObject);
            }
        }

        screenBuffer[14] = assetTable[currentValue];
        currentValue += NAMETABLE_A + 0x3c0;
        screenBuffer[12] = MSB(currentValue);
        screenBuffer[13] = LSB(currentValue);

        screenBuffer[15] = NT_UPD_EOF;
        set_vram_update(screenBuffer);
        ppu_wait_nmi();
        set_vram_update(NULL);


    } else if (editorSelectedTileId == TILE_EDITOR_POSITION_INFO) {
        // FIXME Remove
    } else if (editorSelectedTileId == TILE_EDITOR_POSITION_PLAYER) {
        // FIXME Player updating
        // FIXME Also need initial player placement loading
    }
}

// TODO: May want to move this into the kernel; reproduced in a couple places now
void put_map_str(unsigned int adr, const char* str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)+0x60);//-0x20 because ASCII code 0x20 is placed in tile 80 of the CHR
	}
}


void draw_editor_help() {


    vram_adr(NTADR_A(2,1));
    vram_put('M' + 0x60);
    vram_put('a' + 0x60);
    vram_put('p' + 0x60);
    vram_put(' ' + 0x60);
    vram_put('0' + 0x60);

    vram_put('0' + (currentLevelId + 1) + 0x60);
    vram_put(' ' + 0x60);
    vram_put('/' + 0x60);
    vram_put(' ' + 0x60);
    vram_put('0' + 0x60);
    vram_put('0' + (MAPS_IN_GAME) + 0x60);

    put_map_str(NTADR_A(2, 21), "Start:  Edit Game Info");
    put_map_str(NTADR_A(2, 22), "Select: Switch Tile/Action");
}