#include "source/map/map_data.h"
#include "source/game_data/game_data.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/system_constants.h"

CODE_BANK(PRG_BANK_MAP_DATA);

const unsigned char builtInMapData[1024] = {

// Demo 1
0x19, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2B, 0x00, 0x00, 0x00, 
0x53, 0x49, 0x4D, 0x50, 0x4C, 0x45, 0x20, 0x44, 0x45, 0x4D, 0x4F, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x40, 0x43, 0x50, 0x50, 0x43, 0x48, 0x52, 0x49, 0x53, 0x43, 0x50, 0x50, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xC0, 0x02, 0x5A, 0xC0, 0x02, 0x5A, 0x80, 0x82, 0x53, 0xC0, 0x02, 0x5A, 0xC0, 0x02, 0x5A, 0x80, 
0x82, 0x53, 0xC0, 0x02, 0x5A, 0xC0, 0x3A, 0x5A, 0x00, 0x00, 0x00, 0x12, 0x12, 0x52, 0x00, 0x00, 
0x40, 0x02, 0x12, 0x50, 0x02, 0x38, 0x50, 0x02, 0x00, 0x40, 0x02, 0x12, 0x52, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x12, 0x12, 0x52, 0x09, 0x09, 0x49, 0x09, 0x09, 0x49, 0x39, 0x89, 0x09, 0x09, 
0x09, 0x49, 0x09, 0x09, 0x49, 0x09, 0x09, 0x49, 0xC0, 0x40, 0x04, 0x80, 0x12, 0x12, 0x00, 0x02, 
0x10, 0x00, 0x02, 0x10, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x02, 0x10, 0x00, 0x02, 0x10, 
0x49, 0x8F, 0x49, 0x4B, 0x9B, 0x5B, 0x49, 0x89, 0x49, 0x5B, 0x9B, 0x19, 0x49, 0x89, 0x49, 0xDB, 
0x99, 0x4B, 0xDB, 0x99, 0x4B, 0x49, 0x89, 0x09, 0x00, 0x00, 0x00, 0x90, 0x12, 0x52, 0x00, 0x17, 
0x00, 0x90, 0x12, 0x12, 0x00, 0x02, 0x00, 0x92, 0x02, 0x12, 0x00, 0x00, 0x00, 0x92, 0x12, 0x52, 
0x00, 0x00, 0x00, 0x92, 0x12, 0xA4, 0x00, 0x00, 0x00, 0x82, 0x12, 0x10, 0x82, 0x12, 0x52, 0x82, 
0x12, 0x52, 0x87, 0x12, 0x52, 0x92, 0x12, 0x52, 0x00, 0xA0, 0xD0, 0x00, 0x20, 0x10, 0x24, 0x20, 
0x10, 0x00, 0x00, 0x10, 0x10, 0x52, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x12, 0x52, 

/*
    Example with values organized
    
    // 0x00:Id
    GAME_DATA_VERSION_ID,
    // 0x01:Tileset id
    CHR_BANK_ZORIA_DESERT,
    // 0x02:Game Style
    GAME_STYLE_MAZE,
    // 0x03:Song Id
    SONG_OVERWORLD,
    // 0x04:Sprite Id
    0x01,
    // 0x05:Unused/reserved data (0x03 bytes)
    0x00, 0x00, 0x00,
    
    // 0x08:Start positions for all 8 levels
    0x0a, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

    // 0x10:Game Title (12 bytes, raw.)
    'S', 'o', 'm', 'e', ' ', 'G', 'a', 'm', 'e', ' ', ' ', ' ',
    // 0x11, 0x25, 0x73, 0x17, 0x02, 0x00, 0x77, 0x00, 0x45, 0x00, 0x00, 0x00, 
    // 0x1c:Author name (12 bytes, same as above)
    '@', 'c', 'p', 'p', 'c', 'h', 'r', 'i', 's', 'c', 'p', 'p', 
    //0x28:Credits Screen (24 bytes, same as above -- could expand to 32 with 6bit, but punctuation would be lost)
    0x11, 0x25, 0x73, 0x17, 0x02, 0x00, 0x77, 0x00, 0x45, 0x00, 0x00, 0x00, 
    0x11, 0x25, 0x73, 0x17, 0x02, 0x00, 0x77, 0x00, 0x45, 0x00, 0x00, 0x00, 

    //0x40:Game Data (192 bytes, 24 bytes per level, (8 levels) 6bit encoded; expands to 32 per level, or... theoretically 256)
    
    //L1
    0x01, 0x13, 0x25,
    0xc1, 0x53, 0xe5,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,

    //L2
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0x1c, 0xa8, 0x1c,
    0xc1, 0x53, 0xe5,
    0x01, 0x13, 0x25,

    //L3
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,

    //L4
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    
    //L5
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    
    //L6
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    
    //L7
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    
    //L8
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,




*/

};