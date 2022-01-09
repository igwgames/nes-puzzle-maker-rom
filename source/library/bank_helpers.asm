.feature c_comments
.linecont +

.define NO_CHR_BANK 255

.segment "ZEROPAGE"
    nmiChrTileBank: .res 1
    .exportzp nmiChrTileBank

.segment "CODE"

.export _set_prg_bank, _get_prg_bank
.export _set_nmi_chr_tile_bank, _unset_nmi_chr_tile_bank
.export _set_mirroring

_set_prg_bank:

    rts

_get_prg_bank:
    lda BP_BANK
    rts

; Both of these just set/unset a varible used in `neslib.asm` to trigger this during nmi.
_set_nmi_chr_tile_bank: 
    rts

_unset_nmi_chr_tile_bank:
    rts

_set_mirroring:
    rts