#!/bin/sh

set -e

g++ ./code/cpp/WindowManager.cpp -o ./build/main -Wall -Wextra -std=c++17 -lX11

XEPHYR=$(command -v Xephyr) # Absolute path of Xephyr's bin
xinit ./run/xinitrc -- \
    "$XEPHYR" \
        :100 \
        -ac \
        -screen 1380x720\
        -host-cursor