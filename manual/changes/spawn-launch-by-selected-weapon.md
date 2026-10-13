---
title: Launch carried aircraft by the weapon selected for the target
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit:
- MentalHomiega
---

Carried aircraft launch while the target is in range of the weapon its owner selects for it, not only the primary weapon. A destroyer's Osprey therefore launches at a submarine at any distance, because [`NavalTargeting=1`](/keys/navaltargeting/) selects the `Range=-2` ASW launcher there. Before, the primary weapon's range decided, so a submarine more than 8 cells away never got an Osprey.
