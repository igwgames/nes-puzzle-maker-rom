#include "source/c/graphics/fade_animation.h"
#include "source/c/globals.h"
#include "source/c/neslib.h"

// Use these methods to fade the screen in/out with various timeouts

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

// Fade the screen from black to normal colors at a normal pace.
void fade_in() {
    _fade_in(2);
}

// Fade the screen from normal colors down to black at a normal pace.
void fade_out() {
    _fade_out(2);
}

// Instantly "fade" the screen to black. 
void fade_out_instant() {
    pal_bright(0);
}

// Fade the screen in from black to normal colors faster.
void fade_in_fast() {
    _fade_in(1);
}