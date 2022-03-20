#define HUD_PIXEL_HEIGHT 48

#define NES_SPRITE_WIDTH 8
#define NES_SPRITE_HEIGHT 8

// Sprite direction definitions
// NOTE: These values are specifically chosen such that if you add direction
// to the top-left sprite in the "down" animation, you'll get the first animation 
// for that direction. It simplifies some logic.
#define SPRITE_DIRECTION_STATIONARY 0x02
#define SPRITE_DIRECTION_LEFT 0x26
#define SPRITE_DIRECTION_RIGHT 0x06
#define SPRITE_DIRECTION_UP 0x20
#define SPRITE_DIRECTION_DOWN 0x00

#define SPRITE_OFFSCREEN 0xfe

// Change this to change how fast sprites animate; values are from 0-6. 0 is fast. 6 is slow.
#define SPRITE_ANIMATION_SPEED_DIVISOR 3

// Constants for sound effects we play in the game
#define SFX_HURT 0
#define SFX_HEART 1
#define SFX_KEY 2
#define SFX_GAMEOVER 3
#define SFX_WIN 4
#define SFX_PLACE_ITEM 5
#define SFX_MENU_BOP 6
#define SFX_MENU_OPEN 7
#define SFX_MENU_CLOSE 8
#define SFX_CRATE_SMASH 9
#define SFX_CRATE_MOVE 10

// Constants for sfx channels. Completely unnecessary but it's easier to read
#define SFX_CHANNEL_1 0
#define SFX_CHANNEL_2 1
#define SFX_CHANNEL_3 2
#define SFX_CHANNEL_4 3