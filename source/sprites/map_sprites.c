#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/configuration/system_constants.h"
#include "source/sprites/map_sprites.h"
#include "source/sprites/player.h"
#include "source/sprites/sprite_definitions.h"
#include "source/map/map.h"
#include "source/library/bank_helpers.h"
#include "source/sprites/collision.h"

CODE_BANK(PRG_BANK_MAP_SPRITES);

#define currentMapSpriteIndex tempChar1
#define currentSpriteSize tempChar2
#define currentSpriteTileId tempChar3
#define oamMapSpriteIndex tempChar4
#define currentSpriteType tempChar5
#define currentSpriteData tempChar6
#define sprX8 tempChar7
#define sprY8 tempChar8
#define sprX tempInt1
#define sprY tempInt2
#define tempMapSpriteIndex tempChar9
// NOTE: width = height for our examples, so both are set to the same value.
// If you change this, be sure to assign it in the for loop below as well.
#define currentSpriteFullWidth tempInt3
#define currentSpriteFullHeight tempInt3
// Same story, but corrected for tile collisions
#define currentSpriteFullTileCollisionWidth tempInt4
#define currentSpriteFullTileCollisionHeight tempInt4



ZEROPAGE_DEF(unsigned char, lastPlayerSpriteCollisionId);
