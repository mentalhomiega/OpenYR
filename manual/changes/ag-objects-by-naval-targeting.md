---
title: Let an AG=no projectile fire at an object on water or land
category: fix
release: 0.2.0
targets:
- type: key
  id: AG
  effect: changed
credit:
- MentalHomiega
---

A weapon whose projectile is [`AG=no`](/keys/ag/) now fires at an object on the ground or on water when its owner's [`NavalTargeting`](/keys/navaltargeting/) or [`LandTargeting`](/keys/landtargeting/) allows it. Before, any object on the ground refused such a weapon, so a destroyer's Osprey launcher (`ASWVirt`, `AG=no`) could not fire at a submarine. A map cell is still refused.
