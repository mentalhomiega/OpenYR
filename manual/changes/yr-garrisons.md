---
title: Garrison structures
category: feature
release: 0.2.0
targets:
- type: key
  id: CanBeOccupied
  effect: added
- type: key
  id: CanOccupyFire
  effect: added
- type: key
  id: MaxNumberOccupants
  effect: added
- type: key
  id: ShowOccupantPips
  effect: added
- type: key
  id: MuzzleFlash0
  effect: added
- type: key
  id: MuzzleFlash1
  effect: added
- type: key
  id: MuzzleFlash2
  effect: added
- type: key
  id: MuzzleFlash3
  effect: added
- type: key
  id: MuzzleFlash4
  effect: added
- type: key
  id: MuzzleFlash5
  effect: added
- type: key
  id: MuzzleFlash6
  effect: added
- type: key
  id: MuzzleFlash7
  effect: added
- type: key
  id: MuzzleFlash8
  effect: added
- type: key
  id: MuzzleFlash9
  effect: added
- type: key
  id: Occupier
  effect: added
- type: key
  id: OccupyWeapon
  effect: added
- type: key
  id: EliteOccupyWeapon
  effect: added
- type: key
  id: OccupyPip
  effect: added
- type: key
  id: OccupyDamageMultiplier
  effect: added
- type: key
  id: OccupyROFMultiplier
  effect: added
- type: key
  id: OccupyWeaponRange
  effect: added
- type: system
  id: garrisons
  effect: added
credit: [MentalHomiega]
---

Soldiers with `Occupier=yes` can now move into `CanBeOccupied=yes` structures, take them over, and fire their `OccupyWeapon` from inside, as in Yuri's Revenge. The owner empties a garrison with the Deploy command, and the occupants also leave when the structure falls to red health or is destroyed. A garrisonable structure shows one figure per occupant slot to every player.
