---
title: Load terrain packs that end in zero padding after the last record
category: fix
release: 0.2.0
targets:
- type: format
  id: scenario-terrain
  effect: changed
credit:
- MentalHomiega
---

A scenario whose `[IsoMapPack4]` or `[IsoMapPack5]` records end in one to three zero bytes instead of a 0,0 terminator used to be refused as damaged. It now loads in full. A record cut off by nonzero bytes, or inside its body, is still damage.
