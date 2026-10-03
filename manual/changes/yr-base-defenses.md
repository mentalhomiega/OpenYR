---
title: Pick computer base defenses from each side's list
category: feature
release: 0.2.0
targets:
- type: key
  id: AlliedBaseDefenses
  effect: added
- type: key
  id: SovietBaseDefenses
  effect: added
- type: key
  id: ThirdBaseDefenses
  effect: added
credit: [MentalHomiega]
---

A computer house now picks its base defenses only from the `[AI]` list for its side: `AlliedBaseDefenses`, `SovietBaseDefenses` or `ThirdBaseDefenses`. Before, any structure with a defense value was a candidate, so a Soviet computer could build a Yuri Gattling Cannon.
