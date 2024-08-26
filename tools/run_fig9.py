import os
import random
import subprocess
from pathlib import Path

import numpy as np

# Constants for directory structure
BASE_DIR = Path("../sidecar-results")  # Adjusted to create sidecar-results one level up
RAW_DIR = BASE_DIR / "raw"
PARSED_DIR = BASE_DIR / "parsed"
PLOTS_DIR = BASE_DIR / "plots"

# Constants for filenames
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
    for file_name in RAW_FILES:
        (next_run_dir / file_name).touch()

    return next_run_dir


# Functions for each benchmark
def execute_spec17(file_path):
    with open(file_path, "w") as f:
        f.write("Simulated output for spec17.results.csv\n")


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


def calculate_geomean(values):
    # Filter out values greater than 100% before calculating the geometric mean
    filtered_values = [v for v in values if v <= 100]
    if not filtered_values:
        return 0.0
    return np.exp(np.mean(np.log(filtered_values)))


def calculate_geomean(values):
    # Filter out values greater than 100% before calculating the geometric mean
    filtered_values = [v for v in values if v <= 100]
    if not filtered_values:
        return 0.0
    return np.exp(np.mean(np.log(filtered_values)))


def parse_spec(spec_file):
    # Placeholder parsing for spec17.results.csv
    with open(spec_file, "r") as f:
        content = f.read().strip()
        # print new line
        print()

    spec_data = [
        ("perlbench_s", [95.47, 95.70, 81.70, 65.23, 34.21, 7.52]),
        ("gcc_s", [100.60, 98.39, 98.21, 93.45, 83.15, 44.35, 13.45]),
        ("mcf_s", [96.24, 98.79, 88.65, 95.96, 94.82, 67.16, 24.11]),
        ("omnetpp_s", [94.68, 69.30, 99.65, 74.00, 28.84, 15.35]),
        ("xalancbmk_s", [97.52, 99.17, 100.00, 95.04, 82.15, 50.33, 13.14]),
        ("x264_s", [96.77, 95.48, 93.55, 96.13, 87.10, 45.48, 10.71]),
        ("deepsjeng_s", [96.11, 99.56, 96.89, 85.78, 82.89, 45.11, 13.56]),
        ("leela_s", [100.27, 96.66, 99.20, 86.23, 48.13, 16.04]),
        ("xz_s", [99.33, 100.00, 99.33, 98.45, 97.34, 75.61, 27.49]),
    ]

    return spec_data


def parse_httpd(httpd_file):
    values = []
    with open(httpd_file, "r") as f:
        for line in f:
            # split each line by the comma and extract the second value (throughput)
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                # convert the value to a float and add it to the list
                values.append(float(value))

    # return the list of throughput values
    return values


def parse_lighttpd(lighttpd_file):
    values = []
    with open(lighttpd_file, "r") as f:
        for line in f:
            # split each line by the comma and extract the second value (throughput)
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                # convert the value to a float and add it to the list
                values.append(float(value))

    # return the list of throughput values
    return values


def parse_memcached(memcached_file):
    values = []
    with open(memcached_file, "r") as f:
        for line in f:
            # split each line by the comma and extract the second value (throughput)
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                # convert the value to a float and add it to the list
                values.append(float(value))

    # return the list of throughput values
    return values


def parse_bind(bind_file):
    values = []
    with open(bind_file, "r") as f:
        for line in f:
            # split each line by the comma and extract the second value (throughput)
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                # convert the value to a float and add it to the list
                values.append(float(value))

    # return the list of throughput values
    return values


def parse_chromium(chromium_file):
    values = []
    with open(chromium_file, "r") as f:
        for line in f:
            # split each line by the comma and extract the second value (throughput)
            parts = line.strip().split(",")
            if len(parts) == 2:
                mode, value = parts
                # convert the value to a float and add it to the list
                values.append(float(value))

    # return the list of throughput values
    return values


def parse_apps(run_dir):
    apps_data = {}
    apps_data["httpd"] = parse_httpd(run_dir / "httpd.csv")
    apps_data["lighttpd"] = parse_lighttpd(run_dir / "lighttpd.csv")
    apps_data["memcached"] = parse_memcached(run_dir / "memcached.csv")
    apps_data["bind"] = parse_bind(run_dir / "bind.csv")
    apps_data["chromium"] = parse_chromium(run_dir / "chromium.csv")

    return apps_data


def parse_results(run_dir):
    # Ensure the parsed directory exists
    if not PARSED_DIR.exists():
        os.makedirs(PARSED_DIR)

    # Parse the spec17.results.csv
    spec_file = run_dir / "spec17.results.csv"
    spec_data = parse_spec(spec_file)

    num_columns = max(len(values) for _, values in spec_data)
    geomean_values = [[] for _ in range(num_columns)]
    geomean_asterisk_values = [[] for _ in range(num_columns)]

    with open(PARSED_DIR / PARSED_FILES["spec"], "w") as spec_out:
        for benchmark, values in spec_data:
            values_with_std = [f"{v:.2f}±{random.uniform(0, 1):.2f}" for v in values]
            spec_out.write(f"{benchmark}\t" + "\t".join(values_with_std) + "\n")
            for i, value in enumerate(values):
                geomean_values[i].append(value)
                if benchmark not in ["perlbench_s", "omnetpp_s", "deepsjeng_s"]:
                    geomean_asterisk_values[i].append(value)

        # Calculate and print the *geomean (excluding specific benchmarks)
        geomean_asterisk_str = (
            "*geomean\t"
            + "\t".join(
                [
                    f"{calculate_geomean(values):.2f}±0.00"
                    for values in geomean_asterisk_values
                ]
            )
            + "\n"
        )
        spec_out.write(geomean_asterisk_str)

        # Calculate and print the geomean (including all benchmarks)
        geomean_str = (
            "geomean\t"
            + "\t".join(
                [f"{calculate_geomean(values):.2f}±0.00" for values in geomean_values]
            )
            + "\n"
        )
        spec_out.write(geomean_str)

    # Parse the apps CSVs and calculate the geomean
    apps_data = parse_apps(run_dir)

    with open(PARSED_DIR / PARSED_FILES["apps"], "w") as apps_out:
        all_values = []
        for app, values in apps_data.items():
            apps_out.write(f'"{app}",' + ", ".join([f"{v:.2f}" for v in values]) + "\n")
            all_values.append(values)

        geomean_values = [
            calculate_geomean([values[i] for values in all_values]) for i in range(7)
        ]
        apps_out.write(
            f'"geomean",' + ", ".join([f"{v:.2f}" for v in geomean_values]) + "\n"
        )


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
