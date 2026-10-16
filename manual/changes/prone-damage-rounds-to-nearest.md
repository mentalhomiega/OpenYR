---
title: Round prone damage down to a whole point
category: fix
release: 0.2.0
targets:
- type: key
  id: ProneDamage
  effect: changed
credit: [MentalHomiega]
---

A soldier that is lying down takes the scaled damage rounded down to a whole point, as gamemd does, and never less than one point. A 15-point hit against `ProneDamage=50%` deals 7 points.
