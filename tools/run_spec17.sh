#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

# Define the modes
modes=("lto" "cfi" "sidecfi" "safestack" "sidestack" "asan" "sideasan")

# Choose size of inputs
#size="ref"
size="test"
laps=1

# Define the base directory where the Runxxx directories are located
base_dir="$SCRIPT_DIR/../sidecar-results/raw"

# Find the last Runxxx directory
next_run=$(ls -1 "$base_dir" | grep -E '^Run[0-9]{3}$' | sort | tail -n 1)

# Spec2017 directory
spec17_dir="$SCRIPT_DIR/../sidecar-benchmarks/spec2017"

# cd to spec17 directory run ./shrc and then come back
cd "$spec17_dir" || exit
source ./shrc
cd "$SCRIPT_DIR" || exit

# Loop through each mode and print the mode and throughput in CSV format
for mode in "${modes[@]}"; do
    if [ "$mode" == "asan" ]; then
        llvm_path="$SCRIPT_DIR/../install/llvm-orig"
    else
        llvm_path="$SCRIPT_DIR/../install/llvm-sidecar"
    fi

    # Run the spec17 benchmark
    taskset -c 0 runcpu --action=run --config=$mode --size=$size --label=$mode \
      --iterations=${laps} --threads=1 --tune=base -define gcc_dir=${llvm_path} --output_format=csv \
      --noreportable speedint

    # Find the latest csv file in spec17 directory under result
    csv_file=$(ls -1t "$spec17_dir/result/"*intspeed*.csv | head -n 1)

    if [ "$size" == "ref" ]; then
	grep "refspeed(ref)" "$csv_file" > "$base_dir/$next_run/spec17.$mode.csv"
    else
	grep "test iteration" "$csv_file" > "$base_dir/$next_run/spec17.$mode.csv"
    fi
done
