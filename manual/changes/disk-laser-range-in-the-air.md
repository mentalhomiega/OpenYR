---
title: Measure a flying firer's laser range across the ground
category: fix
release: 0.2.0
targets:
- type: key
  id: DiskLaser
  effect: changed
credit: [MentalHomiega]
---

A DiskLaser weapon's range now ignores the firer's height while the firer is in the air. A Floating Disc hovering above a building counts the building as in range, as the original game does, and it strikes. Before, its height counted against the range, so it abandoned every shot at a building that was not drainable.
