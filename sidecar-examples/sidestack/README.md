# SideStack Test Example

## Setup

1. Install the driver:
```bash
sudo insmod ../../sidecar-driver/x86-64/ptw.ko
```

2. Start the monitor:
```bash
../../sidecar-monitors/sidestack/x86-64/monitor
```

## Execution

- To trigger a violation, run:
```bash
./main 1
```

This will cause a violation at the monitor with a message like:
```
-----------Violation-------------
```

- To run without triggering a violation, run:
```bash
./main 2
```
