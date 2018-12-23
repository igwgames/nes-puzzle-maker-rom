// Some defines for the elements in the HUD
#include "source/library/bank_helpers.h"

#define PRG_BANK_HUD 2

#define HUD_POSITION_START 0x0300
#define HUD_HEART_START 0x0356
#define HUD_KEY_START 0x037d
#define HUD_ATTRS_START 0x03f0
#define HUD_EDITOR_TITLE_START 0x0380
#define HUD_EDITOR_TILES_START 0x344
#define HUD_INFO_ICON_START 0x035c

#define HUD_EDITOR_TILES_SPRITE_START 0x20

#define HUD_TILE_HEART 0xe7
#define HUD_TILE_HEART_EMPTY 0xe9
#define HUD_TILE_KEY 0xe8
#define HUD_TILE_NUMBER 0x90
#define HUD_TILE_BLANK 0x80
#define HUD_TILE_BORDER_BL 0xf4
#define HUD_TILE_BORDER_BR 0xf5
#define HUD_TILE_BORDER_HORIZONTAL 0xe5
#define HUD_TILE_BORDER_VERTICAL 0xe4

#define HUD_SPRITE_ZERO_TILE_ID 0xfb

#define HUD_PLAYER_SPRITE_OAM 0xf0
#define HUD_SELECTOR_SPRITE_OAM 0xe0
#define HUD_PLAYER_SPRITE_ID 0x00
#define HUD_SELECTOR_SPRITE_ID 0xe0

ZEROPAGE_EXTERN(unsigned char, editorSelectedTileId);


// Draw the HUD
void draw_hud();

void draw_editor_hud();

// Update the number of hearts, coins, etc in the hud.
void update_hud();

void update_editor_hud();

// Draw a string using hud characters...
void put_hud_str(unsigned int adr, const char* str);
