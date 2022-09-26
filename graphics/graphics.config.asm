
;
; CHR Data
;
; The files below are currently loaded into your game's prg banks, and have to be loaded
; manually in your code. 
; 
; You will probably want to add compression to this data, since you have limited space!
; It also is a good idea to move this to a prg bank other than the primary.
;
; Make sure to start the names with an underscore - this tells the assembler to make the
; data available to C. (C will not use the underscore)
; 
; Any new graphics you add here also need to be added to `graphics.config.h` as extern 
; variables, so they can be read from C code. 
;
;

.segment "GRAPHICS" 
    _background_graphics:
        .incbin "./tiles_mod.chr"
    _sprite_graphics:
        ; NOTE: This is here to help romhackers who might like to have another one :)
        ; Or some day I may add official support.
        .incbin "./tiles_mod.chr"
;
; Make sure to export all symbols created, too, so we can read them from our code!
;
.export _background_graphics
.export _sprite_graphics

.import pushax
    _load_graphics:
        lda #$00
        ldx #$00
        jsr _vram_adr
        lda #<(_background_graphics)
        ldx #>(_background_graphics)
        jsr pushax
        lda #$00
        ldx #$20
        jsr _vram_write
        rts
.export _load_graphics