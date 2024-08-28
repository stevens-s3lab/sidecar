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
├── sidecar-driver               # Kernel drivers for different architectures
│   ├── aarch64                  # Drivers for AArch64 architecture
│   └── x86-64                   # Drivers for x86-64 architecture
├── sidecar-examples             # Example implementations using SideCar
│   ├── sideasan
│   ├── sidecfi
│   └── sidestack
├── sidecar-llvm                 # Modified LLVM source and related tools
│   ├── clang
│   ├── clang-tools-extra
│   ├── compiler-rt
│   ├── debuginfo-tests
│   ├── flang
│   ├── libc
│   ├── libclc
│   ├── libcxx
│   ├── libcxxabi
│   ├── libunwind
│   ├── lld
│   ├── lldb
│   ├── llvm
│   ├── mlir
│   ├── openmp
│   ├── parallel-libs
│   ├── polly
│   ├── pstl
│   ├── runtimes
│   ├── utils
│   └── README.md
├── sidecar-monitors             # Monitors for SideCar components
│   ├── sideasan
│   ├── sidecfi
│   ├── sideguard
│   └── sidestack
└── tools                        # Scripts and tools for setup and experiments
    └── install.sh               # Installation script
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

To build and install the SideCar LLVM toolchain, run the following commands:

```bash
./tools/install.sh llvm-sidecar
```

This will create a `build` directory in the root of the repository for build files and
an `install` directory for the installation of the LLVM toolchain.

### SideCar Monitors

Any of the SideCar monitors can be built by moving to the respective directory and running `make`.
e.g. for SideCFI:

```bash
cd sidecar-monitors/sidecfi
make
```

## Usage

### SideCFI

To compile a file with SideCFI enabled, use `install/llvm-sidecar/bin/clang` and pass the
flags `-flto -fsanitize=cfi-icall,cfi-vcall -fsanitize-cfi-cross-dso`
and `-fsanitize-cfi-decouple` for decoupling all CFI checks or `-fsanitize-cfi-slowpath-decouple`
for decoupling the slow path checks only.

### SideStack

To compile a file with SideStack enabled, use `install/llvm-sidecar/bin/clang` and pass the
flags `-fsanitize=shadow-call-stack -fsanitize-sidestack`.

### SideASan

To compile a file with SideASan enabled, use `install/llvm-sidecar/bin/clang` and pass the
flags `-fsanitize=address -mllvm -asan-decouple`.

## Example

Example programs built with SideCFI, SideStack, and SideASan can be found in the `sidecar-examples` directory. Entering the
respective directory and running `make` will compile the example with the respective SideCar
defense enabled. e.g. for SideCFI:

```bash
cd sidecar-examples/sidecfi
make
```
