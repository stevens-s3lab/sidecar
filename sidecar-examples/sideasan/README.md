# SideASAN Test Example

## Setup

1. Install the driver:
```bash
sudo insmod ../../sidecar-driver/x86-64/ptw.ko
```

2. Start the monitor:
```bash
../../sidecar-monitors/sideasan/x86-64/monitor
```

## Execution

- To avoid context switches that could cause packet loss pin the process to a specific core (e.g. taskset -c 0)
- The Leak Sanitizer needs to be disabled to avoid being triggered:
```bash
export ASAN_OPTIONS=detect_leaks=0
```

- To trigger an overflow run the test program with an input string of 31 chars or longer:
```bash
taskset -c 0 ./so aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
```

This will cause a violation at the monitor with a message indicating the overflow and a representation of the shadow memory like below:
```
ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7ffe0426b060
WRITE of size 1 at 0x7ffe0426b060 thread T0
Shadow bytes around the buggy address:
  0x1000408455b0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000408455c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000408455d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000408455e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x1000408455f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x100040845600: 00 00 00 00 f1 f1 f1 f1 00 00 00 00[f2]f2 f2 f2
  0x100040845610: f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8 f8
  0x100040845620: f3 f3 f3 f3 00 00 00 00 00 00 00 00 00 00 00 00
  0x100040845630: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100040845640: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100040845650: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc

```

- To run without triggering a violation, run with a string input of less than 31 chars:
```bash
taskset -c 0 ./so aaaaaaaaaaaaaa
```
