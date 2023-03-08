#!/bin/sh
dd if="$1" of="$1".bin count=22528 bs=1
dd if="$1".bin of="$1_part1".bin bs=16K count=1
dd if="$1".bin of="$1_part2".bin bs=16K skip=1
./bin2bank.elf "$1_part1".bin "$2" "$1_part2".bin "$3"
