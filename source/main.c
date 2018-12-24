/*
main.c is the entrypoint of your game. Everything starts from here.
This has the main loop for the game, which is then used to call out to other code.
*/

#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/game_states.h"
#include "source/menus/title.h"
#include "source/globals.h"
#include "source/menus/error.h"
#include "source/menus/credits.h"
#include "source/map/load_map.h"
#include "source/map/map.h"
#include "source/graphics/game_text.h"
#include "source/graphics/hud.h"
#include "source/graphics/fade_animation.h"
#include "source/sprites/player.h"
#include "source/menus/pause.h"
#include "source/sprites/map_sprites.h"
#include "source/sprites/sprite_definitions.h"
#include "source/menus/input_helpers.h"
#include "source/menus/game_over.h"
#include "source/game_data/game_data.h"
#include "source/menus/editor_info.h"
#include "source/menus/list_games.h"


// Method to set a bunch of variables to default values when the system starts up.
// Note that if variables aren't set in this method, they will start at 0 on NES startup.
void initialize_variables() {

    playerOverworldPosition = 0; // Which tile on the overworld to start with; 0-62
    playerHealth = 5; // Player's starting health - how many hearts to show on the HUD.
    playerMaxHealth = 5; // Player's max health - how many hearts to let the player collect before it doesn't count.
    playerXPosition = (128 << PLAYER_POSITION_SHIFT); // X position on the screen to start (increasing numbers as you go left to right. Just change the number)
    playerYPosition = (128 << PLAYER_POSITION_SHIFT); // Y position on the screen to start (increasing numbers as you go top to bottom. Just change the number)
    playerDirection = SPRITE_DIRECTION_DOWN; // What direction to have the player face to start.

    lastPlayerSpriteCollisionId = NO_SPRITE_HIT;

    movementInProgress = 0;
    playerGridPosition = 0;
    selectedGameId = 0;
    
    // Little bit of generic initialization below this point - we need to set
    // The system up to use a different hardware bank for sprites vs backgrounds.
    bank_spr(1);
}   

void main() {
    fade_out_instant();
    gameState = GAME_STATE_SYSTEM_INIT;

    while (1) {
        everyOtherCycle = !everyOtherCycle;
        switch (gameState) {
            case GAME_STATE_SYSTEM_INIT:
                initialize_variables();
                gameState = GAME_STATE_TITLE_DRAW;
                break;

            case GAME_STATE_TITLE_DRAW:
                banked_call(PRG_BANK_TITLE, draw_title_screen);
                music_play(SONG_TITLE);
                fade_in();
                break;
            case GAME_STATE_TITLE_INPUT:
                banked_call(PRG_BANK_TITLE, handle_title_input);
                break;
            case GAME_STATE_POST_TITLE:
                currentLevelId = 7;

                gameTime = frameCount;
                gameKeys = 0;
                gameCrates = 0;


                fade_out();
                
                bank_push(PRG_BANK_GAME_LIST);
                draw_list_games(0);
                fade_in();
                do_list_game_input(0);
                bank_pop();
                music_stop();
                gameState = GAME_STATE_LOAD_LEVEL_1;
                break;
            case GAME_STATE_LOAD_LEVEL:
            case GAME_STATE_LOAD_LEVEL_1:
                playerKeyCount = 0;
                playerCrateCount = 0;
                fade_out();
                oam_clear();

                load_game();
                load_map();

                banked_call(PRG_BANK_MAP_LOGIC, draw_current_map_to_a);
                banked_call(PRG_BANK_MAP_LOGIC, init_map);
                banked_call(PRG_BANK_MAP_LOGIC, load_sprites);
                // Set player position -- NOTE: this might not actually be ideal here. 
                playerSpriteTileId = ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0x01)<<3) + ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0xfe)<<5);

                
                // The draw map methods handle turning the ppu on/off, but we weren't quite done yet. Turn it back off.
                ppu_off();
                banked_call(PRG_BANK_HUD, draw_hud);
                ppu_on_all();

                // Seed the random number generator here, using the time since console power on as a seed
                set_rand(frameCount);

                if (gameState == GAME_STATE_LOAD_LEVEL_1) {
                    music_play(currentGameData[GAME_DATA_OFFSET_SONG_ID]);
                }
                
                // Map drawing is complete; let the player play the game!
                
                fade_in();
                gameState = GAME_STATE_RUNNING;

                break;

            case GAME_STATE_RUNNING:
                // TODO: Might be nice to have this only called when we have something to update, and maybe only update the piece we 
                // care about. (For example, if you get a key, update the key count; not everything!
                banked_call(PRG_BANK_HUD, update_hud);
                banked_call(PRG_BANK_MAP_SPRITES, update_map_sprites);
                banked_call(PRG_BANK_PLAYER_SPRITE, handle_player_movement);
                banked_call(PRG_BANK_PLAYER_SPRITE, update_player_sprite);
                break;
            case GAME_STATE_EDITOR_INIT:
                currentLevelId = 0;

                fade_out();

                bank_push(PRG_BANK_GAME_LIST);
                draw_list_games(0);
                fade_in();
                do_list_game_input(0);
                fade_out();
                music_stop();
                load_game();
                bank_pop();

                load_map();
                oam_clear();

                banked_call(PRG_BANK_MAP_LOGIC, draw_current_map_to_a);
                banked_call(PRG_BANK_MAP_LOGIC, init_map);

                // Set player position -- NOTE: this might not actually be ideal here. 
                playerGridPosition = currentGameData[GAME_DATA_OFFSET_START_POSITIONS + currentLevelId];
                playerSpriteTileId = ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0x01)<<3) + ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0xfe)<<5);


                playerGridPosition = 0;
    
                // The draw map methods handle turning the ppu on/off, but we weren't quite done yet. Turn it back off.
                ppu_off();
                banked_call(PRG_BANK_HUD, draw_editor_hud);
                banked_call(PRG_BANK_MAP_LOGIC, draw_editor_help);
                music_play(currentGameData[GAME_DATA_OFFSET_SONG_ID]);
                ppu_on_all();

                playerGridPosition = 0;

                // TODO: Load up game music for this map

                fade_in();
                gameState = GAME_STATE_EDITOR;
                break;
            case GAME_STATE_EDITOR:
                
                banked_call(PRG_BANK_PLAYER_SPRITE, handle_editor_input);

                banked_call(PRG_BANK_HUD, update_editor_hud);

                break;
            case GAME_STATE_EDITOR_REDRAW:
                oam_clear();
                fade_out();
                load_map(); // FIXME: This will overwrite any saved data in every redraw. Be sure to always call save.
                banked_call(PRG_BANK_MAP_LOGIC, draw_current_map_to_a);
                banked_call(PRG_BANK_MAP_LOGIC, init_map);


                ppu_off();
                banked_call(PRG_BANK_HUD, draw_editor_hud);
                banked_call(PRG_BANK_MAP_LOGIC, draw_editor_help);
                ppu_on_all();
                fade_in();
                gameState = GAME_STATE_EDITOR;

                break;
            case GAME_STATE_EDITOR_INFO:
                fade_out();
                banked_call(PRG_BANK_EDITOR_INFO, draw_editor_info);
                fade_in();
                banked_call(PRG_BANK_EDITOR_INFO, handle_editor_info_input);
                gameState = GAME_STATE_EDITOR_REDRAW;
                
                break;
            case GAME_STATE_SCREEN_SCROLL:
                // Hide all non-player sprites in play, so we have an empty screen to add new ones to
                oam_hide_rest(FIRST_ENEMY_SPRITE_OAM_INDEX);

                // If you don't like the screen scrolling transition, you can replace the transition with `do_fade_screen_transition`
                // banked_call(PRG_BANK_MAP_LOGIC, do_scroll_screen_transition);
                banked_call(PRG_BANK_MAP_LOGIC, do_fade_screen_transition);
                break;
            case GAME_STATE_SHOWING_TEXT:
                banked_call(PRG_BANK_GAME_TEXT, draw_game_text);
                gameState = GAME_STATE_RUNNING;
                break;
            case GAME_STATE_PAUSED:
                sfx_play(SFX_MENU_OPEN, SFX_CHANNEL_4);  
                fade_out();
                banked_call(PRG_BANK_PAUSE_MENU, draw_pause_screen);
                fade_in();
                banked_call(PRG_BANK_PAUSE_MENU, handle_pause_input);
                if (gameState != GAME_STATE_RUNNING) {
                    break;
                }

                // When we get here, the player has unpaused. 
                // Pause has its own mini main loop in handle_input to make logic easier.
                sfx_play(SFX_MENU_CLOSE, SFX_CHANNEL_4);
                fade_out();
                banked_call(PRG_BANK_MAP_LOGIC, draw_current_map_to_a);
                banked_call(PRG_BANK_MAP_LOGIC, init_map);
                
                // The draw map methods handle turning the ppu on/off, but we weren't quite done yet. Turn it back off.
                ppu_off();
                banked_call(PRG_BANK_HUD, draw_hud);
                ppu_on_all();
                fade_in();

                break;
            case GAME_STATE_GAME_OVER:
                fade_out();

                // Draw the "you lose" screen
                banked_call(PRG_BANK_GAME_OVER, draw_game_over_screen);
                fade_in();
                banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start);
                fade_out();
                reset();
                break;
            case GAME_STATE_CREDITS:
                music_stop();
                sfx_play(SFX_WIN, SFX_CHANNEL_1);

                fade_out();
                // Draw the "you won" screen
                banked_call(PRG_BANK_CREDITS_MENU, draw_win_screen);
                fade_in();
                banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start);
                fade_out();

                // Folow it up with the credits.
                banked_call(PRG_BANK_CREDITS_MENU, draw_credits_screen);
                fade_in();
                banked_call(PRG_BANK_MENU_INPUT_HELPERS, wait_for_start);
                fade_out();
                reset();
                break;
            default:
                crash_error_use_banked_details(ERR_UNKNOWN_GAME_STATE, ERR_UNKNOWN_GAME_STATE_EXPLANATION, "gameState value", gameState);
                
        }
        ppu_wait_nmi();
        
    }
}
