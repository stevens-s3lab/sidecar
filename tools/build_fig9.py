import subprocess

# List of values to use as arguments
build_types = ["lto", "cfi", "sidecfi", "safestack", "sidestack", "asan", "sideasan"]

# Loop over each build type and execute the script
for build_type in build_types:
    try:
        print(f"Building lighttpd with {build_type}...")
        # Execute the script with the current build type
        result = subprocess.run(
            ["./tools/build_lighttpd.sh", build_type, "all"],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        print(f"Build completed for {build_type}.\n")
        print(result.stdout.decode("utf-8"))
    except subprocess.CalledProcessError as e:
        print(f"An error occurred while building {build_type}.")
        print(e.stderr.decode("utf-8"))

print("All builds completed.")
