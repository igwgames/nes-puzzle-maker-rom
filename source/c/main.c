/*
main.c is the entrypoint of your game. Everything starts from here.
This has the main loop for the game, which is then used to call out to other code.
*/

#include "source/c/neslib.h"
#include "source/c/mapper.h"
#include "source/c/library/user_data.h"
#include "source/c/library/bank_helpers.h"
#include "source/c/configuration/game_states.h"
#include "source/c/menus/title.h"
#include "source/c/globals.h"
#include "source/c/menus/credits.h"
#include "source/c/map/load_map.h"
#include "source/c/map/map.h"
#include "source/c/graphics/hud.h"
#include "source/c/graphics/fade_animation.h"
#include "source/c/sprites/player.h"
#include "source/c/menus/pause.h"
#include "source/c/menus/input_helpers.h"
#include "source/c/menus/intro.h"
#include "graphics/splash.h"

extern void load_graphics(void);

// Method to set a bunch of variables to default values when the system starts up.
// Note that if variables aren't set in this method, they will start at 0 on NES startup.
void initialize_variables() {

    playerDirection = SPRITE_DIRECTION_DOWN; // What direction to have the player face to start.

    playerGridPositionX = 0;
    playerGridPositionY = 0;
    
    // Little bit of generic initialization below this point - we need to set
    // The system up to use a different hardware bank for sprites vs backgrounds.
    bank_spr(0);
}   

void main() {
    fade_out_instant();
    gameState = GAME_STATE_SYSTEM_INIT;

    while (1) {
        everyOtherCycle = !everyOtherCycle;
        switch (gameState) {
            case GAME_STATE_SYSTEM_INIT:
                unrom_set_prg_bank(BANK_USER_DATA);
                initialize_variables();
                load_user_data();
                set_vram_update(NULL);
                pal_bg(gamePaletteData);
                pal_spr(spritePalette);
                unrom_set_prg_bank(BANK_GRAPHICS);
                load_graphics();

                oam_clear();

                // Trick to allow us to patch the rom from the editor to pick the starting level, for testing.
                if (singleLevelOverride != 255) {
                    // Jump straight into the game without showing the title
                    gameState = GAME_STATE_POST_TITLE;
                    fade_in_fast();
                    continue;
                }
                gameState = GAME_STATE_TITLE_DRAW;
                break;

            case GAME_STATE_TITLE_DRAW:

                // Draw splash screen using data in rle format
                ppu_off();
                vram_adr(0x2000);
                vram_unrle(splash);
                ppu_on_all();
                fade_in();
                // Wait a few... 
                // If you're editing your own copy, please consider leaving this, since the tool helped you get here.
                // Please? I can't stop you from deleting the splash, of course, it'd just make me happy to see it.
                delay(30);
                unrom_set_prg_bank(BANK_MENUS);
                wait_for_start();
                fade_out();
                
                draw_title_screen();
                unrom_set_prg_bank(BANK_SOUND);
                music_play(titleSong);
                fade_in();
                break;
            case GAME_STATE_TITLE_INPUT:
                unrom_set_prg_bank(BANK_MENUS);
                wait_for_start();
                gameState = GAME_STATE_POST_TITLE;
                break;
            case GAME_STATE_POST_TITLE:
                currentLevelId = 0;

                gameTime = frameCount;
                gameCollectableCount = 0;
                gameCrates = 0;

                if (singleLevelOverride != 255) {
                    currentLevelId = singleLevelOverride;
                } else if (introScreenEnabled) {
                    fade_out();
                    load_map(); // Needed to get proper tile data loaded 

                    draw_intro_screen();
                    fade_in();
                    unrom_set_prg_bank(BANK_MENUS);
                    wait_for_start();
                }


                unrom_set_prg_bank(BANK_SOUND);
                music_stop();
                gameState = GAME_STATE_LOAD_LEVEL_1;
                break;
            case GAME_STATE_LOAD_LEVEL:
            case GAME_STATE_LOAD_LEVEL_1: // Used to start music in the case above.
                playerCollectableCount = 0;
                playerCrateCount = 0;
                undoPosition = 0;
                keyCount = 0;
                unrom_set_prg_bank(BANK_PLAYER);
                clear_undo();
                fade_out();
                oam_clear();

                load_map();

                ppu_off();
                unrom_set_prg_bank(BANK_PLAYER);
                draw_current_map_to_a_inline();
                ppu_on_all();
                //unrom_set_prg_bank(BANK_PLAYER);
                init_map();

                // Aim the player down to start
                playerSpriteTileId = 0x40;

                
                // The draw map methods handle turning the ppu on/off, but we weren't quite done yet. Turn it back off.
                ppu_off();
                unrom_set_prg_bank(BANK_PLAYER);
                draw_hud();
                ppu_on_all();

                // Seed the random number generator here, using the time since console power on as a seed
                set_rand(frameCount);

                if (gameState == GAME_STATE_LOAD_LEVEL_1) {
                    unrom_set_prg_bank(BANK_SOUND);
                    music_play(gameplaySong);
                }
                
                // Map drawing is complete; let the player play the game!
                
                fade_in();
                gameState = GAME_STATE_RUNNING;

                break;

            case GAME_STATE_RUNNING:
                unrom_set_prg_bank(BANK_PLAYER);
                update_hud();
                // unrom_set_prg_bank(BANK_PLAYER);
                handle_player_movement();
                update_player_sprite();
                break;
            case GAME_STATE_PAUSED:
                sfx_play(SFX_MENU_OPEN, SFX_CHANNEL_4);  
                fade_out();
                unrom_set_prg_bank(BANK_MENUS);
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
                unrom_set_prg_bank(BANK_PLAYER);
                init_map();
                
                // The draw map methods handle turning the ppu on/off, but we weren't quite done yet. Turn it back off.
                ppu_off();
                unrom_set_prg_bank(BANK_PLAYER);
                draw_hud();
                ppu_on_all();
                fade_in();

                break;
            case GAME_STATE_CREDITS:
            
                unrom_set_prg_bank(BANK_SOUND);
                music_stop();
                music_play(creditsSong);

                fade_out();
                // Draw the "you won" screen
                unrom_set_prg_bank(BANK_MENUS);
                draw_win_screen();

                // Folow it up with the credits.
                draw_credits_screen();
                reset();
                break;
                
        }
        ppu_wait_nmi();
        
    }
}
