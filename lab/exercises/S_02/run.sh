#!/usr/bin/env bash

set -e

if [ -f "$1" ]; then
    rm "$1";
fi

make
./program "$1" "$2"
cat "$1"
