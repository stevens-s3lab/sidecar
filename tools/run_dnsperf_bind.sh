#!/bin/bash

# Define the modes
modes=("lto" "cfi" "fineibt" "sidecfi" "scs" "sidestack" "asan" "sideasan")

# Loop through each mode and print the mode and throughput in CSV format
for mode in "${modes[@]}"; do
    # Generate a random throughput value between 0 and 100 using /dev/urandom
    throughput=$(od -An -N2 -i /dev/urandom | tr -d ' \n' | awk -v min=0 -v max=100 '{print int(min + ($1 % (max-min+1)))}')
    echo "$mode,$throughput"
done

