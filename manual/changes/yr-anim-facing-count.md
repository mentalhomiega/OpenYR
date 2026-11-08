---
title: Pick a firing animation by direction only from an eight-entry list
category: fix
release: 0.2.0
targets:
- type: key
  id: Anim
  scope: weapontype
  effect: changed
credit: [MentalHomiega]
---

A weapon's [`Anim`](/keys/anim/) list now picks its entry by the direction of the shot only when it has eight entries, as in Yuri's Revenge. A list of 16, 32 or 64 entries plays its first entry on every shot. Set [`ExtendedAnimFacings=yes`](/keys/extendedanimfacings/) on the weapon to keep the per-direction choice for those lengths.
