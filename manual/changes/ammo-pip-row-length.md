---
title: Show one ammunition pip for every shot
category: fix
release: 0.2.0
targets:
- type: key
  id: PipScale
  effect: changed
- type: key
  id: MaxPips
  effect: changed
credit: [MentalHomiega]
---

An object with `PipScale=Ammo` now shows a pip for each shot its type holds, as in Yuri's Revenge. The row was cut off at five pips, so a type with `Ammo=12` showed five. `MaxPips=` still shortens the row.
