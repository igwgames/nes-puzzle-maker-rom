#include "source/c/graphics/fade_animation.h"
#include "source/c/globals.h"
#include "source/c/neslib.h"

// Use this to fade the screen in/out with various timeouts

// Internal functions that the other ones call with a set number of frames.
void _fade_out(unsigned char speed) {
    for (i = 4; i != 255; --i) {
        delay(speed);
        pal_bright(i);
    }

}

void _fade_in(unsigned char speed) {
    for (i = 0; i != 5; ++i) {
        delay(speed);
        pal_bright(i);
    }

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