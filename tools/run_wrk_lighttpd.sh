#!/bin/bash

# Define the modes
modes=("lto" "cfi" "fineibt" "sidecfi" "scs" "sidestack" "asan" "sideasan")

# Initialize variable to save lto throughput
lto_throughput=0

# Set this variable to "wrk" to use run_wrk.sh, or "rand" to use random values
#throughput_source="rand"
throughput_source="wrk"  # or "rand"

# Function to get the throughput
get_throughput() {
    if [ "$throughput_source" == "wrk" ]; then
        # Capture the output of run_wrk.sh
        avg_throughput=$(bash "${SCRIPT_DIR}/run_wrk.sh" | tail -n 1)
    elif [ "$throughput_source" == "rand" ]; then
        # Generate a random throughput value between 300 and 499
        avg_throughput=$(od -An -N2 -i /dev/urandom | tr -d ' \n' | awk -v min=300 -v max=499 '{print int(min + ($1 % (max-min+1)))}')
    fi
    echo "$avg_throughput"
}

# Loop through each mode and print the mode and throughput in CSV format
for mode in "${modes[@]}"; do
    if [ "$mode" == "lto" ]; then
        # Run the workload for lto mode and save its throughput
        lto_throughput=$(get_throughput)
        echo "$mode,$lto_throughput"
    else
        export CFI_MODE=$mode

        if [ "$mode" == "fineibt" ]; then
            throughput=0
        else
            # Run the workload for the current mode and get its throughput
            current_throughput=$(get_throughput)
            # Calculate throughput as (current throughput / lto_throughput) * 100
            throughput=$(awk -v ct="$current_throughput" -v lt="$lto_throughput" 'BEGIN {print int(ct / lt * 100)}')
        fi

        echo "$mode,$throughput"
    fi
done

