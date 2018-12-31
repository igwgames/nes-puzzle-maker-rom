#include "source/library/bank_helpers.h"
#include "source/qr/qr.h"
#include "source/globals.h"
#include "source/menus/input_helpers.h"
#include "source/menus/text_helpers.h"
#include "source/neslib_asm/neslib.h"
#include "source/qr/qrcode.h"
#include "source/game_data/game_data.h"
#include "source/menus/error.h"
#include "source/graphics/palettes.h"
#include "source/configuration/system_constants.h"

CODE_BANK(PRG_BANK_QR);
#define currentByte tempChar1

// This thing is terrifying; delegate everything to WRAM
_Pragma("bssseg (push, \"WRAM\")");
_Pragma("dataseg (push, \"WRAM\")");


const unsigned char qr_palette[] = {
    0x0f, 0x00, 0x10, 0x30,
    0x0f, 0x00, 0x10, 0x30,
    0x0f, 0x00, 0x10, 0x30,
    0x0f, 0x00, 0x10, 0x30
};


// TODO: We need 49x49 bits, not bytes!! This just makes the code a little simpler during development
WRAM_ARRAY_DEF(unsigned char, fullQrCode, 1024);

QRCode theCode;

// FIXME: Kill
unsigned int waitTime;

unsigned char qrType;


// NOTE: YES, the casts here are necessary. The compiler overflows 8bit values otherwise.
// #define LOOK_UP_PIXEL(x, y) fullQrCode[(unsigned int)x + (((unsigned int)y)*50)]
#define LOOK_UP_PIXEL(x, y) (qrcode_getModule(&theCode, x, y))
#define SET_PIXEL(x, y, val) fullQrCode[(unsigned int)x + (((unsigned int)y)*50)] = val
#define FLIP_PIXEL(x, y, val) fullQrCode[(unsigned int)x + (((unsigned int)y)*50)] ^= 0xfe;

void generate_qr(unsigned char* bytes) {
    memfill(fullQrCode, 0, 1024);
    waitTime = frameCount;
    qrcode_initBytes(&theCode, fullQrCode, 6, bytes, 128);
    waitTime = frameCount - waitTime;
    // qrcode_initText(&theCode, fullQrCode, 6, ECC_LOW, "Hello #brewery!",15);
    // qrcode_initBytes(&theCode, fullQrCode, 8, ECC_LOW, &(currentGameData[0]), 192);
}

void draw_last_qr() {
    set_vram_update(NULL);
    ppu_off();

    clear_screen();
    set_chr_bank_0(0x1f);

    pal_bg(qr_palette);

    for (i = 0; i != 25; ++i) { // Y
        vram_adr(NAMETABLE_A + ((i+2/* padding */)<<5) + 4/* padding */);
        for (j = 0; j != 25; ++j) {  // X
            // NOTE: Should add an offset in here
            currentByte = (LOOK_UP_PIXEL((j<<1),(i<<1))/* << 0 */) | (LOOK_UP_PIXEL((j<<1)+1,(i<<1))<<1) | (LOOK_UP_PIXEL((j<<1),(i<<1)+1)<<2) | (LOOK_UP_PIXEL((j<<1)+1,(i<<1)+1)<<3);
            vram_put(currentByte);
        }

    }

    if (qrType == QR_TYPE_PAGE_1) {
        put_str(NTADR_A(1, 26), "Code 1/2.");
    } else {
        put_str(NTADR_A(1, 26), "Code 2/2.");
    }
    put_str(NTADR_A(1, 27), "Take a photo,then press start.");


    // Don't show player sprite
    ppu_on_bg();

    // crash_error("Finished.. time check", "blah", "blaaah", (waitTime >> 6));

}

void draw_loading_qr() {
    set_vram_update(NULL);
    ppu_off();

    clear_screen_with_border();

    set_chr_bank_0(CHR_BANK_ARCADE);
    pal_bg(mainBgPalette);


    put_str(NTADR_A(3, 2), " Loading... ");
    //                 "                             "
    put_str(NTADR_A(3, 6), "This screen will soon show");
    if (qrType == QR_TYPE_PAGE_1) {
        put_str(NTADR_A(3, 7), "two QR codes.");
    } else {
        put_str(NTADR_A(3, 7), "another QR code.");
    }
    put_str(NTADR_A(3, 10), "To share, open a browser"); 
    put_str(NTADR_A(3, 11), "to puzzle.nes.science");

    put_str(NTADR_A(3, 14), "Click \"Share from ");
    put_str(NTADR_A(3, 15), "console\" and upload photos");
    put_str(NTADR_A(3, 16), "of the two QR codes.");

    put_str(NTADR_A(3, 19), "QR generation is very slow.");
    put_str(NTADR_A(3, 20), "Please be patient...");

    ppu_on_bg();

}

_Pragma("bssseg (pop)");
_Pragma("dataseg (pop)");
