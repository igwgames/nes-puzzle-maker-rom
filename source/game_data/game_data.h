// Saving and loading game data, parsing it out of large structures...
// Also handles the temporary storage for the editor.
#include "source/library/bank_helpers.h"

extern unsigned char currentGameData[256];

// NOTE: please don't use this by yourself - it's meant to be used by the macro below. 
ZEROPAGE_EXTERN(unsigned char, __extraBits);

SRAM_ARRAY_EXTERN(unsigned char, savedGameData, 2048);
SRAM_ARRAY_EXTERN(unsigned char, currentSramGameData, 256);

void unload_game();

void load_game();

void load_game_name_to_input();

void save_game();

// Pick a random "magic" number to mark a map with, so we know this is the current id. Increment any time there's a major change.
#define GAME_DATA_VERSION_ID 25

#define GAME_DATA_OFFSET_VERSION 0
#define GAME_DATA_OFFSET_TILESET_ID 1
#define GAME_DATA_OFFSET_GAME_STYLE 2
#define GAME_DATA_OFFSET_SONG_ID 3
#define GAME_DATA_OFFSET_SPRITE_ID 4

// TODO: Store starting positions for levels in each of these? Would buy us a variable tile for players, if that's useful.
// Unused space 5-8 (3 bytes)
#define GAME_DATA_OFFSET_START_POSITIONS 8
#define GAME_DATA_OFFSET_TITLE 16
#define GAME_DATA_OFFSET_TITLE_LENGTH 20
#define GAME_DATA_OFFSET_AUTHOR 36
#define GAME_DATA_OFFSET_AUTHOR_LENGTH 19 // Literally just to make it fit on the page.
// This space ended up being unused (8 bytes)
// #define GAME_DATA_OFFSET_CREDITS 56
// #define GAME_DATA_OFFSET_CREDITS_LENGTH 8
#define GAME_DATA_OFFSET_MAP 64
#define GAME_DATA_OFFSET_MAP_LENGTH 192

#define GAME_DATA_OFFSET_MAP_WORLD_LENGTH 24

// Styles of games to be played 

// Simplest style - just get to the end of each level.
#define GAME_STYLE_MAZE 0
#define GAME_STYLE_CRATES 1
#define GAME_STYLE_COIN 2
#define GAME_STYLE_COUNT 3

#define MAX_GAME_LEVELS 8

#define GAME_SPRITE_ARCADE 0
#define GAME_SPRITE_ZORIA 1
#define GAME_SPRITE_SLIME 2
#define GAME_SPRITE_SPIDER 3
#define GAME_SPRITE_ROBITZ 4
#define GAME_SPRITE_DOG 5
#define GAME_SPRITE_COUNT 6

// We do a lot of packing things into 6 bits to save space. This will copy 6bit data from the first address into
// the second address, stopping after the specified number of bytes.
#define UNPACK_6BIT_DATA(source, destination, length) \
    j = 0; \
    for (i = 0; i != length; ++i) { \
        destination[j++] = (source[i] & 0x38) >> 3; \
        destination[j++] = (source[i] & 0x07); \
        if (i % 3 == 0) { \
            __extraBits = (source[i] & 0xc0) >> 6; \
        } else if (i % 3 == 1) { \
            __extraBits |= (source[i] & 0xc0) >> 4; \
        } else { \
            __extraBits |= (source[i] & 0xc0) >> 2; \
            destination[j++] = (__extraBits & 0x38) >> 3; \
            destination[j++] = __extraBits & 0x07; \
        } \
    }

// This does the reverse of the script above.
#define PACK_6BIT_DATA(source, destination, length) \
    j = 0; \
    for (i = 0; i != length; i += 2) { \
        if (i % 8 != 6) { \
            destination[j] = ((source[i] & 0x07)<< 3) | ((source[i+1] & 0x07)); \
            ++j; \
        } else { \
            __extraBits = (((source[i] & 0x07)) << 3) | ((source[i+1] & 0x07)); \
            destination[j-1] |= ((__extraBits & 0x30) << 2); \
            destination[j-2] |= ((__extraBits & 0x0c) << 4); \
            destination[j-3] |= ((__extraBits & 0x03) << 6); \
        } \
    } 