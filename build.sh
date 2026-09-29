#!/bin/sh
set -eu

TETRIS_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
TETRIS_CC=${CC:-cc}

mkdir -p "$TETRIS_ROOT/bin"
"$TETRIS_CC" -std=gnu11 "$TETRIS_ROOT/src/tetris.c" -lncurses -o "$TETRIS_ROOT/bin/tetris"
printf 'Built bin/tetris\n'
