---
title: Round prone damage to the nearest point
category: fix
release: 0.2.0
targets:
- type: key
  id: ProneDamage
  effect: changed
credit: [MentalHomiega]
---

A soldier that is lying down now takes the scaled damage rounded to the nearest point, not rounded down. A 15-point hit against `ProneDamage=50%` deals 8 points instead of 7. The minimum of one point is unchanged.
