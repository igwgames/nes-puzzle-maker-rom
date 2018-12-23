#include "source/neslib_asm/neslib.h"
#include "source/sprites/player.h"
#include "source/library/bank_helpers.h"
#include "source/globals.h"
#include "source/map/map.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/sprites/collision.h"
#include "source/sprites/sprite_definitions.h"
#include "source/sprites/map_sprites.h"
#include "source/menus/error.h"
#include "source/graphics/hud.h"
#include "source/graphics/game_text.h"
#include "Source/game_data/game_data.h"
#include "source/menus/error.h"
#include "source/map/load_map.h"

CODE_BANK(PRG_BANK_PLAYER_SPRITE);

// Some useful global variables
ZEROPAGE_DEF(int, playerXPosition);
ZEROPAGE_DEF(int, playerYPosition);
// FIXME: axe
ZEROPAGE_DEF(int, playerXVelocity);
ZEROPAGE_DEF(int, playerYVelocity);
ZEROPAGE_DEF(unsigned char, playerGridPosition);
ZEROPAGE_DEF(unsigned char, movementInProgress);
ZEROPAGE_DEF(unsigned char, playerControlsLockTime);
ZEROPAGE_DEF(unsigned char, playerInvulnerabilityTime);
ZEROPAGE_DEF(unsigned char, playerDirection);

// Huge pile of temporary variables
#define rawXPosition tempChar1
#define rawYPosition tempChar2
#define rawTileId tempChar3
#define collisionTempDirection tempChar4
#define collisionTempX tempChar4
#define collisionTempY tempChar5
#define collisionTempYBottom tempChar7

#define collisionTempTileId tempChar8
#define collisionTempTileObject tempChar9


#define collisionTempValue tempInt1
#define tempSpriteCollisionY tempInt2

#define collisionTempXInt tempInt3
#define collisionTempYInt tempInt4

// TODO: axe all text communication stuff.
 const unsigned char* introductionText = 
                                "Welcome to nes-starter-kit! I " 
                                "am an NPC.                    "
                                "                              "

                                "Hope you're having fun!       "
                                "                              "
                                "- Chris";
const unsigned char* movedText = 
                                "Hey, you put me on another    "
                                "screen! Cool!";

// NOTE: This uses tempChar1 through tempChar3; the caller must not use these.
void update_player_sprite() {
    // Calculate the position of the player itself, then use these variables to build it up with 4 8x8 NES sprites.
    // FIXME: Space to space animations plox
    rawXPosition = (PLAY_AREA_LEFT + ((playerGridPosition & 0x07) << 4));
    rawYPosition = (PLAY_AREA_TOP + ((playerGridPosition & 0x38) << 1));
    rawTileId = PLAYER_SPRITE_TILE_ID + playerDirection;

    if (playerInvulnerabilityTime && frameCount & PLAYER_INVULNERABILITY_BLINK_MASK) {
        // If the player is invulnerable, we hide their sprite about half the time to do a flicker animation.
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);

    } else {
        oam_spr(rawXPosition, rawYPosition, rawTileId, 0x03, PLAYER_SPRITE_INDEX);
        oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition, rawTileId + 1, 0x03, PLAYER_SPRITE_INDEX+4);
        oam_spr(rawXPosition, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 16, 0x03, PLAYER_SPRITE_INDEX+8);
        oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 17, 0x03, PLAYER_SPRITE_INDEX+12);
    }

}

// FIXME: Case
void update_single_tile(unsigned char id, unsigned char newTile, unsigned char palette) {
    collisionTempValue = NTADR_A(((id & 0x07)<<1) + 8, ((id & 0x38) >> 2) + 4);
    screenBuffer[0] = MSB(collisionTempValue);
    screenBuffer[1] = LSB(collisionTempValue);
    screenBuffer[2] = newTile;
    ++collisionTempValue;
    screenBuffer[3] = MSB(collisionTempValue);
    screenBuffer[4] = LSB(collisionTempValue);
    screenBuffer[5] = newTile+1;
    collisionTempValue += 31;
    screenBuffer[6] = MSB(collisionTempValue);
    screenBuffer[7] = LSB(collisionTempValue);
    screenBuffer[8] = newTile+16;
    ++collisionTempValue;
    screenBuffer[9] = MSB(collisionTempValue);
    screenBuffer[10] = LSB(collisionTempValue);
    screenBuffer[11] = newTile+17;

    // Raw X / Y positions on-screen
    collisionTempX = (id & 0x07) + 4;
    collisionTempY = ((id & 0x38) >> 3) + 2;

    // Calculate raw attr table address
    collisionTempValue = ((collisionTempY >> 1) << 3) + (collisionTempX >> 1);

    if (collisionTempX & 0x01) {
        if (collisionTempY & 0x01) {
            assetTable[collisionTempValue] &= 0x3f;
            assetTable[collisionTempValue] |= (palette) << 6;
        } else {
            assetTable[collisionTempValue] &= 0xf3;
            assetTable[collisionTempValue] |= (palette) << 2;
        }
    } else {
        if (collisionTempY & 0x01) {
            assetTable[collisionTempValue] &= 0xcf;
            assetTable[collisionTempValue] |= (palette) << 4;
        } else {
            assetTable[collisionTempValue] &= 0xfc;
            assetTable[collisionTempValue] |= (palette);
        }
    }

    screenBuffer[14] = assetTable[collisionTempValue];
    collisionTempValue += NAMETABLE_A + 0x3c0;
    screenBuffer[12] = MSB(collisionTempValue);
    screenBuffer[13] = LSB(collisionTempValue);

    screenBuffer[15] = NT_UPD_EOF;
    set_vram_update(screenBuffer);
    ppu_wait_nmi();
    set_vram_update(NULL);
}


void handle_player_movement() {
    lastControllerState = controllerState;
    controllerState = pad_poll(0);

    // If Start is pressed now, and was not pressed before...
    if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
        gameState = GAME_STATE_PAUSED;
        return;
    }

    if (movementInProgress) {
        // One input at a time, bud...
        --movementInProgress;
        return;
    }
    
    rawTileId = playerGridPosition;

    if (controllerState & PAD_LEFT) {
        if ((rawTileId & 0x07) != 0) {
            rawTileId--;
            collisionTempDirection = PAD_LEFT;
        }
        // Graphical only
        playerDirection = SPRITE_DIRECTION_LEFT;
    } else if (controllerState & PAD_RIGHT) {
        if ((rawTileId & 0x07) != 7) {
            rawTileId++;
            collisionTempDirection = PAD_RIGHT;
        }
        // Graphical only
        playerDirection = SPRITE_DIRECTION_RIGHT;

    } else if (controllerState & PAD_UP) {
        if ((rawTileId & 0x38) != 0) {
            rawTileId -= 8;
            collisionTempDirection = PAD_UP;
        }
        // Graphical only
        playerDirection = SPRITE_DIRECTION_UP;

    } else if (controllerState & PAD_DOWN) {
        if ((rawTileId & 0x38) != 0x38) {
            rawTileId += 8;
            collisionTempDirection = PAD_DOWN;
        }
        // Graphical only
        playerDirection = SPRITE_DIRECTION_DOWN;

    }

    if (rawTileId == playerGridPosition) {
        // Ya didn't move...
        return; 
    }

    movementInProgress = PLAYER_TILE_MOVE_FRAMES;
    // TODO: Take special action based on the game type?
    switch (currentMapTileData[currentMap[rawTileId]+TILE_DATA_LOOKUP_OFFSET_COLLISION]) {
        // Ids are multiplied by 4, which is their index 
        case TILE_COLLISION_WALKABLE:
        case TILE_COLLISION_UNUSED:
        case TILE_COLLISION_GAP: // Hole for crate/ball - no action on its own
            // Walkable.. Go !
            break;
        case TILE_COLLISION_SOLID: // Solid 1
            // Nope, go back. These are solid.
            rawTileId = playerGridPosition;
            break;
        case TILE_COLLISION_CRATE:
            // So, we know that rawTileId is the crate we intend to move. Test if it can move anywhere, and if so, bunt it. If not... stop.
            switch (collisionTempDirection) {
                // TODO: This is ugly... what can we do to pretty it up?
                case PAD_RIGHT:
                    if ((rawTileId & 0x07) == 7) {
                        rawTileId = playerGridPosition;
                        break;
                    }
                    collisionTempTileId = currentMapTileData[currentMap[rawTileId+1]+TILE_DATA_LOOKUP_OFFSET_COLLISION];
                    if (collisionTempTileId == TILE_COLLISION_WALKABLE) {
                        // Do it
                        currentMap[rawTileId+1] = currentMap[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId+1] + TILE_DATA_LOOKUP_OFFSET_ID];
                        update_single_tile(rawTileId+1, collisionTempTileId, currentMapTileData[currentMap[rawTileId+1] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_ID];

                        update_single_tile(rawTileId, collisionTempTileId, currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);
                        sfx_play(SFX_CRATE_MOVE, SFX_CHANNEL_1);
                    } else if (collisionTempTileId == TILE_COLLISION_GAP) {
                        ++playerCrateCount;
                        ++gameCrates;
                        currentMap[rawTileId+1] = 0;

                        collisionTempTileId = currentMapTileData[currentMap[rawTileId+1] + TILE_DATA_LOOKUP_OFFSET_ID];
                        update_single_tile(rawTileId+1, collisionTempTileId, currentMapTileData[currentMap[rawTileId+1] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_ID];

                        update_single_tile(rawTileId, collisionTempTileId, currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);
                        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);
                    } else {
                        rawTileId = playerGridPosition;
                    }
                    break;
                case PAD_LEFT:
                    if ((rawTileId & 0x07) == 0) {
                        rawTileId = playerGridPosition;
                        break;
                    }
                    collisionTempTileId = currentMapTileData[currentMap[rawTileId-1]+TILE_DATA_LOOKUP_OFFSET_COLLISION];
                    if (collisionTempTileId == TILE_COLLISION_WALKABLE) {
                        // Do it
                        currentMap[rawTileId-1] = currentMap[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId-1] + TILE_DATA_LOOKUP_OFFSET_ID];
                        update_single_tile(rawTileId-1, collisionTempTileId, currentMapTileData[currentMap[rawTileId-1] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_ID];

                        update_single_tile(rawTileId, collisionTempTileId, currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);
                        sfx_play(SFX_CRATE_MOVE, SFX_CHANNEL_1);
                    } else if (collisionTempTileId == TILE_COLLISION_GAP) {
                        currentMap[rawTileId-1] = 0;
                        ++playerCrateCount;
                        ++gameCrates;

                        collisionTempTileId = currentMapTileData[currentMap[rawTileId-1] + TILE_DATA_LOOKUP_OFFSET_ID];
                        update_single_tile(rawTileId-1, collisionTempTileId, currentMapTileData[currentMap[rawTileId-1] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_ID];

                        update_single_tile(rawTileId, collisionTempTileId, currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);
                        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);

                    } else {
                        rawTileId = playerGridPosition;
                    }
                    break;
                case PAD_UP:
                    if ((rawTileId & 0x38) == 0) {
                        rawTileId = playerGridPosition;
                        break;
                    }
                    collisionTempTileId = currentMapTileData[currentMap[rawTileId-8]+TILE_DATA_LOOKUP_OFFSET_COLLISION];
                    if (collisionTempTileId == TILE_COLLISION_WALKABLE) {
                        // Do it
                        currentMap[rawTileId-8] = currentMap[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId-8] + TILE_DATA_LOOKUP_OFFSET_ID];
                        update_single_tile(rawTileId-8, collisionTempTileId, currentMapTileData[currentMap[rawTileId-8] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_ID];

                        update_single_tile(rawTileId, collisionTempTileId, currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);
                        sfx_play(SFX_CRATE_MOVE, SFX_CHANNEL_1);
                    } else if (collisionTempTileId == TILE_COLLISION_GAP) {
                        currentMap[rawTileId-8] = 0;
                        ++playerCrateCount;
                        ++gameCrates;

                        collisionTempTileId = currentMapTileData[currentMap[rawTileId-8] + TILE_DATA_LOOKUP_OFFSET_ID];
                        update_single_tile(rawTileId-8, collisionTempTileId, currentMapTileData[currentMap[rawTileId-8] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_ID];

                        update_single_tile(rawTileId, collisionTempTileId, currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);
                        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);

                    } else {
                        rawTileId = playerGridPosition;
                    }
                    break;
                case PAD_DOWN:
                    if ((rawTileId & 0x38) == 0x38) {
                        rawTileId = playerGridPosition;
                        break;
                    }
                    collisionTempTileId = currentMapTileData[currentMap[rawTileId+8]+TILE_DATA_LOOKUP_OFFSET_COLLISION];
                    if (collisionTempTileId == TILE_COLLISION_WALKABLE) {
                        // Do it
                        currentMap[rawTileId+8] = currentMap[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId+8] + TILE_DATA_LOOKUP_OFFSET_ID];
                        update_single_tile(rawTileId+8, collisionTempTileId, currentMapTileData[currentMap[rawTileId+8] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_ID];

                        update_single_tile(rawTileId, collisionTempTileId, currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);
                        sfx_play(SFX_CRATE_MOVE, SFX_CHANNEL_1);
                    } else if (collisionTempTileId == TILE_COLLISION_GAP) {
                        currentMap[rawTileId+8] = 0;
                        ++playerCrateCount;
                        ++gameCrates;

                        collisionTempTileId = currentMapTileData[currentMap[rawTileId+8] + TILE_DATA_LOOKUP_OFFSET_ID];
                        update_single_tile(rawTileId+8, collisionTempTileId, currentMapTileData[currentMap[rawTileId+8] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_ID];

                        update_single_tile(rawTileId, collisionTempTileId, currentMapTileData[currentMap[rawTileId] + TILE_DATA_LOOKUP_OFFSET_PALETTE]);
                        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);

                    } else {
                        rawTileId = playerGridPosition;
                    }
                    break;

                    
                default:
                    rawTileId = playerGridPosition;
                    break;
            }
            break;
        case TILE_COLLISION_COLLECTABLE:
            ++playerKeyCount;
            ++gameKeys;
            currentMap[rawTileId] = 0;
            update_single_tile(rawTileId, 0, currentMapTileData[TILE_DATA_LOOKUP_OFFSET_PALETTE]);
            sfx_play(SFX_HEART, SFX_CHANNEL_1);
            break;
        case TILE_COLLISION_LEVEL_END: // Level end!
            collisionTempTileId = 0;
            switch (currentGameData[GAME_DATA_OFFSET_GAME_STYLE]) {
                case GAME_STYLE_MAZE:
                    // Do nothing; you're just allowed to pass.
                    break;
                case GAME_STYLE_COIN:
                    for (i = 0; i != 64; ++i) {
                        if (currentMapTileData[currentMap[i] + TILE_DATA_LOOKUP_OFFSET_COLLISION] == TILE_COLLISION_COLLECTABLE) {
                            // Sorry, you didn't get em all. Plz try again.
                            collisionTempTileId = 1;
                        }
                    }
                    break;
                case GAME_STYLE_CRATES: 
                    for (i = 0; i != 64; ++i) {
                        if (currentMapTileData[currentMap[i] + TILE_DATA_LOOKUP_OFFSET_COLLISION] == TILE_COLLISION_GAP) {
                            // Sorry, you didn't get em all. Plz try again.
                            collisionTempTileId = 1;
                        }
                    }
                    break;
                default:
                    break;
            }
            if (!collisionTempTileId) {
                ++currentLevelId;
                if (currentLevelId == MAX_GAME_LEVELS) {
                    gameState = GAME_STATE_CREDITS;
                } else {
                    gameState = GAME_STATE_LOAD_LEVEL;
                    sfx_play(SFX_WIN, SFX_CHANNEL_1);
                }
                return;
            }
            break;
        default:
            // Stop you when you hit an unknown tile... idk seems better than walking?
            rawTileId = playerGridPosition;
            break;
    }
    playerGridPosition = rawTileId;



}

void test_player_tile_collision() {
}

#define currentMapSpriteIndex tempChar1
void handle_player_sprite_collision() {
}

void handle_editor_input() {
    lastControllerState = controllerState;
    controllerState = pad_poll(0);
    if (controllerState & PAD_SELECT && !(lastControllerState & PAD_SELECT)) {
        sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
        if (editorSelectedTileId == 7) { // End of regular tiles
            editorSelectedTileId = TILE_EDITOR_POSITION_PLAYER;
        } else if (editorSelectedTileId == TILE_EDITOR_POSITION_PLAYER) {
            if (currentLevelId < (MAX_GAME_LEVELS-1)) {
                editorSelectedTileId = TILE_EDITOR_POSITION_RIGHT;
            } else {
                if (currentLevelId > 0) {
                    editorSelectedTileId = TILE_EDITOR_POSITION_LEFT;
                } else {
                    editorSelectedTileId = 0;
                }
            }
        } else if (editorSelectedTileId == TILE_EDITOR_POSITION_RIGHT) {
            if (currentLevelId > 0) {
                editorSelectedTileId = TILE_EDITOR_POSITION_LEFT;
            } else {
                editorSelectedTileId = 0;
            }
        } else if (editorSelectedTileId == TILE_EDITOR_POSITION_LEFT) {
            editorSelectedTileId = 0;
        } else {
            ++editorSelectedTileId;
        }
    }

    if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
        save_map();
        sfx_play(SFX_MENU_OPEN, SFX_CHANNEL_4);
        gameState = GAME_STATE_EDITOR_INFO;
        return;
    }

    if (movementInProgress)
        --movementInProgress;

    if (!movementInProgress) { 
        if (controllerState & PAD_A) {

            banked_call(PRG_BANK_MAP_LOGIC, update_editor_map_tile);
        }

        if (controllerState & PAD_RIGHT) {
            if ((playerGridPosition & 0x07) == 0x07) {
                playerGridPosition -= 7;
            } else {
                playerGridPosition++;
            }
            movementInProgress = PLAYER_TILE_MOVE_FRAMES;
        }
        if (controllerState & PAD_LEFT) {
            if ((playerGridPosition & 0x07) == 0x00) {
                playerGridPosition += 7;
            } else {
                playerGridPosition--;
            }
            movementInProgress = PLAYER_TILE_MOVE_FRAMES;
        }

        if (controllerState & PAD_UP) {
            if ((playerGridPosition & 0x38) == 0x00) {
                playerGridPosition += 56;
            } else {
                playerGridPosition -= 8;
            }
            movementInProgress = PLAYER_TILE_MOVE_FRAMES;
        } 

        if (controllerState & PAD_DOWN) {
            if ((playerGridPosition & 0x38) == 0x38) {
                playerGridPosition -= 56;
            } else {
                playerGridPosition += 8;
            }
            movementInProgress = PLAYER_TILE_MOVE_FRAMES;
        }

    }

    // Draw player in the right spot
    rawXPosition = (PLAY_AREA_LEFT + ((currentGameData[GAME_DATA_OFFSET_START_POSITIONS+currentLevelId] & 0x07) << 4));
    rawYPosition = (PLAY_AREA_TOP + ((currentGameData[GAME_DATA_OFFSET_START_POSITIONS+currentLevelId] & 0x38) << 1));
    rawTileId = PLAYER_SPRITE_TILE_ID;

    oam_spr(rawXPosition, rawYPosition, rawTileId, 0x03, PLAYER_SPRITE_INDEX);
    oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition, rawTileId + 1, 0x03, PLAYER_SPRITE_INDEX+4);
    oam_spr(rawXPosition, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 16, 0x03, PLAYER_SPRITE_INDEX+8);
    oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 17, 0x03, PLAYER_SPRITE_INDEX+12);


    rawXPosition = (64 + ((playerGridPosition & 0x07)<<4));
    rawYPosition = (80 + ((playerGridPosition & 0x38)<<1));
    // FIXME: Sprite constant
    oam_spr(rawXPosition, rawYPosition, 0xe2, 0x03, 0xd0);
    oam_spr(rawXPosition+8, rawYPosition, 0xe2+1, 0x03, 0xd0+4);
    oam_spr(rawXPosition, rawYPosition+8, 0xe2+16, 0x03, 0xd0+8);
    oam_spr(rawXPosition+8, rawYPosition+8, 0xe2+17, 0x03, 0xd0+12);


}