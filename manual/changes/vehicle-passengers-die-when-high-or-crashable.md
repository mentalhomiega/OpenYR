---
title: Kill a destroyed vehicle's passengers when it is high or Crashable
category: fix
release: 0.2.0
targets:
- type: system
  id: transports
  effect: changed
- type: key
  id: Crashable
  effect: changed
credit:
- MentalHomiega
---

A vehicle destroyed more than 208 leptons above the ground now kills its passengers, and so does a vehicle whose type sets `Crashable=yes` in `rules.ini`. Before, its passengers got out onto its cell when they could. `Crashable` now applies to vehicles as well as aircraft, and a vehicle that omits it still lets its passengers out.
