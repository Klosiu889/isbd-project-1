#!/usr/bin/env bash

KB=1024
MB=$((KB * 1024))
GB=$((MB * 1024))

MULTIPLIER=1

if [ $# -lt 2 ]; then
    echo "Usage $0: <size in KB> <filename>"
    exit 1
fi

case $3 in
    k) MULTIPLIER=$KB ;;
    m) MULTIPLIER=$MB ;;
    g) MULTIPLIER=$GB ;;
    *) ;;
esac

SIZE=$1
FILE_NAME=$2

head -c $((SIZE * MULTIPLIER)) /dev/urandom > $2
