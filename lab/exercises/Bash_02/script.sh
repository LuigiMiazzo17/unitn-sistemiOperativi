#!/usr/bin/env bash

# Esercizio 2 Bash Lab Slides 1
# Scrivere uno script che mostri il contenuto della cartella corrente in ordine
# inverso rispetto all’output generato da “ls” (che si può usare ma senza
# opzioni). Per semplicità, assumere che tutti i file e le cartelle non abbiano
# spazi nel nome.

output=$(ls)
# lines=$(echo "$output" | wc -l)
#
# while [ "$lines" -gt 0 ]
# do
#     echo "$(echo "$output" | sed "${lines}q;d")"
#     lines=$((lines-1))
# done

echo "$output" | tac | tr '\n' ' '
echo ""
