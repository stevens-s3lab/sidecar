# SideCFI Test Example

## Setup

1. Install the driver:
```bash
sudo insmod ../../sidecar-driver/x86-64/ptw.ko
```

2. Start the monitor:
```bash
../../sidecar-monitors/sidecfi/x86-64/monitor
```

## Execution

- To avoid context switches that could cause packet loss pin the process to a specific core (e.g. taskset -c 0)

- To trigger a violation, run:
```bash
taskset -c 0 ./main 1
```

This will cause a violation at the monitor with a message like:
```
CFI CHECK ERROR: no matching address found in the typemap!
MSG{0x56402793eef0, 6386}
```

- To run without triggering a violation, run:
```bash
taskset -c 0 ./main 2
```
