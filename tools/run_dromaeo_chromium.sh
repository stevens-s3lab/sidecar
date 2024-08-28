#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
DROMAEO="${SCRIPT_DIR}/run_dromaeo.sh"
ROOT_DIR="$(cd -- "${SCRIPT_DIR}/.." &> /dev/null && pwd)"
RES_DIR=${ROOT_DIR}/sidecar-results
RAW_DIR=${RES_DIR}/raw
CUR_DIR=$(ls -1 "${RAW_DIR}" | grep -E '^Run[0-9]{3}$' | sort | tail -n 1)
DROMAEO_OUT=${RAW_DIR}/${CUR_DIR}/dromaeo.csv

# Define the modes
modes=("lto" "cfi" "fineibt" "sidecfi" "safestack" "sidestack" "asan" "sideasan")

# Run the Dromaeo benchmark
${DROMAEO}

# Initialize variables for geomean calculations
declare -A lto_sums cfi_sums sidecfi_sums
geomean_cfi_list=()
geomean_sidecfi_list=()

# Parse Dromaeo CSV output and calculate sums
while IFS=, read -r name stories avg displayLabel; do
    if [[ "$name" != "name" ]]; then  # Skip the header
        refined_name="${name}+${stories}"

        if [[ "$displayLabel" == "lto" ]]; then
            lto_sums[$refined_name]+="$avg "
        elif [[ "$displayLabel" == "cfi" ]]; then
            cfi_sums[$refined_name]+="$avg "
        elif [[ "$displayLabel" == "sidecfi" ]]; then
            sidecfi_sums[$refined_name]+="$avg "
        fi
    fi
done < <(tail -n +2 "${DROMAEO_OUT}")

# Initialize variables to calculate totals and geometric mean
total_lto_sum=0
total_cfi_sum=0
total_sidecfi_sum=0

# Calculate the average for each name+suffix and use them to calculate percentages and geometric means
for name in "${!lto_sums[@]}"; do
    lto_avg=$(echo "${lto_sums[$name]}" | awk '{sum=0; for (i=1; i<=NF; i++) sum+=$i; print sum/NF}')
    cfi_avg=$(echo "${cfi_sums[$name]}" | awk '{sum=0; for (i=1; i<=NF; i++) sum+=$i; print sum/NF}')
    sidecfi_avg=$(echo "${sidecfi_sums[$name]}" | awk '{sum=0; for (i=1; i<=NF; i++) sum+=$i; print sum/NF}')

    # Avoid division by zero
    if [[ -n "$cfi_avg" && "$cfi_avg" != "0" && -n "$sidecfi_avg" && "$sidecfi_avg" != "0" ]]; then
        if [[ "$lto_avg" != "0" ]]; then
            cfi_percentage=$(awk "BEGIN {print ($lto_avg / $cfi_avg) * 100}")
            sidecfi_percentage=$(awk "BEGIN {print ($lto_avg / $sidecfi_avg) * 100}")

            # Update the totals
            total_lto_sum=$(awk "BEGIN {print $total_lto_sum + $lto_avg}")
            total_cfi_sum=$(awk "BEGIN {print $total_cfi_sum + $cfi_avg}")
            total_sidecfi_sum=$(awk "BEGIN {print $total_sidecfi_sum + $sidecfi_avg}")

            # Add to geomean lists if the percentage is valid
            if (( $(echo "$cfi_percentage < 100" | bc -l) )); then
                geomean_cfi_list+=($cfi_percentage)
            fi
            if (( $(echo "$sidecfi_percentage < 100" | bc -l) )); then
                geomean_sidecfi_list+=($sidecfi_percentage)
            fi
        fi
    fi
done

# Calculate total percentages
if [[ "$total_cfi_sum" != "0" ]]

