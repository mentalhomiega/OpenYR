---
title: Play built-in CnCNet matches on the adaptive timing
category: performance
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

A match started from the built-in CnCNet lobby now plays on the [adaptive timing](/systems/network-synchronization/#adaptive-policy). Before, it played gamemd's online timing, which sends orders every 10 frames and schedules each one 30 frames ahead or more. That is about 0.6 seconds from an order to the unit moving, even on a 100 ms round trip. The adaptive timing starts at 2 frames and 6 ahead and follows the measured round trip, so a unit answers in about 0.1 to 0.2 seconds on the same connection.

The host's setting decides the timing for everyone. `TimingPolicy=Gamemd` in the `[CnCNet]` section of the settings file brings back gamemd's timing for matches that player hosts.
