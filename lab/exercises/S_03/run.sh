#!/usr/bin/env bash

set -e

if [ -f "$1" ]; then
    rm "$1";
fi

if [ "$1" = "" ]; then
    echo "No input file specified";
    exit 1;
fi

if [ "$2" = "" ]; then
    echo "No output file specified";
    exit 1;
fi

make
./app "$1" "$2"
