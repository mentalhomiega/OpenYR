---
title: Stun through a warhead's blast with EMP.Duration and EMP.Cap
category: feature
release: 0.2.0
targets:
- type: key
  id: EMP.Duration
  effect: added
- type: key
  id: EMP.Cap
  effect: added
credit:
- MentalHomiega
---

A warhead with `EMP.Duration=` other than `0` stuns, or with a negative value reactivates, the objects its blast damages instead of making a pulse. `EMP.Cap=` decides how repeated hits add up. `EMEffect=yes` without `EMP.Duration=` makes a pulse as before.
