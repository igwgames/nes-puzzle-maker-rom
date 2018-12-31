#include "source/sprites/sprite_definitions.h"

// NOTE: All of this data is in the primary bank.

// Put all newly-designed sprites here. 8 Bytes per sprite, defined mostly from constants
// in sprite_definitions.h. The 8 bytes are: 
// 1st byte: Sprite type 
// 2nd byte: Tile id for first tile, using 8x8 grid. See the guide for more detail on this.
// 3rd byte: Split; contains sprite size and palette. Combine the constants
//           using the logical OR operator (represented by the bar character: |) 
// 4th byte: Animation type
// 5th byte: How the sprite moves.
// 6th-8th byte: Miscellaneous; you can use this for whatever extra data you need. For enemies, byte 5
//           is used as the damage they deal. Health powerups use byte 5 to store how much to restore. 
//           Byte 6 is used for movement speed on enemies and npcs.
//           Byte 7 is used for health on enemy sprites in our `add_sword` branch.
// NOTE: This array cannot contain more than 64 sprites, or other logic will break.
const unsigned char spriteDefinitions[] = {
};