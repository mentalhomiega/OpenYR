---
title: Give CanC4=no structures one point per hit
category: fix
release: 0.2.0
targets:
- type: key
  id: CanC4
  effect: changed
credit:
- MentalHomiega
---

Before, a hit that armor and `Verses` reduced to nothing did no damage to a structure with `CanC4=no`. We now make it take one point, so each hit costs such a structure strength.
