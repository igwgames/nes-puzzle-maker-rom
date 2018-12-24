#include "source/neslib_asm/neslib.h"
#include "source/menus/editor_info.h"
#include "source/menus/text_helpers.h"
#include "source/configuration/game_states.h"
#include "source/configuration/system_constants.h"
#include "source/library/bank_helpers.h"
#include "source/globals.h"
#include "source/menus/input_helpers.h"
#include "source/graphics/palettes.h"
#include "source/menus/about_screen.rle.h"
#include "source/game_data/game_data.h"
#include "source/menus/text_input.h"
#include "source/graphics/fade_animation.h"
#include "source/map/load_map.h"
#include "source/map/map.h"
#include "source/menus/list_games.h"

#define editorInfoPosition tempChar1
#define editorInfoPositionFull tempChar2
#define editorInfoPositionLeft tempChar4
#define tempScreenByte tempChar3
#define redraw tempChar9 // Using a higher one, so it's unlikely our called functions steal it.
#define editorInfoTempInt tempInt1


CODE_BANK(PRG_BANK_EDITOR_INFO);


const unsigned char spriteTilesetName0[] = "Arcade";
const unsigned char spriteTilesetName1[] = "Zoria";
const unsigned char* spriteTilesetNames[] = { spriteTilesetName0, spriteTilesetName1, spriteTilesetName0, spriteTilesetName1, spriteTilesetName1, spriteTilesetName1 };

void draw_editor_info() {
    oam_clear();
    ppu_off();
    //clear_screen_with_border();
    // We reuse the title palette here, though we have the option of making our own if needed.
    // ACtually we're using the game palette... because we need to show real tiles n stuff
    // FIXME: Also show player.

    vram_adr(0x2000);
    vram_unrle(about_screen);
    //pal_bg(titlePalette);
	//pal_spr(titlePalette);

    // Now draw all the user-generated stuff onto the screen
    vram_adr(NTADR_A(10, 4));
    for (i = 0; i != GAME_DATA_OFFSET_TITLE_LENGTH; ++i) {
        vram_put(currentGameData[GAME_DATA_OFFSET_TITLE+i] + 0x60);
    }

    vram_adr(NTADR_A(11, 20));
    for (i = 0; i != GAME_DATA_OFFSET_AUTHOR_LENGTH; ++i) {
        vram_put(currentGameData[GAME_DATA_OFFSET_AUTHOR+i] + 0x60);
    }

    vram_adr(NTADR_A(12, 6));
    i = 0;
    editorInfoTempInt = (int)tilesetNames[currentGameData[GAME_DATA_OFFSET_TILESET_ID]-2];
    while ((j = ((unsigned char*)editorInfoTempInt)[i++]) != NULL) {
        vram_put(j + 0x60);
    }

    vram_adr(NTADR_A(15, 16));
    i = 0; 
    editorInfoTempInt = (int)gameModeNames[currentGameData[GAME_DATA_OFFSET_GAME_STYLE]];
    while ((j = ((unsigned char*)editorInfoTempInt)[i++]) != NULL) {
        vram_put(j + 0x60); 
    }

    vram_adr(NTADR_A(10, 18));
    i = 0; 
    editorInfoTempInt = (int)songNames[currentGameData[GAME_DATA_OFFSET_SONG_ID]];
    while ((j = ((unsigned char*)editorInfoTempInt)[i++]) != NULL) {
        vram_put(j + 0x60); 
    }

    vram_adr(NTADR_A(18, 11));
    i = 0; 
    editorInfoTempInt = (int)spriteTilesetNames[currentGameData[GAME_DATA_OFFSET_SPRITE_ID]];
    while ((j = ((unsigned char*)editorInfoTempInt)[i++]) != NULL) {
        vram_put(j + 0x60); 
    }


    // Recolor tileset area
    vram_adr(0x23d1); // (Gotten from NES Screen Tool)
    tempScreenByte = 0x50;
    for (i = 0; i != 8; ++i) {
        if (i & 0x01) {
            tempScreenByte |= (currentMapTileData[(i<<2)+1] << 2);
            vram_put(tempScreenByte);
            tempScreenByte = 0x50;
        } else {
            tempScreenByte |= currentMapTileData[(i<<2)+1];
        }
    }


    scroll(0, 0);

    // Actually, keep the same one
	//set_chr_bank_0(CHR_BANK_MENU);
    //set_chr_bank_1(CHR_BANK_MENU);

    // Just write "- Paused -" on the screen... there's plenty of nicer things you could do if you wanna spend time!
    //put_str(NTADR_A(11, 13), "- Paused -");



    set_vram_update(NULL);
    ppu_on_all();
}

#define EDITOR_INFO_POSITION_TITLE 0
#define EDITOR_INFO_POSITION_TILESET 1
#define EDITOR_INFO_POSITION_MUSIC 4
#define EDITOR_INFO_POSITION_SPRITE 2
#define EDITOR_INFO_POSITION_AUTHOR 5
#define EDITOR_INFO_POSITION_PLAY_STYLE 3
#define EDITOR_INFO_POSITION_SAVE 6
#define EDITOR_INFO_POSITION_EXPORT 7

#define EDITOR_INFO_PLAYER_X (9<<3)
#define EDITOR_INFO_PLAYER_Y (13<<3)-2

void handle_editor_info_input() {
    editorInfoPosition = 0;

    do_redraw:
    screenBuffer[0] = MSB(NTADR_A(3, 4)) | NT_UPD_VERT;
    screenBuffer[1] = LSB(NTADR_A(3, 4));
    screenBuffer[2] = 20;
    for (i = 3; i != 23; ++i) {
        // FIXME: Constant
        screenBuffer[i] = 0xee;
    }
    screenBuffer[23] = MSB(NTADR_A(7, 25));
    screenBuffer[24] = LSB(NTADR_A(7, 25));
    screenBuffer[25] = ' ' + 0x60;
    screenBuffer[26] = MSB(NTADR_A(17, 25));
    screenBuffer[27] = LSB(NTADR_A(17, 25));
    screenBuffer[28] = ' ' + 0x60;
    screenBuffer[29] = MSB(NTADR_A(3, 4));
    screenBuffer[30] = LSB(NTADR_A(3, 4));
    // FIXME: Constant
    screenBuffer[31] = 0xe2;
    // NOTE: This is just a random tile to update and keep black - this should be updated when we need a second tile.
    screenBuffer[32] = MSB(NTADR_A(8, 3));
    screenBuffer[33] = LSB(NTADR_A(8, 3));
    screenBuffer[34] = 0xee; // AGAIN, FIXME: Constant
    screenBuffer[35] = NT_UPD_EOF;
    set_vram_update(screenBuffer);
    redraw = 0;
    while (1) {
        lastControllerState = controllerState;
        controllerState = pad_poll(0);

        // If Start is pressed now, and was not pressed before...
        if (controllerState & PAD_START && !(lastControllerState & PAD_START)) {
            break;
        }

        if (controllerState & PAD_UP && !(lastControllerState & PAD_UP)) {
            if (editorInfoPosition != 0) {
                --editorInfoPosition;
            }
            sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
        }

        if (controllerState & PAD_DOWN && !(lastControllerState & PAD_DOWN)) {
            if (editorInfoPosition != EDITOR_INFO_POSITION_EXPORT) {
                ++editorInfoPosition;
            }
            sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
        }

        if (controllerState & PAD_A && !(lastControllerState & PAD_A)) {
            if (editorInfoPosition == EDITOR_INFO_POSITION_TITLE) { 
                memcpy(inputText, &(currentGameData[GAME_DATA_OFFSET_TITLE]), 12);
                do_text_input("game name", 12);
                memcpy(&(currentGameData[GAME_DATA_OFFSET_TITLE]), inputText, 12);
                redraw = 1;
                break;
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_AUTHOR) {
                memcpy(inputText, &(currentGameData[GAME_DATA_OFFSET_AUTHOR]), 12);
                do_text_input("author", 12);
                memcpy(&(currentGameData[GAME_DATA_OFFSET_AUTHOR]), inputText, 12);
                redraw = 1;
                break;
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_SAVE) {
                // You hit save. So, let's save.
                fade_out();
                bank_draw_list_games(1);
                fade_in();
                bank_do_list_game_input(1);
                redraw = 1;
                save_game();
                break;

            }

        }

        if (controllerState & PAD_LEFT && !(lastControllerState & PAD_LEFT)) {
            if (editorInfoPosition == EDITOR_INFO_POSITION_TILESET) {
                --currentGameData[GAME_DATA_OFFSET_TILESET_ID];
                if (currentGameData[GAME_DATA_OFFSET_TILESET_ID] < CHR_BANK_ARCADE) {
                    currentGameData[GAME_DATA_OFFSET_TILESET_ID] = CHR_BANK_LAST;
                }
                redraw = 1;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
                break;
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_MUSIC) {
                --currentGameData[GAME_DATA_OFFSET_SONG_ID];
                if (currentGameData[GAME_DATA_OFFSET_SONG_ID] == 255) {
                    currentGameData[GAME_DATA_OFFSET_SONG_ID] = SONG_COUNT - 1;
                }
                music_play(currentGameData[GAME_DATA_OFFSET_SONG_ID]);
                redraw = 1;
                break;
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_PLAY_STYLE) {
                --currentGameData[GAME_DATA_OFFSET_GAME_STYLE];
                if (currentGameData[GAME_DATA_OFFSET_GAME_STYLE] == 255) {
                    currentGameData[GAME_DATA_OFFSET_GAME_STYLE] = GAME_STYLE_COUNT - 1;
                }
                redraw = 1;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
                break;
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_SAVE) {
                editorInfoPosition = EDITOR_INFO_POSITION_EXPORT;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_EXPORT) {
                editorInfoPosition = EDITOR_INFO_POSITION_SAVE;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_SPRITE) {
                --currentGameData[GAME_DATA_OFFSET_SPRITE_ID];
                if (currentGameData[GAME_DATA_OFFSET_SPRITE_ID] == 255) {
                    currentGameData[GAME_DATA_OFFSET_SPRITE_ID] = GAME_SPRITE_COUNT-1;
                }
                playerSpriteTileId = ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0x01)<<3) + ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0xfe)<<5);
                redraw = 1;
                break;
            }
        }

        if ((controllerState & PAD_RIGHT && !(lastControllerState & PAD_RIGHT)) || (controllerState & PAD_A && !(lastControllerState & PAD_A))) {
            if (editorInfoPosition == EDITOR_INFO_POSITION_TILESET) {
                ++currentGameData[GAME_DATA_OFFSET_TILESET_ID];
                if (currentGameData[GAME_DATA_OFFSET_TILESET_ID] > CHR_BANK_LAST) {
                    currentGameData[GAME_DATA_OFFSET_TILESET_ID] = CHR_BANK_ARCADE;
                }
                redraw = 1;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
                break;
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_MUSIC) {
                ++currentGameData[GAME_DATA_OFFSET_SONG_ID];
                if (currentGameData[GAME_DATA_OFFSET_SONG_ID] == SONG_COUNT) {
                    currentGameData[GAME_DATA_OFFSET_SONG_ID] = 0;
                }
                music_play(currentGameData[GAME_DATA_OFFSET_SONG_ID]);
                redraw = 1;
                break;
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_PLAY_STYLE) {
                ++currentGameData[GAME_DATA_OFFSET_GAME_STYLE];
                if (currentGameData[GAME_DATA_OFFSET_GAME_STYLE] == GAME_STYLE_COUNT) {
                    currentGameData[GAME_DATA_OFFSET_GAME_STYLE] = 0;
                }
                redraw = 1;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
                break;
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_SAVE) {
                editorInfoPosition = EDITOR_INFO_POSITION_EXPORT;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_EXPORT) {
                editorInfoPosition = EDITOR_INFO_POSITION_SAVE;
                sfx_play(SFX_MENU_BOP, SFX_CHANNEL_4);
            } else if (editorInfoPosition == EDITOR_INFO_POSITION_SPRITE) {
                ++currentGameData[GAME_DATA_OFFSET_SPRITE_ID];
                if (currentGameData[GAME_DATA_OFFSET_SPRITE_ID] == GAME_SPRITE_COUNT) {
                    currentGameData[GAME_DATA_OFFSET_SPRITE_ID] = 0;
                }
                playerSpriteTileId = ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0x01)<<3) + ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0xfe)<<5);
                redraw = 1;
                break;
            }
        }

        editorInfoPositionLeft = 3;
        switch (editorInfoPosition) {
            case 0:
                editorInfoPositionFull = 4;
                break;
            case 1:
                editorInfoPositionFull = 6;
                break;
            case 2:
                editorInfoPositionFull = 11;
                break;
            case 3:
                editorInfoPositionFull = 16;
                break;
            case 4:
                editorInfoPositionFull = 18;
                break;
            case 5:
                editorInfoPositionFull = 20;
                break;
            case 6:
                editorInfoPositionFull = 25;
                editorInfoPositionLeft = 7;
                break;
            case 7:
                editorInfoPositionFull = 25;
                editorInfoPositionLeft = 17;
                break;
            default:
                editorInfoPositionFull = 10 + ((editorInfoPosition-2)*3);
                break;
        }
        editorInfoTempInt = NTADR_A(editorInfoPositionLeft, editorInfoPositionFull);
        screenBuffer[29] = MSB(editorInfoTempInt);
        screenBuffer[30] = LSB(editorInfoTempInt);


        oam_spr(EDITOR_INFO_PLAYER_X, EDITOR_INFO_PLAYER_Y, playerSpriteTileId, 0x03, 0xd0); // FIXME: OAM id constant
        oam_spr(EDITOR_INFO_PLAYER_X + NES_SPRITE_WIDTH, EDITOR_INFO_PLAYER_Y, playerSpriteTileId + 1, 0x03, 0xd0+4);
        oam_spr(EDITOR_INFO_PLAYER_X, EDITOR_INFO_PLAYER_Y + NES_SPRITE_HEIGHT, playerSpriteTileId + 16, 0x03, 0xd0+8);
        oam_spr(EDITOR_INFO_PLAYER_X + NES_SPRITE_WIDTH, EDITOR_INFO_PLAYER_Y + NES_SPRITE_HEIGHT, playerSpriteTileId + 17, 0x03, 0xd0+12);


        ppu_wait_nmi();

    }
    if (redraw) {
        fade_out();
        // Use new palettes immediately
        banked_call(PRG_BANK_MAP_LOGIC,  load_map_tiles_and_palette);
        draw_editor_info();
        fade_in();
        goto do_redraw;
    }
    gameState = GAME_STATE_EDITOR;
    oam_clear();
    sfx_play(SFX_MENU_CLOSE, SFX_CHANNEL_4);
}