#define STATIC_SCREEN_AFTER_LEVEL 0
#define STATIC_SCREEN_AFTER_SCREEN 64
#define STATIC_SCREEN_AFTER_SPLASH 100
#define STATIC_SCREEN_AFTER_TITLE 101
#define STATIC_SCREEN_AFTER_ENDING 102
#define STATIC_SCREEN_AFTER_CREDITS 103
#define STATIC_SCREEN_UNUSED 255

extern const unsigned char user_staticScreenTypes[14];
extern const unsigned char user_staticScreens[/* a lot */];

extern void show_relevant_screen(unsigned char position);