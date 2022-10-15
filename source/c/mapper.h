// Effectively pulling these from the .cfg file, with some helper connections in mapper.asm
// This converts it from a pointer back to a number we can use. This makes it so we don't have to use
// a mapper byte.
extern void* BANK_USER_DATA_PTR;
#define BANK_USER_DATA ((unsigned char)&BANK_USER_DATA_PTR)

extern void* BANK_MENUS_PTR;
#define BANK_MENUS ((unsigned char)&BANK_MENUS_PTR)

extern void* BANK_GRAPHICS_PTR;
#define BANK_GRAPHICS ((unsigned char)&BANK_GRAPHICS_PTR)

extern void* BANK_PLAYER_PTR;
#define BANK_PLAYER ((unsigned char)&BANK_PLAYER_PTR)

extern void* BANK_SOUND_PTR;
#define BANK_SOUND ((unsigned char)&BANK_SOUND_PTR)

extern void* BANK_STATIC_SC_PTR;
#define BANK_STATIC_SC ((unsigned char)&BANK_STATIC_SC_PTR)

extern void* BANK_UNKOWN04_PTR;
#define BANK_UNKNOWN04 ((unsigned char)&BANK_UNKNOWN04_PTR)

extern void* USE_COMPRESSION;
#define USE_COMPRESSION ((unsigned char)&USE_COMPRESSION_PTR)


// Set the prg bank currently used in the first slot.
extern void __fastcall__ unrom_set_prg_bank(unsigned char bank);

