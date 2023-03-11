#!/bin/sh
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 inputfile bank_number_1 bank_number_2 size_of_file_to_crop"
    exit 1
fi

if [ "$#" -ne 4 ]; then
	echo ""
    echo "WARNING : size_of_file_to_crop not specified"
    echo "Assuming you want the full image size instead"
	echo ""
fi

BANKSIZE="${4:-27143}"
echo $BANKSIZE

filename="$1"
new_filename="$(echo "$filename" | sed 's/\(.*\)\./\1_/g')"

dd if="$1" of="$1".bin.temp count="${BANKSIZE}" bs=1
dd if="$1".bin.temp of="$1".bin skip=7 bs=1

dd if="$1".bin of="$1_part1".bin bs=16K count=1
dd if="$1".bin of="$1_part2".bin bs=16K skip=1

./pal2c.elf "$1" $new_filename"_pal.c"
mkdir -p output
./bin2c_bank.elf "$1_part1".bin "$2" "$1_part2".bin "$3"
rm "$1_part1".bin "$1_part2".bin "$1".bin

echo $new_filename
mv $new_filename"_part1_bin".c output/
mv $new_filename"_part2_bin".c output/
rm *.temp
rm *.bin
