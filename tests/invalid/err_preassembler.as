; err_preassembler.as
; Every error the pre-assembler is responsible for.
; Expected: all of them reported with line numbers, NO .am file left on disk,
; and the assembler must not continue to the first pass for this file.

; 1. macro named after an operation
mcro add
hlt
mcroend

; 2. macro named after a directive
mcro asciz
hlt
mcroend

; 3. extraneous text on the mcro line
mcro GOOD_MAC extra text here
hlt
mcroend

; 4. extraneous text on the mcroend line
mcro OTHER_MAC
hlt
mcroend trailing junk

; 5. missing macro name
mcro
hlt
mcroend

; 6. macro name that is not a legal symbol
mcro 1bad
hlt
mcroend

; 7. line longer than 80 characters (the next line is 90 characters long)
add $1, $2, $3                                                                   ; padding
