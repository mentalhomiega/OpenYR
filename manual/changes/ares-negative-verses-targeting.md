---
title: Let a healing multiplier allow targeting
category: fix
release: 0.2.0
targets:
- type: system
  id: armor-types
  effect: changed
credit:
- MentalHomiega
---

A weapon whose warhead has a negative `Versus.<armor>` or `Verses=` entry, such as `-100%`, now fires at, retaliates against and picks targets of that armor by the size of the multiplier. Before, a negative entry counted as below every threshold, so the weapon was refused automatic targeting and retaliation.
