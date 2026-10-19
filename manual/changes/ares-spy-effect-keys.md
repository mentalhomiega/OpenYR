---
title: Choose the effects of a spy infiltrating a structure
category: feature
release: 0.2.0
targets:
- type: key
  id: SpyEffect.Custom
  effect: added
- type: key
  id: SpyEffect.ResetRadar
  effect: added
- type: key
  id: SpyEffect.PowerOutageDuration
  effect: added
- type: key
  id: SpyEffect.StolenMoneyPercentage
  effect: added
- type: key
  id: SpyEffect.StolenMoneyAmount
  effect: added
- type: key
  id: SpyEffect.ResetSuperweapons
  effect: added
- type: key
  id: SpyEffect.StolenTechIndex
  effect: added
- type: key
  id: SpyEffect.UndoReverseEngineer
  effect: added
- type: key
  id: SpyEffect.InfantryVeterancy
  effect: added
- type: key
  id: SpyEffect.VehicleVeterancy
  effect: added
- type: key
  id: SpyEffect.UnitVeterancy
  effect: added
- type: key
  id: SpyEffect.NavalVeterancy
  effect: added
- type: key
  id: SpyEffect.AircraftVeterancy
  effect: added
- type: key
  id: SpyEffect.BuildingVeterancy
  effect: added
- type: key
  id: Prerequisite.StolenTechs
  effect: added
credit:
- MentalHomiega
---

A structure with `SpyEffect.Custom=yes` does what its other `SpyEffect.` keys say when a spy infiltrates it, in place of the stock effect: it can shroud the owner's map, cut the owner's power, steal credits, restart super weapon charges, steal kinds of technology, undo reverse engineering, and make the infiltrator's new soldiers, vehicles, ships, aircraft or structures veterans. `Prerequisite.StolenTechs` makes a type wait for stolen technology. The keys follow the Ares documentation; revealing radar or production, granting a super weapon and the delayed sabotage are not read. A structure that sets none of them behaves as before.
