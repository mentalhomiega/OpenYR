---
title: Read more Ares keys that Mental Omega uses, part six
category: feature
release: 0.2.0
targets:
- type: key
  id: AirRate
  effect: added
- type: key
  id: Temporal.HealthFactor
  effect: added
- type: key
  id: GateUpSound
  effect: added
- type: key
  id: GateDownSound
  effect: added
- type: key
  id: Flash.Duration
  effect: added
- type: key
  id: AllowZeroDamage
  effect: added
- type: key
  id: RelativeDamage
  effect: added
- type: key
  id: RelativeDamage.Buildings
  effect: added
- type: key
  id: RelativeDamage.Aircraft
  effect: added
- type: key
  id: RelativeDamage.Infantry
  effect: added
- type: key
  id: RelativeDamage.Vehicles
  effect: added
- type: key
  id: Academy.InfantryVeterancy
  effect: added
- type: key
  id: Academy.VehicleVeterancy
  effect: added
- type: key
  id: Academy.AircraftVeterancy
  effect: added
- type: key
  id: Academy.BuildingVeterancy
  effect: added
- type: key
  id: Academy.Types
  effect: added
- type: key
  id: Academy.Ignore
  effect: added
- type: key
  id: SpyEffect.RevealProduction
  effect: added
- type: key
  id: AIInnerBase
  effect: added
- type: key
  id: RadarJamRadius
  effect: added
credit:
- MentalHomiega
---

`AirRate=` paces an object's animation in the air, and `RadarJamRadius=` lets an object jam the radar structures of houses that are not its owner's allies. `Temporal.HealthFactor=` shortens the warp of a hurt object, `Flash.Duration=` flashes what a warhead hits, `AllowZeroDamage=` lets a blast with no damage reach its targets, and `RelativeDamage=` with its `RelativeDamage.` percentages hurts by a share of the victim's strength. `GateUpSound=` and `GateDownSound=` give a gate its own sounds, the `Academy.` keys give a house's new objects veterancy, `SpyEffect.RevealProduction=` decides what a spy's house sees of a `SpyEffect.Custom=yes` structure, and `AIInnerBase=` has the computer place a structure near the center of its base. The keys follow the Ares documentation, and a type, warhead or rule that sets none of them behaves as before.
