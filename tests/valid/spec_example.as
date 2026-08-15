; spec_example.as
; The reference program from the course booklet (pages 62-64).
; Its .ob, .ent and .ext outputs are printed in the booklet, so the expected
; results of this test are not our own guesses.
; The only change from the booklet is "END: hlt" - the booklet prints "END hlt"
; without a colon, which is a typo: its own symbol table lists END as a code
; label at 148, and the bgt distance of 28 only works if END is a label.
.entry NEXT
.extern wNumber
STR: .asciz "aBcd"
MAIN: add $3,$5,$9
LOOP: ori $9,-5,$2
la val1
jmp NEXT
NEXT: move $20,$4
LIST: .db 6,-9
bgt $4,$2,END
la K
sw $0,4,$10
bne $31,$9, LOOP
call val1
jmp $4
la wNumber
.extern val1
.dh 27056
K: .dw 31,-12
END: hlt
.entry K
