---
title: A NeedsEngineer structure is out of service until captured
category: fix
release: 0.2.0
targets:
- type: key
  id: NeedsEngineer
  effect: changed
credit:
- MentalHomiega
---

A structure with `NeedsEngineer=yes` is out of service until an engineer captures it, as the original game does. It no longer fires its weapon, runs its cloak generator, gap generator or sensor array, or counts for a spy satellite. A neutral Tech Outpost stopped shooting at a vehicle parked beside it. The same applies to a structure that a map places under a player's house: it stays out of service until its owner changes once.
