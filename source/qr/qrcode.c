/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Richard Moore
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 *  Special thanks to Nayuki (https://www.nayuki.io/) from which this library was
 *  heavily inspired and compared against.
 *
 *  See: https://github.com/nayuki/QR-Code-generator/tree/master/cpp
 */

#include "source/qr/qrcode.h"
#include "source/library/bank_helpers.h"
#include "source/qr/qr.h"
#include "source/globals.h"
#include "source/neslib_asm/neslib.h"
#include "source/menus/error.h"

CODE_BANK(PRG_BANK_QR);

// #include <stdlib.h>
// #include <string.h>

// // #pragma mark - Error Correction Lookup tables

// This thing is terrifying; delegate everything to WRAM
_Pragma("bssseg (push, \"WRAM\")");
_Pragma("dataseg (push, \"WRAM\")");


WRAM_DEF(int8_t,signed_i);
WRAM_DEF(int8_t, signed_j);
WRAM_DEF(uint32_t, offset);
WRAM_DEF(uint8_t, mask);
WRAM_DEF(uint8_t, qrCode_mask);
WRAM_DEF(uint8_t, size);
WRAM_DEF(uint8_t, x);
WRAM_DEF(uint8_t, y);
ZEROPAGE_DEF(uint8_t, z);
WRAM_DEF(uint16_t, ii);
WRAM_DEF(bool, on);

WRAM_DEF(uint32_t, data);
WRAM_DEF(uint32_t, rem);

WRAM_DEF(uint8_t, invert);

WRAM_DEF(char, c_result);

#if LOCK_VERSION == 0


static const uint16_t NUM_ERROR_CORRECTION_CODEWORDS[4][40] = {
    // 1,  2,  3,  4,  5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,   25,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40    Error correction level
    { 10, 16, 26, 36, 48,  64,  72,  88, 110, 130, 150, 176, 198, 216, 240, 280, 308, 338, 364, 416, 442, 476, 504, 560,  588,  644,  700,  728,  784,  812,  868,  924,  980, 1036, 1064, 1120, 1204, 1260, 1316, 1372},  // Medium
    {  7, 10, 15, 20, 26,  36,  40,  48,  60,  72,  80,  96, 104, 120, 132, 144, 168, 180, 196, 224, 224, 252, 270, 300,  312,  336,  360,  390,  420,  450,  480,  510,  540,  570,  570,  600,  630,  660,  720,  750},  // Low
    { 17, 28, 44, 64, 88, 112, 130, 156, 192, 224, 264, 308, 352, 384, 432, 480, 532, 588, 650, 700, 750, 816, 900, 960, 1050, 1110, 1200, 1260, 1350, 1440, 1530, 1620, 1710, 1800, 1890, 1980, 2100, 2220, 2310, 2430},  // High
    { 13, 22, 36, 52, 72,  96, 108, 132, 160, 192, 224, 260, 288, 320, 360, 408, 448, 504, 546, 600, 644, 690, 750, 810,  870,  952, 1020, 1050, 1140, 1200, 1290, 1350, 1440, 1530, 1590, 1680, 1770, 1860, 1950, 2040},  // Quartile
};

static const uint8_t NUM_ERROR_CORRECTION_BLOCKS[4][40] = {
    // Version: (note that index 0 is for padding, and is set to an illegal value)
    // 1, 2, 3, 4, 5, 6, 7, 8, 9,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40    Error correction level
    {  1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},  // Medium
    {  1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},  // Low
    {  1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
    {  1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},  // Quartile
};

static const uint16_t NUM_RAW_DATA_MODULES[40] = {
    //  1,   2,   3,   4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
      208, 359, 567, 807, 1079, 1383, 1568, 1936, 2336, 2768, 3232, 3728, 4256, 4651, 5243, 5867, 6523,
    //   18,   19,   20,   21,    22,    23,    24,    25,   26,    27,     28,    29,    30,    31,
       7211, 7931, 8683, 9252, 10068, 10916, 11796, 12708, 13652, 14628, 15371, 16411, 17483, 18587,
    //    32,    33,    34,    35,    36,    37,    38,    39,    40
       19723, 20891, 22091, 23008, 24272, 25568, 26896, 28256, 29648
};

// @TODO: Put other LOCK_VERSIONS here
#elif LOCK_VERSION == 3

static const int16_t NUM_ERROR_CORRECTION_CODEWORDS[4] = {
    26, 15, 44, 36
};

static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4] = {
    1, 1, 2, 2
};

static const uint16_t NUM_RAW_DATA_MODULES = 567;

#elif LOCK_VERSION == 6

static const int16_t NUM_ERROR_CORRECTION_CODEWORDS[4] = {
    64, 36, 112, 96
};

static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4] = {
    4, 2, 4, 4
};

static const uint16_t NUM_RAW_DATA_MODULES = 1383;

#define BUFFER_SIZE_BYTES ((1383 + 7) / 8)


#elif LOCK_VERSION == 7

static const int16_t NUM_ERROR_CORRECTION_CODEWORDS[4] = {
    72, 40, 130, 108
};

static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4] = {
    4, 2, 5, 6
};

static const uint16_t NUM_RAW_DATA_MODULES = 1568;


#elif LOCK_VERSION == 8

static const int16_t NUM_ERROR_CORRECTION_CODEWORDS[4] = {
    88, 48, 156, 132
};

static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4] = {
    4, 2, 6, 6
};

static const uint16_t NUM_RAW_DATA_MODULES = 1936;


#else

#error Unsupported LOCK_VERSION (add it...)

#endif

WRAM_ARRAY_DEF(uint8_t,result, 512);

    // NOTE: I picked this number completely randomly.
//    uint8_t codewordBytes[128];
WRAM_ARRAY_DEF(unsigned char, codewordBytes, BUFFER_SIZE_BYTES);

    // Ditto
WRAM_ARRAY_DEF(unsigned char, isFunctionGridBytes, 64);


static int max(int a, int b) {
    if (a > b) { return a; }
    return b;
}

/*
static int ABS(int value) {
    if (value < 0) { return -value; }
    return value;
}
*/

#define ECC_LEVEL ECC_LOW

// We store the Format bits tightly packed into a single byte (each of the 4 modes is 2 bits)
// The format bits can be determined by ECC_FORMAT_BITS >> (2 * ecc)
static const uint8_t ECC_FORMAT_BITS = (0x02 << 6) | (0x03 << 4) | (0x00 << 2) | (0x01 << 0);


#define ECC_FORMAT_BITS_FINAL ((ECC_FORMAT_BITS >> (ECC_LEVEL<<1)) & 0x03)


// // #pragma mark - Mode testing and conversion

static int8_t getAlphanumeric(char c) {
    
    if (c >= '0' && c <= '9') { return (c - '0'); }
    if (c >= 'A' && c <= 'Z') { return (c - 'A' + 10); }
    
    switch (c) {
        case ' ': return 36;
        case '$': return 37;
        case '%': return 38;
        case '*': return 39;
        case '+': return 40;
        case '-': return 41;
        case '.': return 42;
        case '/': return 43;
        case ':': return 44;
    }
    
    return -1;
}

static bool isAlphanumeric(const char *text, uint16_t length) {
    while (length != 0) {
        if (getAlphanumeric(text[--length]) == -1) { return false; }
    }
    return true;
}


static bool isNumeric(const char *text, uint16_t length) {
    while (length != 0) {
        char c = text[--length];
        if (c < '0' || c > '9') { return false; }
    }
    return true;
}


// #pragma mark - Counting

// We store the following tightly packed (less 8) in modeInfo
//               <=9  <=26  <= 40
// NUMERIC      ( 10,   12,    14);
// ALPHANUMERIC (  9,   11,    13);
// BYTE         (  8,   16,    16);
const unsigned int modeInfo = 0x7bbb80a;

static char getModeBits(uint8_t version, uint8_t mode) {
    // Note: We use 15 instead of 16; since 15 doesn't exist and we cannot store 16 (8 + 8) in 3 bits
    // hex(int("".join(reversed([('00' + bin(x - 8)[2:])[-3:] for x in [10, 9, 8, 12, 11, 15, 14, 13, 15]])), 2))
    
#if LOCK_VERSION == 0 || LOCK_VERSION > 9
    if (version > 9) { modeInfo >>= 9; }
#endif
    
#if LOCK_VERSION == 0 || LOCK_VERSION > 26
    if (version > 26) { modeInfo >>= 9; }
#endif
    
    c_result = 8 + ((modeInfo >> (3 * mode)) & 0x07);
    if (c_result == 15) { c_result = 16; }
    
    return c_result;
}


// #pragma mark - BitBucket

typedef struct BitBucket {
    uint32_t bitOffsetOrWidth;
    uint16_t capacityBytes;
    uint8_t *data;
} BitBucket;

/*
void bb_dump(BitBucket *bitBuffer) {
    printf("Buffer: ");
    for (uint32_t i = 0; i < bitBuffer->capacityBytes; i++) {
        printf("%02x", bitBuffer->data[i]);
        if ((i % 4) == 3) { printf(" "); }
    }
    printf("\n");
}
*/

static uint16_t bb_getGridSizeBytes(uint8_t size) {
    return (((size * size) + 7) / 8);
}

/*
static uint16_t bb_getBufferSizeBytes(uint32_t bits) {
    return ((bits + 7) / 8);
}
*/

static void bb_initBuffer(BitBucket *bitBuffer, uint8_t *data, int32_t capacityBytes) {
    bitBuffer->bitOffsetOrWidth = 0;
    bitBuffer->capacityBytes = capacityBytes;
    bitBuffer->data = data;
    
    memfill(data, 0, bitBuffer->capacityBytes);
}

static void bb_initGrid(BitBucket *bitGrid, uint8_t *data, uint8_t size) {
    bitGrid->bitOffsetOrWidth = size;
    bitGrid->capacityBytes = bb_getGridSizeBytes(size);
    bitGrid->data = data;

    memfill(data, 0, bitGrid->capacityBytes);
}

static void bb_appendBits(BitBucket *bitBuffer, uint32_t val, uint8_t length) {
    offset = bitBuffer->bitOffsetOrWidth;

    
    for (signed_i = length - 1; signed_i >= 0; --signed_i, offset++) {
        bitBuffer->data[offset >> 3] |= ((val >> signed_i) & 1) << (7 - (offset & 7));            
    }
    bitBuffer->bitOffsetOrWidth = offset;
}
/*
void bb_setBits(BitBucket *bitBuffer, uint32_t val, int offset, uint8_t length) {
    for (int8_t i = length - 1; i >= 0; i--, offset++) {
        bitBuffer->data[offset >> 3] |= ((val >> i) & 1) << (7 - (offset & 7));
    }
}
*/
/*
static void bb_setBit(BitBucket *bitGrid, uint8_t x, uint8_t y, bool on) {
    offset = y * bitGrid->bitOffsetOrWidth + x;
    mask = 1 << (7 - (offset & 0x07));
    if (on) {
        bitGrid->data[offset >> 3] |= mask;
    } else {
        bitGrid->data[offset >> 3] &= ~mask;
    }
}
*/
#define bb_setBit(bitGrid, x, y, on) \
    offset = y * bitGrid->bitOffsetOrWidth + x; \
    mask = 1 << (7 - (offset & 0x07)); \
    if (on) { \
        bitGrid->data[offset >> 3] |= mask; \
    } else { \
        bitGrid->data[offset >> 3] &= ~mask; \
    }

#define bb_setBit_defaultOffset(bitGrid, x, y, on) \
    mask = 1 << (7 - (offset & 0x07)); \
    if (on) { \
        bitGrid->data[offset >> 3] |= mask; \
    } else { \
        bitGrid->data[offset >> 3] &= ~mask; \
    }


static void bb_invertBit(BitBucket *bitGrid, uint8_t x, uint8_t y) {
    // HACK: offset is already set correctly by bb_getBit
    //offset = y * bitGrid->bitOffsetOrWidth + x;
    mask = 1 << (7 - (offset & 0x07));
    on = ((bitGrid->data[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0);
    if (!on) {
        bitGrid->data[offset >> 3] |= mask;
    } else {
        bitGrid->data[offset >> 3] &= ~mask;
    }
}

static bool bb_getBit(BitBucket *bitGrid, uint8_t x, uint8_t y) {
    offset = y * bitGrid->bitOffsetOrWidth + x;
    return (bitGrid->data[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0;
}


// #pragma mark - Drawing Patterns

// XORs the data modules in this QR Code with the given mask pattern. Due to XOR's mathematical
// properties, calling applyMask(m) twice with the same value is equivalent to no change at all.
// This means it is possible to apply a mask, undo it, and try another mask. Note that a final
// well-formed QR Code symbol needs exactly one mask applied (not zero, not two, etc.).
static void applyMask(BitBucket *modules, BitBucket *isFunction/*, uint8_t mask*/) {
    size = modules->bitOffsetOrWidth;
    
    for (y = 0; y != size; ++y) {
        for (x = 0; x != size; ++x) { // 4
            if (bb_getBit(isFunction, x, y)) { continue; }
            
            // invert = 0;
            /*switch (mask) {
                case 0:  invert = (x + y) % 2 == 0;                    break;
                case 1:  */invert = (y & 0x01) == 0;/*                     break;
                case 2:  invert = x % 3 == 0;                          break;
                case 3:  invert = (x + y) % 3 == 0;                    break;
                case 4:  invert = (x / 3 + y / 2) % 2 == 0;            break;
                case 5:  invert = x * y % 2 + x * y % 3 == 0;          break;
                case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;    break;
                case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0;  break;
            }*/
            if (invert) {
                bb_invertBit(modules, x, y); // 2
            }
        }
    }
}

static void setFunctionModule(BitBucket *modules, BitBucket *isFunction, uint8_t x, uint8_t y, bool on) {
    bb_setBit(modules, x, y, on);
    bb_setBit(isFunction, x, y, true);
}

// Draws a 9*9 finder pattern including the border separator, with the center module at (x, y).
static void drawFinderPattern(BitBucket *modules, BitBucket *isFunction, uint8_t x, uint8_t y) {
    uint8_t dist;
    int16_t xx, yy;
    size = modules->bitOffsetOrWidth;

    for (signed_i = -4; signed_i <= 4; ++signed_i) {
        for (signed_j = -4; signed_j <= 4; ++signed_j) {
            dist = max(ABS(signed_i), ABS(signed_j));  // Chebyshev/infinity norm
            xx = x + signed_j;
            yy = y + signed_i;
            if (0 <= xx && xx < size && 0 <= yy && yy < size) {
                setFunctionModule(modules, isFunction, xx, yy, dist != 2 && dist != 4);
            }
        }
    }
}

// Draws a 5*5 alignment pattern, with the center module at (x, y).
static void drawAlignmentPattern(BitBucket *modules, BitBucket *isFunction, uint8_t x, uint8_t y) {
    for (signed_i = -2; signed_i <= 2; ++signed_i) {
        for (signed_j = -2; signed_j <= 2; ++signed_j) {
            setFunctionModule(modules, isFunction, x + signed_j, y + signed_i, max(ABS(signed_i), ABS(signed_j)) != 1);
        }
    }
}

// Draws two copies of the format bits (with its own error correction code)
// based on the given mask and this object's error correction level field.
static void drawFormatBits(BitBucket *modules, BitBucket *isFunction, uint8_t mask) {
    
    // Calculate error correction code and pack bits
    data = ECC_FORMAT_BITS_FINAL << 3 | mask;  // errCorrLvl is uint2, mask is uint3
    rem = data;

    size = modules->bitOffsetOrWidth;
    for (i = 0; i != 10; ++i) {
        rem = (rem << 1) ^ ((rem >> 9) * 0x537);
    }
    
    data = data << 10 | rem;
    data ^= 0x5412;  // uint15
    
    // Draw first copy
    for (i = 0; i != 6; ++i) {
        setFunctionModule(modules, isFunction, 8, i, ((data >> i) & 1) != 0);
    }
    
    setFunctionModule(modules, isFunction, 8, 7, ((data >> 6) & 1) != 0);
    setFunctionModule(modules, isFunction, 8, 8, ((data >> 7) & 1) != 0);
    setFunctionModule(modules, isFunction, 7, 8, ((data >> 8) & 1) != 0);
    
    for (signed_i = 9; signed_i < 15; ++signed_i) {
        setFunctionModule(modules, isFunction, 14 - signed_i, 8, ((data >> signed_i) & 1) != 0);
    }
    
    // Draw second copy
    for (signed_i = 0; signed_i <= 7; ++signed_i) {
        setFunctionModule(modules, isFunction, size - 1 - signed_i, 8, ((data >> signed_i) & 1) != 0);
    }
    
    for (signed_i = 8; signed_i < 15; ++signed_i) {
        setFunctionModule(modules, isFunction, 8, size - 15 + signed_i, ((data >> signed_i) & 1) != 0);
    }
    
    setFunctionModule(modules, isFunction, 8, size - 8, true);
}


// Draws two copies of the version bits (with its own error correction code),
// based on this object's version field (which only has an effect for 7 <= version <= 40).
static void drawVersion(BitBucket *modules, BitBucket *isFunction, uint8_t version) {
    // NOTE: This is signed; size is unsigned
    
    // NOTE: This will break on v7 but we really don't need to be doing this...
    /*
    int8_t size = modules->bitOffsetOrWidth;
    // uint32_t rem, data;
    uint8_t bit, a, b;
    */

#if LOCK_VERSION != 0 && LOCK_VERSION < 7
    return;
    
#else
    if (version < 7) { return; }
    
    // Calculate error correction code and pack bits
    rem = version;  // version is uint6, in the range [7, 40]
    for (i = 0; i < 12; i++) {
        rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
    }
    
    data = version << 12 | rem;  // uint18
    
    // Draw two copies
    for (i = 0; i < 18; i++) {
        bit = ((data >> i) & 1) != 0;
        a = size - 11 + i % 3;
        b = i / 3;
        setFunctionModule(modules, isFunction, a, b, bit);
        setFunctionModule(modules, isFunction, b, a, bit);
    }
    
#endif
}

#define ALIGN_COUNT (LOCK_VERSION / 7 + 2)

static void drawFunctionPatterns(BitBucket *modules, BitBucket *isFunction, uint8_t version) {
    
    uint8_t alignPositionIndex;
    uint8_t alignPosition[ALIGN_COUNT];
    uint8_t step;
    uint8_t pos;
    size = modules->bitOffsetOrWidth;


    // Draw the horizontal and vertical timing patterns
    for (i = 0; i < size; ++i) {
        setFunctionModule(modules, isFunction, 6, i, (i & 0x01) == 0);
        setFunctionModule(modules, isFunction, i, 6, (i & 0x01) == 0);
    }
    
    // Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
    drawFinderPattern(modules, isFunction, 3, 3);
    drawFinderPattern(modules, isFunction, size - 4, 3);
    drawFinderPattern(modules, isFunction, 3, size - 4);
    
#if LOCK_VERSION == 0 || LOCK_VERSION > 1

    if (version > 1) {

        // Draw the numerous alignment patterns
        
        //if (version != 32) {
            step = ((version<<2) + (ALIGN_COUNT<<1) + 1) / ((2 * ALIGN_COUNT) - 2) << 1;  // ceil((size - 13) / (2*numAlign - 2)) * 2
        //} else { // C-C-C-Combo breaker!
        //    step = 26;
        //}
        
        alignPositionIndex = ALIGN_COUNT - 1;
        
        alignPosition[0] = 6;
        
        size = version * 4 + 17;
        for (i = 0, pos = size - 7; i < ALIGN_COUNT - 1; ++i, pos -= step) {
            alignPosition[alignPositionIndex--] = pos;
        }
        
        for (i = 0; i < ALIGN_COUNT; ++i) {
            for (j = 0; j < ALIGN_COUNT; ++j) {
                if ((i == 0 && j == 0) || (i == 0 && j == ALIGN_COUNT - 1) || (i == ALIGN_COUNT - 1 && j == 0)) {
                    continue;  // Skip the three finder corners
                } else {
                    drawAlignmentPattern(modules, isFunction, alignPosition[i], alignPosition[j]);
                }
            }
        }
    }
    
#endif
    
    // Draw configuration data
    drawFormatBits(modules, isFunction, 0);  // Dummy mask value; overwritten later in the constructor
    drawVersion(modules, isFunction, version);
}



// Draws the given sequence of 8-bit codewords (data and error correction) onto the entire
// data area of this QR Code symbol. Function modules need to be marked off before this is called.
static void drawCodewords(BitBucket *modules, BitBucket *isFunction, BitBucket *codewords) {
    
    uint16_t bitLength = codewords->bitOffsetOrWidth;
    uint8_t *data = codewords->data;
    
    int16_t right;

    uint8_t vert;
    uint8_t upwards;

    ii = 0;

    size = modules->bitOffsetOrWidth;
    
    // Do the funny zigzag scan
    for (right = size - 1; right >= 1; right -= 2) {  // Index of right column in each column pair // 6
        if (right == 6) { right = 5; }
        
        for (vert = 0; vert != size; ++vert) {  // Vertical counter
            for (j = 0; j != 2; ++j) {
                x = right - j;  // Actual x coordinate
                upwards = ((right & 2) == 0) ^ (x < 6);
                y = upwards ? size - 1 - vert : vert;  // Actual y coordinate
                offset = y * size + x; // SLOW
                if (ii < bitLength && (isFunction->data[offset >> 3] & (1 << (7 - (offset & 0x07)))) == 0) { // 3
                    bb_setBit_defaultOffset(modules, x, y, ((data[ii >> 3] >> (7 - (ii & 7))) & 1) != 0);
                    ++ii;
                }
                // If there are any remainder bits (0 to 7), they are already
                // set to 0/false/white when the grid of modules was initialized
            }
        }
    }
}



// #pragma mark - Penalty Calculation

#define PENALTY_N1      3
#define PENALTY_N2      3
#define PENALTY_N3     40
#define PENALTY_N4     10

// Calculates and returns the penalty score based on state of this QR Code's current modules.
// This is used by the automatic mask choice algorithm to find the mask pattern that yields the lowest score.
// @TODO: This can be optimized by working with the bytes instead of bits.
static uint32_t getPenaltyScore(BitBucket *modules) {
    uint32_t result = 0;
    
    
    uint8_t colorX, cx, cy, colorY, color, runX, runY;
    uint16_t black = 0;
    uint8_t bitsRow;
    uint8_t bitsCol;

    uint16_t total, k;

    bool colorUL, colorUR, colorL;
    size = modules->bitOffsetOrWidth;


    
    // Adjacent modules in row having same color
    for (y = 0; y < size; y++) {
        
        colorX = bb_getBit(modules, 0, y);
        for (x = 1, runX = 1; x < size; x++) {
            cx = bb_getBit(modules, x, y);
            if (cx != colorX) {
                colorX = cx;
                runX = 1;
                
            } else {
                runX++;
                if (runX == 5) {
                    result += PENALTY_N1;
                } else if (runX > 5) {
                    result++;
                }
            }
        }
    }
    
    // Adjacent modules in column having same color
    for (x = 0; x < size; x++) {
        colorY = bb_getBit(modules, x, 0);
        for (y = 1, runY = 1; y < size; y++) {
            cy = bb_getBit(modules, x, y);
            if (cy != colorY) {
                colorY = cy;
                runY = 1;
            } else {
                runY++;
                if (runY == 5) {
                    result += PENALTY_N1;
                } else if (runY > 5) {
                    result++;
                }
            }
        }
    }
    
    for (y = 0; y < size; y++) {
        bitsRow = 0;
        bitsCol = 0;
        for (x = 0; x < size; x++) {
            color = bb_getBit(modules, x, y);

            // 2*2 blocks of modules having same color
            if (x > 0 && y > 0) {
                colorUL = bb_getBit(modules, x - 1, y - 1);
                colorUR = bb_getBit(modules, x, y - 1);
                colorL = bb_getBit(modules, x - 1, y);
                if (color == colorUL && color == colorUR && color == colorL) {
                    result += PENALTY_N2;
                }
            }

            // Finder-like pattern in rows and columns
            bitsRow = ((bitsRow << 1) & 0x7FF) | color;
            bitsCol = ((bitsCol << 1) & 0x7FF) | bb_getBit(modules, y, x);

            // Needs 11 bits accumulated
            if (x >= 10) {
                if (bitsRow == 0x05D || bitsRow == 0x5D0) {
                    result += PENALTY_N3;
                }
                if (bitsCol == 0x05D || bitsCol == 0x5D0) {
                    result += PENALTY_N3;
                }
            }

            // Balance of black and white modules
            if (color) { black++; }
        }
    }

    // Find smallest k such that (45-5k)% <= dark/total <= (55+5k)%
    total = size * size;
    for (k = 0; black * 20 < (9 - k) * total || black * 20 > (11 + k) * total; k++) {
        result += PENALTY_N4;
    }
    
    return result;
}


// #pragma mark - Reed-Solomon Generator
ZEROPAGE_DEF(unsigned int, tempRsMult);

static uint8_t rs_multiply(uint8_t x, uint8_t y) {
    // Russian peasant multiplication
    // See: https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication
    z = 0;
    for (signed_i = 7; signed_i != -1; --signed_i) {
        z = (z << 1) ^ ((z >> 7) ? 0x11D : 0);
        tempRsMult = ((y >> signed_i) & 1);
        if (tempRsMult) {
            z ^= x;
        }
    }
    return z;
}

static void rs_init(uint8_t degree, uint8_t *coeff) {
    uint16_t root = 1;

    memfill(coeff, 0, degree);
    coeff[degree - 1] = 1;
    
    // Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
    // drop the highest term, and store the rest of the coefficients in order of descending powers.
    // Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
    for (i = 0; i < degree; ++i) {
        // Multiply the current product by (x - r^i)
        for (j = 0; j < degree; ++j) {
            coeff[j] = rs_multiply(coeff[j], root);
            if (j + 1 < degree) {
                coeff[j] ^= coeff[j + 1];
            }
        }
        root = (root << 1) ^ ((root >> 7) * 0x11D);  // Multiply by 0x02 mod GF(2^8/0x11D)
    }
}

ZEROPAGE_DEF(uint8_t, factor);
ZEROPAGE_DEF(uint8_t, rs_remainder_stride);
/*
static void rs_getRemainder(uint8_t degree, uint8_t *coeff, uint8_t *data, uint8_t length, uint8_t *result, uint8_t stride) {
    // Compute the remainder by performing polynomial division
    //uint8_t factor;
    //for (uint8_t i = 0; i < degree; i++) { result[] = 0; }
    //memfill(result, 0, degree);
    
    for (i = 0; i != length; ++i) {
        factor = data[i] ^ result[0];
        for (j = 1; j != degree; ++j) {
            result[(j - 1) * stride] = result[j * stride];
        }
        result[(degree - 1) * stride] = 0;
        
        for (j = 0; j < degree; j++) {
            result[j * stride] ^= rs_multiply(coeff[j], factor);
        }
    }
}
*/

#define rs_getRemainder(degree, coeff, data, length, result, stride) \
    for (i = 0; i != length; ++i) { \
        rs_remainder_stride = stride; \
        factor = data[i] ^ result[0]; \
        for (j = 1; j != degree; ++j) { \
            result[rs_remainder_stride-stride] = result[rs_remainder_stride]; \
            rs_remainder_stride += stride; \
        } \
        result[(degree - 1) * stride] = 0; \
        \
        rs_remainder_stride = 0; \
        for (j = 0; j != degree; ++j) { \
            result[rs_remainder_stride] ^= rs_multiply(coeff[j], factor); \
            rs_remainder_stride += stride; \
        } \
    }
// NOTE: rs_multiply: 5



// #pragma mark - QrCode

static int8_t encodeDataCodewords(BitBucket *dataCodewords, const uint8_t *text, uint16_t length, uint8_t version) {
    int8_t mode = MODE_BYTE;
    uint16_t accumData = 0;
    uint8_t accumCount = 0;

    bb_appendBits(dataCodewords, 1 << MODE_BYTE, 4);
    bb_appendBits(dataCodewords, length, getModeBits(version, MODE_BYTE));
    for (ii = 0; ii != length; ++ii) {
        bb_appendBits(dataCodewords, (char)(text[ii]), 8);
    }
    
    //bb_setBits(dataCodewords, length, 4, getModeBits(version, mode));
    
    return mode;
}

// FIXME: hardcode
#define ECC_NUM_BLOCKS 2
#define ECC_TOTAL_ECC 36
#define ECC_MODULE_COUNT 1383U
#define ECC_BLOCK_ECC_LEN (ECC_TOTAL_ECC / ECC_NUM_BLOCKS)
#define ECC_NUM_SHORT_BLOCKS (ECC_NUM_BLOCKS - ECC_MODULE_COUNT / 8 % ECC_NUM_BLOCKS)
#define ECC_SHORT_BLOCK_LEN (ECC_MODULE_COUNT / 8 / ECC_NUM_BLOCKS)
#define ECC_SHORT_DATA_BLOCK_LEN (ECC_SHORT_BLOCK_LEN - ECC_BLOCK_ECC_LEN)

static void performErrorCorrection(uint8_t version, BitBucket *data) {
    
    // See: http://www.thonky.com/qr-code-tutorial/structure-final-message
   /* 
#if LOCK_VERSION == 0
    // NOTE: I hard-coded 10 here as the version, again due to c89 features
    uint8_t numBlocks = NUM_ERROR_CORRECTION_BLOCKS[ecc][10 - 1];
    uint16_t totalEcc = NUM_ERROR_CORRECTION_CODEWORDS[ecc][10 - 1];
    uint16_t moduleCount = NUM_RAW_DATA_MODULES[10 - 1];
#else
    uint8_t numBlocks = NUM_ERROR_CORRECTION_BLOCKS[ECC_FORMAT_BITS_FINAL];
    uint16_t totalEcc = NUM_ERROR_CORRECTION_CODEWORDS[ECC_FORMAT_BITS_FINAL];
    uint16_t moduleCount = NUM_RAW_DATA_MODULES;
#endif
    
    // uint8_t blockEccLen = totalEcc / numBlocks;
    uint8_t numShortBlocks = numBlocks - moduleCount / 8 % numBlocks;
    uint8_t shortBlockLen = moduleCount / 8 / numBlocks;
    
    uint8_t shortDataBlockLen = shortBlockLen - blockEccLen;*/

    uint16_t offset = 0;
    uint8_t *dataBytes = data->data;

    // uint8_t coeff[blockEccLen];
    uint8_t coeff[22];// This one was actually calculated correctly for our case.

    uint16_t index;
    uint8_t stride;
    uint8_t blockNum;
    uint8_t blockSize;


    
    // uint8_t result[data->capacityBytes]; // NOTE: Due to C89 featureset, had to hardcode this, AND make it global. too
    memfill(result, 0, 512);
    
    rs_init(ECC_BLOCK_ECC_LEN, coeff);
    
    
    
    // Interleave all short blocks
    // 0
    for (i = 0; i != ECC_SHORT_DATA_BLOCK_LEN; ++i) {
        index = i;
        stride = ECC_SHORT_DATA_BLOCK_LEN;
        for (blockNum = 0; blockNum != ECC_NUM_BLOCKS; ++blockNum) {
            result[offset++] = dataBytes[index];
            
#if LOCK_VERSION == 0 || LOCK_VERSION >= 5
            if (blockNum == ECC_NUM_SHORT_BLOCKS) { stride++; }
#endif
            index += stride;
        }
    }
    
    // Version less than 5 only have short blocks
#if LOCK_VERSION == 0 || LOCK_VERSION >= 5 // 0
    {
        // Interleave long blocks
        index = (ECC_SHORT_DATA_BLOCK_LEN * (ECC_NUM_SHORT_BLOCKS + 1));
        stride = ECC_SHORT_DATA_BLOCK_LEN;
        for (blockNum = 0; blockNum != (ECC_NUM_BLOCKS - ECC_NUM_SHORT_BLOCKS); ++blockNum) {
            result[offset++] = dataBytes[index];
            
            if (blockNum == 0) { stride++; }
            index += stride;
        }
    }
#endif
    
    // Add all ecc blocks, interleaved 
    // 7
    blockSize = ECC_SHORT_DATA_BLOCK_LEN;
    for (blockNum = 0; blockNum != ECC_NUM_BLOCKS; ++blockNum) {
        
#if LOCK_VERSION == 0 || LOCK_VERSION >= 5
        if (blockNum == ECC_NUM_SHORT_BLOCKS) { blockSize++; }
#endif
        // TIMING NOTE: I optimized this pretty heavily but it's STILL pretty slow. Investigate why. (~30/90)
        rs_getRemainder(ECC_BLOCK_ECC_LEN, coeff, dataBytes, blockSize, (&(result[offset + blockNum])), ECC_NUM_BLOCKS); // 7
        dataBytes += blockSize;
    }
    
    memcpy(data->data, result, data->capacityBytes);
    data->bitOffsetOrWidth = ECC_MODULE_COUNT;
}

// #pragma mark - Public QRCode functions

// @TODO: Return error if data is too big.
int8_t qrcode_initBytes(QRCode *qrcode, uint8_t *modules, uint8_t version, uint8_t *data, uint16_t length) {
    
    // uint8_t eccFormatBits = ((ECC_FORMAT_BITS >> (ECC_LEVEL<<1)) & 0x03);

    uint16_t moduleCount, dataCapacity;

    int8_t mode;
    uint8_t padding; // was 32
    uint8_t padByte;

    struct BitBucket codewords;
    BitBucket modulesGrid;
    BitBucket isFunctionGrid;

    size = version * 4 + 17;

    qrcode->version = version;
    qrcode->size = size;
    qrcode->modules = modules;

    
#if LOCK_VERSION == 0
    moduleCount = NUM_RAW_DATA_MODULES[version - 1];
    dataCapacity = moduleCount / 8 - NUM_ERROR_CORRECTION_CODEWORDS[ECC_FORMAT_BITS_FINAL][version - 1];
#else
    version = LOCK_VERSION;
    moduleCount = NUM_RAW_DATA_MODULES;
    dataCapacity = moduleCount / 8 - NUM_ERROR_CORRECTION_CODEWORDS[ECC_FORMAT_BITS_FINAL];
#endif
    
    // uint8_t codewordBytes[bb_getBufferSizeBytes(moduleCount)];
    bb_initBuffer(&codewords, codewordBytes, BUFFER_SIZE_BYTES);

    // Place the data code words into the buffer
    mode = encodeDataCodewords(&codewords, data, length, version);
    
    if (mode < 0) { return -1; }
    qrcode->mode = mode;
    
    // Add terminator and pad up to a byte if applicable
    padding = (dataCapacity * 8) - codewords.bitOffsetOrWidth;
    if (padding > 4) { padding = 4; }
    bb_appendBits(&codewords, 0, padding);
    bb_appendBits(&codewords, 0, (8 - codewords.bitOffsetOrWidth & 0x07) & 0x07);

    // Pad with alternate bytes until data capacity is reached
    for (padByte = 0xEC; codewords.bitOffsetOrWidth < (dataCapacity * 8); padByte ^= 0xEC ^ 0x11) {
        bb_appendBits(&codewords, padByte, 8);
    }

    bb_initGrid(&modulesGrid, modules, size);
    
    // uint8_t isFunctionGridBytes[bb_getGridSizeBytes(size)];
    bb_initGrid(&isFunctionGrid, isFunctionGridBytes, size);
    
    // Draw function patterns, draw all codewords, do masking
    drawFunctionPatterns(&modulesGrid, &isFunctionGrid, version); // Timing note: NOT a bottleneck
    performErrorCorrection(version, &codewords); // TIMING NOTE: 6
    drawCodewords(&modulesGrid, &isFunctionGrid, &codewords); // TIMING NOTE: 2
    
    // Find the best (lowest penalty) mask
    // NOTE: I hard-coded this to one that should be reasonably fast to save calculation.
    qrCode_mask = 1;
    // NOTE: Just hard-coding the first mask for speed.
    /*minPenalty = INT32_MAX;
    for (i = 0; i < 8; ++i) {
        drawFormatBits(&modulesGrid, &isFunctionGrid, eccFormatBits, i);
        applyMask(&modulesGrid, &isFunctionGrid, i);
        penalty = getPenaltyScore(&modulesGrid);
        if (penalty < minPenalty) {
            qrCode_mask = i;
            minPenalty = penalty;
        }
        applyMask(&modulesGrid, &isFunctionGrid, i);  // Undoes the mask due to XOR
    }*/
    
    // qrcode->mask = qrCode_mask;
    
    // Overwrite old format bits
    drawFormatBits(&modulesGrid, &isFunctionGrid, qrCode_mask);
    
    // Apply the final choice of mask
    applyMask(&modulesGrid, &isFunctionGrid/*, qrCode_mask*/); // TIMING NOTE: 3

    return 0;
}

int8_t qrcode_initText(QRCode *qrcode, uint8_t *modules, uint8_t version, const char *data, const unsigned char length) {
    // return qrcode_initBytes(qrcode, modules, version, ecc, (uint8_t*)data, strlen(data));
    return qrcode_initBytes(qrcode, modules, version, (uint8_t*)data, length);
}

bool qrcode_getModule(QRCode *qrcode, uint8_t x, uint8_t y) {
    if (x < 0 || x >= qrcode->size || y < 0 || y >= qrcode->size) {
        return false;
    }

    offset = y * qrcode->size + x;
    return (qrcode->modules[offset >> 3] & (1 << (7 - (offset & 0x07)))) != 0;
}

/*
uint8_t qrcode_getHexLength(QRCode *qrcode) {
    return ((qrcode->size * qrcode->size) + 7) / 4;
}

void qrcode_getHex(QRCode *qrcode, char *result) {
    
}
*/

_Pragma("bssseg (pop)");
_Pragma("dataseg (pop)");
