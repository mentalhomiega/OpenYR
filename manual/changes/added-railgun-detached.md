---
title: Fire railgun beams without waiting with IsDetachedRailgun
category: feature
release: 0.2.0
targets:
- type: key
  id: IsDetachedRailgun
  effect: added
credit:
- MentalHomiega
---

A weapon with `IsDetachedRailgun=yes` fires the railgun beam of `IsRailgun=yes` and may fire again after `ROF` frames, without waiting for the beam's particle system to burn out.
