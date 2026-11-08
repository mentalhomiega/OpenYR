---
title: Show the parts of each frame's checksum in the out-of-sync report
category: internal
release: 0.2.0
targets:
- type: system
  id: developer-mode
  effect: changed
credit: [MentalHomiega]
---

The out-of-sync report's list of recent frame checksums now gives, for each frame, the checksum after the infantry, the units, the buildings, the map layers, the logic list and the random number, with the number of infantry, units, buildings and logic objects. Two players' reports can be compared line by line to find the first frame that differed and which part of the game it came from. The debug log of a network game also writes this once a second as a `SYNC check` line, so the logs of two machines show where they first differed even with the sync log off.

The report's header also lists the timing policy, the current MaxAhead, the timing master house, the MXCSR register, the processor's name and instruction sets, and the mods in force. The game logs the rounding mode of the game thread on its first frame, and logs a later change of it. The autotest command `rounding` sets the mode for a test.
