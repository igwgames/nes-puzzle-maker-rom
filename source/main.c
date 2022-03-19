/*
main.c is the entrypoint of your game. Everything starts from here.
This has the main loop for the game, which is then used to call out to other code.
*/

#include "source/neslib_asm/neslib.h"
#include "source/library/bank_helpers.h"
#include "source/configuration/game_states.h"
#include "source/menus/title.h"
#include "source/globals.h"
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
#include "source/menus/intro.h"
#include "source/graphics/palettes.h"
#include "graphics/splash.h"


// Method to set a bunch of variables to default values when the system starts up.
// Note that if variables aren't set in this method, they will start at 0 on NES startup.
void initialize_variables() {

    playerOverworldPosition = 0; // Which tile on the overworld to start with; 0-62
    playerHealth = 5; // Player's starting health - how many hearts to show on the HUD.
    playerMaxHealth = 5; // Player's max health - how many hearts to let the player collect before it doesn't count.
    playerDirection = SPRITE_DIRECTION_DOWN; // What direction to have the player face to start.

    lastPlayerSpriteCollisionId = NO_SPRITE_HIT;

    movementInProgress = 0;
    playerGridPositionX = 0;
    playerGridPositionY = 0;
    selectedGameId = 0;
    
    // Little bit of generic initialization below this point - we need to set
    // The system up to use a different hardware bank for sprites vs backgrounds.
    bank_spr(0);
}   

#pragma code-name ("CODE")
#pragma rodata-name ("CODE")
void set_up_graphics() {
    set_vram_update(NULL);
	pal_bg(gamePaletteData);
	pal_spr(spritePalette);

	oam_clear();
}

void main() {
    fade_out_instant();
    gameState = GAME_STATE_SYSTEM_INIT;

    while (1) {
        everyOtherCycle = !everyOtherCycle;
        switch (gameState) {
            case GAME_STATE_SYSTEM_INIT:
                initialize_variables();
                set_up_graphics();
                if (singleLevelOverride != 255) {
                    gameState = GAME_STATE_POST_TITLE;
                    fade_in_fast();
                    continue;
                }
                gameState = GAME_STATE_TITLE_DRAW;
                break;

            case GAME_STATE_TITLE_DRAW:

                ppu_off();
                vram_adr(0x2000);
                vram_unrle(splash);
                ppu_on_all();
                fade_in();
                delay(30);
                wait_for_start();
                fade_out();
            
                draw_title_screen();
                music_play(titleSong);
                fade_in();
                break;
            case GAME_STATE_TITLE_INPUT:
                wait_for_start();
                gameState = GAME_STATE_POST_TITLE;
                break;
            case GAME_STATE_POST_TITLE:
                currentLevelId = 0;

                gameTime = frameCount;
                gameKeys = 0;
                gameCrates = 0;

                if (singleLevelOverride != 255) {
                    currentLevelId = singleLevelOverride;
                } else if (introScreenEnabled ) {
                    fade_out();
                    load_map(); // Needed to get proper tile data loaded 

                    draw_intro_screen();
                    fade_in();
                    handle_intro_input();
                }


                music_stop();
                gameState = GAME_STATE_LOAD_LEVEL_1;
                break;
            case GAME_STATE_LOAD_LEVEL:
            case GAME_STATE_LOAD_LEVEL_1: // Used to start music in the case above.
                playerKeyCount = 0;
                playerCrateCount = 0;
                thisLevelMoves = 0;
                fade_out();
                oam_clear();

                load_map();

                ppu_off();
                draw_current_map_to_a_inline();
                ppu_on_all();
                init_map();
                load_sprites();
                // Set player position -- NOTE: this might not actually be ideal here. 
                // playerSpriteTileId = ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0x01)<<3) + ((currentGameData[GAME_DATA_OFFSET_SPRITE_ID] & 0xfe)<<5);
                playerSpriteTileId = 0x40;

                
                // The draw map methods handle turning the ppu on/off, but we weren't quite done yet. Turn it back off.
                ppu_off();
                draw_hud();
                ppu_on_all();

                // Seed the random number generator here, using the time since console power on as a seed
                set_rand(frameCount);

                if (gameState == GAME_STATE_LOAD_LEVEL_1) {
                    // Song 0: title, song 1: gameplay. No choices.
                    music_play(gameplaySong);
                }
                
                // Map drawing is complete; let the player play the game!
                
                fade_in();
                gameState = GAME_STATE_RUNNING;

                break;

            case GAME_STATE_RUNNING:
                update_hud();
                handle_player_movement();
                update_player_sprite();
                break;
            case GAME_STATE_PAUSED:
                sfx_play(SFX_MENU_OPEN, SFX_CHANNEL_4);  
                fade_out();
                draw_pause_screen();
                fade_in();
                handle_pause_input();
                if (gameState != GAME_STATE_RUNNING) {
                    break;
                }

                // When we get here, the player has unpaused. 
                // Pause has its own mini main loop in handle_input to make logic easier.
                sfx_play(SFX_MENU_CLOSE, SFX_CHANNEL_4);
                fade_out();
                // banked_call(PRG_BANK_MAP_LOGIC, draw_current_map_to_a);
                init_map();
                
                // The draw map methods handle turning the ppu on/off, but we weren't quite done yet. Turn it back off.
                ppu_off();
                draw_hud();
                ppu_on_all();
                fade_in();

                break;
            case GAME_STATE_CREDITS:
                music_stop();
                // sfx_play(SFX_WIN, SFX_CHANNEL_1);
                music_play(creditsSong);

                fade_out();
                // Draw the "you won" screen
                draw_win_screen();
                fade_in();
                wait_for_start();
                fade_out();

                // Folow it up with the credits.
                draw_credits_screen();
                fade_in();
                wait_for_start();
                fade_out();
                reset();
                break;
                
        }
        ppu_wait_nmi();
        
    }
}
