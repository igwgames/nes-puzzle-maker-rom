#include "source/menus/intro.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/configuration/game_states.h"
#include "source/menus/text_helpers.h"
#include "source/menus/input_helpers.h"
#include "source/game_data/game_data.h"
#include "source/map/map.h"

CODE_BANK(PRG_BANK_INTRO_SCREEN);

#define titleLen tempChar1

#define crateTile tempChar2
#define holeTile tempChar3
#define coinTile tempChar4
#define goalTile tempChar5
#define tempTile tempChar6

void draw_intro_screen() {
    ppu_off();
    clear_screen_with_border();
    scroll(0, 0);


    vram_adr(NTADR_A(3,2));
    vram_put(' ' + 0x60);

    titleLen = GAME_DATA_OFFSET_TITLE_LENGTH;
    for (i = 0; i != GAME_DATA_OFFSET_TITLE_LENGTH; ++i) {
        if (currentGameData[GAME_DATA_OFFSET_TITLE+i] == ' ' || currentGameData[GAME_DATA_OFFSET_TITLE+i] == 0) {
            // If all that's left is spaces, don't keep printing
            for (j = i; j != GAME_DATA_OFFSET_TITLE_LENGTH; ++j) {
                if (currentGameData[GAME_DATA_OFFSET_TITLE+j] != ' ' && currentGameData[GAME_DATA_OFFSET_TITLE+j] != 0) {
                    goto keep_going;
                }
            }
            titleLen = i;
            break;
        }
        keep_going:
        vram_put(currentGameData[GAME_DATA_OFFSET_TITLE+i] + 0x60);
    }
    vram_put(' ' + 0x60);

    vram_adr(NTADR_A(15 - (titleLen / 2), 5));
    for (i = 0; i != GAME_DATA_OFFSET_TITLE_LENGTH; ++i) {
        vram_put(currentGameData[GAME_DATA_OFFSET_TITLE+i] + 0x60);
    }

    vram_adr(NTADR_A(3, 25));
    for (i = 0; i != GAME_DATA_OFFSET_AUTHOR_LENGTH; ++i) {
        vram_put(currentGameData[GAME_DATA_OFFSET_AUTHOR+i] + 0x60);
    }

    for (i = 0; i != 8; ++i) {
        switch (currentMapTileData[(i<<2) + TILE_DATA_LOOKUP_OFFSET_COLLISION]) {
            case TILE_COLLISION_COLLECTABLE:
                coinTile = i;
                break;
            case TILE_COLLISION_CRATE:
                crateTile = i;
                break;
            case TILE_COLLISION_GAP:
                holeTile = i;
                break;
            case TILE_COLLISION_LEVEL_END:
                goalTile = i;
                break;
        }
    }

    switch (currentGameData[GAME_DATA_OFFSET_GAME_STYLE]) {
        case GAME_STYLE_MAZE:
            put_str(NTADR_A(7, 16), "Get to the finish!");
            break;
        case GAME_STYLE_COIN:
            put_str(NTADR_A(6, 9), "Collect all coins...");
            put_str(NTADR_A(6, 17), "And get to the finish!");

            vram_adr(NTADR_A(14, 12));
            tempTile = currentMapTileData[((coinTile)<<2)+TILE_DATA_LOOKUP_OFFSET_ID];
            vram_put(tempTile);
            vram_put(tempTile+1);
            vram_adr(NTADR_A(14, 13));
            vram_put(tempTile+16);
            vram_put(tempTile+17);
            vram_adr(NAMETABLE_A + 0x03db);
            tempTile = currentMapTileData[((coinTile)<<2)+TILE_DATA_LOOKUP_OFFSET_PALETTE];
            tempTile = tempTile | (tempTile<<2) | (tempTile<<4) | (tempTile<<6);
            vram_put(tempTile);

            break;
        case GAME_STYLE_CRATES:
            put_str(NTADR_A(6, 9), "Put crates in holes...");
            put_str(NTADR_A(6, 17), "And get to the finish!");
            vram_adr(NTADR_A(14, 12));
            tempTile = currentMapTileData[((crateTile)<<2)+TILE_DATA_LOOKUP_OFFSET_ID];
            vram_put(tempTile);
            vram_put(tempTile+1);
            vram_adr(NTADR_A(14, 13));
            vram_put(tempTile+16);
            vram_put(tempTile+17);
            vram_adr(NAMETABLE_A + 0x03db);
            tempTile = currentMapTileData[((crateTile)<<2)+TILE_DATA_LOOKUP_OFFSET_PALETTE];
            tempTile = tempTile | (tempTile<<2) | (tempTile<<4) | (tempTile<<6);
            vram_put(tempTile);

            break;
    }

    vram_adr(NTADR_A(14, 20));
    tempTile = currentMapTileData[((goalTile)<<2)+TILE_DATA_LOOKUP_OFFSET_ID];
    vram_put(tempTile);
    vram_put(tempTile+1);
    vram_adr(NTADR_A(14, 21));
    vram_put(tempTile+16);
    vram_put(tempTile+17);
    vram_adr(NAMETABLE_A + 0x03eb);
    tempTile = currentMapTileData[((goalTile)<<2)+TILE_DATA_LOOKUP_OFFSET_PALETTE];
    tempTile = tempTile | (tempTile<<2) | (tempTile<<4) | (tempTile<<6);
    vram_put(tempTile);


    // We purposely leave sprites off, so they do not clutter the view. 
    // This means all menu drawing must be done with background tiles - if you want to use sprites (eg for a menu item),
    // you will have to hide all sprites, then put them back after. 
    ppu_on_bg();
}

void handle_intro_input() {
    banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start);

}