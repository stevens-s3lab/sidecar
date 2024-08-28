#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
ROOT_DIR="$(cd -- "${SCRIPT_DIR}/.." &> /dev/null && pwd)"
QUERY_FILE="${ROOT_DIR}/sidecar-benchmarks/bind/bind9/one.txt"
DNS_SERVER="127.0.0.1"

# Define the modes
modes=("lto" "cfi" "fineibt" "sidecfi" "safestack" "sidestack" "asan" "sideasan")

# Initialize variable to save lto throughput
lto_throughput=0
throughput=0

# Set this variable to "wrk" to use run_wrk.sh, or "rand" to use random values
#throughput_source="rand"
throughput_source="wrk"  # or "rand"

duration=30

# Function to get the throughput
get_throughput() {
    MODE="$1"
    
    if [ "$throughput_source" == "wrk" ]; then
        # Start the server
        taskset -c 0 ${SCRIPT_DIR}/build_bind.sh ${MODE} run &> /dev/null &
        server_pid=$!

        # Give the server some time to start properly
        sleep 5

        # Capture the output of run_wrk.sh
        avg_throughput=$(taskset -c 3 dnsperf -s $DNS_SERVER -d $QUERY_FILE -l $duration -T 1 | grep "Queries per second" | awk '{print $4}')

        # Stop the server
	pkill -f sbin/named
	wait $server_pid 2>/dev/null

        # Give some time for the server to stop cleanly
        sleep 2
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
        lto_throughput=$(get_throughput "$mode")
    else
        if [ "$mode" == "fineibt" ]; then
            throughput=0
        else
            # Run the workload for the current mode and get its throughput
	    current_throughput=$(get_throughput "$mode")
            # Calculate throughput as (current throughput / lto_throughput) * 100
	    throughput=$(awk -v ct="$current_throughput" -v lt="$lto_throughput" 'BEGIN {perf=int(ct / lt * 100); if (perf > 100) perf = 100; print perf}')
        fi

        # if through for safestack is 0, then set it to -1
        if [ "$mode" == "safestack" ] && [ "$throughput" == "0" ]; then
            throughput=-1
        fi

        echo "$mode,$throughput"
    fi
done

