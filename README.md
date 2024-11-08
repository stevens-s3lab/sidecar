# SideCar

### Leveraging Debugging Extensions in Commodity Processors to Secure Software

_We present the implementation of SideCar, a novel approach leveraging debugging infrastructure in Intel and Arm processors to create secure,
append-only channels between applications and security monitors.
We implemented security defenses over SideCar for control-flow integrity (SideCFI), shadow call stacks (SideStack), and memory error checking (SideASan).
We implemented an x86-64 and aarch64 Linux kernel driver to
facilitate communication between the monitors and the processor's debugging infrastructure
and a modified version of the LLVM 12.0.1 compiler to generate SideCar-compatible code that can be used with the monitors._

## Project Structure

```bash
.
├── README.md                    # Project overview and instructions
├── docs                         # Documentation files
│   ├── llvm-changes.diff        # Diff results between LLVM 12.0.1 and SideCar LLVM
│   └── llvm-modifications.md    # Modifications to LLVM 12.0.1 for SideCar
├── sidecar-driver               # Kernel drivers for different architectures
│   ├── aarch64                  # Drivers for AArch64 architecture
│   └── x86-64                   # Drivers for x86-64 architecture
├── sidecar-examples             # Example implementations using SideCar
│   ├── sideasan                 # Example with SideASan enabled
│   ├── sidecfi                  # Example with SideCFI enabled
│   ├── sideguard                # Example with SideGuard enabled
│   └── sidestack                # Example with SideStack enabled
├── sidecar-llvm                 # Modified LLVM source and related tools
├── sidecar-monitors             # Monitors for SideCar components
│   ├── sideasan                 # Monitor for SideASan
│   ├── sidecfi                  # Monitor for SideCFI
│   ├── sideguard                # Monitor for SideGuard
│   └── sidestack                # Monitor for SideStack
└── tools                        # Scripts and tools for setup and experiments
    ├── gen_tp.sh                # Generate typemaps for SideCFI
    └── install.sh               # LLVM installation script
```

## Installation

### SideCar Linux Kernel Driver

##### x86-64

To build and install the SideCar Linux kernel driver on Intel, run the following commands:

```bash
cd sidecar-driver/x86-64
make
sudo insmod ptw.ko
```

##### Aarch64

If cross-compiling the SideCar Linux kernel driver for ARM,
the environment variables `CROSS_COMPILE` and `KERNELDIR` need to be set like below:

```
export CROSS_COMPILE=/path/to/toolchain/bin/aarch64-linux-gnu-
export KERNELDIR=/path/to/kernel
```

To build run the following commands:

```bash
cd sidecar-driver/aarch64
make
```

To install run the following (after moving driver to ARM device in case of cross-compilation):

```bash
sudo insmod cs_driver.ko
```

### SideCar LLVM Toolchain

To build and install the SideCar LLVM toolchain,
ensure that the following dependencies are installed:

```bash
sudo apt-get install zlib1g-dev binutils-dev binutils-gold build-essential
```

Once the dependencies are installed,
run the following commands:

```bash
./tools/install.sh llvm-sidecar
```

This will create a `build` directory in the root of the repository for build files and
an `install` directory for the installation of the LLVM toolchain.

### SideCar Monitors

Any of the SideCar monitors can be built by moving to the respective directory and running `make`.
e.g. for the x86-64 SideCFI:

```bash
cd sidecar-monitors/sidecfi/x86-64
make
```

## Usage

### SideCFI

To compile a file with SideCFI enabled, use `install/llvm-sidecar/bin/clang` and pass the
flags `-flto -fsanitize=cfi-icall,cfi-vcall -fsanitize-cfi-cross-dso`
and `-fsanitize-cfi-decouple` for decoupling all CFI checks or `-fsanitize-cfi-slowpath-decouple`
for decoupling the slow path checks only. Currently the SideCFI monitor requires
a typemap file containing metadata about the compiled binary's symbols which can be
produced by executing the script `./sidecar/tools/gen_tp.sh <binary>`.

### SideStack

To compile a file with SideStack enabled, use `install/llvm-sidecar/bin/clang` and pass the
flags `-fsanitize=shadow-call-stack -fsanitize-sidestack`.

### SideASan

To compile a file with SideASan enabled, use `install/llvm-sidecar/bin/clang` and pass the
flags `-fsanitize=address -mllvm -asan-decouple`. To avoid triggering the Leak Sanitizer
disable it by running `export ASAN_OPTIONS=detect_leaks=0`.

## Example

Example programs built with SideCFI, SideStack, and SideASan can be found in the `sidecar-examples` directory. Entering the
respective directory and running `make` will compile the example with the respective SideCar
defense enabled. e.g. for SideCFI:

```bash
cd sidecar-examples/sidecfi
make
```
