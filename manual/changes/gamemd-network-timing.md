---
title: Time online and LAN frames as gamemd does
category: performance
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

Online and LAN games now use gamemd's timing. The game speed sets the frame rate and the starting look-ahead, which is 40 frames at the two fastest speeds. The master then sets the look-ahead again every 256 frames, from the worst round trip. Each frame waits a whole number of milliseconds. Online games add 10 ms per frame when a peer falls behind.

The adaptive table from the earlier timing change is still there, selected with `TimingPolicy=Adaptive` in a launch file.
