#include "source/c/configuration/system_constants.h"

// Loads the map at the player's current position into the ram variable given. 
void load_map();

// Load just the tiles and palette; not the level itself or other variable manipulation
// NOTE: This is banked, unlike everything else.
void load_map_tiles_and_palette();

void save_map();

extern const unsigned char spritePalettes[];