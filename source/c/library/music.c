#include "source/c/library/music.h";
#include "source/c/globals.h"
#include "source/c/library/user_data.h"
#include "source/c/neslib.h"

void playSongForEvent(unsigned char event) {
    for (i = 0; i < 16; ++i) {
        if (customSongShowTimes[i] == event) {
            music_play(i);
        }
    }
}