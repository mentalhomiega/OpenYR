---
title: Read BuildTimeMultiplier
category: fix
release: 0.2.0
targets:
- type: key
  id: BuildTimeMultiplier
  effect: added
credit: [MentalHomiega]
---

`BuildTimeMultiplier` in rulesmd.ini was ignored. It now multiplies the type's build time, after the house's build speed and before the low power and extra factory adjustments.
