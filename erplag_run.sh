#!/bin/bash

set -e

if [ -z "$1" ]; then
    echo "No erplag source file specified." 1>&2
    exit 2
else
    ./compiler -c $1 -o out.asm
    nasm -felf64 out.asm -o out.o
    gcc out.o -o ./a.out
    ./a.out
fi
