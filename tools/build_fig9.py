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

print("All builds completed.")
