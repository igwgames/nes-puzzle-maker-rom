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
#include "source/graphics/hud.h"
#include "source/graphics/game_text.h"
#include "source/map/load_map.h"
#include "source/map/map.h"

CODE_BANK(PRG_BANK_PLAYER_SPRITE);

// Some useful global variables
ZEROPAGE_DEF(unsigned char, playerGridPositionX);
ZEROPAGE_DEF(unsigned char, playerGridPositionY);
ZEROPAGE_DEF(unsigned char, movementInProgress);
ZEROPAGE_DEF(unsigned char, playerControlsLockTime);
ZEROPAGE_DEF(unsigned char, playerInvulnerabilityTime);
ZEROPAGE_DEF(unsigned char, playerDirection);

ZEROPAGE_DEF(unsigned char, nextPlayerGridPositionX);
ZEROPAGE_DEF(unsigned char, nextPlayerGridPositionY);

ZEROPAGE_DEF(signed char, animationPositionX);
ZEROPAGE_DEF(signed char, animationPositionY);


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

CODE_BANK_POP();

// Code here goes in PRG instead. because space is hard
// NOTE: This uses tempChar1 through tempChar3; the caller must not use these.
void update_player_sprite() {
    // Calculate the position of the player itself, then use these variables to build it up with 4 8x8 NES sprites.

    rawXPosition = (PLAY_AREA_LEFT + (playerGridPositionX << 4));
    rawYPosition = (PLAY_AREA_TOP + (playerGridPositionY << 4));
    rawTileId = playerSpriteTileId + playerDirection;

    if (animationPositionX) {
        rawXPosition += animationPositionX;
        rawTileId += 2 + (((animationPositionX >> 3) & 0x01) << 1);
    }

    if (animationPositionY) {
        rawYPosition += animationPositionY;
        rawTileId += 2 + (((animationPositionY >> 3) & 0x01) << 1);
    }


    if (playerInvulnerabilityTime && frameCount & PLAYER_INVULNERABILITY_BLINK_MASK) {
        // If the player is invulnerable, we hide their sprite about half the time to do a flicker animation.
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
        oam_spr(SPRITE_OFFSCREEN, SPRITE_OFFSCREEN, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);

    } else {
        oam_spr(rawXPosition, rawYPosition, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
        oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
        oam_spr(rawXPosition, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
        oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);
    }

}

CODE_BANK(0);

void update_single_tile(unsigned char x, unsigned char y, unsigned char newTile, unsigned char palette) {

    if (newTile > 7) {
        newTile -= 8;
        newTile <<= 1;
        newTile += 32;
    } else {
        newTile <<= 1;
    }

    collisionTempValue = 0x2000 + ((x + 2)<<1) + ((y + 1)<<6);
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
    collisionTempX = x + 2;
    collisionTempY = y + 1;

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
    
    nextPlayerGridPositionX = playerGridPositionX;
    nextPlayerGridPositionY = playerGridPositionY;

    if (controllerState & PAD_LEFT) {
        if (playerGridPositionX > 0) {
            --nextPlayerGridPositionX;
            collisionTempDirection = PAD_LEFT;
        }
        // Graphical only
        playerDirection = SPRITE_DIRECTION_LEFT;
    } else if (controllerState & PAD_RIGHT) {
        if (playerGridPositionX < 11) {
            ++nextPlayerGridPositionX;
            collisionTempDirection = PAD_RIGHT;
        }
        // Graphical only
        playerDirection = SPRITE_DIRECTION_RIGHT;

    } else if (controllerState & PAD_UP) {
        if (playerGridPositionY > 0) {
            --nextPlayerGridPositionY;
            collisionTempDirection = PAD_UP;
        }
        // Graphical only
        playerDirection = SPRITE_DIRECTION_UP;

    } else if (controllerState & PAD_DOWN) {
        if (playerGridPositionY < 9) {
            ++nextPlayerGridPositionY;
            collisionTempDirection = PAD_DOWN;
        }
        // Graphical only
        playerDirection = SPRITE_DIRECTION_DOWN;

    }


    if (playerGridPositionX == nextPlayerGridPositionX && playerGridPositionY == nextPlayerGridPositionY) {
        // Ya didn't move...
        return; 
    }
    rawTileId = nextPlayerGridPositionX + (nextPlayerGridPositionY * 12);

    movementInProgress = 1;
    switch (tileCollisionTypes[currentMap[rawTileId]]) {
        // Ids are multiplied by 4, which is their index 
        case TILE_COLLISION_WALKABLE:
        case TILE_COLLISION_UNUSED:
            // Walkable.. Go !
            break;
        case TILE_COLLISION_GAP:
        case TILE_COLLISION_SOLID: // Solid 1
            // Nope, go back. These are solid.
                nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
            break;
        case TILE_COLLISION_CRATE:
            // So, we know that rawTileId is the crate we intend to move. Test if it can move anywhere, and if so, bunt it. If not... stop.
            switch (collisionTempDirection) {
                // TODO: This is ugly... what can we do to pretty it up?
                case PAD_RIGHT:
                    if (nextPlayerGridPositionX == 11) {
                        nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
                        break;
                    }
                    collisionTempTileId = tileCollisionTypes[currentMap[rawTileId+1]];
                    if (collisionTempTileId == TILE_COLLISION_WALKABLE) {
                        // Do it
                        currentMap[rawTileId+1] = currentMap[rawTileId];
                        collisionTempTileId = currentMap[rawTileId+1];
                        update_single_tile(nextPlayerGridPositionX + 1, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId+1]]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMap[rawTileId];

                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId]]);
                        update_hud();
                        sfx_play(SFX_CRATE_MOVE, SFX_CHANNEL_1);
                    } else if (collisionTempTileId == TILE_COLLISION_GAP) {
                        ++playerCrateCount;
                        ++gameCrates;
                        currentMap[rawTileId+1] = currentMapOrig[rawTileId+1];

                        collisionTempTileId = currentMap[rawTileId+1];
                        update_single_tile(nextPlayerGridPositionX + 1, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId+1]]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMap[rawTileId];

                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId]]);
                        update_hud();
                        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);
                    } else {
                        nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;

                    }
                    break;
                case PAD_LEFT:
                    if (nextPlayerGridPositionX == 0) {
                        nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
                        break;
                    }
                    collisionTempTileId = tileCollisionTypes[currentMap[rawTileId-1]];
                    if (collisionTempTileId == TILE_COLLISION_WALKABLE) {
                        // Do it
                        currentMap[rawTileId-1] = currentMap[rawTileId];
                        collisionTempTileId = currentMap[rawTileId-1];
                        update_single_tile(nextPlayerGridPositionX - 1, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId-1]]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMap[rawTileId];

                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId]]);
                        update_hud();
                        sfx_play(SFX_CRATE_MOVE, SFX_CHANNEL_1);
                    } else if (collisionTempTileId == TILE_COLLISION_GAP) {
                        currentMap[rawTileId-1] = currentMapOrig[rawTileId-1];
                        ++playerCrateCount;
                        ++gameCrates;

                        collisionTempTileId = currentMap[rawTileId-1];
                        update_single_tile(nextPlayerGridPositionX - 1, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId-1]]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMap[rawTileId];

                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId]]);
                        update_hud();
                        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);

                    } else {
                        nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
                    }
                    break;
                case PAD_UP:
                    if (nextPlayerGridPositionY == 0) {
                        nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
                        break;
                    }
                    collisionTempTileId = tileCollisionTypes[currentMap[rawTileId-12]];
                    if (collisionTempTileId == TILE_COLLISION_WALKABLE) {
                        // Do it
                        currentMap[rawTileId-12] = currentMap[rawTileId];
                        collisionTempTileId = currentMap[rawTileId-12];
                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY-1, collisionTempTileId, tilePalettes[currentMap[rawTileId-12]]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMap[rawTileId];

                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId]]);
                        update_hud();
                        sfx_play(SFX_CRATE_MOVE, SFX_CHANNEL_1);
                    } else if (collisionTempTileId == TILE_COLLISION_GAP) {
                        currentMap[rawTileId-12] = 0;
                        ++playerCrateCount;
                        ++gameCrates;

                        collisionTempTileId = currentMap[rawTileId-12];
                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY-1, collisionTempTileId, tilePalettes[currentMap[rawTileId-12]]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMap[rawTileId];

                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId]]);
                        update_hud();
                        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);

                    } else {
                        nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
                    }
                    break;
                case PAD_DOWN:
                    if (nextPlayerGridPositionY == 9) {
                        nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
                        break;
                    }
                    collisionTempTileId = tileCollisionTypes[currentMap[rawTileId+12]];
                    if (collisionTempTileId == TILE_COLLISION_WALKABLE) {
                        // Do it
                        currentMap[rawTileId+12] = currentMap[rawTileId];
                        collisionTempTileId = currentMap[rawTileId+12];
                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY+1, collisionTempTileId, tilePalettes[currentMap[rawTileId+12]]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMap[rawTileId];

                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId]]);
                        update_hud();
                        sfx_play(SFX_CRATE_MOVE, SFX_CHANNEL_1);
                    } else if (collisionTempTileId == TILE_COLLISION_GAP) {
                        currentMap[rawTileId+12] = currentMapOrig[rawTileId + 12];
                        ++playerCrateCount;
                        ++gameCrates;

                        collisionTempTileId = currentMap[rawTileId+12];
                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY+1, collisionTempTileId, tilePalettes[currentMap[rawTileId+12]]);

                        currentMap[rawTileId] = currentMapOrig[rawTileId];
                        collisionTempTileId = currentMap[rawTileId];

                        update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, collisionTempTileId, tilePalettes[currentMap[rawTileId]]);
                        update_hud();
                        sfx_play(SFX_CRATE_SMASH, SFX_CHANNEL_1);

                    } else {
                        nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
                    }
                    break;

                    
                default:
                    nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
                    break;
            }
            break;
        case TILE_COLLISION_COLLECTABLE:
            ++playerKeyCount;
            ++gameKeys;
            currentMap[rawTileId] = currentMapOrig[rawTileId];
            update_single_tile(nextPlayerGridPositionX, nextPlayerGridPositionY, currentMap[rawTileId], currentMapTileData[TILE_DATA_LOOKUP_OFFSET_PALETTE]);
            update_hud();
            sfx_play(SFX_HEART, SFX_CHANNEL_1);
            break;
        case TILE_COLLISION_LEVEL_END: // Level end!
            collisionTempTileId = 0;
            switch (currentGameStyle) {
                case GAME_STYLE_MAZE:
                    // Do nothing; you're just allowed to pass.
                    break;
                case GAME_STYLE_COIN:
                    for (i = 0; i != 120; ++i) {
                        if (tileCollisionTypes[currentMap[i]] == TILE_COLLISION_COLLECTABLE) {
                            // Sorry, you didn't get em all. Plz try again.
                            collisionTempTileId = 1;
                        }
                    }
                    break;
                case GAME_STYLE_CRATES: 
                    for (i = 0; i != 120; ++i) {
                        if (totalCrateCount != playerCrateCount) {
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
                if (currentLevelId == totalGameLevels) {
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
            nextPlayerGridPositionX = playerGridPositionX; nextPlayerGridPositionY = playerGridPositionY;
            break;
    }

    if (playerGridPositionX > nextPlayerGridPositionX) {
        for (i = 0; i < 8; ++i) {
            animationPositionX = 0 - (i<<1);
            update_player_sprite();
            delay(2);
        }
    } else if (playerGridPositionX < nextPlayerGridPositionX) {
        for (i = 0; i < 8; ++i) {
            animationPositionX = (i<<1);
            update_player_sprite();
            delay(2);
        }
    }

    if (playerGridPositionY > nextPlayerGridPositionY) {
        for (i = 0; i < 8; ++i) {
            animationPositionY = 0 - (i<<1);
            update_player_sprite();
            delay(2);
        }

    } else if (playerGridPositionY < nextPlayerGridPositionY) {
        for (i = 0; i < 8; ++i) {
            animationPositionY = (i<<1);
            update_player_sprite();
            delay(2);
        }
    }
    update_player_sprite();
    animationPositionX = 0; animationPositionY = 0;
    playerGridPositionX = nextPlayerGridPositionX; playerGridPositionY = nextPlayerGridPositionY;



}

