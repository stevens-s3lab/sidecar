#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
ROOT_DIR="$(cd -- "${SCRIPT_DIR}/.." &> /dev/null && pwd)"
RES_DIR=${ROOT_DIR}/sidecar-results
RAW_DIR=${RES_DIR}/raw
CUR_DIR=$(ls -1 "${RAW_DIR}" | grep -E '^Run[0-9]{3}$' | sort | tail -n 1)

# TODO: move this in sidecar
CHROMIUM_DIR="/home/kleftog/repos_other/chromium/src"
DEPOT_DIR="/home/kleftog/repos_other/depot_tools"

RUN_BENCH_DIR=${CHROMIUM_DIR}/tools/perf/run_benchmark
CHROMIUM_OUT=${CHROMIUM_DIR}/tools/perf/results.csv
OUT_PATH=${RAW_DIR}/${CUR_DIR}/dromaeo.csv


labels=("cfi" "sidecfi" "lto")
paths=("_cfi" "_sidecfi" "_clean")
benchmarks=("dromaeo")
story="dom-traverse"

iterations=1

export PATH=$DEPOT_DIR:$PATH

# Loop through each benchmark, label, and path, running the benchmark
for benchmark in "${benchmarks[@]}"; do
    for i in "${!labels[@]}"; do
        label=${labels[$i]}
        path_suffix=${paths[$i]}
	BROWSER_PATH=${CHROMIUM_DIR}/out_gn/ra_official${path_suffix}/chrome

        taskset -c 0 xvfb-run -s "-screen 0 1024x768x24" ${RUN_BENCH_DIR} \
            --browser=exact \
            --browser-executable=${BROWSER_PATH} \
            --results-label=${label} --output-format csv \
            --pageset-repeat=${iterations} ${benchmark}  \
	    --extra-browser-args="--no-sandbox --headless" --story-filter=${story} < /dev/null \
	    > /dev/null 2>&1
    done
done

mv ${CHROMIUM_OUT} ${OUT_PATH}
