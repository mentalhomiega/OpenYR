---
title: Read Level for bullet types
category: feature
release: 0.2.0
targets:
- type: key
  id: Level
  effect: added
  scope: bullettype
credit:
- MentalHomiega
---

We read `Level` for each BulletType, as Yuri's Revenge does. A homing projectile with `Level=yes` keeps the pitch it was fired with while it has a target. It still turns sideways toward its target. Before, the key was not read, and every homing projectile steered its pitch toward its target. A projectile with no `ROT` ignores the key.

Saves made by earlier builds no longer load, because bullet types now save this setting.
