set -e

#  Linux
CC=cc
CFLAGS=-g
CLIBS="-lGL -lX11 -lXrandr -lm -I./include"
OUTPUT=game.elf

#  MS Windows
# CC=x86_64-w64-mingw32-gcc
# CFLAGS=
# CLIBS="-lopengl32 -lgdi32 -lm -I./include"
# OUTPUT=game.exe

ENGINE_SRC=engine/*.c
GAME_SRC=game/*.c

set -x

$CC $CFLAGS $ENGINE_SRC $GAME_SRC -o $OUTPUT $CLIBS
