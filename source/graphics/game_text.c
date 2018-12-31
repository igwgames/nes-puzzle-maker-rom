#include "source/neslib_asm/neslib.h"
#include "source/globals.h"
#include "source/configuration/system_constants.h"
#include "source/configuration/game_states.h"
#include "source/library/bank_helpers.h"
#include "source/menus/error.h"
#include "source/graphics/game_text.h"
#include "source/graphics/hud.h"

unsigned char* currentText;
unsigned char currentBank;

#define haveHitNull tempChar1
#define bufferIndex tempChar2
#define hasInput tempChar3
#define currentChar tempChar4
#define stringIndex tempInt1

