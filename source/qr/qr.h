#include "source/library/bank_helpers.h"
#define PRG_BANK_QR 5

#define QR_TYPE_PAGE_1 1
#define QR_TYPE_PAGE_2 2
extern unsigned char qrType;

void generate_qr(unsigned char* bytes);

void draw_last_qr();

void draw_loading_qr();