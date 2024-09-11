;
; UNROM Mapper registers, for use with the functions below
; 
.define UNROM_BANK_SELECT $8000

.segment "ZEROPAGE"
    unrom_current_bank: 
    _unrom_current_bank: 
        .res 1
    .export unrom_current_bank, _unrom_current_bank

.segment "CODE"

    ; Bank table, used to 
    unrom_banktable:
        .byte $00, $01, $02, $03, $04, $05, $06, $07
        .byte $08, $09, $0a, $0b, $0c, $0d, $0e, $0f


    ; Set the prg bank to be used.
    unrom_set_prg_bank: 
    _unrom_set_prg_bank:
    sta _unrom_current_bank
    unrom_set_prg_bank_nosave:
    _unrom_set_prg_bank_nosave:
        tax
        sta unrom_banktable, x
        rts
    .export _unrom_set_prg_bank

    initialize_mapper:
        ; Start in bank 0
        lda #0
        jsr unrom_set_prg_bank
        rts

; Make sure to put something in every bank, so the library/mapper doesn't get confused. Let's just jump to reset.
.segment "USER_DATA" 
    jmp reset
.segment "MENUS"
    jmp reset
.segment "GRAPHICS" 
    jmp reset
.segment "PLAYER" 
    jmp reset
.segment "SOUND" 
    jmp reset
.segment "STATIC_SC" 
    jmp reset
.segment "UNKNOWN04" 
    jmp reset

; This ugly little mess of assembly pulls in a linker symbol, then exports it as if it were a pointer
; This allows us to read it from C without storing it to a memory location.
.macro make_ptr var_name
    .import var_name
    .ident(.concat("_", .string(var_name), "_PTR")) = var_name
    .export .ident(.concat("_", .string(var_name), "_PTR"))
.endmacro

make_ptr BANK_USER_DATA
make_ptr BANK_MENUS
make_ptr BANK_GRAPHICS
make_ptr BANK_PLAYER
make_ptr BANK_SOUND
make_ptr BANK_STATIC_SC
make_ptr BANK_UNKNOWN04