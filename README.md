# SideCar

#### Leveraging Debugging Extensions in Commodity Processors to Secure Software

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

### SideCar Monitors

## Usage

## Example
