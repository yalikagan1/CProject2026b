# Test Plan — Maman 14 Assembler (20465)

Version 1.0 — covers the whole pipeline defined in the course booklet, pages 33–65.

---

## 1. Scope and structure

The assembler has four sequential stages. A test plan that "covers all flows" has to
exercise each stage on its own **and** the hand-offs between them:

```
argv  →  pre-assembler  →  first pass  →  second pass  →  output files
(.as)      (.am)            (symbol table,   (resolve      (.ob / .ent / .ext)
                             IC/DC, images)   symbols)
```

Test cases are grouped by stage and each has a stable ID so you can tick them off:

| Group | ID prefix | Subject |
|---|---|---|
| A | `BLD` | Build, compiler flags, ANSI C90 compliance |
| B | `CLI` | Command line, file names, multiple files |
| C | `PRE` | Pre-assembler (macro expansion, `.am`) |
| D | `LEX` | Line-level lexing: length, whitespace, comments |
| E | `SYM` | Labels and the symbol table |
| F | `DIR` | Directives and the data image |
| G | `INS` | Instructions, operands, the code image |
| H | `PS1` | First pass counters and the `+ICF` fix-up |
| I | `PS2` | Second pass, encoding, symbol resolution |
| J | `OUT` | Output file contents and formats |
| K | `ERR` | Error reporting behaviour |
| L | `MEM` | Memory, robustness, resource cleanup |

Each case states **Input**, **Expected**, and (where relevant) **Why the booklet requires it**.

---

## 2. Environment and how to run

Everything must be verified on **Ubuntu with gcc**, because that is what the graders use.
macOS is fine for day-to-day work but is not the reference platform.

```bash
make clean && make
./assembler tests/valid/spec_example
```

Note the current `add_file_extention()` **appends** `.as`, so the program is invoked with the
base name and no suffix. Decide early whether you accept `prog` or `prog.as` on the command
line and keep it consistent — the booklet's example is `assembler test.as myprog.as hello.as`,
i.e. names **with** the suffix, while its summary section writes `assembler myprog`. Either is
accepted by graders as long as it is documented in your README, but the code must not produce
`prog.as.as`.

A runner script is provided at `tests/run_tests.sh` — it builds, runs every case, and diffs
against `tests/expected/`.

---

## 3. Exit criteria

The project is ready to submit when all of the following hold:

1. Every `BLD` case passes (zero warnings with `-Wall -ansi -pedantic`).
2. Every case in groups `CLI` through `OUT` passes.
3. `OUT-01` reproduces the booklet's `ps.ob` / `ps.ent` / `ps.ext` **byte for byte**.
4. `valgrind --leak-check=full` reports zero leaks and zero invalid accesses on the full suite.
5. You have at least 3 valid input files with their outputs, and at least 3 error input files
   with captured screen output (booklet requirement, page 33).

---

## Group A — Build and language compliance (`BLD`)

| ID | Test | Expected |
|---|---|---|
| BLD-01 | `make clean && make` on Ubuntu/gcc | Builds with **zero** warnings and zero errors |
| BLD-02 | Flags in the Makefile | `-Wall -ansi -pedantic` all present |
| BLD-03 | Grep the sources for `//` comments | None; C90 uses `/* */` only |
| BLD-04 | Grep for declarations after statements and `for (int i = ...)` | None |
| BLD-05 | Grep for `bool` / `stdbool.h` / `snprintf` / `strdup` | None — all are post-C90 |
| BLD-06 | `make` twice in a row | Second run says "up to date", does not relink from scratch |
| BLD-07 | `make clean` | Removes `build/` and `assembler`; a fresh `make` still works |
| BLD-08 | No external libraries beyond the C standard library | Booklet forbids third-party code |

> Current state: `src/main.c` and `src/macro_expand.c` still contain `//` comments and
> `macro_expand.c` uses `bool`. `BLD-03` and `BLD-05` fail today.

---

## Group B — Command line and file handling (`CLI`)

| ID | Input | Expected |
|---|---|---|
| CLI-01 | `./assembler` with no arguments | Friendly message on how to use the program; exit without crashing |
| CLI-02 | `./assembler tests/valid/spec_example` | Full pipeline runs, all output files produced |
| CLI-03 | `./assembler f1 f2 f3` (three valid files) | Each file processed **independently**, three sets of outputs |
| CLI-04 | `./assembler good bad good2` where `bad` has errors | `bad` reports errors and produces no output files; `good` and `good2` still produce full output |
| CLI-05 | `./assembler does_not_exist` | "cannot open file" message, program continues to the next file, no crash |
| CLI-06 | A file whose name has a path: `./assembler tests/valid/spec_example` | Outputs land next to the input (`tests/valid/spec_example.ob`), not in the CWD — or wherever you documented, but consistently |
| CLI-07 | Same base name given twice on the command line | Second run overwrites cleanly, no append, no crash |
| CLI-08 | A file with no read permission (`chmod 000`) | Open error reported, moves on to the next file |
| CLI-09 | Empty `.as` file (0 bytes) | No errors; produces an empty `.am`, an `.ob` with header `0 0`, and no `.ent`/`.ext` |

State to verify per file: **the assembler must reset IC, DC, the symbol table, the code image
and the data image between files.** CLI-03 with two files that both define `MAIN` is the
sharpest test: if you get a "symbol already exists" error on the second file, your state is
leaking across files.

---

## Group C — Pre-assembler / macros (`PRE`)

The booklet is explicit about what must and must not be checked here (page 43).

### Valid flows

| ID | Input | Expected |
|---|---|---|
| PRE-01 | Source with no macros at all | `.am` is identical in content to the `.as` |
| PRE-02 | One macro, invoked once | Body substituted at the call site; `mcro`/`mcroend` lines and the definition body removed from the `.am` |
| PRE-03 | One macro invoked **three** times | Body appears three times |
| PRE-04 | Two different macros, each invoked | Both expanded correctly, no cross-contamination |
| PRE-05 | Macro with a single line in its body | Expanded correctly (off-by-one guard) |
| PRE-06 | Macro whose body contains a label definition, e.g. `L1: add $1,$2,$3` | Copied verbatim; note that invoking it twice will legitimately produce a duplicate-label error in the first pass |
| PRE-07 | Macro invocation with leading/trailing whitespace and tabs around the name | Still recognised and expanded |
| PRE-08 | `mcro NAME` with extra tabs between `mcro` and the name | Accepted |
| PRE-09 | Last line of the file has no trailing `\n` | Handled as a normal line (booklet page 25 requires this) |
| PRE-10 | Macro name that is a **prefix** of another word, e.g. macro `m1` and a line `m1x` | `m1x` is **not** expanded |

### Error flows

| ID | Input | Expected error |
|---|---|---|
| PRE-11 | `mcro add` (macro named after an operation) | Invalid macro name; **no `.am` produced**; skip to next file |
| PRE-12 | `mcro asciz` or `mcro .db` (directive name) | Invalid macro name |
| PRE-13 | `mcro GEN_MAC extra_text` | Extraneous text after macro definition |
| PRE-14 | `mcroend extra_text` | Extraneous text after `mcroend` |
| PRE-15 | `mcro` with no name at all | Missing macro name |
| PRE-16 | A line longer than 80 characters (excluding `\n`) | Line too long, with the line number |

The critical assertion for all of PRE-11..16: **the `.am` file must not exist afterwards**, and
the assembler must not proceed to the first pass for that file. The booklet says so directly:
"אם נמצאה שגיאה בשלב פרישת המאקרו – אי אפשר לעבור לשלבים הבאים".

> Current state: PRE-14 is not implemented (`mcroend` extra-character check is missing), and
> macro-name validation only checks reserved words — it does not enforce "starts with a letter,
> alphanumeric afterwards". PRE-10 needs verifying because expansion matches on the first word.

---

## Group D — Line lexing (`LEX`)

| ID | Input line | Expected |
|---|---|---|
| LEX-01 | Completely empty line | Ignored, no counter advance |
| LEX-02 | Line of only spaces and tabs | Ignored |
| LEX-03 | `; this is a comment` | Ignored entirely |
| LEX-04 | `   ; comment after whitespace` | The booklet defines a comment line as one whose **first non-white character** is `;`, so this is a comment |
| LEX-05 | `add $1,$2,$3 ; trailing comment` | **Not** a comment line — `;` mid-line is extraneous text, i.e. an error |
| LEX-06 | Exactly 80 characters | Accepted |
| LEX-07 | Exactly 81 characters | Line-too-long error |
| LEX-08 | `\t\tadd\t$1,\t$2,\t$3\t` (tabs everywhere) | Parsed identically to the clean form |
| LEX-09 | `add $1 , $2 , $3` (spaces around commas) | Accepted |
| LEX-10 | Windows line endings (`\r\n`) in the input file | Either handled or a clear error — but **never** a stray `\r` inside a label name or an operand. Worth testing because graders may edit files on Windows |
| LEX-11 | A line containing only `\n` at the very end of the file | Ignored, no spurious error |

---

## Group E — Labels and symbol table (`SYM`)

### Valid

| ID | Input | Expected |
|---|---|---|
| SYM-01 | `x: add $1,$2,$3` (1-character label) | Accepted |
| SYM-02 | 31-character label | Accepted |
| SYM-03 | `He78902: hlt` (letters + digits) | Accepted |
| SYM-04 | `hEllo:` and `HELLO:` in the same file | Two **distinct** symbols — case matters |
| SYM-05 | Label on a `.db` line | Attribute `data`, value = current DC |
| SYM-06 | Label on an instruction line | Attribute `code`, value = current IC |
| SYM-07 | Forward reference: `jmp A` before `A:` is defined | Resolved in the second pass |
| SYM-08 | `.entry K` appearing **before** `K:` is defined | Legal; resolved at the end of pass one |
| SYM-09 | `.entry K` appearing **after** `K:` | Legal |
| SYM-10 | `.extern val1` declared twice with the same name | **Not** an error (booklet page 53) |
| SYM-11 | `.entry K` declared twice | **Not** an error (booklet page 53) |
| SYM-12 | An operand referring to a symbol never defined locally but declared `.extern` | Legal |

### Errors

| ID | Input | Expected error |
|---|---|---|
| SYM-13 | 32-character label | Label too long |
| SYM-14 | `1abc: hlt` | Invalid label (must start with a letter) |
| SYM-15 | `my_label: hlt` (underscore) | Invalid label — only letters and digits allowed |
| SYM-16 | `my label: hlt` | Invalid label |
| SYM-17 | `add: hlt` | Label is a reserved word |
| SYM-18 | `asciz: hlt` | Label is a reserved word (booklet names this case explicitly) |
| SYM-19 | `Add: hlt` and `ASCiz: hlt` | **Legal** — reserved words are lowercase only |
| SYM-20 | Same label defined on two different lines | Symbol already exists |
| SYM-21 | Same name used as both a macro name and a label | Error — booklet says this must be checked |
| SYM-22 | `.entry K` where `K` is never defined in the file | Entry not defined |
| SYM-23 | `.entry K` and `.extern K` in the same file | Entry-and-extern conflict |
| SYM-24 | A label defined locally **and** declared `.extern` | Conflict |
| SYM-25 | `LBL : add $1,$2,$3` (space before the colon) | Error — the booklet requires the `:` to touch the label |
| SYM-26 | `LBL:add $1,$2,$3` (no space after the colon) | Accepted — nothing requires whitespace there |

### Warnings (not errors)

| ID | Input | Expected |
|---|---|---|
| SYM-27 | `LBL: .entry K` | The label is meaningless; the booklet permits a warning and requires that it be ignored, **not** treated as an error |
| SYM-28 | `LBL: .extern val1` | Same |

---

## Group F — Directives and the data image (`DIR`)

### `.db` / `.dh` / `.dw`

| ID | Input | Expected |
|---|---|---|
| DIR-01 | `.db 7, -57, 17, +9` | 4 bytes; the `+` sign is legal |
| DIR-02 | `.dw 120056` | 4 bytes, little-endian |
| DIR-03 | `.dh 0, -60431, 1700, 3, -1` | 10 bytes |
| DIR-04 | `.db 6 , -9` (spaces around commas) | Accepted |
| DIR-05 | `.db     7` (many spaces before the value) | Accepted |
| DIR-06 | `.db 127` and `.db -128` | Boundary values, accepted |
| DIR-07 | `.db 128` or `.db -129` | Number out of range |
| DIR-08 | `.dh 32767` / `.dh -32768` | Accepted |
| DIR-09 | `.dh 32768` / `.dh -32769` | Out of range |
| DIR-10 | `.dw 2147483647` / `.dw -2147483648` | Accepted |
| DIR-11 | `.dw 2147483648` | Out of range |
| DIR-12 | `.db` with no parameters | Missing parameter |
| DIR-13 | `.db 1,,2` | Multiple consecutive commas |
| DIR-14 | `.db 1,2,` | Trailing comma is illegal |
| DIR-15 | `.db ,1,2` | Leading comma is illegal |
| DIR-16 | `.db 1 2` | Missing comma |
| DIR-17 | `.db 1, abc` | Not a number |
| DIR-18 | `.db 1.5` | Not an integer — the booklet supports decimal integers only |
| DIR-19 | `.db 0x1F` | Not a number — no hex support |

### `.asciz`

| ID | Input | Expected |
|---|---|---|
| DIR-20 | `.asciz "aBcd"` | 5 bytes: `61 42 63 64 00` |
| DIR-21 | `.asciz ""` | 1 byte: `00` |
| DIR-22 | `.asciz " hello world "` | Leading and trailing spaces **inside** the quotes are part of the string (12 bytes) |
| DIR-23 | `.asciz "abc"` with trailing whitespace after the closing quote | Accepted, whitespace ignored |
| DIR-24 | `.asciz abc` (no quotes) | Invalid string |
| DIR-25 | `.asciz "abc` (unterminated) | Invalid string |
| DIR-26 | `.asciz "abc" "def"` | Extraneous text |
| DIR-27 | `.asciz "a,b"` | Comma inside the string is just a character, not a separator |

### `.entry` / `.extern`

| ID | Input | Expected |
|---|---|---|
| DIR-28 | `.entry` with no operand | Missing parameter |
| DIR-29 | `.extern` with no operand | Missing parameter |
| DIR-30 | `.entry A, B` | Extraneous text — exactly one parameter is allowed |
| DIR-31 | `.entry 5` | Invalid label |

### Unknown directives

| ID | Input | Expected |
|---|---|---|
| DIR-32 | `.data 1,2,3` | Unknown directive — `.data` belongs to a different course version, it is **not** in this spec |
| DIR-33 | `.string "abc"` | Unknown directive (same reason) |
| DIR-34 | `.DB 1,2` | Unknown directive — directives are lowercase only |
| DIR-35 | `.` alone | Unknown directive |

> Note: `test_files/test1.as` currently uses `mov`, `prn`, `inc`, `stop`, `@r3`, `.string` and
> `.data`, none of which exist in this ISA. It is a leftover from a different assignment. Either
> delete it or repurpose it as a deliberate "unknown operation/directive" error file.

---

## Group G — Instructions and operands (`INS`)

### Coverage of every operation

`INS-01` is a single file (`tests/valid/all_ops.as`) that uses **all 27 operations** at least
once. The booklet explicitly asks you to demonstrate "מגוון הפעולות וטיפוסי הנתונים".

| Family | Operations | Format |
|---|---|---|
| R arithmetic/logic | `add sub and or nor` | `op $rs, $rt, $rd` |
| R copy | `move mvhi mvlo` | `op $rd, $rs` |
| I arithmetic/logic | `addi subi andi ori nori` | `op $rs, imm, $rt` |
| I branch | `bne beq blt bgt` | `op $rs, $rt, label` |
| I load/store | `lb sb lw sw lh sh` | `op $rs, imm, $rt` |
| J | `jmp la call hlt` | `jmp label` / `jmp $reg` / `la label` / `call label` / `hlt` |

### Register operands

| ID | Input | Expected |
|---|---|---|
| INS-02 | `add $0,$31,$15` | Boundary register numbers accepted |
| INS-03 | `add $32,$1,$2` | Invalid operand — register out of range |
| INS-04 | `add $-1,$1,$2` | Invalid operand |
| INS-05 | `add $,$1,$2` | Invalid operand |
| INS-06 | `add r3,$1,$2` (missing `$`) | Invalid operand |
| INS-07 | `add $03,$1,$2` | Decide and document: is `$03` the same as `$3`? The current register table only matches exact strings, so `$03` is rejected. That is a defensible reading — just be consistent |
| INS-08 | `or $1,$2,$1` (same register twice) | **Legal** — booklet says so explicitly |

### Immediate operands

| ID | Input | Expected |
|---|---|---|
| INS-09 | `addi $9,-45,$8` | Accepted, negative immediate |
| INS-10 | `addi $9,+45,$8` | Accepted |
| INS-11 | `addi $9,32767,$8` / `addi $9,-32768,$8` | Boundary, accepted |
| INS-12 | `addi $9,32768,$8` | Out of range — the immediate field is 16 bits, two's complement |
| INS-13 | `addi $9,abc,$8` | Invalid operand — not a number |
| INS-14 | `addi $9,$5,$8` | Invalid operand — a register where an immediate is required |
| INS-15 | `sw $0,4,$10` and `sw $7,-28,$18` | Positive and negative offsets accepted |

### Operand count and separators

| ID | Input | Expected |
|---|---|---|
| INS-16 | `add $1,$2` | Too few operands |
| INS-17 | `add $1,$2,$3,$4` | Too many operands |
| INS-18 | `move $1,$2,$3` | Too many operands |
| INS-19 | `hlt $1` | Too many operands / extraneous text |
| INS-20 | `add` (no operands) | Missing operands |
| INS-21 | `add $1 $2 $3` | Missing comma |
| INS-22 | `add $1,,$2,$3` | Multiple consecutive commas |
| INS-23 | `add, $1,$2,$3` | Illegal comma after the operation name |
| INS-24 | `add $1,$2,$3,` | Trailing comma / extraneous text |
| INS-25 | `add $1,$2,$3 garbage` | Extraneous text after end of command |

### Operation names

| ID | Input | Expected |
|---|---|---|
| INS-26 | `ADD $1,$2,$3` | Unknown operation — operation names are lowercase only |
| INS-27 | `addx $1,$2,$3` | Unknown operation |
| INS-28 | A line that is only a label: `LBL:` with nothing after it | Decide and document. Simplest defensible behaviour is "missing operation" |

### Branch and jump targets

| ID | Input | Expected |
|---|---|---|
| INS-29 | `bne $31,$9,LOOP` where `LOOP` is backwards | Negative distance encoded |
| INS-30 | `bgt $4,$2,END` where `END` is forwards | Positive distance encoded |
| INS-31 | `bne $1,$2,extlabel` where `extlabel` is `.extern` | **Error** — the booklet forbids an external label as a conditional branch target |
| INS-32 | `bne $1,$2,NOSUCH` (undefined symbol) | Error, symbol not found |
| INS-33 | `bne $1,$2,$5` (register instead of label) | Invalid operand |
| INS-34 | `jmp $7` | Register form: `reg` bit = 1, address field = 7 |
| INS-35 | `jmp LABEL` | Label form: `reg` bit = 0 |
| INS-36 | `jmp 100` (a bare number) | Invalid operand |
| INS-37 | `la val1` where `val1` is external | Address field = 0, recorded in `.ext` |
| INS-38 | `call NOSUCH` | Error, symbol not found |
| INS-39 | `la $5` | Invalid operand — `la` takes a label, not a register |

---

## Group H — First pass counters (`PS1`)

| ID | Test | Expected |
|---|---|---|
| PS1-01 | A file with 3 instructions and no data | `ICF = 112`, `DCF = 0` |
| PS1-02 | Each instruction advances IC | IC increases by exactly 4 per instruction, regardless of type |
| PS1-03 | `.db` advances DC by 1 per value, `.dh` by 2, `.dw` by 4, `.asciz` by length+1 | Verify against `spec_example` where `DCF = 17` |
| PS1-04 | Data symbols get `+ICF` at the end of pass one | In `spec_example`: `STR` moves from DC 0 to 152, `LIST` from 5 to 157, `K` from 9 to 161 |
| PS1-05 | Code symbols are **not** shifted | `MAIN = 100`, `LOOP = 104`, `NEXT = 116`, `END = 148` |
| PS1-06 | External symbols keep value 0 | `val1 = 0`, `wNumber = 0` |
| PS1-07 | Errors in pass one prevent pass two from running | A file with a bad label produces **no** `.ob`, `.ent` or `.ext` |
| PS1-08 | Pass one does not stop at the first error | A file with 5 independent errors reports all 5 |
| PS1-09 | Interleaved code and data | Data directives placed between instructions still end up contiguous after `ICF` |

`spec_example` is the reference for PS1-03 through PS1-06; its expected symbol table is:

| Symbol | Value | Attributes |
|---|---|---|
| `wNumber` | 0 | external |
| `STR` | 152 | data |
| `MAIN` | 100 | code |
| `LOOP` | 104 | code |
| `NEXT` | 116 | code, entry |
| `LIST` | 157 | data |
| `val1` | 0 | external |
| `K` | 161 | data, entry |
| `END` | 148 | code |

---

## Group I — Second pass and encoding (`PS2`)

Bit-level verification. The most efficient way to check these is to run `spec_example` and diff
the `.ob`, but each encoding rule deserves a targeted case too.

| ID | Instruction | Expected 32-bit encoding |
|---|---|---|
| PS2-01 | `add $3,$5,$9` | `000000 00011 00101 01001 00001 000000` — note `rs=3, rt=5, rd=9` |
| PS2-02 | `move $20,$4` | `000001 10100 00000 00100 00001 000000` — for `move` the **first** operand goes to `rd` and the second to `rs`, and `rt` is 0 |
| PS2-03 | `ori $9,-5,$2` | `001101 01001 00010 1111111111111011` — the immediate is sign-extended to 16 bits |
| PS2-04 | `sw $0,4,$10` | `010110 00000 01010 0000000000000100` — the **second** operand is the offset, the third is `rt` |
| PS2-05 | `bgt $4,$2,END` at IC 120, `END = 148` | immediate = `148 - 120 = 28` |
| PS2-06 | `bne $31,$9,LOOP` at IC 132, `LOOP = 104` | immediate = `104 - 132 = -28` → `1111111111100100` |
| PS2-07 | `jmp NEXT` where `NEXT = 116` | `reg` bit 0, address = 116 |
| PS2-08 | `jmp $4` | `reg` bit 1, address = 4 |
| PS2-09 | `la val1` (external) | `reg` bit 0, address = 0 |
| PS2-10 | `hlt` | opcode 63, every other bit 0 |
| PS2-11 | Unused bits in R-type | Bits 0–5 are zero for `add`; bits 0–5 **and** `rt` are zero for `move` |
| PS2-12 | Negative `.dw` value | `-12` → `FF FF FF F4` in memory, written little-endian as `F4 FF FF FF` |

**Gotchas worth an explicit test**, because they are the classic sources of lost points:

- `add $3,$5,$9` — the operand order in the source is `rs, rt, rd`, but the *encoding* field
  order is `opcode, rs, rt, rd, funct`. It is easy to swap `rd` and `rt`.
- `addi $9,-45,$8` — source order is `rs, immed, rt`, so the second operand is *not* `rt`.
- `move $23,$2` copies `$2` into `$23`, so `rd = 2` and `rs = 23`. The booklet's wording
  ("copies the content of `rd` into `rs`") is confusing but the example is unambiguous.
- Branch distance is measured from the branch instruction's **own** address, not the next one.

---

## Group J — Output files (`OUT`)

| ID | Test | Expected |
|---|---|---|
| OUT-01 | Run `spec_example` | `.ob`, `.ent`, `.ext` match `tests/expected/` byte for byte |
| OUT-02 | `.ob` header | Two decimal numbers separated by one space: `ICF-100` then `DCF`. For `spec_example`: `52 17` |
| OUT-03 | `.ob` address column | 4 decimal digits with leading zeros: `0100`, not `100` |
| OUT-04 | `.ob` byte column | 2 uppercase hex digits with a leading zero: `0A`, not `a` or `A` |
| OUT-05 | `.ob` byte order | Little-endian: byte at the lowest address is leftmost on the line |
| OUT-06 | Data packed into the instruction stream | Data starts on a new line at the address right after the last instruction byte, with **no gaps** and no padding |
| OUT-07 | Final `.ob` line may be short | `spec_example` ends with `0168 FF` — one byte only |
| OUT-08 | `.ent` format | `NAME` + one space + 4-digit address, one line per entry symbol |
| OUT-09 | `.ext` format | `NAME` + one space + 4-digit address of the referencing instruction |
| OUT-10 | The same external used twice | Two separate lines in `.ext` — `val1` appears at `0108` and `0136` |
| OUT-11 | Source with no `.entry` | **No** `.ent` file is created (not an empty one) |
| OUT-12 | Source with no `.extern` | **No** `.ext` file is created |
| OUT-13 | Source with neither | Only `.am` and `.ob` |
| OUT-14 | Any error anywhere | **No** `.ob`, `.ent` or `.ext`. The `.am` survives only if the pre-assembler itself succeeded |
| OUT-15 | Output file names | `prog.as` → `prog.am`, `prog.ob`, `prog.ent`, `prog.ext` |
| OUT-16 | Stale files from a previous run | A run that fails must not leave the previous run's `.ob` behind, or you will submit misleading output |

> Current state: `second_pass()` opens a `.ent` path and writes object-file content into it,
> then `write_entries()` reopens `.ent` and overwrites it. No `.ob` file is produced anywhere in
> the codebase. The header format is `"     %d %d"` with raw `ICF`, not `ICF-100`. Group J is
> the biggest gap and should be fixed before anything else here can be tested.

---

## Group K — Error reporting behaviour (`ERR`)

| ID | Test | Expected |
|---|---|---|
| ERR-01 | Every message goes to `stdout` | The booklet says `stdout` specifically, page 51 |
| ERR-02 | Every message includes the source line number | Numbering starts at 1 |
| ERR-03 | Line numbers refer to the right file | Decide whether you report positions in the `.as` or the `.am` and document it; after macro expansion these differ |
| ERR-04 | Every message names the file it came from | Necessary when several files are on the command line |
| ERR-05 | Multiple errors on different lines | All reported, one per line; the assembler does not stop at the first |
| ERR-06 | An error inside a macro body used twice | Reported twice, once per expansion — the booklet acknowledges this is expected and acceptable |
| ERR-07 | Messages are specific | "Register number out of range: $32" beats "invalid operand". The booklet asks for "הודעות מפורטות ככל הניתן" |
| ERR-08 | All 24 codes in `error_handle.h` are reachable | Write at least one input line that triggers each |
| ERR-09 | Malloc failure path | Hard to trigger naturally; at minimum, read the code and confirm every allocation is checked and the program exits cleanly rather than dereferencing NULL |

---

## Group L — Memory and robustness (`MEM`)

| ID | Test | Expected |
|---|---|---|
| MEM-01 | `valgrind --leak-check=full ./assembler spec_example` | Zero leaks, zero errors |
| MEM-02 | Valgrind on a file that errors out in the pre-assembler | Zero leaks — early-exit paths are where leaks hide |
| MEM-03 | Valgrind on a file that errors out in pass one | Zero leaks; `free_symbols_table` and `free_code_image` are currently never called from the pipeline |
| MEM-04 | Valgrind with three files on one command line | No accumulation across files |
| MEM-05 | A source with ~500 instructions and ~2000 data bytes | Completes correctly; no fixed-size overflow |
| MEM-06 | Data image overflow | Exceeding `MAX_DATA_BYTES` (8192) reports the "data image full" error rather than corrupting memory |
| MEM-07 | A label with exactly 31 characters plus a colon | No off-by-one in the `MAX_LABEL_LENGTH + 1` buffer |
| MEM-08 | A line of exactly 80 characters | No off-by-one in the line buffer |
| MEM-09 | Binary garbage as an input file | No crash; either errors out or reports invalid characters |
| MEM-10 | A source file that is a directory | Open error, no crash |

---

## 4. Deliverables mapping

The booklet (page 33) requires specific artefacts. This is how the suite supplies them:

| Requirement | Supplied by |
|---|---|
| ≥ 3 valid input files | `tests/valid/`: `spec_example.as`, `all_ops.as`, `macros.as`, `edge_cases.as` |
| Their `.am`, `.ob`, `.ent`, `.ext` outputs | `tests/expected/` (golden), `tests/actual/` (produced by the runner) |
| ≥ 3 error input files | `tests/invalid/`: `err_preassembler.as`, `err_symbols.as`, `err_instructions.as`, `err_secondpass.as` |
| Screen captures of error output | `./assembler tests/invalid/err_symbols > err_symbols.out 2>&1` |
| Demonstration of all operations and data types | `all_ops.as` |
| Demonstration of a wide variety of errors | The four `invalid/` files together cover all 24 error codes |

### Test asset inventory

| File | Covers |
|---|---|
| `tests/valid/spec_example.as` | The booklet's own reference program; golden `.ob`/`.ent`/`.ext` |
| `tests/valid/all_ops.as` | All 27 operations, all 6 directives, every data-type boundary |
| `tests/valid/macros.as` | Multi-macro expansion, repeated invocation, single-line body |
| `tests/valid/edge_cases.as` | Legal-but-tricky: whitespace, case, duplicate `.entry`/`.extern`, label limits |
| `tests/invalid/err_preassembler.as` | PRE-11 … PRE-16 |
| `tests/invalid/err_symbols.as` | SYM-13 … SYM-25, DIR-28 … DIR-31 |
| `tests/invalid/err_instructions.as` | INS-03 … INS-39, DIR-07 … DIR-35 |
| `tests/invalid/err_secondpass.as` | INS-31, INS-32, INS-38 — errors that only pass two can find |
| `tests/expected/spec_example.{ob,ent,ext}` | Transcribed from booklet pages 63–64 and bit-verified |
| `tests/run_tests.sh` | Builds, runs every case, diffs, checks no output on error, valgrind mode |

---

## 5. Blockers found while writing this plan

The pipeline is wired (`main.c` loops over `argv` calling
`main_macro_expand → first_pass → second_pass`), but `second_pass.c` cannot yet produce a
correct object file. These must be fixed before the suite can run end to end, in this order.

**Measured baseline.** Running `./assembler tests/valid/spec_example` today gives:

- `.am` is byte-identical to the `.as` — **PRE-01 passes**.
- The first instruction encodes to `40 48 65 00`, which is exactly the booklet's expected
  value — **R-type `add` encoding and the little-endian hex writer are correct**.
- The header line is `     152 17` instead of `52 17`.
- That same first line then repeats forever; the run had to be killed after writing 400 KB
  into `spec_example.ent`.

So the front half of the assembler is in good shape and the damage is concentrated in
`second_pass()`. Run this under a file-size limit until item 1 is fixed:
`( ulimit -f 400; ./assembler tests/valid/spec_example )`.

### Must fix before anything can run

1. **Infinite loop in `second_pass()`.** The code-image loop at `src/second_pass.c:29` is
   `while (code != NULL)` and never advances `code = code->next`. Any source file with at
   least one instruction hangs and writes output until the disk fills. Fix this first, before
   running `tests/run_tests.sh` at all.

2. **No `.ob` file is ever produced.** `second_pass()` opens `filepath + ".ent"` at line 16 and
   writes the object image into it; `write_entries()` then reopens `.ent` with `"w"` and
   truncates it. The string `".ob"` does not appear anywhere in the codebase.

### Wrong output format

3. **`.ob` header prints raw `ICF`.** Line 25 writes `"     %d %d"` with `*icf`. The spec wants
   `ICF - 100` and `DCF`, separated by a single space and with no leading padding. For
   `spec_example` the first line must be `52 17`, not `     152 17`.

4. **Data section addresses are off.** `current_counter += 4` happens *before* the first data
   address is printed and continues from the last instruction's IC, so the data starts one
   4-byte step past where it should. It must start at `ICF` — 152 in `spec_example`. When the
   code image is empty, `current_counter` is read uninitialised.

### Wrong encoding (each verified against the booklet's own expected bytes)

5. **J-type address field is shifted by 24.** `create_j_code` returns `... | (address << 24)`,
   but the address occupies bits 0–24, so it must not be shifted. `jmp NEXT` with `NEXT = 116`
   currently encodes as `00 00 00 7C` instead of `74 00 00 78`.

6. **`jmp $reg` is clobbered.** `create_j_code` correctly sets `reg = 1, address = 4` for
   `jmp $4`, and then the unconditional `if (opcode != 63)` overwrites `address` with
   `get_symbol_value("$4")`, which is `-1`. The register form has to skip the symbol lookup.

7. **`move` / `mvhi` / `mvlo` read `rd` from `arg3`, which is `NULL`.** `parse_register(NULL)`
   returns `-1`, so the `rd` field is filled with ones: `move $20,$4` encodes as `40 F8 80 06`
   instead of `40 20 80 06`. The correct mapping is `rs = arg1`, `rd = arg2`, `rt = 0`.

8. **Conditional branch distance uses `atoi` on a label name.** In `create_i_code`,
   `imm = (short)atoi(op.arg3)` yields 0 for `bne $31,$9,LOOP`, so every branch encodes a
   distance of `-current_ic`. The target address has to come from the symbol table, which
   `create_i_code` is not currently given.

9. **Undefined symbols encode as `-1` with no diagnostic.** `get_symbol_value` returns `-1` on a
   miss and no caller checks it, so `jmp NOSUCH` silently produces a wrong object file instead
   of an error. This is exactly what `tests/invalid/err_secondpass.as` targets.

10. **A conditional branch to an external symbol is not rejected.** The booklet forbids it, and
    `write_externals` would happily add it to `.ext` because it scans `arg1..arg3` of every
    instruction type rather than only J-type instructions.

### Hygiene

11. **Three leaks per file in `second_pass.c`** — the `add_file_extention` results in
    `second_pass`, `write_entries` and `write_externals` are never freed (MEM-01).

12. **Two C90 warnings remain**: `bool` at `src/macro_expand.c:24` and a `//` comment at
    `src/macro_expand.c:83`. Everything else compiles clean under `-Wall -ansi -pedantic`.

13. **Macro body accumulation in `macro_expand.c`** reallocs without room for the terminating
    NUL and does not reassign the `realloc` result on the `strcat` path — a latent heap
    overflow that valgrind will catch (MEM-01, MEM-05).

14. **`.entry` / `.extern` operands are not trimmed** by `parse_line`, so the symbol name may
    carry a trailing newline and never match an existing symbol. SYM-08 catches this.

15. **`test_files/test1.as` targets a different ISA** (`mov`, `prn`, `stop`, `@r3`, `.string`,
    `.data`) and `golden_files/test1.as` closes its macro with `endmcro` instead of `mcroend`.
    Both are stale leftovers; delete them or fold them into `tests/invalid/`.
