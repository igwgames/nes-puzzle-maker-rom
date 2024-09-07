#include "source/c/library/bank_helpers.h"

// The id of the first sprite in the 4 sprites comprising the player. (out of 64 total sprites)
#define PLAYER_SPRITE_INDEX 0x10

#define PLAY_AREA_TOP 60
#define PLAY_AREA_LEFT 32

// Track the player's position and direction
ZEROPAGE_EXTERN(unsigned char, playerDirection);
ZEROPAGE_EXTERN(unsigned char, playerGridPositionX);
ZEROPAGE_EXTERN(unsigned char, playerGridPositionY);

// Used to animate the player's movement between tiles
ZEROPAGE_EXTERN(signed char, animationPositionX);
ZEROPAGE_EXTERN(signed char, animationPositionY);
ZEROPAGE_EXTERN(unsigned char, undoPosition);


// Helper macro to convert the player's X and Y position into a position on the map
#define PLAYER_MAP_POSITION(xPos, yPos) (xPos>>4) + (yPos & 0xf0)

// Move the player around, and otherwise deal with controller input. (NOTE: Pause/etc are handled here too)
void handle_player_movement();

// Update the player's sprite, and put it onto the screen as necessary
void update_player_sprite();

// Wipe the undo list
void clear_undo(void);

#define UPDATE_TILE_TRIGGER_NONE 0
#define UPDATE_TILE_TRIGGER_END 1
#define UPDATE_TILE_TRIGGER_SWITCH 2
void set_update_tile_trigger(unsigned char val);

extern unsigned char switchState;