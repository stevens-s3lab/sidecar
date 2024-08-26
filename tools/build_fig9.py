import subprocess

# List of values to use as arguments
build_types = ["lto", "cfi", "sidecfi", "safestack", "sidestack", "asan", "sideasan"]

# List of build scripts to execute
build_scripts = [
    "./tools/build_bind.sh",
    "./tools/build_httpd.sh",
    "./tools/build_lighttpd.sh",
    "./tools/build_memcached.sh",
]

# Loop over each build type and each script
for build_type in build_types:
    for build_script in build_scripts:
        try:
            script_name = build_script.split("/")[-1]
            print(f"Building {script_name} with {build_type}...")
            # Execute the script with the current build type
            result = subprocess.run(
                [build_script, build_type, "all"],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )
            print(f"Build completed for {script_name} with {build_type}.\n")
            print(result.stdout.decode("utf-8"))
        except subprocess.CalledProcessError as e:
            print(f"An error occurred while building {script_name} with {build_type}.")
            print(e.stderr.decode("utf-8"))

print("App builds completed.")

print("Spec2017 builds TBA.")

# Paths to wrk and memtier_benchmark
wrk_path = "../sidecar-benchmarks/wrk"
memtier_path = "../sidecar-benchmarks/memtier_benchmark"

# Build directory
build_dir = "../build"

install_commands = [
    f"cd {wrk_path} && make && cp wrk {build_dir}/wrk",
    f"cd {memtier_path} && make && cp memtier_benchmark {build_dir}/memtier_benchmark",
]

for command in install_commands:
    try:
        print(f"Executing: {command}")
        subprocess.run(
            command,
            shell=True,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        print(f"Installation completed: {command}\n")
    except subprocess.CalledProcessError as e:
        print(f"An error occurred during installation.")
        print(e.stderr.decode("utf-8"))

print("Testing-tool builds completed.")
