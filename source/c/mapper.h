#define BANK_USER_DATA 0
#define BANK_MENUS 1
#define BANK_GRAPHICS 2
#define BANK_PLAYER 3
#define BANK_SOUND 4
#define BANK_STATIC_SC 5
#define BANK_UNKNOWN04 6

// Set the prg bank currently used in the first slot.
extern void __fastcall__ unrom_set_prg_bank(unsigned char bank);

