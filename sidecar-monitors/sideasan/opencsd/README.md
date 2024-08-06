OpenCSD - An open source CoreSight(tm) Trace Decode library        {#mainpage}
===========================================================

This library provides an API suitable for the decode of ARM(r) CoreSight(tm) trace streams.

The library will decode formatted trace in three stages:

1. *Frame Deformatting* : Removal CoreSight frame formatting from individual trace streams.
2. *Packet Processing*  : Separate individual trace streams into discrete packets.
3. *Packet Decode*      : Convert the packets into fully decoded trace describing the program flow on a core.

The library is implemented in C++ with an optional "C" API.

Building OpenCSD Library
-------------------------

```
cd decoder/build/linux
make
sudo make install
```

CoreSight Trace Component Support.
----------------------------------

_Current Version 1.3.0_

### Current support:

- ETE   (v1.2) instruction trace - packet processing and packet decode.
- ETMv4 (v4.6 [A/R profile] v4.4 [M profile]) instruction trace - packet processing and packet decode.
- PTM   (v1.1) instruction trace - packet processing and packet decode.
- ETMv3 (v3.5) instruction trace - packet processing and packet decode.
- ETMv3 (v3.5) data trace - packet processing.
- STM   (v1.1) software trace - packet processing and packet decode.

- External Decoders - support for addition of external / custom decoders into the library.

