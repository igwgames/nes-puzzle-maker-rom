#include "source/c/neslib.h"
#include "source/c/mapper.h"
#include "source/c/library/user_data.h"
#include "source/c/graphics/hud.h"
#include "source/c/map/map.h"
#include "source/c/menus/text_helpers.h"
#include "source/c/globals.h"

#pragma code-name ("PLAYER")
#pragma rodata-name ("PLAYER")

ZEROPAGE_DEF(unsigned char, editorSelectedTileId);

#define tempTileId tempChar1
#define tempTileIndex tempChar2
#define tempTileId2 tempChar3

// Draw the "hud" at the top of the screen, with its border and attributes. Also the level if the user turned
// that feature on.
void draw_hud() {

    load_hud_vram(BANK_PLAYER);

    set_vram_update(NULL);

    if (showGameTitle) {
        vram_adr(NAMETABLE_A + HUD_POSITION_START + 0x22);
        load_gamename_to_buffer(BANK_PLAYER);
        for (i = 0; i != 0x1c; ++i) {
            vram_put(userDataBuffer[i] + 0x60);
        }
    }

    if (!enableLevelShow) {
        // Have to override the section that says "level:" in this case.
        vram_adr(NAMETABLE_A + HUD_LEVEL_TEXT_START);
        for (i = 0; i < 6; ++i) {
            vram_put(' ' + 0x60);
        }
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
    if (showGoal) {
        switch (currentGameStyle) {
            case GAME_STYLE_MAZE:
                for (j = 0; j !=32; ++j) {
                    if (tileCollisionTypes[j] == TILE_COLLISION_LEVEL_END) {
                        tempTileIndex = j;
                        break;
                    }
                }
                screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 63) | NT_UPD_HORZ;
                screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 63);
                screenBuffer[i++] = 4;
                screenBuffer[i++] = 'E' + 0x60;
                screenBuffer[i++] = 'x' + 0x60;
                screenBuffer[i++] = 'i' + 0x60;
                screenBuffer[i++] = 't' + 0x60;

                break;
            case GAME_STYLE_COIN:
                for (j = 0; j != 32; ++j) {
                    if (tileCollisionTypes[j] == TILE_COLLISION_COLLECTABLE) {
                        tempTileIndex = j;
                        break;
                    }
                }
                screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 62) | NT_UPD_HORZ;
                screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 62);
                screenBuffer[i++] = 5;
                draw_num_to_sb(playerCollectableCount);
                screenBuffer[i++] = '/' + 0x60;
                draw_num_to_sb(totalCollectableCount);

                break;
            case GAME_STYLE_CRATES:
                for (j = 0; j != 32; ++j) {
                    if (tileCollisionTypes[j] == TILE_COLLISION_CRATE) {
                        tempTileIndex = j;
                        break;
                    }
                }
                i = 0;
                screenBuffer[i++] = MSB(NAMETABLE_A + HUD_HEART_START + 62) | NT_UPD_HORZ;
                screenBuffer[i++] = LSB(NAMETABLE_A + HUD_HEART_START + 62);
                screenBuffer[i++] = 5;
                draw_num_to_sb(playerCrateCount);
                screenBuffer[i++] = '/' + 0x60;
                draw_num_to_sb(totalCrateCount);

                break;
        }

        // tempTileId = (tempTileIndex < 8) ? (tempTileIndex << 1) : (((tempTileIndex - 8) << 1) + 32);
        tempTileId = tempTileIndex;
        tempTileId2 = tempTileId;
        tempTileId &= 0x07;
        tempTileId2 >>= 3;
        tempTileId <<= 1;
        tempTileId2 <<= 5;
        tempTileId += tempTileId2;

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
        tempTileId = (user_get_hud_palette_for_goal(BANK_PLAYER) & 0x3f) | (tilePalettes[tempTileIndex] << 6);
        screenBuffer[i++] = MSB(NAMETABLE_A + 0x03f5);
        screenBuffer[i++] = LSB(NAMETABLE_A + 0x03f5);
        screenBuffer[i++] = tempTileId;
    }



    if (enableLevelShow) {
        screenBuffer[i++] = MSB(NAMETABLE_A + HUD_POSITION_START + 0x89) | NT_UPD_HORZ;
        screenBuffer[i++] = LSB(NAMETABLE_A + HUD_POSITION_START + 0x89);
        screenBuffer[i++] = 5;
        draw_num_to_sb(currentLevelId + 1);
        screenBuffer[i++] = '/' + 0x60;
        draw_num_to_sb(totalGameLevels);
    }
    if (enableKeyCount) {
        screenBuffer[i++] = MSB(NAMETABLE_A + HUD_POSITION_START + 0x63) | NT_UPD_HORZ;
        screenBuffer[i++] = LSB(NAMETABLE_A + HUD_POSITION_START + 0x63);
        screenBuffer[i++] = 2;
        draw_num_to_sb(keyCount);

    }
    screenBuffer[i++] = NT_UPD_EOF;
    set_vram_update(screenBuffer);

}