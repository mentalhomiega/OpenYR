---
title: Deal one point from a zero-damage hit
category: fix
release: 0.2.0
targets:
- type: system
  id: warheads
  effect: changed
credit:
- MentalHomiega
---

Before, a hit of zero damage did nothing to a vehicle, infantryman, aircraft or structure. We now raise it to one point before armor and `Verses` apply, so a warhead with a 100% `Verses` entry for the target's armor deals one point. A `Webby=yes` hit still does nothing.
