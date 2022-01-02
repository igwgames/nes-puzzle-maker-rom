#include "source/library/bank_helpers.h"

#define PRG_BANK_MAP_LOGIC 1

// This defines how many pixels to move the screen during each frame of the screen scroll animation.
// Must be a multiple of 2 (or w`hatever value is set below)
// This likely requires a bunch of tweaking the other variables to make this work. I don't actively
// support other speeds, but if you make another one work reliably let me know with a github issue and I may update
// this documentation.
#define SCREEN_SCROLL_SPEED 4
// This is how many pixels are done in each loop - the variable above controls when we wait for interrupts. 
// You probably don't want to change these...
#define SCREEN_SCROLL_LOOP_INCREMENT_LR 2
#define SCREEN_SCROLL_LOOP_INCREMENT_UD 4

// These two variables handle how much we shift the player each loop to match screen scrolling
#define SCREEN_SCROLL_MOVEMENT_INCREMENT_LR 29 + ((frameCount & 0x10) >> 3)
#define SCREEN_SCROLL_MOVEMENT_INCREMENT_UD 80

// This controls how much ram we reserve for sprite data lookups. This sets us up to have 16 byte per sprite,
// but if you change the code around a bit, you might be able to drop this to 3 to use 8 bytes per sprite instead.
#define MAP_SPRITE_DATA_SHIFT 4

// This controls how many oam sprites we reserve for a single "sprite". By default, we reserve 4
// sprites, so this is set to 4. (Since 1 shifted left 4 times is 16, and each sprite uses 4 bytes
// in oam.)
#define MAP_SPRITE_OAM_SHIFT 4

// Max number of sprites to load from a map tile. Note that this is also coded into the conversion tool that
// nes-starter-kit uses. 
// Note: You can bump this to 12, but if a room has more than 8 sprites, the last 4 will not have their state
// persisted. This means sprites can be duplicated if there are more than 8 on a screen, including hearts or keys. 
// Also note that there isn't much room for more than that... unless you really know the NES hardware
// intricately, you probably don't want to touch this one.
#define MAP_MAX_SPRITES 8

// Lookup positions for sprite data in currentMapSpriteData.
#define MAP_SPRITE_DATA_POS_X                   0
#define MAP_SPRITE_DATA_POS_Y                   2
#define MAP_SPRITE_DATA_POS_TYPE                4
#define MAP_SPRITE_DATA_POS_SIZE_PALETTE        5
#define MAP_SPRITE_DATA_POS_ANIMATION_TYPE      6
#define MAP_SPRITE_DATA_POS_HEALTH              7
#define MAP_SPRITE_DATA_POS_SLIDE_SPEED         7
#define MAP_SPRITE_DATA_POS_NPC_ID              7
#define MAP_SPRITE_DATA_POS_TILE_ID             8
#define MAP_SPRITE_DATA_POS_MOVEMENT_TYPE       9
#define MAP_SPRITE_DATA_POS_CURRENT_DIRECTION   10
#define MAP_SPRITE_DATA_POS_DIRECTION_TIME      11
#define MAP_SPRITE_DATA_POS_MOVE_SPEED          12
#define MAP_SPRITE_DATA_POS_DAMAGE              13

// How many tiles are in the map before we start getting into sprite data.
#define MAP_DATA_TILE_LENGTH 192

// The current map; usable for collisions/etc
extern unsigned char currentMap[120];
extern unsigned char currentMapOrig[120];

extern unsigned char assetTable[64];

// Supporting data for sprites; 16 bytes per sprite. Look at the sprite loader function in `map.h` (or the guide) for more details.
extern unsigned char currentMapSpriteData[(16 * MAP_MAX_SPRITES)];

ZEROPAGE_EXTERN(unsigned char, currentGameStyle);

// This stores the state for the 8 sprites on every map screen. Each screen is represented by one byte.
// We do this by storing 1 bit for each sprite - 0 if not collected, 1 if it collected. We don't re-spawn
// collected sprites.
extern unsigned char currentMapSpritePersistance[64];

// The player's position on the world map. 0-7 are first row, 8-15 are 2nd, etc...
ZEROPAGE_EXTERN(unsigned char, playerOverworldPosition);

// Stores information about the current available tiles for the map/game. 8 tiles, 4 bytes per.. 32 bytes total.
extern unsigned char currentMapTileData[32];

// Load the sprites from the current map into currentMapSpriteData, and the sprite list.
void load_sprites();

// Set some default variables and hardware settings to prepare to draw the map after showing menus/etc.
void init_map();

// Assuming the map is available in currentMap, draw it to a nametable. Assumes ppu is already turned off.
void draw_current_map_to_a();
void draw_current_map_to_a_inline();
void draw_current_map_to_b();
void draw_current_map_to_c();
void draw_current_map_to_d();

// Take the value of playerOverworldPosition, and transition to this with a pretty scrolling animation.
void do_scroll_screen_transition();

// Take the value of playerOverworldPosition, and transition onto that screen with a fade animation.
void do_fade_screen_transition();

// Based on the current position and tile, update the map and the attribute table
void update_editor_map_tile();

// Do the same thing, but use the first tile
void update_editor_map_tile_rm();

// Adds some color text around the editor map - current map id, etc
void draw_editor_help();

// Defines world ids we use, which are also PRG bank ids to save storage and simplify code.
#define WORLD_OVERWORLD PRG_BANK_MAP_OVERWORLD

// Handles collisions of all sorts in this engine.
#define TILE_COLLISION_WALKABLE 0
#define TILE_COLLISION_SOLID 1
#define TILE_COLLISION_HOLE 2
#define TILE_COLLISION_GAP 3
#define TILE_COLLISION_CRATE 4
#define TILE_COLLISION_COLLECTABLE 5
#define TILE_COLLISION_UNUSED 6
#define TILE_COLLISION_LEVEL_END 7

#define TILE_DATA_LOOKUP_OFFSET_ID 0
#define TILE_DATA_LOOKUP_OFFSET_PALETTE 1
#define TILE_DATA_LOOKUP_OFFSET_COLLISION 2
// NOTE: Currently unused; rename if use
#define TILE_DATA_LOOKUP_OFFSET_EXTRA 3

#define TILE_EDITOR_POSITION_PLAYER 11

#define TILE_EDITOR_POSITION_LEFT 64
#define TILE_EDITOR_POSITION_RIGHT 65

#define MAPS_IN_GAME 8

// Values used to adjust where to start drawing the map on-screen. (NOTE: This bypasses the solid block "border" we draw)
#define MAP_LEFT_PADDING 4
#define MAP_TOP_PADDING 128

#define GAME_BORDER_TILE_START 0x08
extern const unsigned char gameName[];
extern const unsigned char tileCollisionTypes[];
extern const unsigned char tilePalettes[];

extern const unsigned char gameLevelData[];
extern const unsigned char totalGameLevels;

// Simplest style - just get to the end of each level.
#define GAME_STYLE_MAZE 0
#define GAME_STYLE_CRATES 1
#define GAME_STYLE_COIN 2
#define GAME_STYLE_COUNT 3