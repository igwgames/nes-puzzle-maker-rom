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
#define collisionTempX tempChar4
#define collisionTempY tempChar5
#define collisionTempXRight tempChar6
#define collisionTempYBottom tempChar7

#define tempSpriteCollisionX tempInt1
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
    rawTileId = PLAYER_SPRITE_TILE_ID;

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

    if (controllerState & PAD_LEFT && (rawTileId & 0x07) != 0) {
        rawTileId--;
    } else if (controllerState & PAD_RIGHT && (rawTileId & 0x07) != 7) {
        rawTileId++;
    } else if (controllerState & PAD_UP && (rawTileId & 0x38) != 0) {
        rawTileId -= 8;
    } else if (controllerState & PAD_DOWN && (rawTileId & 0x38) != 0x38) {
        rawTileId += 8;
    }

    if (rawTileId == playerGridPosition) {
        // Ya didn't move...
        return; 
    }

    movementInProgress = PLAYER_TILE_MOVE_FRAMES;
    // TODO: Take special action based on the game type?
    switch (currentMap[rawTileId] >> 2) {
        // Ids are multiplied by 4, which is their index 
        case 0:
        case 3: // Hole for crate/ball - no action on its own
        case 5: // Unimplemented for now... so walkable
        case 6: // Ditto

            // Walkable.. Go !
            break;
        case 1: // Solid 1
        case 2: // Solid 2
            // Nope, go back. These are solid.
            rawTileId = playerGridPosition;
            break;
        case 4:
            // TODO: Implement crate behavior
            break;
        case 7: // Level end!
            // TODO: implement level end
            break;
        default:
            // Stop you when you hit an unknown tile... idk seems better than walking?
            rawTileId = playerGridPosition;
            break;
    }
    playerGridPosition = rawTileId;


    // Using a variable, so we can change the velocity based on pressing a button, having a special item,
    // or whatever you like!
    /*
    int maxVelocity = PLAYER_MAX_VELOCITY;
    lastControllerState = controllerState;
    controllerState = pad_poll(0);

    // If Start is pressed now, and was not pressed before...
    if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
        gameState = GAME_STATE_PAUSED;
        return;
    }
    if (playerControlsLockTime) {
        // If your controls are locked, just tick down the timer until they stop being locked. Don't read player input.
        playerControlsLockTime--;
    } else {
        if (controllerState & PAD_RIGHT && playerXVelocity >= 0) {
            // If you're moving right, and you're not at max, speed up.
            if (playerXVelocity < maxVelocity) {
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            // If you're over max somehow, we'll slow you down a little.
            } else if (playerXVelocity > maxVelocity) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            }
        } else if (controllerState & PAD_LEFT && playerXVelocity <= 0) {
            // If you're moving left, and you're not at max, speed up.
            if (ABS(playerXVelocity) < maxVelocity) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            // If you're over max, slow you down a little...
            } else if (ABS(playerXVelocity) > maxVelocity) { 
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            }
        } else if (playerXVelocity != 0) {
            // Not pressing anything? Let's slow you back down...
            if (playerXVelocity > 0) {
                playerXVelocity -= PLAYER_VELOCITY_ACCEL;
            } else {
                playerXVelocity += PLAYER_VELOCITY_ACCEL;
            }
        }

        if (controllerState & PAD_UP && playerYVelocity <= 0) {
            if (ABS(playerYVelocity) < maxVelocity) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            } else if (ABS(playerYVelocity) > maxVelocity) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            }
        } else if (controllerState & PAD_DOWN && playerYVelocity >= 0) {
            if (playerYVelocity < maxVelocity) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            } else if (playerYVelocity > maxVelocity) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            }
        } else { 
            if (playerYVelocity > 0) {
                playerYVelocity -= PLAYER_VELOCITY_ACCEL;
            } else if (playerYVelocity < 0) {
                playerYVelocity += PLAYER_VELOCITY_ACCEL;
            }
        }
    }

    // While we're at it, tick down the invulnerability timer if needed
    if (playerInvulnerabilityTime) {
        playerInvulnerabilityTime--;
    }

    // This will knock out the player's speed if they hit anything.
    test_player_tile_collision();
    handle_player_sprite_collision();

    rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
    rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        if (rawXPosition > SCREEN_EDGE_RIGHT) {
        // We use sprite direction to determine which direction to scroll in, so be sure this is set properly.
        if (playerInvulnerabilityTime) {
            playerXPosition -= playerXVelocity;
            rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_RIGHT;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition++;
        }
    } else if (rawXPosition < SCREEN_EDGE_LEFT) {
        if (playerInvulnerabilityTime) {
            playerXPosition -= playerXVelocity;
            rawXPosition = (playerXPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_LEFT;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition--;
        }
    } else if (rawYPosition > SCREEN_EDGE_BOTTOM) {
        if (playerInvulnerabilityTime) {
            playerYPosition -= playerYVelocity;
            rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_DOWN;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition += 8;
        }
    } else if (rawYPosition < SCREEN_EDGE_TOP) {
        if (playerInvulnerabilityTime) {
            playerYPosition -= playerYVelocity;
            rawYPosition = (playerYPosition >> PLAYER_POSITION_SHIFT);
        } else {
            playerDirection = SPRITE_DIRECTION_UP;
            gameState = GAME_STATE_SCREEN_SCROLL;
            playerOverworldPosition -= 8;
        }
    }*/

}

void test_player_tile_collision() {

	if (playerYVelocity != 0) {
        collisionTempYInt = playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity;
        collisionTempXInt = playerXPosition + PLAYER_X_OFFSET_EXTENDED;

		collisionTempY = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
		collisionTempX = ((collisionTempXInt) >> PLAYER_POSITION_SHIFT);

        collisionTempYInt += PLAYER_HEIGHT_EXTENDED;
        collisionTempXInt += PLAYER_WIDTH_EXTENDED;

        collisionTempYBottom = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = (collisionTempXInt) >> PLAYER_POSITION_SHIFT;
        
        // Due to how we are calculating the sprite's position, there is a slight chance we can exceed the area that our
        // map takes up near screen edges. To compensate for this, we clamp the Y position of our character to the 
        // window between 0 and 192 pixels, which we can safely test collision within.

        // If collisionTempY is > 240, it can be assumed we calculated a position less than zero, and rolled over to 255
        if (collisionTempY > 240) {
            collisionTempY = 0;
        }
        // The lowest spot we can test collision is at pixel 192 (the 12th 16x16 tile). If we're past that, bump ourselves back.
        if (collisionTempYBottom > 190) { 
            collisionTempYBottom = 190;
        }

		if (playerYVelocity < 0) {
            // We're going up - test the top left, and top right
			if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempY)], 1)) {
                playerYVelocity = 0;
                playerControlsLockTime = 0;
            }
            if (!playerControlsLockTime) {
                playerDirection = SPRITE_DIRECTION_UP;
            }
		} else {
            // Okay, we're going down - test the bottom left and bottom right
			if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempYBottom)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempYBottom)], 1)) {
                playerYVelocity = 0;
                playerControlsLockTime = 0;

			}
            if (!playerControlsLockTime) {
                playerDirection = SPRITE_DIRECTION_DOWN;
            }
		}
    }

	if (playerXVelocity != 0) {
        collisionTempXInt = playerXPosition + PLAYER_X_OFFSET_EXTENDED + playerXVelocity;
        collisionTempYInt = playerYPosition + PLAYER_Y_OFFSET_EXTENDED + playerYVelocity;
		collisionTempX = (collisionTempXInt) >> PLAYER_POSITION_SHIFT;
		collisionTempY = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;

        collisionTempXInt += PLAYER_WIDTH_EXTENDED;
        collisionTempYInt += PLAYER_HEIGHT_EXTENDED;

        collisionTempYBottom = ((collisionTempYInt) >> PLAYER_POSITION_SHIFT) - HUD_PIXEL_HEIGHT;
        collisionTempXRight = ((collisionTempXInt) >> PLAYER_POSITION_SHIFT);


        // Depending on how far to the left/right the player is, there's a chance part of our bounding box falls into
        // the next column of tiles on the opposite side of the screen. This if statement prevents those collisions.
        if (collisionTempX > 2 && collisionTempX < 238) {
            if (playerXVelocity < 0) {
                // Okay, we're moving left. Need to test the top-left and bottom-left
                if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempX, collisionTempYBottom)], 1)) {
                    playerXVelocity = 0;
                    playerControlsLockTime = 0;

                }
                if (!playerControlsLockTime) {
                    playerDirection = SPRITE_DIRECTION_LEFT;
                }
            } else {
                // Going right - need to test top-right and bottom-right
                if (test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempY)], 1) || test_collision(currentMap[PLAYER_MAP_POSITION(collisionTempXRight, collisionTempYBottom)], 1)) {
                    playerXVelocity = 0;
                    playerControlsLockTime = 0;

                }
                if (!playerControlsLockTime) {
                    playerDirection = SPRITE_DIRECTION_RIGHT;
                }
            }
        }
	}

    playerXPosition += playerXVelocity;
    playerYPosition += playerYVelocity;

}

#define currentMapSpriteIndex tempChar1
void handle_player_sprite_collision() {
    /*
    // We store the last sprite hit when we update the sprites in `map_sprites.c`, so here all we have to do is react to it.
    if (lastPlayerSpriteCollisionId != NO_SPRITE_HIT) {
        currentMapSpriteIndex = lastPlayerSpriteCollisionId<<MAP_SPRITE_DATA_SHIFT;
        switch (currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE]) {
            case SPRITE_TYPE_HEALTH:
                // This if statement ensures that we don't remove hearts if you don't need them yet.
                if (playerHealth < playerMaxHealth) {
                    playerHealth += currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_HEALTH];
                    if (playerHealth > playerMaxHealth) {
                        playerHealth = playerMaxHealth;
                    }
                    // Hide the sprite now that it has been taken.
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                    // Play the heart sound!
                    sfx_play(SFX_HEART, SFX_CHANNEL_3);

                    // Mark the sprite as collected, so we can't get it again.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                }
                break;
            case SPRITE_TYPE_KEY:
                if (playerKeyCount < MAX_KEY_COUNT) {
                    playerKeyCount++;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                    sfx_play(SFX_KEY, SFX_CHANNEL_3);

                    // Mark the sprite as collected, so we can't get it again.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];
                }
                break;
            case SPRITE_TYPE_REGULAR_ENEMY:
            case SPRITE_TYPE_INVULNERABLE_ENEMY:

                if (playerInvulnerabilityTime) {
                    return;
                }
                playerHealth -= currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_DAMAGE]; 
                // Since playerHealth is unsigned, we need to check for wraparound damage. 
                // NOTE: If something manages to do more than 16 damage at once, this might fail.
                if (playerHealth == 0 || playerHealth > 240) {
                    gameState = GAME_STATE_GAME_OVER;
                    music_stop();
                    sfx_play(SFX_GAMEOVER, SFX_CHANNEL_1);
                    return;
                }
                // Knock the player back
                playerControlsLockTime = PLAYER_DAMAGE_CONTROL_LOCK_TIME;
                playerInvulnerabilityTime = PLAYER_DAMAGE_INVULNERABILITY_TIME;
                if (playerDirection == SPRITE_DIRECTION_LEFT) {
                    // Punt them back in the opposite direction
                    playerXVelocity = PLAYER_MAX_VELOCITY;
                    // Reverse their velocity in the other direction, too.
                    playerYVelocity = 0 - playerYVelocity;
                } else if (playerDirection == SPRITE_DIRECTION_RIGHT) {
                    playerXVelocity = 0-PLAYER_MAX_VELOCITY;
                    playerYVelocity = 0 - playerYVelocity;
                } else if (playerDirection == SPRITE_DIRECTION_DOWN) {
                    playerYVelocity = 0-PLAYER_MAX_VELOCITY;
                    playerXVelocity = 0 - playerXVelocity;
                } else { // Make being thrown downward into a catch-all, in case your direction isn't set or something.
                    playerYVelocity = PLAYER_MAX_VELOCITY;
                    playerXVelocity = 0 - playerXVelocity;
                }
                sfx_play(SFX_HURT, SFX_CHANNEL_2);

                
                break;
            case SPRITE_TYPE_DOOR: 
                // Doors without locks are very simple - they just open! Hide the sprite until the user comes back...
                // note that we intentionally *don't* store this state, so it comes back next time.
                currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;
                break;
            case SPRITE_TYPE_LOCKED_DOOR:
                // First off, do you have a key? If so, let's just make this go away...
                if (playerKeyCount > 0) {
                    playerKeyCount--;
                    currentMapSpriteData[(currentMapSpriteIndex) + MAP_SPRITE_DATA_POS_TYPE] = SPRITE_TYPE_OFFSCREEN;

                    // Mark the door as gone, so it doesn't come back.
                    currentMapSpritePersistance[playerOverworldPosition] |= bitToByte[lastPlayerSpriteCollisionId];

                    break;
                }
                // So you don't have a key...
                // Okay, we collided with a door before we calculated the player's movement. After being moved, does the 
                // new player position also collide? If so, stop it. Else, let it go.

                // Calculate position...
                tempSpriteCollisionX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
                tempSpriteCollisionY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));

                // Are we colliding?
                // NOTE: We take a bit of a shortcut here and assume all doors are 16x16 (the hard-coded 16 value below)
                if (
                    playerXPosition < tempSpriteCollisionX + (16 << PLAYER_POSITION_SHIFT) &&
                    playerXPosition + PLAYER_WIDTH_EXTENDED > tempSpriteCollisionX &&
                    playerYPosition < tempSpriteCollisionY + (16 << PLAYER_POSITION_SHIFT) &&
                    playerYPosition + PLAYER_HEIGHT_EXTENDED > tempSpriteCollisionY
                ) {
                    playerXPosition -= playerXVelocity;
                    playerYPosition -= playerYVelocity;
                    playerControlsLockTime = 0;
                }
                break;
            case SPRITE_TYPE_ENDGAME:
                gameState = GAME_STATE_CREDITS;
                break;
            case SPRITE_TYPE_NPC:
                // Okay, we collided with this NPC before we calculated the player's movement. After being moved, does the 
                // new player position also collide? If so, stop it. Else, let it go.

                // Calculate position...
                tempSpriteCollisionX = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_X + 1]) << 8));
                tempSpriteCollisionY = ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y]) + ((currentMapSpriteData[currentMapSpriteIndex + MAP_SPRITE_DATA_POS_Y + 1]) << 8));
                // Are we colliding?
                // NOTE: We take a bit of a shortcut here and assume all NPCs are 16x16 (the hard-coded 16 value below)
                if (
                    playerXPosition < tempSpriteCollisionX + (16 << PLAYER_POSITION_SHIFT) &&
                    playerXPosition + PLAYER_WIDTH_EXTENDED > tempSpriteCollisionX &&
                    playerYPosition < tempSpriteCollisionY + (16 << PLAYER_POSITION_SHIFT) &&
                    playerYPosition + PLAYER_HEIGHT_EXTENDED > tempSpriteCollisionY
                ) {
                    playerXPosition -= playerXVelocity;
                    playerYPosition -= playerYVelocity;
                    playerControlsLockTime = 0;
                }

                if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
                    // Show the text for the player on the first screen
                    if (playerOverworldPosition == 0) {
                        trigger_game_text(introductionText);
                    } else {
                        // If it's on another screen, show some different text :)
                        trigger_game_text(movedText);
                    }
                }
                break;


        }

    }*/
}

void handle_editor_input() {
    lastControllerState = controllerState;
    controllerState = pad_poll(0);
    if (controllerState & PAD_SELECT && !(lastControllerState & PAD_SELECT)) {
        if (editorSelectedTileId == 7) { // End of regular tiles
            editorSelectedTileId = TILE_EDITOR_POSITION_PLAYER;
        } else if (editorSelectedTileId == TILE_EDITOR_POSITION_PLAYER) {
            editorSelectedTileId = TILE_EDITOR_POSITION_INFO;
        } else if (editorSelectedTileId == TILE_EDITOR_POSITION_INFO) {
            editorSelectedTileId = 0;
        } else {
            ++editorSelectedTileId;
        }
    }

    if (movementInProgress)
        --movementInProgress;

    if (!movementInProgress) { 
        if (controllerState & PAD_A) {
            currentMap[playerGridPosition] = editorSelectedTileId;

            // Pseudocode: find position, calculate nametable pos based on map x+y, MSB should be trustworthy for all 4 tiles, LSB math is basic
            // Don't forget palettes...
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

    oam_spr(rawXPosition, rawYPosition, rawTileId, 0x00, PLAYER_SPRITE_INDEX);
    oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition, rawTileId + 1, 0x00, PLAYER_SPRITE_INDEX+4);
    oam_spr(rawXPosition, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 16, 0x00, PLAYER_SPRITE_INDEX+8);
    oam_spr(rawXPosition + NES_SPRITE_WIDTH, rawYPosition + NES_SPRITE_HEIGHT, rawTileId + 17, 0x00, PLAYER_SPRITE_INDEX+12);


    rawXPosition = (64 + ((playerGridPosition & 0x07)<<4));
    rawYPosition = (80 + ((playerGridPosition & 0x38)<<1));
    // FIXME: Sprite constant
    oam_spr(rawXPosition, rawYPosition, 0xe2, 0x00, 0xd0);
    oam_spr(rawXPosition+8, rawYPosition, 0xe2+1, 0x00, 0xd0+4);
    oam_spr(rawXPosition, rawYPosition+8, 0xe2+16, 0x00, 0xd0+8);
    oam_spr(rawXPosition+8, rawYPosition+8, 0xe2+17, 0x00, 0xd0+12);


}