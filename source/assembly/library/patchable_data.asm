; This is done in assembly so we can be a little more sure about where the linker will put this data.
; If it gets moved around, the application patcher will no longer work.

; NOTE: If you're editing the raw code of your own game you can ignore the warnings in this file, and move it wherever
; you want! These warnings only apply if your rom will continue to be used with the  web-based editor.

; Also note, if you want to patch in a game from the editor, check out "game.json" in the 

.segment "USER_DATA"

; DO NOT REMOVE/EDIT! Used by engine to identify game and align patching
.asciiz "PATCH TARGET 1.01" ; 0x12

; These are imported in map.h for the most part
_user_gameName:
    .asciiz "     Retro Puzzle Maker     " ; 1c
.export _user_gameName: absolute

_user_totalGameLevels: 
    .byte 2 ;1
.export _user_totalGameLevels: absolute

_user_gameLevelData:; 128 (4096 total, res for rest)
    
    .byte $01, $99, $99, $99, $99, $99
    .byte $01, $22, $22, $22, $22, $29
    .byte $01, $21, $06, $00, $12, $29
    .byte $01, $21, $48, $00, $12, $21
    .byte $01, $21, $00, $00, $12, $21

    .byte $00, $00, $00, $00, $cb, $00
    .byte $01, $21, $50, $45, $12, $21
    .byte $01, $21, $45, $40, $12, $21
    .byte $01, $99, $99, $11, $99, $11
    .byte $01, $11, $11, $11, $91, $17


    ; Extra data...
    ; Tile id for border
    .byte $02
    ; Gameplay mode
    .byte $1
    
    ; start position (top nybble is y, bottom nybble is x - starts at first playable space, no border)
    .byte $44
    ; Unused
    .byte $67

    ; ---

    .byte $01, $11, $11, $11, $11, $11
    .byte $01, $22, $22, $22, $22, $21
    .byte $01, $21, $06, $00, $12, $21
    .byte $01, $21, $50, $00, $12, $21
    .byte $01, $21, $00, $00, $12, $21

    .byte $00, $00, $00, $00, $00, $00
    .byte $01, $21, $50, $45, $12, $21
    .byte $01, $21, $00, $40, $12, $21
    .byte $01, $22, $17, $11, $22, $21
    .byte $01, $11, $11, $11, $11, $11


    ; Extra data...
    ; Tile id for border
    .byte $02
    ; Gameplay mode
    .byte $00 
    
    ; start position (top nybble is y, bottom nybble is x - starts at first playable space, no border)
    .byte $44
    ; Unused
    .byte $67
; rest of the levels
.res $f80, $ef


.export _user_gameLevelData: absolute

_user_tileCollisionTypes:
    .byte $00, $00, $01, $01, $04, $06, $05, $07
    .byte $02, $0a, $00, $08, $09, $00, $00, $00
.export _user_tileCollisionTypes: absolute

_user_tilePalettes:
    .byte $00, $01, $02, $03, $00, $01, $02, $03
    .byte $01, $01, $02, $03, $00, $01, $02, $03
.export _user_tilePalettes: absolute

_user_titleScreenData:
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $e0, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $e1, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $b2, $c5, $d4
    .byte $d2, $cf, $80, $b0, $d5, $da, $da, $cc
    .byte $c5, $80, $ad, $c1, $cb, $c5, $d2, $80
    .byte $b4, $c5, $d3, $d4, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $a3, $cf, $d0, $d9, $d2, $c9, $c7
    .byte $c8, $d4, $80, $92, $90, $92, $92, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $b2, $c5, $d4, $d2
    .byte $cf, $80, $b0, $d5, $da, $da, $cc, $c5
    .byte $80, $ad, $c1, $cb, $c5, $d2, $80, $b4
    .byte $c5, $c1, $cd, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $f0, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f1, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    
    
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $05, $05, $05, $05, $05, $05, $05, $05


.export _user_titleScreenData: absolute

_user_creditsScreenData:
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $e0, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $e1, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $b2, $c5, $d4
    .byte $d2, $cf, $80, $b0, $d5, $da, $da, $cc
    .byte $c5, $80, $ad, $c1, $cb, $c5, $d2, $80
    .byte $b4, $c5, $d3, $d4, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $a3, $cf, $d0, $d9, $d2, $c9, $c7
    .byte $c8, $d4, $80, $92, $90, $92, $92, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $b2, $c5, $d4, $d2
    .byte $cf, $80, $b0, $d5, $da, $da, $cc, $c5
    .byte $80, $ad, $c1, $cb, $c5, $d2, $80, $b4
    .byte $c5, $c1, $cd, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $f0, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f1, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    
    
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $55, $55, $55, $55, $55, $55, $55, $55
    .byte $05, $05, $05, $05, $05, $05, $05, $05
.export _user_creditsScreenData: absolute

_user_gamePaletteData:
    .byte $0f, $0a, $19, $2a, $0f, $11, $21, $31, $0f, $16, $28, $2a, $0f, $10, $20, $30
.export _user_gamePaletteData: absolute

_user_spritePalette: 
    .byte $0f, $01, $21, $31
.export _user_spritePalette: absolute

_user_unusedByte1:
    .byte $00
.export _user_unusedByte1: absolute

_user_singleLevelOverride:
    .byte $ff
.export _user_singleLevelOverride: absolute

_user_unusedByte2:
    .byte $00
.export _user_unusedByte2 :absolute

_user_unusedByte3:
    .byte $03
.export _user_unusedByte3: absolute

_user_unusedByte4:
    .byte 1
.export _user_unusedByte4

_user_movementSpeed:
    .byte 2
.export _user_movementSpeed

_user_coinsCollectedText: 
    .asciiz "Coins collected:   "
.export _user_coinsCollectedText

_user_cratesRemovedText: 
    .asciiz "Crates Removed:    "
.export _user_cratesRemovedText

_user_enableUndo:
    .byte 1
.export _user_enableUndo

_user_enableLevelShow:
    .byte 1
.export _user_enableLevelShow

_user_enableKeyCount:
    .byte 1
.export _user_enableKeyCount

_user_showGameTitle:
    .byte 1
.export _user_showGameTitle

_user_showGoal:
    .byte 1
.export _user_showGoal

_user_hudData:
    .byte $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $80, $80, $f8, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $80, $80, $ac, $c5, $d6, $c5, $cc, $9a, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80 
    .byte $f4, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5
    .byte $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $e5, $f5 

.export _user_hudData

_user_hudDataAttrs:
    .byte $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff
.export _user_hudDataAttrs

_user_customSongShowTimes:
    .byte 101, 0, 102, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff
.export _user_customSongShowTimes

.segment "STATIC_SC"

_user_staticScreenTypes:
    .byte $65, $ff, $ff, $ff, $ff, $ff, $ff, $ff
    .byte $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff

.export _user_staticScreenTypes: absolute



_user_staticScreens:
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $e0, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $e1, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $a8, $cf, $d7, $80
    .byte $b4, $cf, $80, $b0, $cc, $c1, $d9, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $91, $8e, $80, $ad
    .byte $cf, $d6, $c5, $80, $c1, $cc, $cc, $80
    .byte $c2, $cc, $cf, $c3, $cb, $d3, $80, $80
    .byte $80, $80, $08, $09, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $c9
    .byte $ce, $d4, $cf, $80, $d4, $c8, $c5, $80
    .byte $c8, $cf, $cc, $c5, $d3, $81, $80, $80
    .byte $80, $80, $18, $19, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $0e, $0f, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $92, $8e, $80, $a7
    .byte $c5, $d4, $80, $d4, $cf, $80, $d4, $c8
    .byte $c5, $80, $c5, $d8, $c9, $d4, $81, $80
    .byte $80, $80, $1e, $1f, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $e6, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $e6, $80, $80
    .byte $80, $80, $f0, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f6, $f6, $f6
    .byte $f6, $f6, $f6, $f6, $f6, $f1, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $80, $80, $80, $80, $80, $80, $80, $80
    .byte $00, $00, $00, $00, $00, $00, $00, $00
    .byte $44, $55, $55, $55, $55, $55, $55, $11
    .byte $44, $55, $55, $55, $55, $55, $95, $11
    .byte $44, $55, $55, $55, $55, $55, $95, $11
    .byte $44, $55, $55, $55, $55, $55, $55, $11
    .byte $44, $55, $55, $55, $55, $55, $55, $11
    .byte $04, $05, $05, $05, $05, $05, $05, $01
.if .defined(USE_COMPRESSION) 
    ; hopefully enough space for your data with rle...
    ; 8 * 1024
    .res (8192), $ef
.else
    ; 13 * 10 1024 screens
    .res 13312, $ef
.endif

.export _user_staticScreens: absolute
