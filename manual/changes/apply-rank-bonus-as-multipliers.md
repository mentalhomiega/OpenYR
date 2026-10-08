---
title: Apply rank bonus values as multipliers
category: fix
release: 0.2.0
targets:
- type: key
  id: VeteranCombat
  effect: changed
- type: key
  id: VeteranSpeed
  effect: changed
- type: key
  id: VeteranSight
  effect: changed
- type: key
  id: VeteranArmor
  effect: changed
- type: key
  id: VeteranROF
  effect: changed
credit: [MentalHomiega]
---

We now multiply firepower by `VeteranCombat`, speed by `VeteranSpeed` and sight range by `VeteranSight`, multiply reload delay by `VeteranROF`, and divide incoming damage by `VeteranArmor`, as Yuri's Revenge does. Earlier builds used each value plus one, so `VeteranCombat=1.1` doubled the firepower of a veteran with `FIREPOWER`. The default `1` now changes nothing.
