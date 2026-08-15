#!/bin/bash
# Regression runner for the Maman 14 assembler.
#
#   ./tests/run_tests.sh            build, run everything, diff against expected
#   ./tests/run_tests.sh -v         also print the full assembler output
#   VALGRIND=1 ./tests/run_tests.sh run every case under valgrind
#
# Valid cases are compared against tests/expected/. Invalid cases are checked for
# two things only: that the assembler reports at least one error, and that it
# leaves no .ob / .ent / .ext behind.

set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/assembler"
VALID_DIR="$ROOT/tests/valid"
INVALID_DIR="$ROOT/tests/invalid"
EXPECTED_DIR="$ROOT/tests/expected"
OUT_DIR="$ROOT/tests/actual"

VERBOSE=0
[ "${1:-}" = "-v" ] && VERBOSE=1

RUNNER=""
if [ "${VALGRIND:-0}" = "1" ]; then
    RUNNER="valgrind --leak-check=full --error-exitcode=99 -q"
fi

# A runaway loop in the assembler would otherwise fill the disk with output, so
# every invocation is capped. gtimeout is the coreutils name on macOS.
TIMEOUT=""
if command -v timeout > /dev/null 2>&1; then
    TIMEOUT="timeout 10"
elif command -v gtimeout > /dev/null 2>&1; then
    TIMEOUT="gtimeout 10"
fi
RUNNER="$TIMEOUT $RUNNER"

pass=0
fail=0

red()   { printf '\033[31m%s\033[0m\n' "$1"; }
green() { printf '\033[32m%s\033[0m\n' "$1"; }

ok()  { green "  PASS  $1"; pass=$((pass + 1)); }
bad() { red   "  FAIL  $1"; fail=$((fail + 1)); }

echo "== building =="
make -C "$ROOT" clean >/dev/null 2>&1
if ! make -C "$ROOT" 2>&1 | tee "$ROOT/tests/build.log" | grep -qv .; then
    :
fi
if [ ! -x "$BIN" ]; then
    red "build failed, see tests/build.log"
    exit 1
fi
if grep -qiE 'warning:|error:' "$ROOT/tests/build.log"; then
    bad "build is clean (-Wall -ansi -pedantic)"
    grep -iE 'warning:|error:' "$ROOT/tests/build.log" | head -20
else
    ok "build is clean (-Wall -ansi -pedantic)"
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

echo
echo "== valid inputs =="
for src in "$VALID_DIR"/*.as; do
    name="$(basename "$src" .as)"
    cp "$src" "$OUT_DIR/$name.as"

    # The program is invoked with the base name; adjust here if you switch to
    # accepting the .as suffix on the command line.
    $RUNNER "$BIN" "$OUT_DIR/$name" > "$OUT_DIR/$name.stdout" 2>&1
    status=$?

    [ "$VERBOSE" = "1" ] && cat "$OUT_DIR/$name.stdout"

    if [ "$status" = "99" ]; then
        bad "$name: valgrind reported a memory problem"
        continue
    fi

    for ext in ob ent ext; do
        want="$EXPECTED_DIR/$name.$ext"
        got="$OUT_DIR/$name.$ext"
        [ -f "$want" ] || continue
        if [ ! -f "$got" ]; then
            bad "$name.$ext was not produced"
        elif diff -u "$want" "$got" > "$OUT_DIR/$name.$ext.diff"; then
            ok "$name.$ext"
        else
            bad "$name.$ext differs"
            head -30 "$OUT_DIR/$name.$ext.diff"
        fi
    done

    if [ -f "$OUT_DIR/$name.am" ]; then
        ok "$name.am was produced"
    else
        bad "$name.am was not produced"
    fi
done

echo
echo "== invalid inputs =="
for src in "$INVALID_DIR"/*.as; do
    name="$(basename "$src" .as)"
    cp "$src" "$OUT_DIR/$name.as"

    $RUNNER "$BIN" "$OUT_DIR/$name" > "$OUT_DIR/$name.stdout" 2>&1
    status=$?

    [ "$VERBOSE" = "1" ] && cat "$OUT_DIR/$name.stdout"

    if [ "$status" = "99" ]; then
        bad "$name: valgrind reported a memory problem"
        continue
    fi

    if [ -s "$OUT_DIR/$name.stdout" ]; then
        ok "$name reported errors ($(wc -l < "$OUT_DIR/$name.stdout") lines)"
    else
        bad "$name produced no error output at all"
    fi

    leftovers=""
    for ext in ob ent ext; do
        [ -f "$OUT_DIR/$name.$ext" ] && leftovers="$leftovers .$ext"
    done
    if [ -z "$leftovers" ]; then
        ok "$name produced no output files"
    else
        bad "$name wrongly produced:$leftovers"
    fi
done

echo
echo "== multi-file run =="
$RUNNER "$BIN" "$OUT_DIR/spec_example" "$OUT_DIR/err_symbols" "$OUT_DIR/all_ops" \
    > "$OUT_DIR/multi.stdout" 2>&1
if [ -f "$OUT_DIR/spec_example.ob" ] && [ -f "$OUT_DIR/all_ops.ob" ] \
   && [ ! -f "$OUT_DIR/err_symbols.ob" ]; then
    ok "a bad file in the middle does not stop the good ones"
else
    bad "a bad file in the middle broke the run"
fi

echo
echo "== no arguments =="
$RUNNER "$BIN" > "$OUT_DIR/noargs.stdout" 2>&1
if [ -s "$OUT_DIR/noargs.stdout" ]; then
    ok "usage message printed when no files are given"
else
    bad "no usage message when run without arguments"
fi

echo
echo "== missing file =="
$RUNNER "$BIN" "$OUT_DIR/does_not_exist" > "$OUT_DIR/missing.stdout" 2>&1
if [ -s "$OUT_DIR/missing.stdout" ]; then
    ok "missing input file is reported"
else
    bad "missing input file was silently ignored"
fi

echo
echo "==============================="
green "passed: $pass"
[ "$fail" -gt 0 ] && red "failed: $fail"
echo "artifacts in tests/actual/"
[ "$fail" -eq 0 ]
