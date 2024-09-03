#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

FILENAME=$1
TYPEMAP_FILE="${FILENAME}.typemap"
OUTPUT_FILE="${FILENAME}.tp"

rm -rf $OUTPUT_FILE

if [ ! -f "$TYPEMAP_FILE" ]; then
    echo "Error: ${TYPEMAP_FILE} not found!"
    exit 1
fi

> "$OUTPUT_FILE"

declare -A seen_symbols

# Temporary file for storing data
RODATA_DUMP=$(mktemp)

# Dump the .data.rel.ro section in ASCII
objdump -s -j .data.rel.ro "$FILENAME" > "$RODATA_DUMP" 2>/dev/null 

# Format rodata to concatenate columns appropriately
awk '{print $1, $2 $3, $4 $5, $6 $7}' $RODATA_DUMP | sed '/^$/d' > "$RODATA_DUMP.tmp"
mv "$RODATA_DUMP.tmp" "$RODATA_DUMP"

# Function to convert address to little-endian format for search
convert_to_little_endian() {
    local address="$1"
    # Discard leading zeros and format to 8 hex digits if necessary
    address=$(printf "%016x" "$((16#$address))")
    # Format for little-endian search by reversing the order of bytes
    echo "$address" | sed 's/../& /g' | awk '{for (i=NF; i>=1; i--) printf "%s", $i; print ""}'
}

while read -r line; do
    SYMBOL=$(echo "$line" | awk '{print $1}')
    ID=$(echo "$line" | awk '{print $2}')

    if [ -z "${seen_symbols[$line]}" ]; then
        if [[ $SYMBOL == _ZTV* ]]; then
		BASE_SYMBOL="${SYMBOL:4}"
		BASE_SYMBOL="${BASE_SYMBOL%%.*}"

		OFFSET=$(nm -a "$FILENAME" | grep _ZTI | grep "$BASE_SYMBOL" | awk '{print $1}') 

		# Add offset to reach the beginning of vfptr

        else
		OFFSET=$(nm -a "$FILENAME" | awk -v sym="$SYMBOL" '{if ($NF == sym) print $1}')
	fi

	if [ -n "$OFFSET" ]; then
		echo "$SYMBOL $ID $OFFSET" >> "$OUTPUT_FILE"
		OFFSET=""
	fi

        seen_symbols[$line]=1
    fi
done < <(sort "$TYPEMAP_FILE" | uniq)

rm "$RODATA_DUMP"

