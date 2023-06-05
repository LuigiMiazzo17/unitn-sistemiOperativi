#!/usr/bin/env bash

# Esercizio 1 Bash Lab Slides 1
# Scrivere uno script che dato un qualunque numero di argomenti li restituisca
# in output in ordine inverso.

# get the number of arguments
i=$#

# iterate over arguments
while [ "$i" -gt 0 ]
do
    echo ${!i}
    i=$((i-1))
done
