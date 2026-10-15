---
title: Keep prone damage rounded down
category: fix
release: 0.2.0
targets:
- type: key
  id: ProneDamage
  effect: changed
credit: [MentalHomiega]
---

A soldier that is lying down takes the scaled damage rounded down to a whole point, so a 15-point hit against `ProneDamage=50%` deals 7 points. The minimum of one point is unchanged.
