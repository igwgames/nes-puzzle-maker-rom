; All this does is put the raw bytes in the palettes you export from nesst into the rom, then give it
; an address to reference. These are prefixed with an underscore (_) because that's how C knows that they
; are meant to be accessible from our C code. Don't forget to use .export on new items!
