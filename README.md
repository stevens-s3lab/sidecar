# SideCar

### Leveraging Debugging Extensions in Commodity Processors to Secure Software

We present the implementation of SideCar, a novel approach leveraging debugging infrastructure in Intel and Arm processors to create secure,
append-only channels between applications and security monitors.
We implemented security defenses over SideCar for control-flow integrity (SideCFI), shadow call stacks (SideStack), and memory error checking (SideASan).
We implemented an x86-64 and aarch64 Linux kernel driver to
facilitate communication between the monitors and the processor's debugging infrastructure
and a modified version of the LLVM compiler to generate SideCar-compatible code that can be used with the monitors.

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
./tools/install.sh llvm
```

This will create a `build` directory in the root of the repository for build files and
an `install` directory for the installation of the LLVM toolchain.

### SideCar Monitors

## Usage

## Example
