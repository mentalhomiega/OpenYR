---
title: Play the nuclear build-up before the blast
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A projectile whose warhead is named `NUKE` now plays the `NUKEBALL` animation where it lands, and deals its damage when that animation ends. Before, the damage went off the moment the projectile landed, and `NUKEBALL` never played.
