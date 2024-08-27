import csv
import os
import subprocess
from pathlib import Path

import numpy as np

# Constants for directory structure
BASE_DIR = Path("../sidecar-results")
RAW_DIR = BASE_DIR / "raw"
PARSED_DIR = BASE_DIR / "parsed"
PLOTS_DIR = BASE_DIR / "plots"

# Spec CSV file modes
SPEC_MODES = [
    "spec17.lto.csv",
    "spec17.cfi.csv",
    "spec17.sidecfi.csv",
    "spec17.asan.csv",
    "spec17.sideasan.csv",
    "spec17.scs.csv",
    "spec17.sidestack.csv",
]

RAW_FILES = [
    "spec17.results.csv",
    "httpd.csv",
    "lighttpd.csv",
    "memcached.csv",
    "bind.csv",
    "chromium.csv",
]

PARSED_FILES = {"spec": "spec17.csv", "apps": "apps.csv"}


def setup_directories():
    # Create base directories if they don't exist
    for dir_path in [RAW_DIR, PARSED_DIR, PLOTS_DIR]:
        if not dir_path.exists():
            os.makedirs(dir_path)

    # Determine the next RunXXX directory
    existing_runs = sorted(RAW_DIR.glob("Run*"))
    next_run_num = len(existing_runs)
    next_run_dir = RAW_DIR / f"Run{next_run_num:03d}"

    # Create the next RunXXX directory
    os.makedirs(next_run_dir)

    # Touch all the necessary raw files
    for file_name in RAW_FILES + SPEC_MODES:
        (next_run_dir / file_name).touch()

    return next_run_dir


# Functions for each benchmark
def execute_spec17(file_path):
    # Call the bash script and capture its output
    result = subprocess.run(
        ["bash", "run_spec17.sh"], stdout=subprocess.PIPE, text=True
    )


def execute_httpd(file_path):
    # Call the bash script and capture its output
    result = subprocess.run(
        ["bash", "run_wrk_httpd.sh"], stdout=subprocess.PIPE, text=True
    )

    # Write the output of the bash script to the file
    with open(file_path, "w") as f:
        f.write(result.stdout)


def execute_lighttpd(file_path):
    # Call the bash script and capture its output
    result = subprocess.run(
        ["bash", "run_wrk_lighttpd.sh"], stdout=subprocess.PIPE, text=True
    )

    # Write the output of the bash script to the file
    with open(file_path, "w") as f:
        f.write(result.stdout)


def execute_memcached(file_path):
    # Call the bash script and capture its output
    result = subprocess.run(
        ["bash", "run_memtier_memcached.sh"], stdout=subprocess.PIPE, text=True
    )

    # Write the output of the bash script to the file
    with open(file_path, "w") as f:
        f.write(result.stdout)


def execute_bind(file_path):
    # Call the bash script and capture its output
    result = subprocess.run(
        ["bash", "run_dnsperf_bind.sh"], stdout=subprocess.PIPE, text=True
    )

    # Write the output of the bash script to the file
    with open(file_path, "w") as f:
        f.write(result.stdout)


def execute_chromium(file_path):
    # Call the bash script and capture its output
    result = subprocess.run(
        ["bash", "run_dromaeo_chromium.sh"], stdout=subprocess.PIPE, text=True
    )

    # Write the output of the bash script to the file
    with open(file_path, "w") as f:
        f.write(result.stdout)


def run_placeholder_tasks(run_dir):
    # Call each benchmark's execution function
    execute_spec17(run_dir / "spec17.results.csv")
    execute_httpd(run_dir / "httpd.csv")
    execute_lighttpd(run_dir / "lighttpd.csv")
    execute_memcached(run_dir / "memcached.csv")
    execute_bind(run_dir / "bind.csv")
    execute_chromium(run_dir / "chromium.csv")


def parse_spec_mode(file_path):
    spec_data = {}
    with open(file_path, "r") as f:
        reader = csv.reader(f)
        for row in reader:
            benchmark = row[0].split(".")[1]  # Extract the benchmark name after the dot

            # Check that benchmark is not "exchange2_s"
            if benchmark == "exchange2_s":
                continue

            if benchmark not in spec_data:
                spec_data[benchmark] = []
            spec_data[benchmark].append(float(row[2]))  # Est. Base Ratio (3rd index)

    avg_stddev_data = {}
    for benchmark, results in spec_data.items():
        avg = round(np.mean(results), 2)  # Round average to two decimal places
        stddev = round(
            np.std(results), 2
        )  # Round standard deviation to two decimal places
        avg_stddev_data[benchmark] = (avg, stddev)

    return avg_stddev_data


def calculate_performance_over_lto(lto_data, mode_data):
    performance_data = {}
    for benchmark, (lto_avg, lto_std) in lto_data.items():
        if benchmark in mode_data:
            mode_avg, mode_std = mode_data[benchmark]
            performance = (lto_avg / mode_avg) * 100
            performance_data[benchmark] = (round(performance, 2), mode_std)
        else:
            performance_data[benchmark] = (-1.0, 0.0)
    return performance_data


def calculate_geomean(values):
    filtered_values = [v for v in values if 0 < v <= 100]
    if not filtered_values:
        return 0.0
    return np.exp(np.mean(np.log(filtered_values)))


def parse_spec_results(run_dir):
    mode_data = {}

    for mode in SPEC_MODES:
        file_path = run_dir / mode
        mode_name = mode.split(".")[1]  # Extracting the mode name from the filename
        mode_data[mode_name] = parse_spec_mode(file_path)

    lto_data = mode_data.pop("lto")  # LTO is the baseline

    final_results = {}

    for mode, data in mode_data.items():
        final_results[mode] = calculate_performance_over_lto(lto_data, data)

        # if mode == "scs" exclude the ones that didn't run
        if mode == "scs":
            final_results[mode]["perlbench_s"] = (-1.0, 0.0)
            final_results[mode]["omnetpp_s"] = (-1.0, 0.0)
            final_results[mode]["leela_s"] = (-1.0, 0.0)

    fineibt_results = {
        "perlbench_s": (95.70, 0.40),
        "gcc_s": (98.39, 0.54),
        "mcf_s": (98.79, 0.35),
        "omnetpp_s": (69.30, 0.09),
        "xalancbmk_s": (99.17, 0.00),
        "x264_s": (95.48, 0.00),
        "deepsjeng_s": (99.56, 0.00),
        "leela_s": (96.66, 0.00),
        "xz_s": (100.00, 0.00),
    }

    final_results["fineibt"] = fineibt_results

    return final_results


def save_parsed_spec_results(final_results):
    with open(PARSED_DIR / PARSED_FILES["spec"], "w", newline="") as f:
        writer = csv.writer(f)

        # Define the correct order of benchmarks
        benchmark_order = [
            "perlbench_s",
            "gcc_s",
            "mcf_s",
            "omnetpp_s",
            "xalancbmk_s",
            "x264_s",
            "deepsjeng_s",
            "leela_s",
            "xz_s",
        ]

        # Define the order of modes (excluding LTO)
        mode_order = [
            "cfi",
            "fineibt",
            "sidecfi",
            "scs",
            "sidestack",
            "asan",
            "sideasan",
        ]

        # Write rows for each benchmark in the specified order
        for benchmark in benchmark_order:
            if (
                benchmark in final_results["cfi"]
            ):  # Ensure the benchmark exists in results
                row = [benchmark]
                for (
                    mode
                ) in mode_order:  # Iterate through the modes in the specified order
                    if final_results[mode][benchmark][0] is not None:
                        row.extend(final_results[mode][benchmark])
                    else:
                        row.extend(["-1.0", "0.0"])
                writer.writerow(row)

        # Calculate *geomean excluding perlbench_s, omnetpp_s, and leela_s for scs and sidestack
        geomean_values_star = {}
        for mode in mode_order:
            if mode in ["scs", "sidestack"]:
                mode_geomeans = []
                for benchmark in benchmark_order:
                    if benchmark not in ["perlbench_s", "omnetpp_s", "leela_s"]:
                        if final_results[mode][benchmark][0] is not None:
                            mode_geomeans.append(final_results[mode][benchmark][0])
                geomean_values_star[mode] = calculate_geomean(mode_geomeans)
            else:
                geomean_values_star[mode] = 0.0

        # Write *geomean row
        geomean_row_star = ["*geomean"]
        for mode in mode_order:
            geomean_row_star.append(f"{geomean_values_star[mode]:.2f}")
            geomean_row_star.append("0.00")  # Std dev is 0 for geomean
        writer.writerow(geomean_row_star)

        # Calculate geomean for all modes except scs
        geomean_values = {}
        for mode in mode_order:
            if mode == "scs":
                geomean_values[mode] = 0.0
            else:
                mode_geomeans = []
                for benchmark in benchmark_order:
                    if final_results[mode][benchmark][0] is not None:
                        mode_geomeans.append(final_results[mode][benchmark][0])
                geomean_values[mode] = calculate_geomean(mode_geomeans)

        # Write geomean row
        geomean_row = ["geomean"]
        for mode in mode_order:
            geomean_row.append(f"{geomean_values[mode]:.2f}")
            geomean_row.append("0.00")  # Std dev is 0 for geomean
        writer.writerow(geomean_row)


def parse_apps(run_dir):
    apps_data = {}
    apps_data["httpd"] = parse_httpd(run_dir / "httpd.csv")
    apps_data["lighttpd"] = parse_lighttpd(run_dir / "lighttpd.csv")
    apps_data["memcached"] = parse_memcached(run_dir / "memcached.csv")
    apps_data["bind"] = parse_bind(run_dir / "bind.csv")
    apps_data["chromium"] = parse_chromium(run_dir / "chromium.csv")

    return apps_data


def parse_httpd(httpd_file):
    values = []
    with open(httpd_file, "r") as f:
        for line in f:
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                values.append(float(value))
    return values


def parse_lighttpd(lighttpd_file):
    values = []
    with open(lighttpd_file, "r") as f:
        for line in f:
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                values.append(float(value))
    return values


def parse_memcached(memcached_file):
    values = []
    with open(memcached_file, "r") as f:
        for line in f:
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                values.append(float(value))
    return values


def parse_bind(bind_file):
    values = []
    with open(bind_file, "r") as f:
        for line in f:
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                values.append(float(value))
    return values


def parse_chromium(chromium_file):
    values = []
    with open(chromium_file, "r") as f:
        for line in f:
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                values.append(float(value))
    return values


def parse_results(run_dir):
    # Ensure the parsed directory exists
    if not PARSED_DIR.exists():
        os.makedirs(PARSED_DIR)

    # Parse the spec results
    final_results = parse_spec_results(run_dir)

    # Save the parsed spec results
    save_parsed_spec_results(final_results)

    # Parse the apps CSVs and calculate the geomean
    apps_data = parse_apps(run_dir)

    with open(PARSED_DIR / PARSED_FILES["apps"], "w") as apps_out:
        all_values = []
        for app, values in apps_data.items():
            values_str = [str(v) for v in values]
            apps_out.write(f'"{app}",' + ", ".join(values_str) + "\n")
            all_values.append(values)

        # Initialize a list to store valid geomean values
        valid_geomean_values = []

        # Iterate over each column (0 to 6 for the 7 values)
        for i in range(7):
            column_values = [values[i] for values in all_values]

            if all(value != 0 for value in column_values):
                valid_geomean_values.append(calculate_geomean(column_values))
            else:
                valid_geomean_values.append(None)

        geomean_str = '"geomean",'
        geomean_str += (
            ", ".join(
                [f"{v:.2f}" if v is not None else "0" for v in valid_geomean_values]
            )
            + "\n"
        )
        apps_out.write(geomean_str)


def main():
    print("Setting up directories...")
    run_dir = setup_directories()

    print(f"Running tasks for {run_dir}...")
    run_placeholder_tasks(run_dir)

    print("Parsing results...")
    parse_results(run_dir)

    print("Done.")


if __name__ == "__main__":
    main()
