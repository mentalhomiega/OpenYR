---
title: Keep full damage for a direct hit
category: fix
release: 0.2.0
targets:
- type: key
  id: PercentAtMax
  effect: changed
credit:
- MentalHomiega
---

Before, rounding in the distance falloff could take a point from a hit at the blast's center: a 7-point hit from a warhead with `PercentAtMax=.02`, such as NUKE, dealt 6 to a heavy-armored vehicle. We now deal the full figure to a direct hit, before `Verses` applies.
