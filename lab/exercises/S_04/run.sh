#!/usr/bin/env bash

set -e

if [ ! -f "$("$1")" ]; then
    echo "No file specified";
    exit 1;
fi

TOWHAT="STDOUT"
while IFS= read -r line; do
    if [ "$TOWHAT" = "STDOUT" ]; then
        echo "$line";
        TOWHAT="STDERR";
    else
        echo "$line" 1>&2;
        TOWHAT="STDOUT";
    fi
done < "$1"
