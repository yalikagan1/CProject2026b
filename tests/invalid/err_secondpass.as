; err_secondpass.as
; Every line here is syntactically valid, so the first pass succeeds and the
; errors can only be found in the second pass. This is the file that catches a
; symbol lookup that silently returns -1 instead of reporting a failure.
; Expected: five errors, and no .ob / .ent / .ext produced.
; The undefined names deliberately carry no underscore. A label is alphanumeric
; only, so a name like NOT_THERE is rejected by the first pass as a malformed
; operand, and the second pass then never runs at all.
.extern EXT

; a conditional branch may not target an external symbol
MAIN:   bne     $1, $2, EXT

; symbols that are never defined and never declared external
        beq     $1, $2, NOSUCH
        jmp     MISSING
        call    ALSOMISSING
        la      NOTTHERE

        hlt
