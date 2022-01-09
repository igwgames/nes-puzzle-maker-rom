#include "source/library/bank_helpers.h"

ZEROPAGE_DEF(char, bankLevel);
ZEROPAGE_ARRAY_DEF(unsigned char, bankBuffer, MAX_BANK_DEPTH);

void banked_call(unsigned char bankId, void (*method)(void)) {

    (*method)();

}

// Switch to the given bank, and keep track of the current bank, so that we may jump back to it as needed.
void bank_push(unsigned char bankId) {
    // Well, I hope we never do this...
    /*
    if (bankLevel > MAX_BANK_DEPTH) {
        crash_error_use_banked_details(ERR_RECURSION_DEPTH, ERR_RECURSION_DEPTH_EXPLANATION, ERR_RECURSION_DEPTH_VAR, MAX_BANK_DEPTH);
    }*/
}

// Go back to the last bank pushed on using bank_push.
void bank_pop() {
}
