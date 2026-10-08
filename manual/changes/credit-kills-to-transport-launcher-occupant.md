---
title: Credit kills to the transport, launcher or occupant
category: fix
release: 0.2.0
targets:
- type: system
  id: veterancy
  effect: changed
credit: [MentalHomiega]
---

We now give a kill to the transport of a unit that fires from an open-topped transport, to the launcher of a `MissileSpawn=yes` object, and to the occupant that fired from a structure with `CanOccupyFire=yes`, as Yuri's Revenge does. Earlier builds gave the experience to the firing object alone, and only when that object was trainable.
