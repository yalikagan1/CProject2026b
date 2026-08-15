; macros.as
; Exercises the pre-assembler: several macros, several invocations of each,
; a single-line macro body, and a macro invoked with surrounding whitespace.
; The resulting .am must contain no mcro/mcroend lines at all.
; The names carry an underscore on purpose: a macro name allows one even though
; a label does not.
.entry MAIN
.extern EXTVAL

mcro GEN_MAC
la VAL
jmp NEXT
mcroend

mcro ONE_LINER
add $1, $2, $3
mcroend

mcro USES_EXTERN
la EXTVAL
mcroend

MAIN:   ori     $9, -5, $2
        GEN_MAC
NEXT:   move    $20, $4
        ONE_LINER
	GEN_MAC
        ONE_LINER
        USES_EXTERN
        USES_EXTERN
        ONE_LINER
        hlt

VAL:    .dw     31, -12
