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
    # Call the bash script and capture its output
    result = subprocess.run(
        ["bash", "run_spec17.sh"], stdout=subprocess.PIPE, text=True
    )

    # Write the output of the bash script to the file
    with open(file_path, "w") as f:
        f.write(result.stdout)


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
    filtered_values = [v for v in values if 0 < v <= 100]
    if not filtered_values:
        return 0.0
    return np.exp(np.mean(np.log(filtered_values)))


def parse_spec(spec_file):
    spec_data = []
    with open(spec_file, "r") as f:
        # Process the output as CSV
        for line in f:
            parts = line.strip().split(",")
            if len(parts) > 1:
                benchmark = parts[0]
                values = list(
                    map(float, parts[1:])
                )  # Convert the rest of the values to float
                spec_data.append((benchmark, values))

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
    geomean_values = [[] for _ in range(num_columns // 2)]
    geomean_asterisk_values = [[] for _ in range(num_columns // 2)]

    with open(PARSED_DIR / PARSED_FILES["spec"], "w") as spec_out:
        for benchmark, values in spec_data:
            # Convert all values to strings and join them with commas
            values_str = [str(v) for v in values]
            spec_out.write(f"{benchmark}," + ",".join(values_str) + "\n")
            for i in range(
                0, len(values) - 1, 2
            ):  # Step by 2 to handle paired columns (percentage and std dev)
                try:
                    value_numeric = float(
                        values[i]
                    )  # Convert the percentage value to float
                    geomean_values[i // 2].append(value_numeric)
                    if benchmark not in ["perlbench_s", "omnetpp_s", "deepsjeng_s"]:
                        geomean_asterisk_values[i // 2].append(value_numeric)
                except (IndexError, ValueError):
                    # Skip if the index is out of range or conversion fails
                    continue

        # Calculate and print the *geomean (excluding specific benchmarks)
        geomean_asterisk_str = (
            "*geomean,"
            + ",".join(
                [
                    f"{calculate_geomean(geomean_asterisk_values[i]):.2f},0.00"
                    for i in range(len(geomean_asterisk_values))
                ]
            )
            + "\n"
        )
        spec_out.write(geomean_asterisk_str)

        # Calculate and print the geomean (including all benchmarks)
        geomean_str = (
            "geomean,"
            + ",".join(
                [
                    f"{calculate_geomean(geomean_values[i]):.2f},0.00"
                    for i in range(len(geomean_values))
                ]
            )
            + "\n"
        )
        spec_out.write(geomean_str)

    # Parse the apps CSVs and calculate the geomean
    apps_data = parse_apps(run_dir)

    with open(PARSED_DIR / PARSED_FILES["apps"], "w") as apps_out:
        all_values = []
        for app, values in apps_data.items():
            values_str = [str(v) for v in values]
            apps_out.write(f'"{app}",' + ", ".join(values_str) + "\n")
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
