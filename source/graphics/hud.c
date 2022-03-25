#include "source/neslib_asm/neslib.h"
#include "source/graphics/hud.h"
#include "source/map/map.h"
#include "source/menus/text_helpers.h"
#include "source/globals.h"

#pragma code-name ("CODE")
#pragma rodata-name ("CODE")

ZEROPAGE_DEF(unsigned char, editorSelectedTileId);

#define tempTileId tempChar1
#define tempTileIndex tempChar2

// Draw the "hud" at the top of the screen, with its border and attributes. Also the level if the user turned
// that feature on.
void draw_hud() {
    vram_adr(NAMETABLE_A + HUD_POSITION_START);
    for (i = 0; i != 160; ++i) {
        vram_put(HUD_TILE_BLANK);
    }
    vram_put(HUD_TILE_BORDER_BL);
    for (i = 0; i != 30; ++i) {
        vram_put(HUD_TILE_BORDER_HORIZONTAL);
    }
    vram_put(HUD_TILE_BORDER_BR);

    vram_adr(NAMETABLE_A + HUD_ATTRS_START);
    for (i = 0; i != 16; ++i) {
        vram_put(0xff);
    }

    set_vram_update(NULL);

    vram_adr(NAMETABLE_A + HUD_POSITION_START + 0x22);
    for (i = 0; i != 0x1c; ++i) {
        vram_put(gameName[i] + 0x60);
    }
    if (enableLevelShow) {
        vram_adr(NAMETABLE_A + HUD_POSITION_START + 0x82);
        vram_put('L' + 0x60);
        vram_put('e' + 0x60);
        vram_put('v' + 0x60);
        vram_put('e' + 0x60);
        vram_put('l' + 0x60);
        vram_put(':' + 0x60);
    }
}

// Draw a number to "screenBuffer". Isolated so we do the /10 and %10 in one spot
void draw_num_to_sb(unsigned char num) {
    screenBuffer[i++] = (num / 10) + '0' + 0x60;
    screenBuffer[i++] = (num % 10) + '0' + 0x60;

}

// Do an ad-hoc update to the hud without resetting the screen. Handles coin/box count, current level, etc.
void update_hud() {
    i = 0;
    switch (currentGameStyle) {
        case GAME_STYLE_MAZE:
            break;
        case GAME_STYLE_COIN:
            for (j = 0; j != 16; ++j) {
                if (tileCollisionTypes[j] == TILE_COLLISION_COLLECTABLE) {
                    tempTileIndex = j;
                    break;
                }
            }
            tempTileId = (tempTileIndex < 8) ? (tempTileIndex << 1) : (((tempTileIndex - 8) << 1) + 32);
            i = 0;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START);
            screenBuffer[i++] = 2;
            screenBuffer[i++] = tempTileId;
            screenBuffer[i++] = tempTileId+1;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 32) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 32);
            screenBuffer[i++] = 2;
            screenBuffer[i++] = tempTileId+16;
            screenBuffer[i++] = tempTileId+17;
            screenBuffer[i++] = MSB(NAMETABLE_A + 0x03f5);
            screenBuffer[i++] = LSB(NAMETABLE_A + 0x03f5);
            screenBuffer[i++] = (tilePalettes[tempTileIndex] << 6) | 0x3f;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 62) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 62);
            screenBuffer[i++] = 5;
            draw_num_to_sb(playerCollectableCount);
            screenBuffer[i++] = '/' + 0x60;
            draw_num_to_sb(totalCollectableCount);

            break;
        case GAME_STYLE_CRATES:
            for (j = 0; j != 16; ++j) {
                if (tileCollisionTypes[j] == TILE_COLLISION_CRATE) {
                    tempTileIndex = j;
                    break;
                }
            }
            tempTileId = (tempTileIndex < 8) ? (tempTileIndex << 1) : (((tempTileIndex - 8) << 1) + 32);
            i = 0;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START);
            screenBuffer[i++] = 2;
            screenBuffer[i++] = tempTileId;
            screenBuffer[i++] = tempTileId+1;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 32) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 32);
            screenBuffer[i++] = 2;
            screenBuffer[i++] = tempTileId+16;
            screenBuffer[i++] = tempTileId+17;
            screenBuffer[i++] = MSB(NAMETABLE_A + 0x03f5);
            screenBuffer[i++] = LSB(NAMETABLE_A + 0x03f5);
            screenBuffer[i++] = (tilePalettes[tempTileIndex] << 6) | 0x3f;
            screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 62) | NT_UPD_HORZ;
            screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 62);
            screenBuffer[i++] = 5;
            draw_num_to_sb(playerCrateCount);
            screenBuffer[i++] = '/' + 0x60;
            draw_num_to_sb(totalCrateCount);

            break;
    }

    if (enableLevelShow) {
        screenBuffer[i++] = MSB(NAMETABLE_A + HUD_POSITION_START + 0x89) | NT_UPD_HORZ;
        screenBuffer[i++] = LSB(NAMETABLE_A + HUD_POSITION_START + 0x89);
        screenBuffer[i++] = 5;
        draw_num_to_sb(currentLevelId + 1);
        screenBuffer[i++] = '/' + 0x60;
        draw_num_to_sb(totalGameLevels);
    }
    screenBuffer[i++] = NT_UPD_EOF;
    set_vram_update(screenBuffer);

}