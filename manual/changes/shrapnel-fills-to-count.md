---
title: Throw every shrapnel piece even when enemies are few
category: fix
release: 0.2.0
targets:
- type: key
  id: ShrapnelCount
  effect: changed
credit: [MentalHomiega]
---

A projectile with a `ShrapnelWeapon` now throws all `ShrapnelCount` pieces, as in Yuri's Revenge. A piece that finds no enemy lands on a random cell within two cells of the impact. Before, the projectile threw only as many pieces as it found enemies, so one with no enemy in range threw none. A projectile whose firer is gone now throws its pieces at random cells too.
