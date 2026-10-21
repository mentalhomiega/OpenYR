---
title: Combine the temporal fade with an animation's own fade
category: fix
release: 0.2.0
targets:
- type: key
  id: DoubleThick
  effect: changed
  scope: animtype
credit:
- MentalHomiega
---

An animation on a structure held by a temporal beam that also fades by `Translucency=25` is now drawn three quarters faded, as the two fades combine in Yuri's Revenge. It kept its own 25 percent fade before.
