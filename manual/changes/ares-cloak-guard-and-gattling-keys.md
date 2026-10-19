---
title: Set cloak sounds and stages, guard scans, manual fire and gattling cycling per type
category: feature
release: 0.2.0
targets:
- type: key
  id: CloakSound
  effect: added
  scope: aircrafttype
- type: key
  id: DecloakSound
  effect: added
- type: key
  id: Cloakable.Stages
  effect: added
- type: key
  id: CanPassiveAquire.Guard
  effect: added
- type: key
  id: CanPassiveAquire.Cloak
  effect: added
- type: key
  id: NoManualFire
  effect: added
- type: key
  id: Gattling.Cycle
  effect: added
credit:
- MentalHomiega
---

An object type can set its own `CloakSound` and `DecloakSound`, and `[AudioVisual]` can set a `DecloakSound` for every type; `none` is silent. `Cloakable.Stages` gives a type its own number of cloaking steps. `CanPassiveAquire.Guard=no` and `CanPassiveAquire.Cloak=no` stop an object on the Guard mission from picking targets on its own. `NoManualFire=yes` takes away the player's attack orders for a type. `Gattling.Cycle=yes` sends a gattling weapon back to its first stage after its last. The keys follow the Ares documentation, and a type that sets none of them behaves as before.
