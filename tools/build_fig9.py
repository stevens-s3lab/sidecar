import os
import subprocess

# Resolve the absolute path to the directory containing this script
script_dir = os.path.dirname(os.path.abspath(__file__))

# List of values to use as arguments
build_types = ["lto", "cfi", "sidecfi", "safestack", "sidestack", "asan", "sideasan"]

# List of build scripts to execute
build_scripts = [
    os.path.join(script_dir, "build_bind.sh"),
    os.path.join(script_dir, "build_httpd.sh"),
    os.path.join(script_dir, "build_lighttpd.sh"),
    os.path.join(script_dir, "build_memcached.sh"),
]

# Loop over each build type and each script
for build_type in build_types:
    for build_script in build_scripts:
        try:
            script_name = os.path.basename(build_script)
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
wrk_path = os.path.join(script_dir, "../sidecar-benchmarks/wrk")
memtier_path = os.path.join(script_dir, "../sidecar-benchmarks/memtier_benchmark")

# Build directory
ins_dir = os.path.join(script_dir, "../install/tools")

os.makedirs(ins_dir, exist_ok=True)

wrk_commands = [
    f"make -C {wrk_path}",
    f"cp {os.path.join(wrk_path, 'wrk')} {os.path.join(ins_dir, 'wrk')}",
]

# Execute the wrk build and installation commands
for command in wrk_commands:
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
        print(f"An error occurred during wrk installation.")

memtier_commands = [
    f"cd {memtier_path} && libtoolize && automake --add-missing && ./configure",
    f"make -C {memtier_path}",
    f"cp {os.path.join(memtier_path, 'memtier_benchmark')} {os.path.join(ins_dir, 'memtier_benchmark')}",
]

# Execute the memtier_benchmark build and installation commands
for command in memtier_commands:
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
        print(f"An error occurred during memtier_benchmark installation.")
        print(e.stderr.decode("utf-8"))

print("Testing-tool builds completed.")
