---
title: Read burst gaps from the vehicle
category: fix
release: 0.2.0
targets:
- type: key
  id: BurstDelay0
  effect: added
  scope: unittype
- type: key
  id: BurstDelay0
  effect: removed
  scope: weapontype
- type: key
  id: BurstDelay1
  effect: added
  scope: unittype
- type: key
  id: BurstDelay1
  effect: removed
  scope: weapontype
- type: key
  id: BurstDelay2
  effect: added
  scope: unittype
- type: key
  id: BurstDelay2
  effect: removed
  scope: weapontype
- type: key
  id: BurstDelay3
  effect: added
  scope: unittype
- type: key
  id: BurstDelay3
  effect: removed
  scope: weapontype
credit:
- MentalHomiega
---

`BurstDelay0` to `BurstDelay3` are now read from a vehicle's own section and cover the bursts of every weapon it fires, as in Yuri's Revenge. We read them from the weapon before. Move them from the weapon to each vehicle that fires it; infantry, aircraft and structures always use a random three to five frames.
