#!/bin/bash

# Set up the directories
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
ROOT_DIR="$(cd -- "${SCRIPT_DIR}/.." &> /dev/null && pwd)"
RES_DIR=${ROOT_DIR}/sidecar-results
RAW_DIR=${RES_DIR}/raw
CUR_DIR=$(ls -1 "${RAW_DIR}" | grep -E '^Run[0-9]{3}$' | sort | tail -n 1)
WRK_DIR=${ROOT_DIR}/install/tools

# Only one file size to test
FILE_SIZE="100KB"

# Number of iterations
iterations=3
duration=1

# Variables for throughput calculation
total_throughput=0

# Run the benchmark and calculate the average throughput
for i in $(seq 1 $iterations); do
    # Run the workload
    throughput=$(taskset -c 3-6 $WRK_DIR/wrk -t2 -c4 -d${duration}s --latency https://127.0.0.1:443/$FILE_SIZE | grep 'Requests/sec' | awk '{print $2}')
    total_throughput=$(echo "$total_throughput + $throughput" | bc)
done

# Calculate the average throughput
average_throughput=$(echo "scale=2; $total_throughput / $iterations" | bc)

# Output the results
echo $average_throughput

