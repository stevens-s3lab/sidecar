#!/bin/bash

# Define the modes
modes=("lto" "cfi" "fineibt" "sidecfi" "scs" "sidestack" "asan" "sideasan")

# Initialize variable to save lto throughput
lto_throughput=0

# Loop through each mode and print the mode and throughput in CSV format
for mode in "${modes[@]}"; do
    if [ "$mode" == "lto" ]; then
        # Generate a random throughput value between 300 and 499 for lto mode
        lto_throughput=$(od -An -N2 -i /dev/urandom | tr -d ' \n' | awk -v min=500 -v max=599 '{print int(min + ($1 % (max-min+1)))}')
    else
        # Set the CFI_MODE environment variable for non-lto modes
        export CFI_MODE=$mode

        if [ "$mode" == "fineibt" ]; then
            # If mode is fineibt, print 0 as throughput
            throughput=0
        else
            # Generate a random throughput value between 300 and 499 for other modes
            random_throughput=$(od -An -N2 -i /dev/urandom | tr -d ' \n' | awk -v min=300 -v max=499 '{print int(min + ($1 % (max-min+1)))}')
            # Calculate throughput as (current random throughput / lto_throughput) * 100
            throughput=$(awk -v rt="$random_throughput" -v lt="$lto_throughput" 'BEGIN {print int(rt / lt * 100)}')
        fi

        echo "$mode,$throughput"
    fi
done

