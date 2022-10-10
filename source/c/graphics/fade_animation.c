#include "source/c/graphics/fade_animation.h"
#include "source/c/globals.h"
#include "source/c/neslib.h"

// Use this to fade the screen in/out with various timeouts
// Tracking the current state since the optional screens put us into an unknown state
// with fades. Prevents double-fade animations.
unsigned char currentBright;

// Internal functions that the other ones call with a set number of frames.
void _fade_out(unsigned char speed) {
    if (currentBright == 0) { return; }
    for (i = 4; i != 255; --i) {
        delay(speed);
        pal_bright(i);
    }
    currentBright = 0;

}

void _fade_in(unsigned char speed) {
    if (currentBright == 5) { return; }
    for (i = 0; i != 5; ++i) {
        delay(speed);
        pal_bright(i);
    }
    currentBright = 5;

}

void fade_in() {
    _fade_in(2);
}

void fade_out() {
    _fade_out(2);
}

void fade_out_instant() {
    pal_bright(0);
}

void fade_in_fast() {
    _fade_in(1);
}