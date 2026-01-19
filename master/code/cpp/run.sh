#!/bin/sh

set -e

g++ WindowManager.cpp -o main -Wall -Wextra -std=c++17 -lX11

XEPHYR=$(command -v Xephyr) # Absolute path of Xephyr's bin
xinit ./xinitrc -- \
    "$XEPHYR" \
        :100 \
        -ac \
        -screen 1380x720\
        -host-cursor