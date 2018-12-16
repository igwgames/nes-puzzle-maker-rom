// Saving and loading game data, parsing it out of large structures...
// Also handles the temporary storage for the editor.

extern unsigned char currentEditorGame[256];

void unload_editor_game();

void load_editor_game(unsigned char* address);

void load_game(unsigned char* address);

void list_games();