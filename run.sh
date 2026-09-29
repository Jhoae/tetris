#!/bin/sh
set -eu

TETRIS_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

if [ ! -t 0 ] || [ ! -t 1 ]; then
  printf 'Run this program in an interactive terminal.\n' >&2
  exit 1
fi

if [ ! -x "$TETRIS_ROOT/bin/tetris" ]; then
  "$TETRIS_ROOT/build.sh"
fi

mkdir -p "$TETRIS_ROOT/runtime"
if [ ! -f "$TETRIS_ROOT/runtime/rank.txt" ]; then
  printf '0\n' > "$TETRIS_ROOT/runtime/rank.txt"
fi
cd "$TETRIS_ROOT/runtime"
export TERM="${TERM:-xterm-256color}"

TETRIS_TTY_STATE=$(stty -g)
restore_tty() {
  stty "$TETRIS_TTY_STATE" 2>/dev/null || :
}
trap restore_tty EXIT
trap 'exit 129' HUP
trap 'exit 130' INT
trap 'exit 143' TERM

stty -icanon min 1 time 0
"$TETRIS_ROOT/bin/tetris"
