---
title: Tint a structure with a Palette= under a Force Shield
category: fix
release: 0.2.0
targets:
- type: key
  id: ForceShieldColor
  effect: changed
- type: key
  id: Palette
  effect: changed
credit: [MentalHomiega]
---

A structure whose art sets `Palette=` now keeps that palette under a Force Shield or an airstrike mark, with the tint color added to it, on its body and its animations, as Yuri's Revenge does. Before, the body fell back to the house colors under the tint and the animations took no tint. An animation of any structure takes the tint on whichever palette it uses, and a voxel vehicle with a `Palette=` takes the Iron Curtain color on it.
