---
title: Stop firing at an armor with 0% damage
category: fix
release: 0.2.0
targets:
- type: key
  id: Verses
  effect: changed
credit:
- MentalHomiega
---

A weapon whose warhead has `0%` against a target's armor no longer fires at that target even when ordered to, as in Yuri's Revenge. A `0%` entry previously only made an object with two weapons pass over that weapon when choosing one. `Versus.<armor>.ForceFire=yes` allows the shot.
