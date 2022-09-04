// Contains functions to help with working with multle PRG/CHR banks

// A few defines to make common tasks simpler.

// The zero page is 255 bytes of space at the beginning of RAM that is faster to access. The space is shared with rest of 
// the game engine. Convert your most heavily used variables to zeropage using this method. This can also save rom space.
// (Usage: ZEROPAGE_DF(type, variableName) ; eg ZEROPAGE_DEF(int, myInt))
// NOTE: If you call this in a .c file and expose it in `globals.h`, be sure to mark it with `ZEROPAGE_EXTERN` there.
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define ZEROPAGE_DEF(defa, defb) \
    _Pragma("bss-name (push,\"ZEROPAGE\")") \
    _Pragma("data-name (push, \"ZEROPAGE\")") \
    defa defb; \
    _Pragma("bss-name (pop)") \
    _Pragma("data-name (pop)")

#define ZEROPAGE_ARRAY_DEF(defa, defb, defArr) \
    _Pragma("bss-name (push,\"ZEROPAGE\")") \
    _Pragma("data-name (push, \"ZEROPAGE\")") \
    defa defb[defArr]; \
    _Pragma("bss-name (pop)") \
    _Pragma("data-name (pop)")

#define SRAM_DEF(defa, defb) \
    _Pragma("bss-name (push,\"SRAM\")") \
    _Pragma("data-name (push, \"SRAM\")") \
    defa defb; \
    _Pragma("bss-name (pop)") \
    _Pragma("data-name (pop)")

#define SRAM_ARRAY_DEF(defa, defb, defArr) \
    _Pragma("bss-name (push,\"SRAM\")") \
    _Pragma("data-name (push, \"SRAM\")") \
    defa defb[defArr]; \
    _Pragma("bss-name (pop)") \
    _Pragma("data-name (pop)")



// Mark a variable referenced in a header file as being a zeropage symbol.
// Any time you set a variable as a ZEROPAGE_DEF, you will want to also update any header files referencing it
// with this function.
// (Usage; ZEROPAGE_EXTERN(type, variableName); eg ZEROPAGE_EXTERN(int, myInt))
#define ZEROPAGE_EXTERN(defa, defb) extern defa defb; _Pragma("zpsym (\"" STR(defb) "\")")
#define ZEROPAGE_ARRAY_EXTERN(defa, defb, defArr) extern defa defb[defArr]; _Pragma("zpsym (\"" STR(defb) "\")")

// Mark a variable referened in a header file as being SRAM. 
// Technically this just creates a regular extern, and you could avoid using this symbol. It is used only for
// consistency with ZEROPAGE variables, to make the source easier to follow.
#define SRAM_EXTERN(defa, defb) extern defa defb;
#define SRAM_ARRAY_EXTERN(defa, defb, defArr) extern defa defb[defArr];
#define WRAM_EXTERN(defa, defb) extern defa defb;
#define WRAM_ARRAY_EXTERN(defa, defb, defArr) extern defa defb[defArr];

// Set the PRG bank to put the code in the current file into.
#define CODE_BANK(id) _Pragma("rodataseg (push, \"ROM_0" STR(id) "\")") _Pragma("codeseg (push, \"ROM_0" STR(id) "\")")

// Reverse the actions of the CODE_BANK function, if you need to go back to the default bank.
#define CODE_BANK_POP() _Pragma("rodataseg (pop)") _Pragma("codeseg (pop)")