#!/bin/bash

set -xe

CFLAGS="-Wall -Werror -Wextra -pedantic -pedantic-errors `pkg-config --cflags raylib`"
LIBS="`pkg-config --libs raylib` -lm"

mkdir -p ./build/
gcc -o ./build/main main.c entity.c $CFLAGS $LIBS
