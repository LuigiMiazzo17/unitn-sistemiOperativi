#!/usr/bin/env bash

if [ ! $# = 10 ]; then
    echo "Numero di argomenti invalido" 1>&2
    exit 1
fi


echo $1 > "./.tmp"
echo $2 >> "./.tmp"
echo $3 >> "./.tmp"
echo $4 >> "./.tmp"
echo $5 >> "./.tmp"
echo $6 >> "./.tmp"
echo $7 >> "./.tmp"
echo $8 >> "./.tmp"
echo $9 >> "./.tmp"
sort < "./.tmp"
rm "./.tmp"