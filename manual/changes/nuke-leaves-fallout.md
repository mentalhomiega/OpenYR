---
title: Leave radiation where a nuclear missile lands
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
- type: system
  id: radiation
  effect: changed
- type: key
  id: RadLevel
  effect: changed
credit:
- MentalHomiega
---

The payload of a nuclear missile now leaves radiation where it lands, as the `RadLevel` of the `NukePayload` weapon sets. Before, the blast did its damage and left no fallout.
