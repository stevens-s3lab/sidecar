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

- To avoid having the process switch between cores that could cause packet loss, pin the process to a specific core (e.g. taskset -c 0)

- To trigger a violation, run:

```bash
taskset -c 0 ./main 1
```

This will cause a violation at the monitor with a message like:

```
-----------Violation-------------
```

- To run without triggering a violation, run:

```bash
taskset -c 0 ./main 2
```
