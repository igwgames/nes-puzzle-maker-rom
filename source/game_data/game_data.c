// Saving and loading game data, parsing it out of large structures...
// Also handles the temporary storage for the editor.
#include "source/neslib_asm/neslib.h"

unsigned char currentEditorGame[256];

void unload_editor_game() {
    memfill(currentEditorGame, 0x00, 256);
}

void load_editor_game(unsigned char* address) {
    memcpy(currentEditorGame, address, 256);
}

void save_editor_game(unsigned char* address) {
    memcpy(address, currentEditorGame, 256);
}

void load_game(unsigned char* address) {
    // FIXME: implement
}

void list_games() {
    // FIXME: Implement.. also what does this even do?
}