---
title: Read elite weapons as Yuri's Revenge does
category: feature
release: 0.2.0
targets:
- type: key
  id: Elite
  effect: removed
- type: key
  id: ElitePrimary
  effect: added
- type: key
  id: EliteSecondary
  effect: added
- type: key
  id: ElitePrimaryFireFLH
  effect: added
- type: key
  id: ElitePBarrelLength
  effect: added
- type: key
  id: ElitePBarrelThickness
  effect: added
- type: key
  id: EliteSecondaryFireFLH
  effect: added
- type: key
  id: EliteSBarrelLength
  effect: added
- type: key
  id: EliteSBarrelThickness
  effect: added
credit: [Lucas]
---

An elite object now takes its weapons from `ElitePrimary` and `EliteSecondary`, which replace the primary and secondary weapons. `Elite=` is no longer read; rename it to `ElitePrimary=`. Each elite slot has its own firing offset and barrel keys in the art file, defaulting to the normal slot's values.
