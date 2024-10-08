// Runs a simple fade animation; can be used for switching maps, getting in and out of menus, etc.
// Note that control is completely ceded during this step - your logic will not continue until the fade is complete.

// Does a fade animation with a somewhat normal timing.
void fade_in();
void fade_out();

// Does the fastest fade animation possible, with one frame per shade.
void fade_in_fast();

// This can be used to instantly turn the screen black - such as when you're starting the game.
// Not really a true "fade", but a nice shortcut method.
void fade_out_instant();